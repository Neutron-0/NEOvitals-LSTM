import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("ecg_data.csv")

plt.figure(figsize=(12,4))
plt.plot(df["ecg"])
plt.title("Raw ECG")
plt.show()