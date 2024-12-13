/******************************************************************************
 *
 * Copyright (C) 2024 Analog Devices, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

/**
 * @file    main.c
 * @brief   Main for UART with DMA example.
 * @details This example demonstrates how to use DMA with UART for data transfer.
 *          The example sets up DMA channels to handle UART transmissions and receptions,
 *          looping back the TX of one UART to the RX of another UART. A jumper must be
 *          connected between P0.20 (RX of UART1) and P0.1 (TX of UART2).
 *          The example uses either automatic or manual DMA handler configuration
 *          depending on the AUTOHANDLERS macro definition.
 */

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mxc_device.h"
#include "pb.h"
#include "board.h"
#include "mxc_delay.h"
#include "uart.h"
#include "dma.h"
#include "nvic_table.h"
#include "led.h"

/***** Definitions *****/
// #define AUTOHANDLERS

#define UART_BAUD 115200
#define BUFF_SIZE 512

#define RX_UART MXC_UART1
#define TX_UART MXC_UART2

#define DMA MXC_DMA0

/***** Globals *****/
volatile int READ_FLAG;
volatile int WRITE_FLAG;
volatile int buttonPressed;
static mxc_uart_req_t read_req;
static mxc_uart_req_t write_req;

#ifndef BOARD_FTHR2
#warning "This example has been written for the MAX32665 FTHR2 board."
#endif

/***** Functions *****/
#ifndef AUTOHANDLERS
void DMA_RX_Handler(void)
{
    MXC_DMA_Handler(DMA);
}

void DMA_TX_Handler(void)
{
    MXC_DMA_Handler(DMA);
}
#endif

void readCallback(mxc_uart_req_t *req, int error)
{
    READ_FLAG = error;
}

void writeCallback(mxc_uart_req_t *req, int error)
{
    WRITE_FLAG = error;
}

void buttonHandler(void)
{
    buttonPressed = 1;
}

#ifdef AUTOHANDLERS
int exampleDMAAutoHandlers(void)
{
    int error = 0;

    // Auto DMA handlers will automatically initialize DMA, acquire & assign channels,
    // and guarantee that each transaction's callback function is executed when
    // the transaction is complete.
    MXC_UART_SetAutoDMAHandlers(RX_UART, true);
    MXC_UART_SetAutoDMAHandlers(TX_UART, true);

    // "READ_FLAG" is set in the read transaction's callback.  It will be set to 0 when
    // the read request completes successfully.  We use it to wait for the DMA transaction
    // to complete, since the DMA APIs are asynchronous (non-blocking)
    READ_FLAG = 1;

    error = MXC_UART_TransactionDMA(&read_req, DMA);
    if (error) {
        printf("-->Error starting DMA read: %d\n", error);
        printf("-->Example Failed\n");
        return error;
    }

    error = MXC_UART_TransactionDMA(&write_req, DMA);
    if (error) {
        printf("-->Error starting DMA write: %d\n", error);
        printf("-->Example Failed\n");
        return error;
    }

    while (READ_FLAG) {}

    printf("-->Transaction completed\n");
    return READ_FLAG;
}
#else
int exampleDMAManualHandlers(void)
{
    int error = 0;
    // Manally initialize DMA
    MXC_DMA_Init(DMA);

    // Manually acquire a channel for the read request and assign it to the drivers.
    int rx_channel = MXC_DMA_AcquireChannel(DMA);
    if (rx_channel >= 0) {
        printf("Acquired DMA channel %i for RX transaction\n", rx_channel);
    } else {
        printf("Failed to acquire RX DMA channel with error %i\n", rx_channel);
        return rx_channel;
    }
    MXC_UART_SetRXDMAChannel(RX_UART, rx_channel);

    // Additionally, assign the NVIC IRQ to a function that calls "MXC_DMA_Handler()".
    // This is required for any assigned callbacks to work.
    NVIC_EnableIRQ(MXC_DMA_CH_GET_IRQ(rx_channel));
    MXC_NVIC_SetVector(MXC_DMA_CH_GET_IRQ(rx_channel), DMA_RX_Handler);

    // Do the same for the write request.
    int tx_channel = MXC_DMA_AcquireChannel(DMA);
    if (tx_channel >= 0) {
        printf("Acquired DMA channel %i for RX transaction\n", tx_channel);
    } else {
        printf("Failed to acquire RX DMA channel with error %i\n", tx_channel);
        return tx_channel;
    }
    MXC_UART_SetTXDMAChannel(TX_UART, tx_channel);
    NVIC_EnableIRQ(MXC_DMA_CH_GET_IRQ(tx_channel));
    MXC_NVIC_SetVector(MXC_DMA_CH_GET_IRQ(tx_channel), DMA_TX_Handler);

    // Initialize flags.  We will use these to monitor when the read/write requests
    // have completed, since the DMA APIs are asynchronous.
    WRITE_FLAG = 1;
    READ_FLAG = 1;

    error = MXC_UART_TransactionDMA(&read_req, DMA);
    if (error) {
        printf("-->Error starting DMA read: %d\n", error);
        printf("-->Example Failed\n");
        return error;
    }

    error = MXC_UART_TransactionDMA(&write_req, DMA);
    if (error) {
        printf("-->Error starting DMA write: %d\n", error);
        printf("-->Example Failed\n");
        return error;
    }

    while (WRITE_FLAG) {}
    while (READ_FLAG) {}

    printf("-->Transaction completed\n");
    return WRITE_FLAG;
}
#endif // AUTOHANDLERS

