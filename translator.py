import os
import json
import time
import cv2
import numpy as np
from tflite_runtime.interpreter import Interpreter

# ---------------- CONFIGURATION ----------------
# Update these paths to match where you placed the files on your Pi
MODEL_PATH = "sign6_int8.tflite"
LABELS_PATH = "labels_bn.json"

# Thresholds for stability and confidence
CONF_THRESHOLD = 0.55
STABLE_FRAMES_REQUIRED = 5 

# ---------------- INITIALIZATION ----------------
# 1. Load Labels
with open(LABELS_PATH, "r", encoding="utf-8") as f:
    meta = json.load(f)
CLASSES = meta["classes"]
BN_WORD = meta["bn_word"]
BN_MSG  = meta["bn_msg"]

# 2. Setup TFLite Interpreter
interpreter = Interpreter(model_path=MODEL_PATH)
interpreter.allocate_tensors()

input_details = interpreter.get_input_details()[0]
output_details = interpreter.get_output_details()[0]

# Get model requirements (96x96 and Quantization params)
in_h, in_w = input_details['shape'][1], input_details['shape'][2]
in_scale, in_zero = input_details["quantization"]
out_scale, out_zero = output_details["quantization"]

# 3. Setup Camera (CSI Camera uses index 0)
cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

def speak_bengali(text):
    """Uses espeak-ng to output Bengali audio through the USB speaker."""
    print(f"Speaking: {text}")
    # -v bn selects Bengali voice
    os.system(f'espeak-ng -v bn "{text}"')

# ---------------- MAIN LOOP ----------------
print("System Ready. Starting Recognition...")

last_speech_time = 0
stable_count = 0
last_idx = -1

try:
    while True:
        ret, frame = cap.read()
        if not ret:
            break

        # Preprocessing: Center crop to square and resize
        h, w = frame.shape[:2]
        side = min(h, w)
        y0, x0 = (h - side) // 2, (w - side) // 2
        crop = frame[y0:y0+side, x0:x0+side]
        resized = cv2.resize(crop, (in_w, in_h), interpolation=cv2.INTER_AREA)
        
        # Color conversion and Quantization for INT8 model
        rgb = cv2.cvtColor(resized, cv2.COLOR_BGR2RGB).astype(np.float32)
        q = np.round(rgb / in_scale + in_zero)
        q = np.clip(q, -128, 127).astype(np.int8)
        input_data = np.expand_dims(q, axis=0)

        # Inference
        interpreter.set_tensor(input_details["index"], input_data)
        interpreter.invoke()
        output_data = interpreter.get_tensor(output_details["index"])[0]
        
        # Dequantize output
        probs = (output_data.astype(np.float32) - out_zero) * out_scale
        idx = np.argmax(probs)
        conf = probs[idx]

        # Stability Logic
        if conf > CONF_THRESHOLD:
            if idx == last_idx:
                stable_count += 1
            else:
                stable_count = 1
                last_idx = idx
            
            # If sign is held long enough and we haven't spoken too recently (2s cooldown)
            if stable_count >= STABLE_FRAMES_REQUIRED and (time.time() - last_speech_time > 2.0):
                bengali_message = BN_MSG[idx]
                speak_bengali(bengali_message)
                last_speech_time = time.time()
                stable_count = 0 # Reset to avoid repeated speaking of same hold
        else:
            stable_count = 0
            last_idx = -1

        # Small delay to keep Pi Zero 2W from overheating
        time.sleep(0.01)

except KeyboardInterrupt:
    print("Stopping...")
finally:
    cap.release()