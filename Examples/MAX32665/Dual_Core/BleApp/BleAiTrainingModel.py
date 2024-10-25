import numpy as np
import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import LabelEncoder
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import LSTM, Dense, Masking
from tensorflow.keras.utils import to_categorical
from sklearn.model_selection import KFold
import matplotlib.pyplot as plt


class Model:
    def __init__(self,model, training_file):
        self.model = model
        self.training_file = training_file
        self.__supported_models = ["LSTM","LSTMKFold"]
        
    def get_supported_model_list(self):
        return self.__supported_models

    def change_model(self,model_name):
        self.model = model_name
    
    def train(self):
        if self.model == self.__supported_models[0]:
            self.__train_lstm()
        elif self.model == self.__supported_models[1]:
            self.__train_lstm_kfold()
        else:
            print("Only these models supported:", self.get_supported_model_list())
    
    def __train_lstm_kfold(self):
        try:
            df = pd.read_excel(self.training_file)
            # Shuffle the data
            df = df.sample(frac=1).reset_index(drop=True)
        except:
            print('Training file cannot be opened')
            exit()

        # Extract motion data (first 200 columns) and target labels (last column)
        X = df.iloc[:, :-1].values  # First 200 columns
        y = df.iloc[:, -1].values   # Last column (motion label)

        # Reshape X into (num_samples, 50, 4) where each sample has 50 motion packets and each packet has 4 values
        X = X.reshape((X.shape[0], 50, 4))

        # Encode the labels (assuming they are categorical)
        label_encoder = LabelEncoder()
        y_encoded = label_encoder.fit_transform(y)
        y_categorical = to_categorical(y_encoded)

        # Set up K-Fold Cross-Validation
        kfold = KFold(n_splits=5, shuffle=True, random_state=42)
        fold_no = 1
        accuracy_list = []

        for train_index, test_index in kfold.split(X):
            print(f'Training fold {fold_no}...')

            X_train, X_test = X[train_index], X[test_index]
            y_train, y_test = y_categorical[train_index], y_categorical[test_index]

            # Define the LSTM model
            model = Sequential()
            model.add(Masking(mask_value=0.0, input_shape=(50, 4)))  # Mask layer to ignore zeros
            model.add(LSTM(64, input_shape=(50, 4), activation='tanh', return_sequences=False))  # LSTM layer
            model.add(Dense(32, activation='relu'))  # Dense layer
            model.add(Dense(y_categorical.shape[1], activation='softmax'))  # Output layer

            # Compile the model
            model.compile(optimizer='adam', loss='categorical_crossentropy', metrics=['accuracy'])

            # Train the model
            history = model.fit(X_train, y_train, epochs=15, batch_size=16, validation_data=(X_test, y_test), verbose=1)

            # Evaluate the model on the test set
            test_loss, test_accuracy = model.evaluate(X_test, y_test)
            print(f'Test Loss: {test_loss}')
            print(f'Test Accuracy: {test_accuracy}')

            accuracy_list.append(test_accuracy)
            fold_no += 1

        # Calculate and print the average accuracy across all folds
        average_accuracy = np.mean(accuracy_list)
        print(f'Average Test Accuracy across {kfold.n_splits} folds: {average_accuracy:.4f}')

        # Save the last trained model (if you want to save the final model after K-Fold)
        model.save('MotionDetection'+self.model+'.keras')        
    

    def __train_lstm(self):
        try:
            df = pd.read_excel(self.training_file)
        except:
            print('Training file can not be opened')
            exit()

        # Shuffle the rows to prevent overfitting for one motion.
        df = df.sample(frac=1).reset_index(drop=True)

        # Extract motion data (first 200 columns) and target labels (last column)
        X = df.iloc[:, :-1].values  # First 200 columns
        y = df.iloc[:, -1].values   # Last column (motion label)

        # Reshape X into (num_samples, 50, 3) where each sample has 50 motion packets and each packet has 4 values
        X = X.reshape((X.shape[0], 50, 4))

        # Encode the labels (assuming they are categorical)
        label_encoder = LabelEncoder()
        y_encoded = label_encoder.fit_transform(y)

        y_categorical = to_categorical(y_encoded)

        # Split the data into training and testing sets
        X_train, X_test, y_train, y_test = train_test_split(X, y_categorical, test_size=0.2, random_state=42)

        # Define the LSTM model
        model = Sequential()
        model.add(Masking(mask_value=0.0, input_shape=(50, 4))) 
        model.add(LSTM(64, input_shape=(50, 4), activation='tanh', return_sequences=False))  # LSTM layer
        model.add(Dense(32, activation='relu'))  # Dense layer
        model.add(Dense(y_categorical.shape[1], activation='softmax'))  # Output layer

        # Compile the model
        model.compile(optimizer='adam', loss='categorical_crossentropy', metrics=['accuracy'])

        # Train the model
        history = model.fit(X_train, y_train, epochs=10, batch_size=16, validation_data=(X_test, y_test))

        # Save model in a file.
        model.save('MotionDetectionLSTM.keras')

        # Evaluate the model on the test set
        test_loss, test_accuracy = model.evaluate(X_test, y_test)
        print(f"Test Loss: {test_loss}")
        print(f"Test Accuracy: {test_accuracy}")

        """
        # exit()
        # Self predicting
        # import pandas as pd

        # def read_excel_data(file_path, sheet_name=0):
        #     # Read the Excel file into a DataFrame
        #     df = pd.read_excel(file_path, sheet_name=sheet_name)

        #     # Initialize a list to hold the data
        #     data = []

        #     # Iterate through the DataFrame rows
        #     for index, row in df.iterrows():
        #         # Convert the row to a list and ensure it has 200 values
        #         values = row.tolist()
        #         data.append(values)
        #     return data

        # # Replace 'data.xlsx' with your actual Excel file path
        # # file_path = '0_0_TrainingData.xlsx'
        # # data_list = read_excel_data(file_path)
        # # class_labels = ['D', 'L', 'N', 'R', 'U']
        # # # Classify the motion
        # # indx = 1
        # # while indx < len(data_list):
        # #     list_val = data_list[indx][:-1]
        # #     list_val = np.reshape(list_val,(1,50,4))
        # #     # print(list_val)
        # #     result = model.predict(list_val)
        # #     predicted_class_idx = np.argmax(result, axis=1) 
        # #     print("Index:", indx,"Motion:", class_labels[predicted_class_idx[0]])
        # #     indx = indx + 1
        """

        # Plot accuracy
        plt.plot(history.history['accuracy'], label='train accuracy')
        plt.plot(history.history['val_accuracy'], label='test accuracy')
        plt.title('Model Accuracy')
        plt.ylabel('Accuracy')
        plt.xlabel('Epoch')
        plt.legend()
        plt.show()

        # Plot loss
        plt.plot(history.history['loss'], label='train loss')
        plt.plot(history.history['val_loss'], label='test loss')
        plt.title('Model Loss')
        plt.ylabel('Loss')
        plt.xlabel('Epoch')
        plt.legend()
        plt.show()

def main():
    # LSTM training
    model = Model("LSTM","0_0_TrainingData.xlsx")
    model.train()

if __name__ == "__main__":
    main()