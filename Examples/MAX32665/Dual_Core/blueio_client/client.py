# (C) 2023 Smart Sensor Devices AB

import time
from datetime import datetime
from bleuio_lib.bleuio_funcs import BleuIO

# Description

# This is an example script that showcase how to get started with the BleuIO library for Python.

# It will show how to setup callback functions for scan results and events.

# How to send a command and what responses you get and how you can handle them.

# How to start and stop a scan.

# How to start and stop advertising.

# How to check the BLE Status of the dongle.

# Creating a callback function for scan results. For this example we just prints out the result.

# Here you can add your code to parse the data.

def my_scan_callback(scan_input):
    print("\n\nmy_evt_callback: " + str(scan_input))

# Creating a callback function for events. For this example we add a timestamp and just prints out the event.

# Here you can add your code to parse the data.

def my_evt_callback(evt_input):
    cbTime = datetime.now()
    currentTime = cbTime.strftime("%H:%M:%S")
    print("\n\n[" + str(currentTime) + "] my_evt_callback: " + str(evt_input))

# Start

# Initiates the dongle. If port param is left as 'auto' it will auto-detect if BleuIO dongle is connected.

# :param port: str

# :param baud: int

# :param timeout: int

# :param debug: bool

# Auto-Detect dongle

my_dongle = BleuIO()

# Specific COM port (Win) 'COMX'

# my_dongle = BleuIo(port='COM7')

# Specific COM port (Linux) 'dev/tty.xxxxx...'

# my_dongle = BleuIo(port='/dev/tty.123546877')

# Specific COM port (Mac) 'dev/cu.xxxx...'

# my_dongle = BleuIo(port='/dev/cu.123546877')

# Registers the callback functions we created earlier.

my_dongle.register_evt_cb(my_evt_callback)
my_dongle.register_scan_cb(my_scan_callback)

print("Welcome to Test BleuIO Python Library!\n\n")

# Here we send a simple AT Command. All commands will return a BleuIORESP obj.

# The object have 4 attributes:

# Cmd: Contains the command data.

# Ack: Contains the acknowledge data.

# Rsp: Contains the response data.

# End: Contains the end data.

at_exemple = my_dongle.at()

# The attributes are in JSON format. Here we we print the different attributes.

print(at_exemple.Cmd)
print(at_exemple.Ack)
print(at_exemple.Rsp) # Not every ccommand has a Response message, AT for example doesn't so this will return an empty list
print(at_exemple.End)

print("\n--\n")

# We can try with the ATI command, it has information in the Response message.
# An AT command can have several response messages so it will return a list of JSON objects
# ati_exemple = my_dongle.ati()
# print(ati_exemple.Cmd)
# print(ati_exemple.Ack)
# print(ati_exemple.Rsp)
# print(ati_exemple.End)

# print("\n--\n")

# # If we only want to see if the was successful we can do like this:

# print("Err: " + str(ati_exemple.Ack["err"]))

# # or

# print("errMsg: " + str(ati_exemple.Ack["errMsg"]))

""" 
Here is an example on how to scan.
 """
# First we need to put the dongle in Central or Dual Gap Role
# my_dongle.at_dual()
my_dongle.at_central()

# Now we start scanning
resp = my_dongle.at_gapscan(10)
print(resp.Cmd)
print(resp.Ack)
print(resp.Rsp)
print(resp.End)

while 1:
    pass
# # We can either send in a timeout as a parameter for the at_gapscan() command or stop the scan when we're done.

# # Here we just set a three second sleep then stop scan.

# # Notice that all the scan data will be printed by our my_scan_callback() function.

# time.sleep(3)
# print("stop scan")
# my_dongle.stop_scan()

# print("\n--\n")

# # The BLEStatus class can help you keep track of if you are currently advertising for example.

# # """A class used to handle BLE Statuses

# # :attr isScanning: Keeps track on if dongle is currently scanning.

# # :attr isConnected: Keeps track on if dongle is currently connected.

# # :attr isAdvertising: Keeps track on if dongle is currently advertising.

# # :attr isSPSStreamOn: Keeps track on if dongle is currently in SPS stream mode.

# # :attr role: Keeps track of the dongle's current GAP Role.

# # """

# print("isScanning: " + str(my_dongle.status.isScanning))
# print("isConnected: " + str(my_dongle.status.isConnected))
# print("isAdvertising: " + str(my_dongle.status.isAdvertising))
# print("isSPSStreamOn: " + str(my_dongle.status.isSPSStreamOn))
# print("role: " + str(my_dongle.status.role))

# print("\n--\n")

# # If we start advertising and check isAdvertising we will see that is changes to True.

# resp = my_dongle.at_advstart()
# print(resp.Cmd)
# print(resp.Ack)
# print(resp.Rsp)
# print(resp.End)

# print("\nisAdvertising: " + str(my_dongle.status.isAdvertising))

# print("\n--\n")

# # Here we stop the advertising.

# resp = my_dongle.at_advstop()
# print(resp.Cmd)
# print(resp.Ack)
# print(resp.Rsp)
# print(resp.End)

# print("\nisAdvertising: " + str(my_dongle.status.isAdvertising))