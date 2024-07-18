# This file can be used to set build configuration
# variables.  These variables are defined in a file called 
# "Makefile" that is located next to this one.

# For instructions on how to use this system, see
# https://analogdevicesinc.github.io/msdk/USERGUIDE/#build-system

#MXC_OPTIMIZE_CFLAGS = -Og
# ^ For example, you can uncomment this line to 
# optimize the project for debugging

# **********************************************************

# Add your config here!
# Uncomment the line below to build for the MAX78000FTHR
# override BOARD=FTHR_RevA

ifeq "$(BOARD)" "FTHR_RevA"
# Determine ADAFRUIT 2.4'' Display version
ADAFRUIT_DISPLAY_VER = 2
SRCS+=adafruit_3315_tft.c

ifeq ($(ADAFRUIT_DISPLAY_VER),2)
SRCS+=tsc2007.c
endif

ifeq ($(ADAFRUIT_DISPLAY_VER),1)
SRCS+=adafruit_3315_touch.c
endif
endif # BOARD == FTHR_RevA

IPATH += resources

# Place build files specific to EvKit_V1 here.
ifeq "$(BOARD)" "EvKit_V1"
VPATH += resources/tft_evkit
endif

# Place build files specific to FTHR_RevA here.
ifeq "$(BOARD)" "FTHR_RevA"
VPATH += resources/tft_fthr
FONTS = LiberationSans12x12 LiberationSans16x16 LiberationSans19x19 LiberationSans24x24 LiberationSans28x28
endif


ifeq ($(BOARD),Aud01_RevA)
$(error ERR_NOTSUPPORTED: This project is not supported for the Audio board)
endif

ifeq ($(BOARD),CAM01_RevA)
$(error ERR_NOTSUPPORTED: This project is not supported for the CAM01 board)
endif

ifeq ($(BOARD),CAM02_RevA)
$(error ERR_NOTSUPPORTED: This project is not supported for the CAM02 board)
endif


