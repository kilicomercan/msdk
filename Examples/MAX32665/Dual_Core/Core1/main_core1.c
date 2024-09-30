/******************************************************************************
 *
 * Copyright (C) 2022-2023 Maxim Integrated Products, Inc. (now owned by 
 * Analog Devices, Inc.),
 * Copyright (C) 2023-2024 Analog Devices, Inc.
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
 * @file    main_core1.c
 * @brief   The main application for Core 1.
 * @details Core 1 synchronizes with Core 0.
 */

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include "mxc_device.h"
#include "mxc_delay.h"
#include "led.h"
#include "board.h"
#include "sema.h"
#include "tmr.h"
#include "adxl363_test.h"
#include "sensor_fw.h"
#include "helper.h"
#include "rtc.h"
#include "flc.h"

/***** Definitions *****/
#define TRAINING_ENABLE 1
#define TRAINING_PORT MXC_GPIO0
#define TRAINING_PIN  MXC_GPIO_PIN_12

extern int count0;
extern int count1;

/***** Macros *****/

/***** Globals *****/

/***** Functions *****/
#if TRAINING_ENABLE
static int __init_buttons(void){
    mxc_gpio_cfg_t training_pin = {0};
    training_pin.port = TRAINING_PORT;
    training_pin.mask = TRAINING_PIN;
    training_pin.pad = MXC_GPIO_PAD_PULL_UP;
    training_pin.func = MXC_GPIO_FUNC_IN;
    training_pin.vssel = MXC_GPIO_VSSEL_VDDIO;
    training_pin.drvstr = MXC_GPIO_DRVSTR_0;

    return MXC_GPIO_Config(&training_pin);
}
#endif // TRAINING

// static int counter_test = 0;
void fw_loop(void){
    int read_set_count = 0;
    MXC_RTC_Init(0,0);
    MXC_RTC_Start();
    uint8_t data_set[SENSOR_SET_LENGTH] = {0};
    int delay = (1000/SHARED_SENSOR_ODR);
    while (1) {

        // Clean local buffers in stack.
        memset(data_set, 0, sizeof(data_set));
        // Reading sample set from sensor without temperature value.
        if(E_NO_ERROR == adxl363_fifo_read_sample_set(&data_set[1+TIMESTAMP_LENGHT], FALSE)){
                #if TRAINING_ENABLE 
                if(MXC_GPIO_InGet(TRAINING_PORT, TRAINING_PIN)){
                    data_set[0] = 1;
                }else{
                    data_set[0] = 0;
                }
                #else
                    data_set[0] = 0;
                #endif
                
                // uint32_t* sec = (uint32_t*)&data_set[1];
                // uint32_t* sub_sec = (uint32_t*)&data_set[5];
                #if TIMESTAMP_LENGHT != 0
                while(E_NO_ERROR != MXC_RTC_GetTime((uint32_t*)&data_set[1], (uint32_t*)&data_set[5])){}
                #endif
                
                // printf("%d. %d\r\n",*sec, *sub_sec);
                while(MXC_SEMA_GetSema(PACK_READY_SEM_ID));
                memcpy(&sensor_pack_buffer[read_set_count*(SENSOR_SET_LENGTH)], data_set, sizeof(data_set));
                ready_flag += 1;
                MXC_SEMA_FreeSema(PACK_READY_SEM_ID);
                /* Increase the length of the whole packet from device to host */
                read_set_count+=1;
                read_set_count %= SHARED_SENSOR_ODR;
                // uint16_t* ptr = &data_set[1];
                // for(;counter_test < 3;counter_test++){
                //     printf("%d ", *ptr);
                //     ptr++;
                // }
                // counter_test = 0;
        }else{
            printf("Set read fail\r\n");
        }
        MXC_Delay(MXC_DELAY_MSEC(delay));
    }
}

// *****************************************************************************
// main_core1 is Core 1's official main function that is called at program startup.
int main_core1(void)
{
    printf("Core 1: enter while loop.\n");
#if TRAINING_ENABLE
    if(E_NO_ERROR != __init_buttons()){
        printf("Button initialization error\r\n");
        return -1;
    }else{
        printf("Button initialization SUCCESS\r\n");
    }
#endif
    if(E_NO_ERROR != init_sensor()){
        printf("ADXL_363 sensor initialization error\r\n");
        return -1;
    }else{
        printf("Sensor init SUCCESS\r\n");
    }
    fw_loop();
    
    return 0;
}
