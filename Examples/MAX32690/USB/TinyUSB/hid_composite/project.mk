# This file can be used to set build configuration
# variables.  These variables are defined in a file called
# "Makefile" that is located next to this one.

# For instructions on how to use this system, see
# https://analogdevicesinc.github.io/msdk/USERGUIDE/#build-system

# **********************************************************

# If you have secure version of MCU, set SBT=1 to generate signed binary
# For more information on how sing process works, see
# https://www.analog.com/en/education/education-library/videos/6313214207112.html
SBT=0

# Enable TINYUSB library
LIB_TINYUSB=1
TINYUSB_CONFIG_DIR = ./