typedef void (*mxc_uart_req_timeout_cb_t)(void *req, int result);

typedef struct {
    mxc_uart_regs_t *uart;
    uint8_t *rxData;
    uint32_t rxLen;
    volatile uint32_t rxCnt;
    int timeout;
    mxc_dma_regs_t *dma;
} mxc_uart_req_timeout_t;

static int dma_channel_timer_count = 0;
void rxCallbackHandler(int par1, int par2) {
    dma_channel_timer_count++;
    printf("%d\r\n", dma_channel_timer_count);
}

/**
 * @brief This macro is defined for initialization of an object to configure uart
 * transaction object with required parameters.
 *
 */
#define UartTimeoutTransaction_req(req, par_uart, par_rxData, par_dma, par_timeout) \
    mxc_uart_req_timeout_t req = {  .uart = par_uart,                     \
                                    .rxData = par_rxData,                 \
                                    .rxLen = par_rxLen,                   \
                                    .rxCount = 0,                         \
                                    .timeout = par_timeout,               \
                                    .dma = par_dma };

volatile int rxChannel = 0;

void DMA0_TestIRQHandler(void){
    uint32_t flags = MXC_DMA_ChannelGetFlags(rxChannel);
    if( (flags & ( 1<< 6))){ // check if it is timeout interrupt.
        MXC_DMA_ChannelClearFlags(rxChannel, flags);
        printf("timeo\r\n");
    }else{
        printf("diff int\r\n");
    }
}

/**
 * @note
 * 
 * DMA_CFG.to_period = 1
 * prescaled = 3 
 * fHCLK to timer after scaling = 96'000'000 / 2^24 = 5.72Hz
 * timeout period = (2^24 *(1))/ 5.72 = 2'933'079.7 microseconds = around 3 seconds.
 * 
 * So, our expectation is to get interrupt triggering
 * for timeout for every 3 seconds.
 * 
 */
