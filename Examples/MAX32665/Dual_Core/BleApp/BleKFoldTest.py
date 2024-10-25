import numpy as np
import pandas as pd
from sklearn.model_selection import KFold
from sklearn.preprocessing import LabelEncoder
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import LSTM, Dense
from tensorflow.keras.utils import to_categorical
from tensorflow.keras.models import load_model

# Load the Excel file
file_path = "0_0_TrainingData.xlsx"  # Replace w\ith your file path
try:
    df = pd.read_excel(file_path)
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
    model.add(LSTM(64, input_shape=(50, 4), activation='tanh', return_sequences=False))  # LSTM layer
    model.add(Dense(32, activation='relu'))  # Dense layer
    model.add(Dense(y_categorical.shape[1], activation='softmax'))  # Output layer

    # Compile the model
    model.compile(optimizer='adam', loss='categorical_crossentropy', metrics=['accuracy'])

    # Train the model
    history = model.fit(X_train, y_train, epochs=10, batch_size=16, validation_data=(X_test, y_test), verbose=1)

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
model.save('MotionDetectionLSTMKfold.keras')