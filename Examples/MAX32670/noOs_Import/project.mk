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

#BOARD=FTHR_RevA
# ^ For example, you can uncomment this line to make the 
# project build for the "FTHR_RevA" board.

# **********************************************************

# Add your config here!
dev_type = adc-dac
dev_name = ad74416h
noOs_src_api_dir = $(MAXIM_PATH)/Libraries/noOS/src

# Add header libraries
IPATH += $(MAXIM_PATH)/Libraries/noOS/include
IPATH += $(MAXIM_PATH)/Libraries/noOS/maxim/$(TARGET)
IPATH += $(MAXIM_PATH)/Libraries/noOS/maxim/common

# Add device driver files to compilation.
IPATH += $(MAXIM_PATH)/Libraries/MiscDrivers/no_os/$(dev_type)/$(dev_name)
VPATH += $(MAXIM_PATH)/Libraries/MiscDrivers/no_os/$(dev_type)/$(dev_name)

# Adding all no_os wrapper functions to compilation process.
SRCS += $(wildcard $(noOs_src_api_dir)/*.c)
SRCS += $(wildcard $(noOs_src_api_dir)/api/*.c)

# Adding maxim_xxx apis to compilation.
SRCS += $(wildcard $(MAXIM_PATH)/Libraries/noOS/maxim/$(TARGET)/*.c)
SRC += $(MAXIM_PATH)/Libraries/noOS/maxim/common/maxim_dma.c