static int MXC_UART_TransactionDMA_Config(void){
    static uint8_t destAddr[100];
    mxc_dma_regs_t *dma_reg = (mxc_dma_regs_t *)MXC_DMA0;
    MXC_DMA_Init(dma_reg);

    int rxChannel = MXC_DMA_AcquireChannel(dma_reg);
    if (0 > rxChannel) {
        return -1;
    }

    mxc_dma_ch_regs_t *regs = MXC_DMA_GetCHRegs(rxChannel);
    regs->cfg &= ~(1 << 31); // ctzien disable.
    regs->cfg &= ~(1 << 30); // chdien disable.
    regs->st &= ~(1 << 2);   // ctz_st disable.

    regs->dst = (uint32_t)destAddr;  // Setting the destination address.
    regs->cnt = sizeof(destAddr); // Set size of the transfer on channel.

    regs->cfg |= (4 << 4);   // <reqsel> Choose uart rx as the source.
    regs->cfg |= (1 << 24);  // <brst> set burst size to 1.
    regs->cfg &= ~(3 << 2);  // <pri> set pri to highest prio.
    regs->cfg &= ~(3 << 20); // <dst_width> destination width is 1 byte.
    regs->cfg |= (1 << 22);  // <dst_inc> enable destination increment.
    regs->cfg &= ~(3 << 16); // <src_width> source width is 1 byte.
    regs->cfg &= ~(1 << 18); // <src_inc> to 0.

    /* Configuration for timeout interrupt 9.7 */
    regs->cfg |= (3 << 14);  // <pssel> timeout prescaler to freq/2^16.
    regs->cfg &= ~(1 << 10); // <reqwait> disable request wait.
    regs->cfg |= ( 4 << 11); // <tosel> timeout period select to 63-64.

    NVIC_EnableIRQ(MXC_DMA_CH_GET_IRQ(rxChannel));
    MXC_NVIC_SetVector(MXC_DMA_CH_GET_IRQ(rxChannel), DMA0_TestIRQHandler);

    // Enable DMA channel interrupt
    dma_reg->cn |= (1 << 0); // use channel 0.

    regs->cfg |= (1 << 0); // <chen> Start channel
    
    regs->cfg |= (1 << 30); // <chdien> Enable CH interrupt.
    regs->cfg |= (1 << 31); // <ctzien> Enable CH count zero interrupt.

    return 0;
}

int MXC_UART_TransactionRXTimeout(mxc_uart_req_timeout_t *req)
{
    // int error = E_NO_ERROR;

    // // 0-> Check input parameters.
    // if (NULL == req->uart || req->rxData == NULL) {
    //     return E_BAD_PARAM;
    // }

    // // Init dma peripheral.
    // MXC_DMA_Init(req->dma);

    // // 1-> Get channel.
    // int rxChannel = MXC_DMA_AcquireChannel(req->dma);
    // if (0 > rxChannel) {
    //     return -1;
    // }

    // MXC_UART_SetRXDMAChannel(req->uart, rxChannel);
    // MXC_DMA_SetCallback(rxChannel, rxCallbackHandler);

    // // 2-> Enable interrupt for channel.
    // NVIC_EnableIRQ(MXC_DMA_CH_GET_IRQ(rxChannel));
    // MXC_NVIC_SetVector(MXC_DMA_CH_GET_IRQ(rxChannel), DMA_RX_Handler);
    // MXC_DMA_ChannelEnableInt(rxChannel, 1);
    // MXC_DMA_EnableInt(rxChannel);

    // // reqwait = 1. So, timeout timer will start from the first dma transaction.
    // mxc_dma_adv_config_t dma_timeo_cfg = { .pssel = 0x0 /* 2^24 prescaler */,
    //                                        .burst_size = 1,
    //                                        .ch = rxChannel,
    //                                        .reqwait_en = 0,
    //                                        .prio = MXC_DMA_PRIO_HIGH,
    //                                        .tosel = MXC_DMA_TIMEOUT_512_CLK };

    // MXC_DMA_AdvConfigChannel(dma_timeo_cfg);

    // mxc_uart_req_t uart_req = {0};
    
    // uart_req.rxCnt = req->rxCnt;
    // uart_req.rxData = req->rxData;
    // uart_req.rxLen = req->rxLen;

    // uart_req.txData = NULL;
    // uart_req.txCnt = 0;
    // uart_req.txLen = 0;

    // uart_req.uart = req->uart;

    // /**
    //  * @brief Implementation of the UART transaction initilization
    //  */

    // MXC_UART_DisableInt(req->uart, 0xFFFFFFFF);
    // MXC_UART_ClearFlags(req->uart, 0xFFFFFFFF);

    // (req->uart)->dma |= (1 << 16); // Set RX DMA threshold to 1 byte

    
    // // 3-> Design a handler function which will
    // //     continue to read until we reach rxLen or timeout.
    // // 4-> Configure uart parameters to start transaction
    // // 5-> Enable DMA timeout interrupt.
    return 0;
}

