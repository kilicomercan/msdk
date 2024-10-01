import openpyxl
from datetime import datetime

class BleFile:
    def __init__(self, mark_sign ='L'):
        self.filename = self.generate_filename()
        self.workbook = self.open_excel_file()
        self.sheet = self.workbook.active
        self.current_row = 1
        self.current_packet = 1
        self.mark_sign = mark_sign

    def generate_filename(self):
        # Generate a filename with the current time of the day
        current_time = datetime.now().strftime("%H-%M-%S")
        filename = current_time+"_DataSet"
        return f"{filename}.xlsx"

    def open_excel_file(self):
        try:
            # Open the Excel file with the generated filename
            workbook = openpyxl.load_workbook(self.filename)
            print(f"Opened file: {self.filename}")
            return workbook
        except FileNotFoundError:
            print(f"File {self.filename} not found. Creating a new one.")
            workbook = openpyxl.Workbook()
            workbook.save(self.filename)
            return workbook
    
    def write_packet(self, timestamp, x_val, y_val, z_val):
        # Write a packet to the current row
        self.sheet.cell(row=self.current_row, column=self.current_packet * 4 - 3, value=timestamp)
        self.sheet.cell(row=self.current_row, column=self.current_packet * 4 - 2, value=x_val)
        self.sheet.cell(row=self.current_row, column=self.current_packet * 4 - 1, value=y_val)
        self.sheet.cell(row=self.current_row, column=self.current_packet * 4, value=z_val)
        
        # If it's the last packet in the row, write the mark sign
        if self.current_packet == 100:
            self.sheet.cell(row=self.current_row, column=401, value=self.mark_sign)
            self.current_row += 1
            self.current_packet = 1
        else:
            self.current_packet += 1

        # Save the workbook after writing each packet
        self.workbook.save(self.filename)
    

    def __self_test(self):
        for i in range(200):  # Example to write 200 packets
            timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            x_val = i
            y_val = i * 2
            z_val = i * 3
            self.write_packet(timestamp, x_val, y_val, z_val)
