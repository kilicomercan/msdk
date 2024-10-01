import json

class Parser:
    def __init__(self):
        """
        Initialize the parser with the input data.
        :param input_data: List of string containing JSON-like structures
        """
        self.data = None
        self.parsed_data = None

    def load_input(self,input_data):
        self.data = input_data

    def parse(self):
        """
        Parses the string data in the input and converts it into a dictionary.
        """
        # Extract the string from the list and convert it to a dictionary using json.loads
        try:
            self.parsed_data = json.loads(self.data[0])
        except json.JSONDecodeError:
            raise ValueError("Invalid JSON format")

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
    def get_se_value(self):
        """
        Returns the 'SE' or similar key value from the parsed data.
        :return: The value of 'SE', 'R', or similar key, if present.
        """
        if "SE" in self.parsed_data:
            return self.parsed_data["SE"]
        elif "R" in self.parsed_data:
            return self.parsed_data["R"]
        else:
            return None

    def get_evt_action(self):
        """
        Returns the 'action' from the 'evt' key in the parsed data, if present.
        :return: Action value in 'evt', if present.
        """
        if "evt" in self.parsed_data and "action" in self.parsed_data["evt"]:
            return self.parsed_data["evt"]["action"]
        else:
            return None

    def get_evt_handle(self):
        """
        Returns the 'handle' from the 'evt' key in the parsed data, if present.
        :return: Handle value in 'evt', if present.
        """
        if "evt" in self.parsed_data and "handle" in self.parsed_data["evt"]:
            return self.parsed_data["evt"]["handle"]
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

# Example usage
if __name__ == "__main__":
    # Input examples
    variable1 = ['{"SE":35,"evt":{"action":"scan completed"}}']
    variable2 = ['{776:"0000","evt":{"handle":"1503","writeStatus":0}}']
    variable3 = ['{777:"0000","evt":{"handle":"1502","hex":"0x0114000000400C0000270009404681","len":15}}']
    variable4 = ['{"R":24,"evt":{"action":"scanning"}}']

    # Initialize and parse each variable
    parser = Parser()
    parser.load_input(variable1)
    parser.parse()
    print("SE Value:", parser.get_se_value())  # Example for SE/R value
    print("Action:", parser.get_evt_action())  # Example for evt action

    parser = Parser()
    parser.load_input(variable3)
    parser.parse()
    handle_id = parser.get_evt_handle()
    if handle_id == "1502":
        hex_val = parser.get_hex_value()
        print("Hex Value:", parser.get_hex_value())  # Extract hex value

    parser.store_hex_to_file()  # Store hex value in file

