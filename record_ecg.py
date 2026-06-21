import serial
import pandas as pd
import time

PORT = "COM3"
BAUD = 115200

ser = serial.Serial(PORT, BAUD)

samples = []

start = time.time()

print("Recording for 30 seconds...")

while time.time() - start < 30:
    try:
        line = ser.readline().decode().strip()

        value = int(line)

        samples.append(value)

    except:
        pass

df = pd.DataFrame({"ecg": samples})

df.to_csv("ecg_data.csv", index=False)

print(f"Saved {len(df)} samples")