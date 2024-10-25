# (C) 2023 Smart Sensor Devices AB

import time
import json
from datetime import datetime
from MainGui import Ui_MainWindow
from PyQt5.QtWidgets import QApplication, QTableWidget, QListWidget, QPushButton, QVBoxLayout, QWidget
from bleuio_lib.bleuio_funcs import BleuIO
from BleParser import BleParser
import threading
from BleBuff import BleBuff
from BleFile import BleFile
class BleConnDevice(BleParser):
    def __init__(self, ui:Ui_MainWindow = None, file:BleFile = None, buff:list[BleBuff]= None):
        self.ble_file = file
        self.connection_response = None
        self.connection_name = None
        self.connection_addr = None
        self.current_addr = None
        self.current_name = None
        self.dongle = None
        self.ui = ui
        self.__static_data_handler_id = 1502
        self.buffers_to_fill = buff

    def __str__(self):
        return f"MotionDetector(name={self.connection_name}, addr={self.connection_addr})"

    def evt_scan_callback(self, scan_input):
        try:
            json_list_element = scan_input[0]   
            json_string = json.loads(json_list_element)
            name = json_string.get("name")
            addr = json_string.get("addr")
            print(addr, name)
            """If name is not defined, we don't add it to list. """
            if name == 'MotionDetector':
                self.connection_addr = addr
                self.connection_name = name

            if name == "MotionDetector" and addr != None:
                self.ui.BleDevicelistWidget.addItem(f"{addr} {name}")
        except:
            """ 
            @note 
            Clicking scan closes the GUI somehow.
            So, exception handling added.
            """
            pass
            
    def evt_callback(self, evt_input):
        # print(evt_input)
        json_str = self.load_input(evt_input)
        if None != json_str:
            handle_id = self.get_evt_handle(json_str)
            # print(type(handle_id))
            if None != handle_id and handle_id == str(self.__static_data_handler_id):
                hex_val = self.get_hex_value(json_str)
                if hex_val != None:
                    # data_packet = self.parse_hex(hex_val)
                    # self.ble_file.write_packet(data_packet)
                    # print("Pack:", data_packet)
                    # This buffer created to add the data to store.
                    # Pack has to be added to this buffer if training is enabled.
                    for buff in self.buffers_to_fill:
                        with buff.lock:
                            buff.append(hex_val)
                else:
                    """We have to analyze the type of the event process it accordingly.
                    Packet may have different key rather than "hex"
                    """
                    pass
            else:
                print(json_str)
        else:
            print(evt_input)
            print("Invalid data event data to parse")
        
        """
        action = self.get_evt_action(json_str)
        if None != action and "disconnected" == action:
            # 1- Disconnect dongle.
            # 2- Update button colors.
            # 3- Stop parser|file writer thread
            
            self.dongle.at_gapdisconnectall()
            time.sleep(0.5)
            
            # Stop sensor thread
            self._sensor_thread_active = False
            self._sensor_data_thread.join()

            # Clear data buffer
            self.sensor_data_buffer.clear()

            # Update state variables
            self.connection_name = None
            self.connection_addr = None
            self.current_addr = None
            self.current_name = None
            
            # Reset ui
            self.gui.ui.pushButton_Scan.setStyleSheet("background-color: None")
            self.gui.ui.pushButton_Stream.setStyleSheet("background-color: None")
            self.gui.ui.pushButton_Connect.setStyleSheet("background-color: None")
            self.gui.ui.pushButton_Disconnect.setStyleSheet("background-color: None")
        """

    def init(self):
        self.dongle = BleuIO()
        self.dongle.register_evt_cb(self.evt_callback)
        self.dongle.register_scan_cb(self.evt_scan_callback)    

    def action_scan_start(self,timeout=10):
        self.dongle.at_central()
        time.sleep(0.2)
        self.dongle.at_gapscan()
        # counter = 5
        # while counter and False == self.is_scanning():
        #     time.sleep(0.8)
        #     counter = counter -1        
        if self.is_scanning() == False:
            print("Fail: Scan Initialization")
        else:
            print("Is scanning: True")


    def action_scan_stop(self):
        self.dongle.at_gapdisconnectall()
        time.sleep(0.5)
        self.dongle.stop_scan()
        time.sleep(0.5)
        counter = 5
        while counter and self.is_scanning():
            counter = counter - 1
            time.sleep(0.8)

        if self.is_scanning() == False:
            self.ui.pushButton_Scan.setStyleSheet("background-color: None")
        else:
            print("Fail: Scan Stop")

        print("Is scanning:", str(self.is_scanning()))

    def action_connection_connect(self):
        """Connect to MotionDetector"""
        self.dongle.at_gapdisconnectall()
        time.sleep(0.5)

        # Hide ascii values from notification/indication/read responses.
        # We need only hex values to parse them.
        self.dongle.ata(False)
        time.sleep(0.5)

        # Disable automatic discovery of services after connection.
        self.dongle.atds(False)
        time.sleep(0.5)

        self.dongle.at_gapconnect(self.connection_addr)
        counter = 5
        while counter > 0 and self.is_connected() == False:
            counter = counter - 1
            time.sleep(0.8)
            print("Connecting")

        print("Is Connected:",str(self.is_connected()))
        if self.is_connected() == False:
            self.dongle.at_gapdisconnectall()
        else:
            self.ui.pushButton_Connect.setStyleSheet("background-color: Green")
            # Discover services only.
            print("Reading services only!")
            self.dongle.at_get_servicesonly()
            time.sleep(0.5)
            print("Service read completed")

    def action_stream_start(self):        
        # Set up connection parameters
        print("Updating connection parameters!")
        self.dongle.at_connparam(intv_min ="24", intv_max ="24", slave_latency = "3", sup_timeout = "3")
        time.sleep(1)

        print("Enable notification for", self.__static_data_handler_id)
        self.dongle.at_set_noti(str(self.__static_data_handler_id))
        time.sleep(1)
        self.ui.pushButton_Stream.setStyleSheet("background-color: Green")
        print("Stream Enabled!")
    
    def action_stream_stop(self):
        self.dongle.at_clearnoti(str(self.__static_data_handler_id))
        self.ui.pushButton_Stream.setStyleSheet("background-color: None") 
        print("Stream Disabled!")   
    
    def is_connected(self):
        return self.dongle.status.isConnected

    def is_scanning(self):
        return self.dongle.status.isScanning

    def action_connection_disconnect(self):
        """Disconnect from MotionDetector"""
        self.dongle.at_gapdisconnectall()
        counter = 5
        while counter and self.is_connected():
            time.sleep(0.8)
            counter = counter -1

        if True == self.is_connected():
            print("Fail: Disconnect")
        else:
            self.ui.pushButton_Connect.setStyleSheet("background-color: None")
        
        print("Is Connected:", str(self.is_connected()))

