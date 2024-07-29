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
 * @brief   CLI Example!
 * @details This example uses the serial COM to call customized functions.
 */

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include "mxc_device.h"
#include "led.h"
#include "board.h"
#include "mxc_delay.h"
#include "led.h"
#include "cli.h"
#include "ctype.h"
#include "stdlib.h"
#include "string.h"
#include "uart.h"

/***** Macro Definitions *****/
#define CUSTOM_COMMANDS_ARRAY_SIZE 3
#define MAX_STRING_SIZE 256
#define CLI_UART MXC_UART1
#define PRINT_LOG(msg) printf("%s\n", msg)

/***** Command Handler Function Declarations *****/
int cli_led_toogle_handler(int argc, char *argv[]);
int cli_uppercase_handler(int argc, char *argv[]);
int cli_square_handler(int argc, char *argv[]);

/***** Globals *****/
const command_t commands[CUSTOM_COMMANDS_ARRAY_SIZE] = {
    { "CMD_LED_TOGGLE", "[CMD_LED_TOGGLE] on command line", "Changes led0 status", cli_led_toogle_handler },
    { "CMD_UPPERCASE", "[CMD_UPPERCASE] [lowercase string]",
      "Converts lower case characters to uppercase ones.", cli_uppercase_handler },
    { "CMD_SQUARE", "[CMD_SQUARE] [number to be squared]",
      "multiples the number entered with itself.", cli_square_handler }
};

static int uppercase_event = 0;
static int toggle_event = 0;
static int square_event = 0;
char uppercase_buff[MAX_STRING_SIZE] = { 0 };

/***** Command Handler Function Definitions *****/
int cli_led_toogle_handler(int argc, char *argv[])
{
    if (argc != 1) {
        toggle_event = -1;
        return -1;
    }

    toggle_event = 1;
    return 0;
}

int cli_square_handler(int argc, char *argv[])
{
    if (2 != argc) {
        square_event = -1;
        return -1;
    }
    int number = atoi(argv[1]);
    number *= number;

    square_event = number;
    return 0;
}

int cli_uppercase_handler(int argc, char *argv[])
{
    if (argc != 2) {
        uppercase_event = -1;
        return -1;
    }
    int string_length = strlen(argv[1]);
    if (MAX_STRING_SIZE < string_length) {
        uppercase_event = -1;
        return -1;
    }

    int i = 0;
    for (; i < string_length; i++) {
        if (97 <= argv[1][i] && 122 >= argv[1][i]) {
            uppercase_buff[i] = (argv[1][i]) - 32;
        } else {
            uppercase_buff[i] = argv[1][i];
        }
    }
    uppercase_event = 1;

    return 0;
}

// *****************************************************************************
int main(void)
{
    MXC_CLI_Init(CLI_UART, commands, CUSTOM_COMMANDS_ARRAY_SIZE);

    while (1) {
        /* Check toggle_event variable to execute the led toogle task. */
        if (toggle_event != 0) {
            if (toggle_event == 1) {
                PRINT_LOG("Led toggled");
                LED_Toggle(0);
            } else {
                PRINT_LOG("Led toggle failed");
            }
            toggle_event = 0;
        }

        /* Check square_event variable to send squared number to host back. */
        if (0 != square_event) {
            if (0 < square_event) {
                char string_num[sizeof(int)] = { 0 };
                sprintf(string_num, "%d", square_event);
                PRINT_LOG(string_num);
            } else {
                PRINT_LOG("Square event failed");
            }
            square_event = 0;
        }

        /* Check uppercase_event variable to send uppercased string to host. */
        if (0 != uppercase_event) {
            if (1 == uppercase_event) {
                PRINT_LOG(uppercase_buff);
            } else {
                PRINT_LOG("Upper case command failed");
            }
            memset(uppercase_buff, 0, MAX_STRING_SIZE);
            uppercase_event = 0;
        }
    };

    return 0;
}
