import numpy as np
import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import LabelEncoder
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import LSTM, Dense
from tensorflow.keras.utils import to_categorical
from tensorflow.keras.models import load_model

# Load the Excel file
file_path = '0_TrainingData.xlsx'  # Replace with your file path
df = pd.read_excel(file_path)

# Extract motion data (first 200 columns) and target labels (last column)
X = df.iloc[:, :-1].values  # First 200 columns
y = df.iloc[:, -1].values   # Last column (motion label)

# Reshape X into (num_samples, 50, 4) where each sample has 50 motion packets and each packet has 4 values
X = X.reshape((X.shape[0], 50, 4))

# Encode the labels (assuming they are categorical)
label_encoder = LabelEncoder()
y_encoded = label_encoder.fit_transform(y)
y_categorical = to_categorical(y_encoded)

# Split the data into training and testing sets
X_train, X_test, y_train, y_test = train_test_split(X, y_categorical, test_size=0.2, random_state=42)

# Define the LSTM model
model = Sequential()
model.add(LSTM(64, input_shape=(50, 4), activation='tanh', return_sequences=False))  # LSTM layer
model.add(Dense(32, activation='relu'))  # Dense layer
model.add(Dense(y_categorical.shape[1], activation='softmax'))  # Output layer

# Compile the model
model.compile(optimizer='adam', loss='categorical_crossentropy', metrics=['accuracy'])

# Train the model
history = model.fit(X_train, y_train, epochs=10, batch_size=16, validation_data=(X_test, y_test))

# Save model in a file.
model.save('MotionDetectionModel.h5')

# Evaluate the model on the test set
test_loss, test_accuracy = model.evaluate(X_test, y_test)
print(f"Test Loss: {test_loss}")
print(f"Test Accuracy: {test_accuracy}")

# Plot training history (optional, requires matplotlib)
import matplotlib.pyplot as plt

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
