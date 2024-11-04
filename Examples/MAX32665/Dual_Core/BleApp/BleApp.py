from BleConnDevice import BleConnDevice
from PyQt5.QtWidgets import QMessageBox
from PyQt5.QtGui import QColor
from BleGui import BleGui
import threading
from BleFile import BleFile
from BleBuff import BleBuff
from BleModelLoader import BleModelLoader
class BleApp():
    def __init__(self, motion_id = 'None'):
        self.__motion_id = motion_id
        if motion_id != "None":
            self.ble_file = BleFile()
        else:
            self.ble_file = None
            self.model = BleModelLoader("LSTM")

        self.__create_buffers()
        self.__initialize_threads()

        self.gui = BleGui()
        self.ble = BleConnDevice(self.gui.ui, file=self.ble_file, buff=self.buffers)

        self.connectionThread = None
        self.selectedDevice = None
        self.inference = False
        self.stream_enabled = False

        self.setup_ble_gui()
        
        """Initialize BLE dongle at this point"""
        # try:
        self.ble.init()
        # except:
        #     self.custom_closeEvent()

        """Show GUI Window"""
        self.gui.start_gui()

    def custom_closeEvent(self,event):
        # 1. Stop Streaming
        if self.stream_enabled:
            self.ble.action_stream_stop()
            time.sleep(1)

        # 2. Stop Save and close event process thread
        if self.ble_file != None:
            with self.ble_file.file_lock:
                self.ble_file.is_recording = False
            self.ble_file.close_file()

        """Close event parser thread"""
        if self.thread_data_collection != None:
            self.thread_data_collection_active = False
            self.thread_data_collection.join()
        
        # 3. Disconnect from all devices
        self.ble.action_connection_disconnect()

        # 4. Close window.
        event.accept()

    def __initialize_threads(self):
        self.thread_data_collection_active = True
        # Initialize data parser thread before notification reading.    
        self.thread_data_collection = threading.Thread(target=self.__thread_sensor_event_handler, name="Th_EventHandler")
        self.thread_data_collection.start()
        
    def __create_buffers(self):
        self.buffers:list[BleBuff] = []
        # This buffer is for data saving in training mode.
        self.buffers.append(BleBuff())

    def __add_sample(self, data_packet):
            if None == data_packet["timestamp"] or 0 == data_packet["timestamp"]:
                # Do not add the sample to inference sample list.
                return
            if None == data_packet["x_val"]:
                data_packet["x_val"] = 0
            if None == data_packet["y_val"]:
                data_packet["y_val"] = 0
            if None == data_packet["z_val"]:
                data_packet["z_val"] = 0
            self.inference_sample.append(data_packet["timestamp"])
            self.inference_sample.append(data_packet["x_val"])
            self.inference_sample.append(data_packet["y_val"])
            self.inference_sample.append(data_packet["z_val"])

    def __thread_sensor_event_handler(self):
        current_pack = None
        self.__pre_padding = []
        self.__motion_samples = []
        self.__inference_sample = []
        self.__motion_started = False
        self.__motion_ended = False
        
        def __motion_pack_preparator_zero_padding(data_packet):
            if data_packet["flag"] == 0:
                if self.__motion_started == False:
                    self.__motion_started = True
                if None != data_packet["timestamp"] and None != data_packet["x_val"] and None != data_packet["y_val"] and None != data_packet["z_val"]:
                    self.__motion_samples.append(data_packet["timestamp"])
                    self.__motion_samples.append(data_packet["x_val"])
                    self.__motion_samples.append(data_packet["y_val"])
                    self.__motion_samples.append(data_packet["z_val"])
            else:
                if True == self.__motion_started:
                    if len(self.__motion_samples) < 80:
                        self.__motion_ended = False
                        self.__motion_started = False
                    else:
                        self.__motion_ended = True
                        pre_padding_size = 200 - len(self.__motion_samples)
                        while pre_padding_size:
                            self.__inference_sample.append(0)
                            pre_padding_size = pre_padding_size - 1
                        
                        for val in self.__motion_samples:
                            self.__inference_sample.append(val)
                        
  
        def __motion_pack_preparator(data_packet):
            if data_packet["flag"] == 0:
                if self.__motion_started == False:
                    self.__motion_started = True
                if None != data_packet["timestamp"] and None != data_packet["x_val"] and None != data_packet["y_val"] and None != data_packet["z_val"]:
                    self.__motion_samples.append(data_packet["timestamp"])
                    self.__motion_samples.append(data_packet["x_val"])
                    self.__motion_samples.append(data_packet["y_val"])
                    self.__motion_samples.append(data_packet["z_val"])
            else:
                if True == self.__motion_started:
                    self.__motion_ended = True
                    if len(self.__motion_samples) < 80:
                        # If there is not enough motion data which means that if we moved
                        # our arm so fast, we will not take this into account.
                        self.__motion_ended = False
                        self.__motion_started = False
                        self.__motion_samples.clear()
                        self.__pre_padding.clear()
                        self.__inference_sample.clear()
                    elif len(self.__motion_samples) == 200:
                        # At least 200 value must be included in self.__motion_samples.
                        print("equal 200")
                        self.__inference_sample = self.__motion_samples
                    elif len(self.__motion_samples) > 200:
                        self.__inference_sample = self.__motion_samples[len(self.__motion_samples)-200:]
                        print("more than 200")
                    else:
                        print("else : ", len(self.__motion_samples), len(self.__pre_padding))
                        required_padding = 200 - len(self.__motion_samples)
                        if required_padding <= len(self.__pre_padding):
                            self.__pre_padding = self.__pre_padding[len(self.__pre_padding)-required_padding:]
                            for i in self.__pre_padding:
                                self.__inference_sample.append(i)
                            for i in self.__motion_samples:
                                self.__inference_sample.append(i)
                        else:
                            zero_padding = required_padding - len(self.__pre_padding)
                            while zero_padding:
                                self.__inference_sample.append(0)
                                zero_padding = zero_padding - 1
                            for i in self.__pre_padding:
                                self.__inference_sample.append(i)
                            for i in self.__motion_samples:
                                self.__inference_sample.append(i)
                else:
                    if 0 != data_packet["timestamp"] and None != data_packet["x_val"] and None != data_packet["y_val"] and None != data_packet["z_val"]:
                        if len(self.__pre_padding) == (30*4): 
                            # 4*30 = 120 value will be stored in self.__pre_padding.
                            self.__pre_padding.pop(0)
                            self.__pre_padding.pop(0)
                            self.__pre_padding.pop(0)
                            self.__pre_padding.pop(0)
                        self.__pre_padding.append(data_packet["timestamp"])
                        self.__pre_padding.append(data_packet["x_val"])
                        self.__pre_padding.append(data_packet["y_val"])
                        self.__pre_padding.append(data_packet["z_val"])
        # self.self.__motion_started = False
        
        while self.thread_data_collection_active:
            if 0 != self.buffers[0].len():
                with self.buffers[0].lock:
                    current_pack = self.buffers[0].pop(0)
                data_packet = self.ble.parse_hex(current_pack)
                if None == self.ble_file and True == self.inference:
                    __motion_pack_preparator(data_packet)

                    if True == self.__motion_started and True == self.__motion_ended:
                        self.__motion_started = False
                        self.__motion_ended = False
                        result = self.model.classify_motion(self.__inference_sample)
                        self.__inference_sample.clear()
                        self.__motion_samples.clear()
                        self.__pre_padding.clear()
                        self.gui.ui.lineEdit_Prediction.setText(result)
                else:
                    if self.ble_file != None:
                        with self.ble_file.file_lock:
                            if self.ble_file.is_recording:
                                self.ble_file.write_packet(data_packet["flag"], data_packet["timestamp"], data_packet["x_val"], data_packet["y_val"], data_packet["z_val"])
                            else:
                                print(data_packet)

        self.buffers[0].lock = None
        self.buffers[0].clear()
        self.thread_data_collection_active = False

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
            self.__execute_parallel(self.ble.action_scan_start,"Th_ScanStart",False)
        else:
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
            if self.stream_enabled == True:
                self.ble.action_stream_stop()

            if None != self.ble_file and self.ble_file.is_recording == True:
                with self.ble_file.file_lock:
                    self.ble_file.is_recording = False
                self.ble_file.close_file()        
                self.gui.ui.pushButton_Save.setStyleSheet("background-color: None") 

            self.__execute_parallel(self.ble.action_connection_disconnect, "Th_Disconnect")
            self.ble.connection_addr = None
            self.ble.connection_name = None

    def __ble_app_btn_stream_clicked_event(self):
        """Get services first"""
        if self.ble.is_connected() :
            if self.stream_enabled == False:
                self.stream_enabled = True
                self.__execute_parallel(function_cb=self.ble.action_stream_start, thread_name="Th_Stream_Start")
            else:
                self.stream_enabled = False
                self.__execute_parallel(function_cb = self.ble.action_stream_stop, thread_name = "Th_Stream_Stop")
        else:
            print("Not connected!")

    def __ble_app_btn_save_clicked_event(self):
        if self.stream_enabled == True:
            if self.ble_file.is_recording == False:
                # Set file configurations first.
                self.ble_file.create_new_file()
                self.ble_file.conf_mark_sign(True, self.__motion_id)

                # Enable file writing in thread.
                with self.ble_file.file_lock:
                    self.ble_file.is_recording = True
                self.gui.ui.pushButton_Save.setStyleSheet("background-color: Green") 
            else:
                # First disable file writing.
                with self.ble_file.file_lock:
                    self.ble_file.is_recording = False

                # Close file and reset variables.
                self.ble_file.close_file()
                self.gui.ui.pushButton_Save.setStyleSheet("background-color: None") 
        else:
            if self.ble_file.is_recording == True:
                with self.ble_file.file_lock:
                    self.ble_file.is_recording = False

                # Close file and reset variables.
                self.ble_file.close_file()
                self.gui.ui.pushButton_Save.setStyleSheet("background-color: None") 

    def __ble_app_btn_inference_clicked_event(self):
        if self.inference == False:
            self.gui.ui.pushButton_Inference.setStyleSheet("background-color: Green") 
        else:
            self.gui.ui.pushButton_Inference.setStyleSheet("background-color: None")

        self.inference = not self.inference

    def setup_ble_gui(self):
        """Push button signal connection configuration"""
        self.gui.ui.pushButton_Scan.clicked.connect(self.__ble_app_btn_scan_clicked_event)
        self.gui.ui.pushButton_Connect.clicked.connect(self.__ble_app_btn_connect_clicked_event)
        self.gui.ui.pushButton_Disconnect.clicked.connect(self.__ble_app_btn_disconnect_clicked_event)
        self.gui.ui.pushButton_Stream.clicked.connect(self.__ble_app_btn_stream_clicked_event)
        self.gui.ui.pushButton_Save.clicked.connect(self.__ble_app_btn_save_clicked_event)
        self.gui.ui.pushButton_Inference.clicked.connect(self.__ble_app_btn_inference_clicked_event)

        self.gui.ui.actionScan.triggered.connect(self.__ble_app_btn_scan_clicked_event)
        self.gui.ui.actionConnect.triggered.connect(self.__ble_app_btn_connect_clicked_event)
        self.gui.ui.actionDisconnect.triggered.connect(self.__ble_app_btn_disconnect_clicked_event)
        self.gui.ui.actionExit.triggered.connect(self.gui.MainWindow.close)
        self.gui.MainWindow.closeEvent = self.custom_closeEvent

        """QListWidget Signal connection configuration"""
        self.gui.ui.BleDevicelistWidget.currentItemChanged.connect(self.__ble_app_current_item_set)

import time
import sys
import signal

def signal_handler(sig, frame):
    print('You pressed Ctrl+C!')
    ble_app.dongle.at_gapdisconnectall()
    sys.exit(0)

def main():
    signal.signal(signal.SIGINT, signal_handler)
    is_training = None
    while True:
        is_training = input("Training or Not!(Y/N)?")
        if is_training == 'y' or is_training == 'Y' or is_training == 'N' or is_training == 'n':
            break
        else:
            print("Invalid Input!")
    motion_id = None
    valid_motion_input = ['L','R','D','U','N']
    if is_training == 'Y' or is_training == 'y':
        while True:
            motion_id = input("Motion Flag! (\'L\',\'R\',\'D\',\'U\',\'N\'):")
            if motion_id.upper() in valid_motion_input:
                break
            else:
                print("Invalid Motion Training")
    else:
        motion_id = "None"

    BleApp(motion_id)

if __name__=="__main__":
    main()