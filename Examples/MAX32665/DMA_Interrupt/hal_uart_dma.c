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
#include "hal_uart_dma.h"

#define UART_BAUD 115200
#define TEST_UART MXC_UART0

#define DEST_LEN 255

typedef struct{
	uint8_t* ptxBuf;
	int32_t  txLength;
	int32_t  txBusyFlag;
	uint8_t  rxBuf[DEST_LEN];
	int32_t  rxLength;
	mxc_uart_callback_fn txCallback;
	mxc_uart_callback_fn rxCallback;
}uart_op_ctrl_t;

static uart_op_ctrl_t uart_ctrl;

volatile int rxChannel = -1;
volatile int txChannel = -1;


void DMA_TxChIRQHandler(void)
{
    int flags = MXC_DMA_ChannelGetFlags(txChannel);
    if(E_BAD_PARAM != flags)
    {
        MXC_DMA_ChannelClearFlags(txChannel, flags);
    }
    else
    {
    	MXC_DMA_ChannelClearFlags(txChannel, 0xff);
    }

    if (uart_ctrl.txCallback != NULL)
    	uart_ctrl.txCallback(uart_ctrl.ptxBuf, uart_ctrl.txLength);

    uart_ctrl.txBusyFlag = 0;

}

void DMA_RxChIRQHandler(void)
{
	MXC_DMA_Stop(rxChannel);
    mxc_dma_ch_regs_t *dma_rx_channel = MXC_DMA_GetCHRegs(rxChannel);
    int flags = MXC_DMA_ChannelGetFlags(rxChannel);
    if(E_BAD_PARAM != flags){
        MXC_DMA_ChannelClearFlags(rxChannel, flags);
    }else{
    	MXC_DMA_ChannelClearFlags(rxChannel, flags);
    }

    // Timeout interrupt.
    if(flags & MXC_F_DMA_ST_TO_ST)
    {
    	uart_ctrl.rxLength = DEST_LEN - dma_rx_channel->cnt;// MXC_DMA_GetChCnt(rxChannel);
        if (uart_ctrl.rxCallback != NULL)
        	uart_ctrl.rxCallback(uart_ctrl.rxBuf, uart_ctrl.rxLength);
    }

	//config for next round
	MXC_DMA->ch[rxChannel].dst = (uint32_t)&uart_ctrl.rxBuf[0];
	MXC_DMA->ch[rxChannel].cnt = DEST_LEN;
	MXC_DMA_Start(rxChannel);

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
    MXC_DMA_ChannelEnableInt(conf.ch, (MXC_F_DMA_CFG_CTZIEN | MXC_F_DMA_CFG_CHDIEN));

    return 0;
}

static int Hal_UartDMAInit(void)
{
	memset(&uart_ctrl, 0, sizeof(uart_op_ctrl_t));

    mxc_dma_regs_t *const operating_dma = MXC_DMA0;
    MXC_DMA_Init(operating_dma);

    /** @brief RX channel configurations */
    rxChannel = MXC_DMA_AcquireChannel(operating_dma);
    if (0 > rxChannel) {
        return E_BAD_PARAM;
    }
    mxc_dma_srcdst_t rxSrcdst = {.len = DEST_LEN, .dest = uart_ctrl.rxBuf, .source = NULL, .ch = rxChannel};
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
                                         .pssel = MXC_DMA_PRESCALE_DIV256,
                                         .tosel = MXC_DMA_TIMEOUT_128_CLK,
                                         .reqwait_en = 1,
                                         .ch = rxChannel};
    
    /** @brief TX channel configurations */
    txChannel = MXC_DMA_AcquireChannel(operating_dma);
    if (0 > rxChannel) {
        return E_BAD_PARAM;
    }

    mxc_dma_srcdst_t txSrcdst = {.dest = NULL, .ch = txChannel};
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

    // UART peripheral configurations for dma usage in uart transaction.
    MXC_UART_SetRXDMAChannel(TEST_UART, rxChannel);
    MXC_UART_SetTXDMAChannel(TEST_UART, txChannel);

    //start rx channel
    MXC_DMA_Start(rxChannel);
    //tx channel is kicked off in Hal_UartSendAsync();

    return E_NO_ERROR;
}

int Hal_UartInit(void)
{
    // Initialization of UART0 and its pins.
    MXC_UART_Init(TEST_UART, UART_BAUD, MAP_A);

    if (Hal_UartDMAInit() != 0)
    {
    	return E_BAD_PARAM;
    }

    TEST_UART->dma |= ( 1 << MXC_F_UART_DMA_RXDMA_LEVEL_POS);// Setting dma RX threshold 
    TEST_UART->dma |= ( 2 << MXC_F_UART_DMA_TXDMA_LEVEL_POS); // Setting dma TX threshold 

    TEST_UART->dma |= MXC_F_UART_DMA_RXDMA_EN;  // Enabling uart dma for rx. Data receive starts from here.
    TEST_UART->dma |= MXC_F_UART_DMA_TXDMA_EN;  // Enabling uart dma for tx. Data transmission starts after this line.

    return E_NO_ERROR;
}

void Hal_UartRegisterCallback(mxc_uart_callback_fn txCallback, mxc_uart_callback_fn rxCallback)
{
	uart_ctrl.txCallback = txCallback;
	uart_ctrl.rxCallback = rxCallback;
}


void Hal_UartSendAsync(uint8_t *data, uint32_t len)
{
	int32_t waitCnt = 0xFFFF;
	while ((uart_ctrl.txBusyFlag) && (waitCnt--)){}

	uart_ctrl.txBusyFlag = 1;
	uart_ctrl.ptxBuf = data;
	uart_ctrl.txLength = len;

	MXC_DMA->ch[txChannel].src = (uint32_t)uart_ctrl.ptxBuf;
	MXC_DMA->ch[txChannel].cnt = len;

	MXC_DMA_Start(txChannel);
}

