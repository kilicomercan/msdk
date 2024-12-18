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

#define UART_BAUD 115200
#define TEST_UART MXC_UART0

volatile int rxChannel = -1;
volatile int txChannel = -1;
volatile int TxFlag = 0;
volatile int RxFlag = 0;
volatile int buttonPressed = 0;

void buttonHandler(void){
    buttonPressed = 1;
}

void DMA_TxChIRQHandler(void){
    int flags = MXC_DMA_ChannelGetFlags(txChannel);
    if(E_BAD_PARAM == flags){
        MXC_DMA_ChannelClearFlags(txChannel, 0xff);
    }

    // Count to Zero interrupt.
    if(flags & MXC_F_DMA_ST_CTZ_ST){
        /* @todo tx completed */
        TxFlag = 1;
    }

    // Timeout interrupt
    if(flags & MXC_F_DMA_ST_TO_ST){
        printf("Tx timeout\r\n");
    }

    MXC_DMA_ChannelClearFlags(txChannel, flags);
}

void DMA_RxChIRQHandler(void){
    int flags = MXC_DMA_ChannelGetFlags(rxChannel);
    if(E_BAD_PARAM == flags){
        MXC_DMA_ChannelClearFlags(rxChannel, 0xff);
    }

    // Count to Zero interrupt
    if(flags & MXC_F_DMA_ST_CTZ_ST){
        // @todo stop the transaction. rxChannel will be disabled.
        RxFlag = 1;
    }
    // Timeout interrupt.
    if(flags & MXC_F_DMA_ST_TO_ST){
        printf("Rx timeout\r\n");
    }
    MXC_DMA_ChannelClearFlags(rxChannel, flags);
}

static int MXC_DMA_ConfChannelWithTimeout(mxc_dma_config_t conf,
                                         mxc_dma_adv_config_t adv_conf,
                                         mxc_dma_srcdst_t src_dst,
                                         void (*irq_handler)(void))
{
    // mxc_dma_ch_regs_t *regs = MXC_DMA_GetCHRegs(conf.ch);
    MXC_DMA_ChannelDisableInt(conf.ch, (MXC_F_DMA_CFG_CTZIEN | MXC_F_DMA_CFG_CHDIEN));
    MXC_DMA_ChannelClearFlags(conf.ch, (MXC_F_DMA_ST_CTZ_ST | MXC_F_DMA_ST_RLD_ST |
                                          MXC_F_DMA_ST_BUS_ERR | MXC_F_DMA_ST_TO_ST));
    MXC_DMA_ConfigChannel(conf, src_dst);

    MXC_DMA_AdvConfigChannel(adv_conf);

    NVIC_EnableIRQ(MXC_DMA_CH_GET_IRQ(conf.ch));
    MXC_NVIC_SetVector(MXC_DMA_CH_GET_IRQ(conf.ch), irq_handler);

    MXC_DMA_EnableInt(conf.ch);

    MXC_DMA_Start(conf.ch);
    MXC_DMA_ChannelEnableInt(conf.ch, (MXC_F_DMA_CFG_CTZIEN | MXC_F_DMA_CFG_CHDIEN));

    return 0;
}

#define SRC_LEN 100
#define DEST_LEN 255
static uint8_t destAddr[DEST_LEN] = {0};
static uint8_t srcAddr[SRC_LEN] = {0};

/******************************************************************************/
int main(void)
{
    printf("\n\n**************** UART Example ******************\n");
    printf("\nPush SW2 to continue\n");

    buttonPressed = 0;
    PB_RegisterCallback(0, (pb_callback)buttonHandler);
    while (!buttonPressed) {}

    memset(srcAddr, 0xfb, SRC_LEN);


    mxc_dma_regs_t *const operating_dma = MXC_DMA0;
    MXC_DMA_Init(operating_dma);

    /** @brief RX channel configurations */
    rxChannel = MXC_DMA_AcquireChannel(operating_dma);
    if (0 > rxChannel) {
        return -1;
    }
    mxc_dma_srcdst_t rxSrcdst = {.len = DEST_LEN, .dest = destAddr, .source = NULL, .ch = rxChannel};
    mxc_dma_config_t confRx = { .ch = rxChannel,
                                .dstinc_en = 1,
                                .srcinc_en = 0,
                                .dstwd = MXC_DMA_WIDTH_BYTE,
                                .srcwd = MXC_DMA_WIDTH_BYTE,
                                .reqsel = MXC_DMA_REQUEST_UART0RX};
    /**
     * rx timeout 
     */
    mxc_dma_adv_config_t rx_adv_conf = { .burst_size = 1,
                                         .prio = MXC_DMA_PRIO_HIGH,
                                         .pssel = MXC_DMA_PRESCALE_DIV16M,
                                         .tosel = MXC_DMA_TIMEOUT_4_CLK,
                                         .reqwait_en = 0,
                                         .ch = rxChannel};

    
    /** @brief TX channel configurations */
    txChannel = MXC_DMA_AcquireChannel(operating_dma);
    if (0 > rxChannel) {
        return -1;
    }
    mxc_dma_srcdst_t txSrcdst = {.len = SRC_LEN, .dest = NULL, .source = srcAddr, .ch = txChannel};
    mxc_dma_config_t confTx = { .ch = txChannel,
                                .dstinc_en = 0,
                                .srcinc_en = 1,
                                .dstwd = MXC_DMA_WIDTH_BYTE,
                                .srcwd = MXC_DMA_WIDTH_BYTE,
                                .reqsel = MXC_DMA_REQUEST_UART0TX};
    mxc_dma_adv_config_t tx_adv_conf = { .burst_size = 1,
                                         .prio = MXC_DMA_PRIO_HIGH,
                                         .pssel = MXC_DMA_PRESCALE_DIV16M,
                                         .tosel = MXC_DMA_TIMEOUT_32_CLK,
                                         .reqwait_en = 0,
                                         .ch = txChannel};                                     

    // Configuration of DMA channels to be used for UART TX and RX
    MXC_DMA_ConfChannelWithTimeout(confRx, rx_adv_conf, rxSrcdst, DMA_RxChIRQHandler);
    MXC_DMA_ConfChannelWithTimeout(confTx, tx_adv_conf, txSrcdst, DMA_TxChIRQHandler);

    // Initialization of UART0 and its pins.
    MXC_UART_Init(TEST_UART, UART_BAUD, MAP_A);

    // UART peripheral configurations for dma usage in uart transaction.
    MXC_UART_SetRXDMAChannel(TEST_UART, rxChannel);
    TEST_UART->dma |= ( 1 << MXC_F_UART_DMA_RXDMA_LEVEL_POS);// Setting dma RX threshold 
    
    MXC_UART_SetTXDMAChannel(TEST_UART, txChannel);
    TEST_UART->dma |= ( 2 << MXC_F_UART_DMA_TXDMA_LEVEL_POS); // Setting dma TX threshold 

    TEST_UART->dma |= MXC_F_UART_DMA_RXDMA_EN;  // Enabling uart dma for rx. Data receive starts from here.
    TEST_UART->dma |= MXC_F_UART_DMA_TXDMA_EN;  // Enabling uart dma for tx. Data transmission starts after this line.

    printf("Dma config completed\r\n");
    while(!TxFlag){}
    printf("TX completed\r\n");
    while(!RxFlag){}
    printf("Rx Completed\r\n");
    printf("Transfer Completed\r\n");
    while(1){}

    return E_NO_ERROR;
}
