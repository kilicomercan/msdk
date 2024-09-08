###############################################################################
 #
 # Copyright (C) 2024 Analog Devices, Inc.
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
# This file can be used to set build configuration
# variables.  These variables are defined in a file called 
# "Makefile" that is located next to this one.

# For instructions on how to use this system, see
# https://analogdevicesinc.github.io/msdk/USERGUIDE/#build-system

# **********************************************************

# Add your config here!

SENSOR_DRIVER_CORE_PATH = Core1

# BLE configurations
override BOARD = FTHR
LIB_CORDIO = 1
INIT_PERIPHERAL = 1
INIT_CENTRAL = 0
IPATH += Core0/services
VPATH += Core0/services
IPATH += Core0/profiles/mcs
VPATH += Core0/profiles/mcs

override ADV_NAME = BtConn
PROJ_CFLAGS += -DADV_NAME=\"$(ADV_NAME)\"
PROJ_CFLAGS += -DINIT_SECURITY=FALSE
TRACE = 1


# ADXL sensor configurations!
VPATH+=$(LIBS_DIR)/MiscDrivers/ACCEL/ADXL
IPATH+=$(LIBS_DIR)/MiscDrivers/ACCEL/ADXL
VPATH+=$(SENSOR_DRIVER_CORE_PATH)/HelperLibs
IPATH+=$(SENSOR_DRIVER_CORE_PATH)/HelperLibs

TEST_ENABLED=1
TEST_INTERRUPT=0

SRCS+=adxl363.c
SRCS+=helper.c

ifeq ($(TEST_ENABLED),1)
VPATH += $(SENSOR_DRIVER_CORE_PATH)/TestApps
IPATH += $(SENSOR_DRIVER_CORE_PATH)/TestApps
SRCS+= test_fifo.c #$(SENSOR_DRIVER_CORE_PATH)/TestApps/test_fifo.c
SRCS+= test_axis.c #$(SENSOR_DRIVER_CORE_PATH)/TestApps/test_axis.c
SRCS+= test_initialization.c #$(SENSOR_DRIVER_CORE_PATH)/TestApps/test_initialization.c
SRCS+= test_sample_set_read.c #$(SENSOR_DRIVER_CORE_PATH)/TestApps/test_sample_set_read.c
endif

# Configurations below this line is related with dual core running.
# Build with the necessary Core1 startup/system files.
ARM_DUALCORE=1

# Separate directories for Core 0 and Core 1 code.
VPATH += Core0
VPATH += Core1

IPATH += Core0
IPATH += Core1
