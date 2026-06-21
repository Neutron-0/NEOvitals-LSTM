import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("ecg_data.csv")

fs = 250  # approximate

start = 0
end = fs * 5   # 5 seconds

plt.figure(figsize=(12,4))
plt.plot(df["ecg"][start:end])
plt.title("ECG - First 5 Seconds")
plt.xlabel("Sample")
plt.ylabel("ADC")
plt.grid(True)
plt.show()