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

void fw_loop(void){
    printf("Loop start\r\n");
    int read_set_count = 0;
    uint8_t data_set[6] = {0};
    adxl363_sample_pkg_t sample_set_pack = {0};
    MXC_RTC_Init(0,0);
    MXC_RTC_Start();

    while (1) {

        // Clean local buffers in stack.
        memset((void*)data_set, 0, 6);
        memset((void*)&sample_set_pack, 0, sizeof(sample_set_pack));

        // Reading sample set from sensor without temperature value.
        if(E_NO_ERROR == adxl363_fifo_read_sample_set(data_set, FALSE)){
            
            // If ready_flag is not cleared by Core0, it may still be trying
            // to parse the data before sending to host.
            if(0 == ready_flag){
                sample_set_pack = adxl363_parse_sample_set((uint16_t*)data_set);
                MXC_RTC_GetTime(&sample_set_pack.sec, &sample_set_pack.subsec);
                
                // Wait for Core 0 to release the semaphore
                while (MXC_SEMA_GetSema(sensor_pack.sem_id) == E_BUSY) {}
                
                #if TRAINING_ENABLE 
                if(MXC_GPIO_InGet(TRAINING_PORT, TRAINING_PIN)){
                    sensor_pack.pack_list[read_set_count*(sizeof(adxl363_sample_pkg_t)+1)] = 1;
                }else{
                    sensor_pack.pack_list[read_set_count*(sizeof(adxl363_sample_pkg_t)+1)] = 0;
                }
                #else
                sensor_pack.pack_list[read_set_count*(sizeof(adxl363_sample_pkg_t)+1)] = 0;
                #endif

                memcpy(&sensor_pack.pack_list[read_set_count*(sizeof(adxl363_sample_pkg_t)+1)+1], &sample_set_pack, sizeof(sample_set_pack));

                MXC_SEMA_FreeSema(sensor_pack.sem_id);

                if(++read_set_count > 100){
                    while(MXC_SEMA_GetSema(PACK_READY_SEM_ID));
                    ready_flag = 1;
                    read_set_count = 0;
                    MXC_SEMA_FreeSema(PACK_READY_SEM_ID);
                }
                send_pack_to_host(&sample_set_pack);
            }
        }
        MXC_Delay(MXC_DELAY_MSEC(100));
        printf("count\r\n");
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
