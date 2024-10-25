import numpy as np
from tensorflow.keras.models import load_model

class BleModelLoader:
    def __init__(self, model_name) -> None:
        # Load the saved model
        self.model = load_model("MotionDetection"+model_name+".keras")
        self.__class_labels = ['D', 'L', 'N', 'R', 'U']
    def __self_pre_process_data(self,real_time_data):
        index = 0
        while index < len(real_time_data):
            if None == real_time_data[index]:
                real_time_data[index] = 0
            index = index + 1
        return real_time_data
    def classify_motion(self, real_time_data):
        # real_time_data = self.__self_pre_process_data(real_time_data)
        # Reshape the input data to match the model's input shape (1, 50, 4)
        input_data = np.reshape(real_time_data, (1, 50, 4))

        # Perform inference
        prediction = self.model.predict(input_data)

        # Assuming 'L' is represented by class 0, check if it's predicted as 'L'
        predicted_class = np.argmax(prediction, axis=1)  # Get the class with the highest probability
        predicted_class_index = predicted_class[0]  # Extract the scalar index

        result = self.__class_labels[predicted_class_index]  # Get the class labe
        return result
    
"""
# This portion is written for self tests.
# Example usage with real-time data
Loader = BleModelLoader('MotionDetectionLSTM.keras')

import pandas as pd

def read_excel_data(file_path, sheet_name=0):
    # Read the Excel file into a DataFrame
    df = pd.read_excel(file_path, sheet_name=sheet_name)
    
    # Initialize a list to hold the data
    data = []

    # Iterate through the DataFrame rows
    for index, row in df.iterrows():
        # Convert the row to a list and ensure it has 200 values
        values = row.tolist()
        data.append(values)
    return data

# Replace 'data.xlsx' with your actual Excel file path
file_path = '0_0_TrainingData.xlsx'
data_list = read_excel_data(file_path)


# Classify the motion
indx = 1
while indx < len(data_list):
    list_val = data_list[indx]
    # print(list_val)
    result = Loader.classify_motion(list_val[:-1])
    print("Real:",list_val[-1],"Index:", indx, "Motion:", result)
    indx = indx + 1
"""