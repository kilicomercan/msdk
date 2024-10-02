from BleConnDevice import BleConnDevice
from PyQt5.QtWidgets import QTableWidgetItem
from PyQt5.QtGui import QColor
from BleGui import BleGui
import threading
from BleFile import BleFile

class BleApp():
    def __init__(self,motion_id = None):
        self.__motion_id = motion_id
        if motion_id != None:
            self.recording = False
            self.__recording_lock = threading.Lock()
            self.ble_file = BleFile(motion_id)
        else:
            self.ble_file = None
        self.gui = BleGui()
        self.ble = BleConnDevice(self.gui.ui, self.ble_file)
        self.scan_thread = 0
        self.connectionThread = None
        self.selectedDevice = None
        self.isConnected = False
        self.state = None
        self.inference = False
        self.stream_enabled = False
        self.setup_ble_gui()

        """Initialize BLE dongle at this point"""
        self.ble.init()
        
        """Show GUI Window"""
        self.gui.start_gui()
    
    def __execute_parallel(self, function_cb, thread_name, join = False, parameter = None):
        self.connectionThread = threading.Thread(target=function_cb, name=thread_name)
        self.connectionThread.start()
        if join:
            self.connectionThread.join()

    def __ble_app_btn_scan_clicked_event(self):
        print("Scan clicked")
        if True == self.ble.is_connected():
            print("Disconnect first.")
            return

        if False == self.ble.is_scanning():
            self.gui.ui.pushButton_Scan.setStyleSheet("background-color: Green")
            self.__execute_parallel(self.ble.action_scan_start,"Th_ScanStart", parameter={5})
        else:
            self.gui.ui.pushButton_Scan.setStyleSheet("background-color: None")
            self.__execute_parallel(self.ble.action_scan_stop,"Th_ScanStop", True)                    
            self.gui.ui.BleDevicelistWidget.clear()

    def __ble_app_current_item_set(self):
        self.selectedDevice = self.gui.ui.BleDevicelistWidget.currentItem()
        if self.selectedDevice != None:
            print("Choosen device:", self.selectedDevice.text())
            parts = self.selectedDevice.text().split(' ', 1)
            self.ble.current_addr = parts[0]
            self.ble.current_name = parts[1]
            
    def __ble_app_btn_connect_clicked_event(self):
        print("Connect btn clicked")
        
        if not self.ble.is_connected() and self.ble.current_addr != None and self.ble.current_name != None:
            # Stop scanning first
            if self.ble.is_scanning():
                print("stopping scan first")
                self.__ble_app_btn_scan_clicked_event()
                while self.ble.is_scanning():
                    pass 
            self.ble.connection_addr = self.ble.current_addr
            self.ble.connection_name = self.ble.current_name
            self.__execute_parallel(self.ble.action_connection_connect, "Th_Connect")
        else:
            print('Invalid connection situation.')
    
    def __ble_app_btn_disconnect_clicked_event(self):
        if not self.ble.is_connected():
            print("Not connected")
        else:
            # Stop streaming first.
            if self.stream_enabled:
                self.ble.action_stream_stop()
            self.__execute_parallel(self.ble.action_connection_disconnect, "Th_Disconnect")
            self.ble.connection_addr = None
            self.ble.connection_name = None
            self.gui.ui.pushButton_Connect.setStyleSheet("background-color: None")

    def __ble_app_btn_stream_clicked_event(self):
        """Get services first"""
        if self.ble.is_connected() :
            if self.stream_enabled == False:
                self.__execute_parallel(function_cb=self.ble.action_stream_start, thread_name="Th_Stream_Start")
                self.stream_enabled = True
                print("Stream Enabled!")
                self.gui.ui.pushButton_Stream.setStyleSheet("background-color: Green")
            else:
                self.stream_enabled = False
                self.__execute_parallel(function_cb = self.ble.action_stream_stop, thread_name = "Th_Stream_Stop")
                print("Stream Disabled!")   
                self.gui.ui.pushButton_Stream.setStyleSheet("background-color: None") 
        else:
            print("Not connected!")

    def __ble_app_btn_save_clicked_event(self):
        if self.ble_file != None:
            with self.__recording_lock:
                self.recording = not self.recording

    def __ble_app_btn_inference_clicked_event(self):
        self.inference != self.inference

    def setup_ble_gui(self):
        """Push button signal connection configuration"""
        self.gui.ui.pushButton_Scan.clicked.connect(self.__ble_app_btn_scan_clicked_event)
        self.gui.ui.pushButton_Connect.clicked.connect(self.__ble_app_btn_connect_clicked_event)
        self.gui.ui.pushButton_Disconnect.clicked.connect(self.__ble_app_btn_disconnect_clicked_event)
        self.gui.ui.pushButton_Stream.clicked.connect(self.__ble_app_btn_stream_clicked_event)
        self.gui.ui.pushButton_Save.clicked.connect(self.__ble_app_btn_save_clicked_event)
        self.gui.ui.pushButton_Inference.clicked.connect(self.__ble_app_btn_inference_clicked_event)

        """QListWidget Signal connection configuration"""
        self.gui.ui.BleDevicelistWidget.currentItemChanged.connect(self.__ble_app_current_item_set)

