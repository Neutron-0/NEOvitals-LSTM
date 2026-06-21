import numpy as np
import pandas as pd

duration_minutes = 30

base_hr = 72.0
total_beats = int(duration_minutes * base_hr)

time_sec = []
rr_ms = []
bpm = []

current_time = 0.0

for beat in range(total_beats):

    # Respiratory sinus arrhythmia (~0.25 Hz breathing)
    breathing_effect = 40 * np.sin(2 * np.pi * beat / 18)

    # Slow autonomic drift
    slow_drift = 20 * np.sin(2 * np.pi * beat / 400)

    # Random beat-to-beat variability
    noise = np.random.normal(0, 12)

    rr = 833 + breathing_effect + slow_drift + noise

    rr = np.clip(rr, 700, 1000)

    hr = 60000 / rr

    current_time += rr / 1000

    time_sec.append(current_time)
    rr_ms.append(rr)
    bpm.append(hr)

df = pd.DataFrame({
    "time_sec": time_sec,
    "rr_interval_ms": rr_ms,
    "heart_rate_bpm": bpm
})

df.to_csv("resting_hr_data.csv", index=False)

print(df.head())
print()
print("Mean HR:", round(df["heart_rate_bpm"].mean(), 2))
print("Min HR :", round(df["heart_rate_bpm"].min(), 2))
print("Max HR :", round(df["heart_rate_bpm"].max(), 2))