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
 * @file    main.c
 * @brief   noOs_Import Example!
 * @details This example configures the environment for no-OS driver usage in MSDK.
 */

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include "mxc_device.h"
#include "led.h"
#include "board.h"
#include "adt7420.h"
#include "maxim_i2c.h"

// // Mock interface initialization parameters
// struct no_os_spi_init_param spi_init_param = {
//     .chip_select = 0,          // Chip select pin
//     .max_speed_hz = 1000000,   // SPI clock speed (1 MHz)
//     .mode = NO_OS_SPI_MODE_0,  // SPI mode
// };

// struct no_os_i2c_init_param i2c_init_param = {
//     .device_id = 1,            // I2C bus number
//     .slave_address = 0x48,     // I2C address of ADT7420
// };


// *****************************************************************************
int main(void)
{
    struct adt7420_init_param init_param = {
        .active_device = ID_ADT7420,  // Specify the ADT7420 device
        .interface_init.i2c_init = {
            .device_id = 0,           // I2C bus ID (adjust as per your hardware)
            .slave_address = 0x48,    // I2C slave address of the ADT7420
            .max_speed_hz = 400000,   // I2C bus speed in Hz
            .platform_ops = &max_i2c_ops,     // Platform-specific operations (if any)
            .extra = MXC_GPIO_VSSEL_VDDIOH,
        },
        .resolution_setting = 1,      // Use 16-bit resolution (1 for 16-bit, 0 for 13-bit)
    };

    struct adt7420_dev *adt7420_device;
    int32_t status;

    status = adt7420_init(&adt7420_device, init_param);
    if (status < 0) {
        printf("Failed to initialize ADT7420 (Error: %ld)\n", status);
        return -1;
    }
    printf("ADT7420 successfully initialized.\n");

    no_os_mdelay(1000);    
    return 0;


}
