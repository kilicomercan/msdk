import json

class BleParser:
    def __init__(self):
        """
        Initialize the parser with the input data.
        :param input_data: List of string containing JSON-like structures
        """
        self.data = None
        self.parsed_data = None

    def __parse_val(self, val):
        """val is integer apply bit manupilation to it to determine type of the data"""
        type = (val >> 14) & 3
        sign = (val >> 12) & 3
        raw_data = (val & 0xfff)

        return type, sign, raw_data
    
    def sensor_data_parser(self, sensor_data_pack):
        """
            type of sensor_data_pack input
                "flag": flag,
                "timestamp":timestamp,
                "x_val": val1,
                "y_val": val2,
                "z_val": val3
        """
        keys = ["x_val", "y_val", "z_val"]
        result = []
        for i in keys:
            result.append(self.__parse_val(sensor_data_pack[i]))
        return result

    def load_input(self,input_data):
        input_str = str(input_data[0])
        evt_index = input_str.find("\"evt\":")
        if evt_index:
            cleaned_str = '{'+input_str[evt_index:]
            try:
                json_str = json.loads(cleaned_str)
                return json_str
            except:
                return None
        else:
            return None
        
    def __get_type_val(self, value):
            val = value & 0xffff  
            val_type = (val >> 14) & 0b11
            val = val & 0x0fff
            if val & 0x800:
                val -= 1<<12
            return [val_type, val]

    def parse_hex(self, hex_value):
        """
        Parses the hex value according to the provided structure.
        1st byte = flag
        2-5 bytes = int1
        6-9 bytes = int2
        10-11 bytes = x_val
        12-13 bytes = y_val
        14-15 bytes = z_val
        :return: A dictionary with parsed hex values.
        """
        if hex_value:
            # Remove "0x" prefix from the hex string
            hex_value = hex_value[2:]

            # Ensure the hex string is of the correct length (30 characters = 15 bytes)
            if len(hex_value) != 30:
                print("Hex value does not have the correct length (15 bytes).")
                return None
            
            # Parse the individual components
            flag = int(hex_value[0:2], 16)  
            seconds = hex_value[8:10] + hex_value[6:8] +hex_value[4:6] + hex_value[2:4]
            seconds = int(seconds, 16) 

            subseconds = hex_value[16:18] + hex_value[14:16] + hex_value[12:14] + hex_value[10:12]  
            subseconds = int(subseconds, 16) 
            x_val = None
            y_val = None 
            z_val = None
            values = [int(hex_value[20:22] + hex_value[18:20],16), int(hex_value[24:26] +hex_value[22:24],16), int(hex_value[28:30] + hex_value[26:28],16)]
            for val in values:
                val_type, real_val = self.__get_type_val(val)
                if val_type == 0b00:
                    x_val = real_val
                elif val_type == 0b01:
                    y_val = real_val
                else:
                    z_val = real_val

            timestamp = seconds + round(subseconds / 4096, 3)
            # timestamp = seconds + (subseconds / (10**len(str(subseconds))))
            # Return parsed values in a dictionary
            return {
                "flag": flag,
                "timestamp":timestamp,
                "x_val": x_val,
                "y_val": y_val,
                "z_val": z_val
            }
        else:
            raise ValueError("No hex value found to parse.")

    def get_evt_action(self, json_str):
        """
        Returns the 'action' from the 'evt' key in the parsed data, if present.
        :return: Action value in 'evt', if present.
        """
        if "evt" in json_str and "action" in json_str["evt"]:
            return json_str["evt"]["action"]
        else:
            return None

    def get_evt_handle(self, json_str):
        """
        Returns the 'handle' from the 'evt' key in the parsed data, if present.
        :return: Handle value in 'evt', if present.
        """
        if "evt" in json_str:
            if "handle" in json_str["evt"]:
                return json_str["evt"]["handle"]
            else:
                return None
        else:
            return None

    def get_hex_value(self, json_str):
        """
        Returns the 'hex' value from the 'evt' key in the parsed data, if present.
        :return: Hex value in 'evt', if present.
        """
        if "evt" in json_str and "hex" in json_str["evt"]:
            return json_str["evt"]["hex"]
        else:
            return None

    def store_hex_to_file(self, filename="hex_value.txt"):
        """
        Stores the hex value to a file if the 'hex' key is present in the parsed data.
        :param filename: Name of the file to store the hex value.
        """
        hex_value = self.get_hex_value()
        if hex_value:
            with open(filename, 'w') as file:
                file.write(hex_value)
            print(f"Hex value '{hex_value}' stored in {filename}")
        else:
            print("No hex value found to store.")

# ble_parser = BleParser()
# json_str = ble_parser.load_input(['{777:"0000","evt":{"handle":"1502","hex":"0x01C60000004B0C0000F67FCF84CF3F","len":15}}'])

# handle_id = ble_parser.get_evt_handle(json_str)
# if "1502" == handle_id:
#     hex_str = ble_parser.get_hex_value(json_str)
#     if hex_str != None:
#         first_process_parsing = ble_parser.parse_hex(hex_str)
#         print(first_process_parsing)


