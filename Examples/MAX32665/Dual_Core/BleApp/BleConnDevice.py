# (C) 2023 Smart Sensor Devices AB

import time
import json
from datetime import datetime
from bleuio_lib.bleuio_funcs import BleuIO

class BleConnDevice:
    def __init__(self):
        self.name = None
        self.addr = None
        self.dongle = None

    def __str__(self):
        return f"BtConn(name={self.name}, addr={self.addr})"

    def evt_scan_callback(self, scan_input):
        json_list_element = scan_input[0]
        json_string = json.loads(json_list_element)
        name = json_string.get("name")
        addr = json_string.get("addr")
        print("Dev:\"", name, "\" Addr:", addr)
        if name != None:
            if name == "BtConn" and self.name == None:
                self.addr = addr[3:]
                self.name = name
                return [1, self.addr, self.name]
            
        return [0, addr, name]

    def evt_callback(self, evt_input):
        cbTime = datetime.now()
        currentTime = cbTime.strftime("%H:%M:%S")
        print("\n\n[" + str(currentTime) + "] my_evt_callback: " + str(evt_input))

    def init(self):
        self.dongle = BleuIO()
        self.dongle.register_evt_cb(self.evt_callback)
        self.dongle.register_scan_cb(self.evt_scan_callback)    
        self.dongle.at_central()

    def action_scan_start(self,timeout=10):
        return self.dongle.at_gapscan(10)
    
    def action_scan_stop(self):
        return self.dongle.stop_scan()

    def action_connection_connect(self):
        """Connect to BtConn"""
        pass
    def action_connection_disconnect(self):
        """Disconnect from BtConn"""
        pass
