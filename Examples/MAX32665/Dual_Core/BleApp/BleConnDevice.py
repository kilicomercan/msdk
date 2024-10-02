# (C) 2023 Smart Sensor Devices AB

import time
import json
from datetime import datetime
from MainGui import Ui_MainWindow
from PyQt5.QtWidgets import QApplication, QTableWidget, QListWidget, QPushButton, QVBoxLayout, QWidget
from bleuio_lib.bleuio_funcs import BleuIO
from BleFile import BleFile
from BleParser import BleParser
import threading

class BleConnDevice(BleParser):
    def __init__(self, ui:Ui_MainWindow = None, file:BleFile = None):
        self.connection_response = None
        self.connection_name = None
        self.connection_addr = None
        self.current_addr = None
        self.current_name = None
        self.dongle = None
        self.ui = ui
        self.file = file
        self.__static_data_handler_id = "1502"

        self.__sensor_data_lock = None
        self.__sensor_thread_active = False
        self.__sensor_data_thread = None
        self.__sensor_data_buffer = []
    
    def __str__(self):
        return f"BtConn(name={self.connection_name}, addr={self.connection_addr})"

    def thread_sensor_data_collection(self):
        current_pack = None
        while self.__sensor_thread_active:
            if 0 != len(self.__sensor_data_buffer):
                with self.__sensor_data_lock:
                    current_pack = self.__sensor_data_buffer.pop(0)
                data_packet = self.parse_hex(current_pack)
                self.file.write_packet(data_packet["timestamp"],data_packet["x_val"], data_packet["y_val"],data_packet["z_val"])

        self.__sensor_data_lock = None
        self.__sensor_thread_active = False

    def evt_scan_callback(self, scan_input):
        json_list_element = scan_input[0]
        json_string = json.loads(json_list_element)
        name = json_string.get("name")
        addr = json_string.get("addr")
        print(addr, name)
        """If name is not defined, we don't add it to list. """
        if name == 'BtConn':
            self.connection_addr = addr
            self.connection_name = name

        if name != None and addr != None:
            self.ui.BleDevicelistWidget.addItem(f"{addr} {name}")
            
    def evt_callback(self, evt_input):
        json_str = self.load_input(evt_input)
        if None != json_str:
            handle_id = self.get_evt_handle(json_str)
            if None != handle_id and "1502" == handle_id:
                with self.__sensor_data_lock:
                    self.__sensor_data_buffer.append(self.get_hex_value())

    def init(self):
        self.dongle = BleuIO()
        self.dongle.register_evt_cb(self.evt_callback)
        self.dongle.register_scan_cb(self.evt_scan_callback)    

    def action_scan_start(self,timeout=10):
        self.dongle.at_central()
        time.sleep(0.2)
        self.dongle.at_gapscan()
        while not self.is_scanning():
            pass
        print("Is scanning:",str(self.is_scanning()))

    def action_scan_stop(self):
        self.dongle.at_gapdisconnectall()
        time.sleep(0.5)
        self.dongle.stop_scan()
        time.sleep(0.5)
        while self.is_scanning():
            pass
        print("Is scanning:", str(self.is_scanning()))

    def action_connection_connect(self):
        """Connect to BtConn"""
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
        # while not self.is_connected():
        #     pass
        time.sleep(1)

        print("Is Connected:",str(self.is_connected()))
        self.ui.pushButton_Connect.setStyleSheet("background-color: Green")

        # Discover services only.
        print("Reading services only!")
        self.dongle.at_get_servicesonly()
        time.sleep(3)
        

    def action_stream_start(self):
        # Initialize sensor buffer and lock mechanism
        self.__sensor_data_lock = threading.Lock()
        self.__sensor_data_buffer.clear()
        self.__sensor_thread_active = True
        
        # Set up connection parameters
        print("Updating connection parameters!")
        self.dongle.at_connparam(intv_min ="24", intv_max ="24", slave_latency = "3", sup_timeout = "3")
        time.sleep(1)

        # Initialize data parser thread before notification reading.     
        self.__sensor_data_thread = threading.Thread(function = self.thread_sensor_data_collection)
        self.__sensor_data_thread.start()

        print("Enabling notification for", self.__static_data_handler_id)
        self.dongle.at_set_noti(self.__static_data_handler_id)
        time.sleep(1)
    
    def action_stream_stop(self):
        self.dongle.at_clearnoti(self.__static_data_handler_id)
        self.__sensor_thread_active = False
        self.__sensor_data_thread.join()

    def is_connected(self):
        return self.dongle.status.isConnected

    def is_scanning(self):
        return self.dongle.status.isScanning

    def action_connection_disconnect(self):
        """Disconnect from BtConn"""
        self.dongle.at_gapdisconnect()
        while self.is_connected():
            pass
        print("Is Disconnected:", str(self.is_connected()))

