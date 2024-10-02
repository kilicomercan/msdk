import json

class BleParser:
    def __init__(self):
        """
        Initialize the parser with the input data.
        :param input_data: List of string containing JSON-like structures
        """
        self.data = None
        self.parsed_data = None

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

    def parse_hex(self):
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
        hex_value = self.get_hex_value()
        if hex_value:
            # Remove "0x" prefix from the hex string
            hex_value = hex_value[2:]

            # Ensure the hex string is of the correct length (30 characters = 15 bytes)
            if len(hex_value) != 30:
                raise ValueError("Hex value does not have the correct length (15 bytes).")

            # Parse the individual components
            flag = int(hex_value[0:2], 16)  # 1st byte (2 hex chars)
            seconds = int(hex_value[2:10], 16)  # 2nd to 5th bytes (8 hex chars)
            subseconds = int(hex_value[10:18], 16)  # 6th to 9th bytes (8 hex chars)
            x_val = int(hex_value[18:22], 16)  # 10th to 11th bytes (4 hex chars)
            y_val = int(hex_value[22:26], 16)  # 12th to 13th bytes (4 hex chars)
            z_val = int(hex_value[26:30], 16)  # 14th to 15th bytes (4 hex chars)
            timestamp = seconds + (subseconds / (10**len(str(subseconds))))
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
    
        decimal_part = int2 / (10 ** len(str(int2)))

        # Combine int1 and int2 to form the float
        combined_float = int1 + decimal_part

    def get_evt_action(self):
        """
        Returns the 'action' from the 'evt' key in the parsed data, if present.
        :return: Action value in 'evt', if present.
        """
        if "evt" in self.parsed_data and "action" in self.parsed_data["evt"]:
            return self.parsed_data["evt"]["action"]
        else:
            return None

    def get_evt_handle(self, json_str):
        """
        Returns the 'handle' from the 'evt' key in the parsed data, if present.
        :return: Handle value in 'evt', if present.
        """
        if "evt" in json_str and "handle" in json_str["evt"]:
            return json_str["evt"]["handle"]
        else:
            return None

    def get_hex_value(self):
        """
        Returns the 'hex' value from the 'evt' key in the parsed data, if present.
        :return: Hex value in 'evt', if present.
        """
        if "evt" in self.parsed_data and "hex" in self.parsed_data["evt"]:
            return self.parsed_data["evt"]["hex"]
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

ble_parser = BleParser()
ble_parser.load_input(['{777:"0000","evt":{"handle":"1502","hex":"0x0114000000400C0000270009404681","len":"15"}}'])
if 1 == ble_parser.parse():
    handle_id =ble_parser.get_evt_handle()
    if "1502" == handle_id:
        hex_str = ble_parser.get_hex_value()
        print(ble_parser.parse_hex())