import time
import sys
import signal

def signal_handler(sig, frame):
    print('You pressed Ctrl+C!')
    ble_app.dongle.at_gapdisconnectall()
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

is_training = None
while True:
    is_training = input("Training or Not!(Y/N)?")
    if is_training == 'y' or is_training == 'Y' or is_training == 'N' or is_training == 'n':
        break
    else:
        print("Invalid Input!")

motion_id = None
if is_training == 'Y' or is_training == 'y':
    while True:
        motion_id = input("Motion Flag! (\'L\', \'R\', \'D\',\'U\')")
        if motion_id == 'L' or motion_id =='R' or motion_id=='D' or motion_id=='U':
            break
        else:
            print("Invalid Motion Training")

ble_app = BleApp(motion_id)

# def print_response(response):
#     print(response.Cmd)
#     print(response.Ack)
#     print(response.Rsp)
#     print(response.End)

# ble_app = BleConnDevice()
# ble_app.init()

# print('Central')
# ble_app.dongle.at_central()
# time.sleep(0.5)
# print('Scan')
# ble_app.dongle.at_gapscan()
# time.sleep(3)

# """Stopping scan"""
# response = ble_app.dongle.stop_scan()
# time.sleep(2)
# print_response(response)

# response = ble_app.dongle.at_gapdisconnectall()
# time.sleep(0.5)
# print_response(response)

# """Connection"""
# # Hide ascii values from notification/indication/read responses.
# response = ble_app.dongle.ata(True)
# time.sleep(0.5)
# print_response(response)

# # Disable automatic discovery of services after connection.
# response = ble_app.dongle.atds(False)
# time.sleep(0.5)
# print_response(response)

# print(ble_app.connection_addr)
# ble_app.dongle.at_gapconnect(ble_app.connection_addr)
# time.sleep(3)
# print_response(response)

# response = ble_app.dongle.at_get_servicesonly()
# print_response(response)
# time.sleep(5)

# # response = ble_app.dongle.at_get_service_details("1500")
# # print_response(response)
# # time.sleep(5)
# response = ble_app.dongle.ata(True)
# time.sleep(2)
# print_response(response)

# repsonse = ble_app.dongle.at_set_noti("1502")
# time.sleep(2)
# print_response(response)

# response = ble_app.dongle.at_noti()
# time.sleep(2)
# print_response(response)


# counter = 0
# notifi = True
# while(1):
#     # ble_app.dongle.at_gattcread("1500")
#     time.sleep(1)
#     counter = counter + 1
#     if(counter == 40):
#         if notifi == True:
#             print("Disable notifications")
#             ble_app.dongle.at_clearnoti("1502")
#             notifi = False
#         else:
#             print("enable notification again")
#             notifi = True
#             ble_app.dongle.at_clearnoti("1502")
#         counter = 0

# while(1):
#     response = ble_app.dongle.at_gattcread('2a53')
#     print_response(response)
#     time.sleep(1)

# print('Exit')


