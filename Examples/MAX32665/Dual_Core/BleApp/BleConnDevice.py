# (C) 2023 Smart Sensor Devices AB

import time
import json
from datetime import datetime
from MainGui import Ui_MainWindow
from PyQt5.QtWidgets import QApplication, QTableWidget, QListWidget, QPushButton, QVBoxLayout, QWidget

from bleuio_lib.bleuio_funcs import BleuIO

class BleConnDevice:
    def __init__(self,ui:Ui_MainWindow = None):
        self.connection_response = None
        self.connection_name = None
        self.connection_addr = None
        self.current_addr = None
        self.current_name = None
        self.dongle = None
        self.ui = ui

    def __str__(self):
        return f"BtConn(name={self.connection_name}, addr={self.connection_addr})"

    def evt_scan_callback(self, scan_input):
        json_list_element = scan_input[0]
        json_string = json.loads(json_list_element)
        name = json_string.get("name")
        addr = json_string.get("addr")
        
        """If name is not defined, we don't add it to list. """
        if name != None and addr != None:
            self.ui.BleDevicelistWidget.addItem(f"{addr} {name}")
            

    def evt_callback(self, evt_input):
        cbTime = datetime.now()
        currentTime = cbTime.strftime("%H:%M:%S")
        print("\n\n[" + str(currentTime) + "] my_evt_callback: " + str(evt_input))

    def init(self):
        self.dongle = BleuIO()
        self.dongle.register_evt_cb(self.evt_callback)
        self.dongle.register_scan_cb(self.evt_scan_callback)    
        
        # self.dongle.at_central()
        self.dongle.at_dual()

    def action_scan_start(self,timeout=10):
        self.dongle.at_gapscan()
        while not self.is_scanning():
            pass
        print("Is scanning:",str(self.is_scanning()))

    def action_scan_stop(self):
        self.dongle.stop_scan()
        while self.is_scanning():
            pass
        print("Is scanning:", str(self.is_scanning()))

    def action_connection_connect(self):
        """Connect to BtConn"""
        print("Try connect to:", self.connection_addr)
        self.dongle.at_gapconnect(self.connection_addr,slave_latency="10",intv_min="5",
        intv_max="500")

        while not self.is_connected():
            pass
        print("Is Connected:",str(self.is_connected()))

    def is_connected(self):
        return self.dongle.status.isConnected

    def is_scanning(self):
        return self.dongle.status.isScanning

    def action_connection_disconnect(self):
        """Disconnect from BtConn"""
        print("Disconnect from:", self.connection_addr)
        self.dongle.at_gapdisconnect()
        while self.is_connected():
            pass
        print("Is Connected:", str(self.is_connected()))

