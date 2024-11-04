import numpy as np
import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import LabelEncoder
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Conv1D, MaxPooling1D, Flatten, Dense, Dropout
from tensorflow.keras.utils import to_categorical
import matplotlib.pyplot as plt

class CNNModel:
    def __init__(self, training_file):
        self.training_file = training_file

    def train(self):
        try:
            df = pd.read_excel(self.training_file)
            df = df.sample(frac=1).reset_index(drop=True)  # Shuffle the data
        except:
            print('Training file cannot be opened')
            exit()

        # Extract motion data (first 200 columns) and target labels (last column)
        X = df.iloc[:, :-1].values  # First 200 columns
        y = df.iloc[:, -1].values   # Last column (motion label)

        # Reshape X into (num_samples, 50, 4)
        X = X.reshape((X.shape[0], 50, 4))

        # Encode the labels
        label_encoder = LabelEncoder()
        y_encoded = label_encoder.fit_transform(y)
        y_categorical = to_categorical(y_encoded)

        # Split the data into training and testing sets
        X_train, X_test, y_train, y_test = train_test_split(X, y_categorical, test_size=0.2, random_state=42)

        # Define the 1D CNN model
        model = Sequential()
        model.add(Conv1D(filters=64, kernel_size=3, activation='relu', input_shape=(50, 4)))
        model.add(MaxPooling1D(pool_size=2))
        model.add(Dropout(0.5))
        model.add(Conv1D(filters=128, kernel_size=3, activation='relu'))
        model.add(MaxPooling1D(pool_size=2))
        model.add(Dropout(0.5))
        model.add(Flatten())
        model.add(Dense(64, activation='relu'))
        model.add(Dense(y_categorical.shape[1], activation='softmax'))  # Output layer

        # Compile the model
        model.compile(optimizer='adam', loss='categorical_crossentropy', metrics=['accuracy'])

        # Train the model
        history = model.fit(X_train, y_train, epochs=10, batch_size=16, validation_data=(X_test, y_test))

        # Save the trained model
        model.save('MotionDetection_1DCNN.keras')

        # Evaluate the model on the test set
        test_loss, test_accuracy = model.evaluate(X_test, y_test)
        print(f"Test Loss: {test_loss}")
        print(f"Test Accuracy: {test_accuracy}")

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
    # CNN training
    model = CNNModel("0_0_TrainingData.xlsx")
    model.train()

if __name__ == "__main__":
    main()
