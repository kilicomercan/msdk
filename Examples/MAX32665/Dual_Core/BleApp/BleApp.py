from BleConnDevice import BleConnDevice
from PyQt5.QtWidgets import QTableWidgetItem
from PyQt5.QtGui import QColor
from BleGui import BleGui
import threading

class BleApp(BleGui, BleConnDevice):
    def __init__(self):
        self.gui = BleGui()
        self.ble = BleConnDevice(self.gui.ui)
        self.scan_active = False
        self.scan_thread = 0
        self.setup_ble_gui()
        
        """Initialize BLE dongle at this point"""
        self.ble.init()

        """Show GUI Window"""
        self.gui.start_gui()
        
    def ble_app_btn_scan_clicked_event(self):
        print("scan clicked")
        name = 'Omercan'
        addr = "TurkiyeIstanbulSonraMunich"
        if not self.scan_active:
            row_count = self.gui.ui.BleDevicestableWidget.insertRow(1)
            self.gui.ui.BleDevicestableWidget.setItem(row_count, 0, QTableWidgetItem(name))
            self.gui.ui.BleDevicestableWidget.setItem(row_count, 1, QTableWidgetItem(addr[3:]))
            if name == 'BtConn':
                self.gui.ui.BleDevicestableWidget.itemAt(row_count,0).setBackground(QColor.green())
                self.gui.ui.BleDevicestableWidget.itemAt(row_count,1).setBackground(QColor.green())
            self.gui.ui.pushButton_Scan.setStyleSheet("background-color: green")
            self.scan_active = True    
            # self.scan_thread = threading.Thread(target=self.ble.action_scan_start,name="ScanThread")
            # self.scan_thread.start()
            # self.ble.action_scan_start()
        else:
            self.gui.ui.pushButton_Scan.setStyleSheet("background-color: None")
            # tempThread = threading.Thread(target=self.ble.action_scan_stop, name="ScanStop")
            # # tempThread.start()
            # self.ui.BleDevicestableWidget.itemAt(0)
            self.scan_active = False

    def setup_ble_gui(self):
        self.gui.ui.pushButton_Scan.clicked.connect(self.ble_app_btn_scan_clicked_event)

ble_app = BleApp()
