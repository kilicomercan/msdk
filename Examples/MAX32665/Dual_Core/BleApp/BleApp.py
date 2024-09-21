from BleConnDevice import BleConnDevice
from BleGui import BleGui

class BleApp(BleGui, BleConnDevice):
    def __init__(self):
        self.ble = BleConnDevice()
        self.gui = BleGui()

    def ble_app_btn_scan_clicked_event(self):
        pass

    def setup_ble_gui(self):
        self.gui.pushButton_Scan.connect(self.ble_app_btn_scan_clicked_event)

ble_app = BleApp()

while 1:
    pass