/******************************************************************************/
int main(void)
{
    int error, i, fail = 0;

    uint8_t TxData[BUFF_SIZE];
    uint8_t RxData[BUFF_SIZE];

    printf("\n\n**************** UART Example ******************\n");
    printf("This example sends data from one UART to another\n");
    printf("\nConnect P0.20 (RX of UART1) and P0.1 (TX of UART2).\n\n");
    printf("To indicate a successful UART transfer, LED1 will illuminate.\n");
    printf("\nPush SW2 to continue\n");

    buttonPressed = 0;
    PB_RegisterCallback(0, (pb_callback)buttonHandler);
    while (!buttonPressed) {}

    MXC_UART_TransactionDMA_Config();
    printf("Dma config completed\r\n");
    while(1){}

    printf("\nUART Baud \t: %d Hz\n", UART_BAUD);
    printf("Test Length \t: %d bytes\n\n", BUFF_SIZE);

    // Initialize the data buffers
    for (i = 0; i < BUFF_SIZE; i++) {
        TxData[i] = i;
    }
    memset(RxData, 0x0, BUFF_SIZE);

    // Initialize the UART
    error = MXC_UART_Init(TX_UART, UART_BAUD, MAP_A);
    if (error < E_NO_ERROR) {
        printf("-->Error initializing UART: %d\n", error);
        printf("-->Example Failed\n");
        return error;
    }

    error = MXC_UART_Init(RX_UART, UART_BAUD, MAP_A);
    if (error < E_NO_ERROR) {
        printf("-->Error initializing UART: %d\n", error);
        printf("-->Example Failed\n");
        return error;
    }

    // Setup request structs describing the transactions.
    // Request structs are placed in the global scope so they
    // don't go out of context.  This can happen when a req struct
    // is declared inside a function and the function completes.
    // The memory would be freed, invalidating the UART driver's
    // pointers to it.
    read_req.uart = RX_UART;
    read_req.rxData = RxData;
    read_req.rxLen = BUFF_SIZE;
    read_req.txLen = 0;
    read_req.callback = readCallback;

    write_req.uart = TX_UART;
    write_req.txData = TxData;
    write_req.txLen = BUFF_SIZE;
    write_req.rxLen = 0;
    write_req.callback = writeCallback;

    printf("-->UART Initialized\n\n");

#ifdef AUTOHANDLERS
    error = exampleDMAAutoHandlers();
#else
    error = exampleDMAManualHandlers();
#endif

    if (READ_FLAG != E_NO_ERROR) {
        printf("-->Error with read callback; %d\n", READ_FLAG);
        fail++;
    }

    if ((error = memcmp(RxData, TxData, BUFF_SIZE)) != 0) {
        printf("-->Error verifying Data: %d\n", error);
        fail++;
    } else {
        printf("-->Data verified\n");
    }

    if (fail != 0) {
        printf("\n-->Example Failed\n");
        LED_On(0); // indicates FAIL
        return E_FAIL;
    }

    LED_On(1); // indicates SUCCESS
    printf("\n-->Example Succeeded\n");
    return E_NO_ERROR;
}
