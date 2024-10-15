import pandas as pd
from openpyxl import load_workbook
import openpyxl
import time

class ProcessedData:
    def __init__(self, timestamp:float, x_val:int, y_val:int, z_val:int):
        self.timestamp = timestamp
        self.x_val = x_val
        self.y_val = y_val
        self.z_val = z_val

# Main function to process Excel file
def process_excel(input_file, output_file):
    new_file_row = 1
    new_file_column = 1
    current_motion_data:list[ProcessedData] = []
    motion_started = False
    motion_ended = False

    raw_data_workbook = load_workbook(input_file)
    raw_data_sheet = raw_data_workbook.active
    max_row = raw_data_sheet.max_row
    max_column = raw_data_sheet.max_column

    try:
        new_file_workbook = load_workbook(output_file)
        new_file_sheet = new_file_workbook.active
    except:
        new_file_workbook  = openpyxl.Workbook()
        new_file_workbook.save(output_file)
        new_file_sheet = new_file_workbook.active
    
    average_motion_duration = 0
    row_count = 0
    for row in raw_data_sheet.iter_rows(min_row=1, max_row= max_row, min_col=1, max_col=max_column, values_only= True ):
        i = 0
        while i < len(row):
            if 0 == row[i]:
                if 0 != row[i+1] and None != row[i+2] and None != row[i+3] and None!= row[i+4]:
                    current_motion_data.append(ProcessedData(row[i+1],row[i+2],row[i+3],row[i+4]))
                motion_started = True
            else:
                if motion_started == True:
                    motion_ended = True
                    break
            i = i+5

        if True == motion_started and True == motion_ended:
            motion_started = False
            motion_ended = False
            # Sampling rate is 10ms for each packet.
            # 15*10 = 250, 50*10 = 500ms.
            # Motion duration must be between this time interval [250ms,500ms].Less than this will not be taken as valid data
            print(len(current_motion_data))
            if len(current_motion_data) >= 20:
                row_count = row_count + 1
                average_motion_duration = average_motion_duration + len(current_motion_data)
                new_file_column = 1
                for pack in current_motion_data:
                    new_file_sheet.cell(row = new_file_row, column= new_file_column * 4 - 3, value=pack.timestamp)
                    new_file_sheet.cell(row = new_file_row, column= new_file_column * 4 - 2, value=pack.x_val)
                    new_file_sheet.cell(row = new_file_row, column= new_file_column * 4 - 1, value=pack.y_val)
                    new_file_sheet.cell(row = new_file_row, column= new_file_column * 4, value=pack.z_val)
                    new_file_column = new_file_column + 1
                    if new_file_column >=50:
                        break
                if len(current_motion_data) != 0:
                    new_file_row = new_file_row +1       

            current_motion_data.clear()

    print(average_motion_duration/row_count)
    new_file_workbook.save(output_file)         

if __name__ == "__main__":
    input_file_name = input("Input filename:")
    output_file_name =  "processedTest"+input_file_name
    process_excel(input_file_name, output_file_name)