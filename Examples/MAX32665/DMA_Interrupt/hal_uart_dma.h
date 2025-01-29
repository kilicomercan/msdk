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

 #ifndef __HAL_UART_DMA_H__
 #define __HAL_UART_DMA_H__

 #include <stdio.h>

typedef void (*mxc_uart_callback_fn)(uint8_t* data, int32_t length);

int Hal_UartInit(void);
void Hal_UartSendAsync(uint8_t *data, uint32_t len);
void Hal_UartRegisterCallback(mxc_uart_callback_fn txCallback, mxc_uart_callback_fn rxCallback);

#endif /* __HAL_UART_DMA_H__ */
