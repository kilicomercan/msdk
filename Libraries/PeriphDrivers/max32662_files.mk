###############################################################################
 #
 # Copyright (C) 2022-2023 Maxim Integrated Products, Inc. (now owned by
 # Analog Devices, Inc.),
 # Copyright (C) 2023-2024 Analog Devices, Inc.
 #
 # Licensed under the Apache License, Version 2.0 (the "License");
 # you may not use this file except in compliance with the License.
 # You may obtain a copy of the License at
 #
 #     http://www.apache.org/licenses/LICENSE-2.0
 #
 # Unless required by applicable law or agreed to in writing, software
 # distributed under the License is distributed on an "AS IS" BASIS,
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 # See the License for the specific language governing permissions and
 # limitations under the License.
 #
 ##############################################################################

# This is the name of the build output file

ifeq "$(TARGET)" ""
$(error TARGET must be specified)
endif

TARGET_UC ?= $(subst m,M,$(subst a,A,$(subst x,X,$(TARGET))))
TARGET_LC ?= $(subst M,m,$(subst A,a,$(subst X,x,$(TARGET))))

ifeq "$(COMPILER)" ""
$(error COMPILER must be specified)
endif

# This is the path to the CMSIS root directory
ifeq "$(CMSIS_ROOT)" ""
CMSIS_ROOT=../CMSIS
endif
ifeq "$(LIBS_DIR)" ""
LIBS_DIR = $(CMSIS_ROOT)/..
endif

PERIPH_DIR := $(LIBS_DIR)/PeriphDrivers
SOURCE_DIR := $(PERIPH_DIR)/Source
INCLUDE_DIR := $(PERIPH_DIR)/Include

PERIPH_DRIVER_INCLUDE_DIR  += $(INCLUDE_DIR)/$(TARGET_UC)/

# Expose a "PINS_FILE" option for easily overriding the pin definitions
PINS_FILE ?= $(SOURCE_DIR)/SYS/pins_me12.c

# Source files
PERIPH_DRIVER_C_FILES += $(PINS_FILE)
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/SYS/sys_me12.c

PERIPH_DRIVER_INCLUDE_DIR += $(SOURCE_DIR)/ADC
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/ADC/adc_me12.c
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/ADC/adc_revb.c

PERIPH_DRIVER_INCLUDE_DIR += $(SOURCE_DIR)/AES
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/AES/aes_me12.c
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/AES/aes_revb.c

PERIPH_DRIVER_INCLUDE_DIR += $(SOURCE_DIR)/CAN
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/CAN/can_me12.c
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/CAN/can_reva.c

PERIPH_DRIVER_INCLUDE_DIR += $(SOURCE_DIR)/DMA
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/DMA/dma_me12.c
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/DMA/dma_reva.c

PERIPH_DRIVER_INCLUDE_DIR += $(SOURCE_DIR)/FLC
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/FLC/flc_common.c
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/FLC/flc_me12.c
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/FLC/flc_reva.c

PERIPH_DRIVER_INCLUDE_DIR += $(SOURCE_DIR)/GPIO
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/GPIO/gpio_common.c
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/GPIO/gpio_me12.c
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/GPIO/gpio_reva.c

PERIPH_DRIVER_INCLUDE_DIR += $(SOURCE_DIR)/I2C
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/I2C/i2c_me12.c
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/I2C/i2c_reva.c

PERIPH_DRIVER_INCLUDE_DIR += $(SOURCE_DIR)/I2S
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/I2S/i2s_me12.c
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/I2S/i2s_reva.c

PERIPH_DRIVER_INCLUDE_DIR += $(SOURCE_DIR)/ICC
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/ICC/icc_common.c
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/ICC/icc_me12.c
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/ICC/icc_reva.c

PERIPH_DRIVER_INCLUDE_DIR += $(SOURCE_DIR)/LP
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/LP/lp_me12.c

PERIPH_DRIVER_INCLUDE_DIR += $(SOURCE_DIR)/PT
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/PT/pt_me12.c
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/PT/pt_reva.c

PERIPH_DRIVER_INCLUDE_DIR += $(SOURCE_DIR)/RTC
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/RTC/rtc_me12.c
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/RTC/rtc_reva.c

PERIPH_DRIVER_INCLUDE_DIR += $(SOURCE_DIR)/SPI
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/SPI/spi_me12.c
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/SPI/spi_reva1.c

PERIPH_DRIVER_INCLUDE_DIR += $(SOURCE_DIR)/TMR
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/TMR/tmr_common.c
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/TMR/tmr_me12.c
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/TMR/tmr_revb.c

PERIPH_DRIVER_INCLUDE_DIR += $(SOURCE_DIR)/TRNG
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/TRNG/trng_me12.c
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/TRNG/trng_revb.c

PERIPH_DRIVER_INCLUDE_DIR += $(SOURCE_DIR)/UART
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/UART/uart_common.c
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/UART/uart_me12.c
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/UART/uart_revb.c

PERIPH_DRIVER_INCLUDE_DIR += $(SOURCE_DIR)/WDT
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/WDT/wdt_common.c
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/WDT/wdt_me12.c
PERIPH_DRIVER_C_FILES += $(SOURCE_DIR)/WDT/wdt_revb.c
