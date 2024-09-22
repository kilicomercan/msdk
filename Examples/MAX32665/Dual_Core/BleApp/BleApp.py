from BleConnDevice import BleConnDevice
from PyQt5.QtWidgets import QTableWidgetItem
from PyQt5.QtGui import QColor
from BleGui import BleGui
import threading
import re

class BleApp(BleGui, BleConnDevice):
    def __init__(self):
        self.gui = BleGui()
        self.ble = BleConnDevice(self.gui.ui)
        self.scan_thread = 0
        self.selectedDevice = None
        self.isConnected = False
        self.setup_ble_gui()
        
        """Initialize BLE dongle at this point"""
        self.ble.init()
        
        """Show GUI Window"""
        self.gui.start_gui()
    
    def __execute_parallel(self, function_cb, thread_name, join = False, parameter = None):
        tempThread = threading.Thread(target=function_cb, name=thread_name)
        tempThread.start()
        if join:
            tempThread.join()

    def __ble_app_btn_scan_clicked_event(self):
        print("Scan clicked")
        if True == self.ble.is_connected():
            print("Disconnect first")
            return
        
        if False == self.ble.is_scanning():    
            self.gui.ui.pushButton_Scan.setStyleSheet("background-color: Green")
            self.__execute_parallel(self.ble.action_scan_start,"Th_ScanStart")
        else:
            self.gui.ui.pushButton_Scan.setStyleSheet("background-color: None")
            self.__execute_parallel(self.ble.action_scan_stop,"Th_ScanStop", True)
            self.gui.ui.BleDevicelistWidget.clear()

    def __ble_app_current_item_set(self):
        self.selectedDevice = self.gui.ui.BleDevicelistWidget.currentItem()
        if self.selectedDevice != None:
            print("current item : ", self.selectedDevice.text())
            parts = self.selectedDevice.text().split(' ', 1)
            self.ble.current_addr = parts[0]
            self.ble.current_name = parts[1]
            
    def __ble_app_btn_connect_clicked_event(self):
        print("Connect btn clicked")
        
        if not self.ble.is_connected():
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
            print('Already connected.')
    
    def __ble_app_btn_disconnect_clicked_event(self):
        if not self.ble.is_connected():
            print("Not connected")
        else:
            self.__execute_parallel(self.ble.action_connection_disconnect, "Th_Disconnect")
            self.ble.connection_addr = None
            self.ble.connection_name = None

    def __ble_app_btn_readData_clicked_event(self):
        """Get services first"""
        if self.ble.is_connected():
            services = self.ble.dongle.at_get_services()
            print(services.Rsp)
            print(services.Cmd)
            print(services.Ack)
            print(services.End)
        else:
            print("Not connected!")

    def setup_ble_gui(self):
        """Push button signal connection configuration"""
        self.gui.ui.pushButton_Scan.clicked.connect(self.__ble_app_btn_scan_clicked_event)
        self.gui.ui.pushButton_Connect.clicked.connect(self.__ble_app_btn_connect_clicked_event)
        self.gui.ui.pushButton_Disconnect.clicked.connect(self.__ble_app_btn_disconnect_clicked_event)
        self.gui.ui.pushButton_ReadData.clicked.connect(self.__ble_app_btn_readData_clicked_event)
        
        """QListWidget Signal connection configuration"""
        self.gui.ui.BleDevicelistWidget.currentItemChanged.connect(self.__ble_app_current_item_set)

ble_app = BleApp()

