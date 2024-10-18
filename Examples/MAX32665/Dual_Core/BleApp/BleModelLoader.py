import numpy as np
from tensorflow.keras.models import load_model

# Load the saved model
model = load_model('MotionDetectionModel.h5')

def classify_motion(real_time_data):
    """
    real_time_data: np.array of shape (50, 4) with motion data [timestamp, x_val, y_val, z_val]
                    representing one motion sequence
    Returns:
        Prediction result: 'L' for left motion or 'Not L'
    """
    
    # Reshape the input data to match the model's input shape (1, 50, 4)
    input_data = np.reshape(real_time_data, (1, 50, 4))
    print(input_data)
    # Perform inference
    prediction = model.predict(input_data)

    # Assuming 'L' is represented by class 0, check if it's predicted as 'L'
    predicted_class = np.argmax(prediction, axis=1)  # Get the class with the highest probability

    if predicted_class == 0:
        return 'L'
    else:
        return 'Not L'

# Example usage with real-time data
# Assume `new_motion_data` is a NumPy array of shape (50, 4) representing real-time motion
new_motion_data = np.random.rand(50, 4)  # Replace this with real-time sensor data
i = 0
new_motion_data = []
while i < 200:
    new_motion_data.append(0)
    i = i+1



# Classify the motion
result = classify_motion(new_motion_data)
print(f"The motion is: {result}")

