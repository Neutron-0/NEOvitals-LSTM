# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Frequently Used Commands

| Task | Command | Notes |
|------|---------|-------|
| **Install Python dependencies** | `pip install -r requirements.txt` *(if a `requirements.txt` is added)* | The project primarily uses `tensorflow`, `numpy`, `pandas`, `scipy`, and `matplotlib`. Adding a requirements file is recommended.
| **Train the LSTM model** | `python train.py` | Reads `training_data.txt` and `hr_data.txt`, trains `csi_hr.keras`, and saves the model. Ensure the training data files are present.
| **Run inference (real‑time heart‑rate)** | `python read_and_process_csi.py -p <SERIAL_PORT>` | Replace `<SERIAL_PORT>` with the COM port of the ESP32 receiver (e.g., `COM3`). The script streams CSI, applies the 5‑step DSP pipeline, and outputs BPM.
| **Collect ground‑truth training data** | `python read_and_process_csi.py -p <PORT> -s csi_data.csv -l csi_log.txt` (modify flags in the script if needed) | Set `COLLECT_TRAINING_DATA = True` at the top of the script, then run to generate `training_data.txt`.
| **Record raw ECG** | `python record_ecg.py` | Uses `COM3` at 115200 baud, records 30 minutes, writes `ecg_data.csv`.
| **Plot recorded ECG** | `python plot_ecg.py` | Reads `ecg_data.csv` and displays a Matplotlib window (first minute by default).
| **Flash ESP32 firmware** | `docker run --rm -v "$(pwd):/src" espressif/idf:latest idf.py flash -p <PORT>` | Firmware files are the `.ino` sketches (`basic-monitor.ino`, `ECG‑AD8236.ino`, `ECG‑h‑sensor.ino`). The project recommends using the Espressif IDF Docker image for reproducibility.
| **Run synthetic ECG generator** | `python synth_resting-phase.py` | Generates a CSV `resting_hr_data.csv` with a plausible heart‑rate time series.
| **Lint / format** | `flake8 .` and `black .` | Python code follows PEP 8; run these to enforce style before committing.
| **Run unit tests (if added)** | `pytest` | Currently no test suite is present; create tests under `tests/` and use this command.

## High‑Level Architecture

The repository implements a **Wi‑Fi CSI‑based contactless heart‑rate monitor** with three logical layers:

1. **Hardware Layer** – Two ESP32 boards (Tx & Rx) acquire raw Channel State Information (CSI). Optional Arduino Nano + MAX30102 pair provides ground‑truth heart‑rate for training.
2. **Signal‑Processing Layer** – Implemented in `read_and_process_csi.py`:
   - **Amplitude conversion** – raw complex CSI → magnitude array.
   - **Stationary noise removal** – high‑pass Butterworth filter (2–5 Hz).
   - **Pulse extraction** – band‑pass filter (0.8–2.17 Hz) isolates cardiac frequencies.
   - **Pulse shaping** – Savitzky‑Golay smoothing.
   - **Segmentation** – sliding 100‑sample windows (≈5 s) fed to the neural net.
3. **Machine‑Learning Layer** – `train.py` builds a stacked LSTM (64 → 32 units) with dropout regularization, trained on paired CSI‑BPM data. The trained model (`csi_hr.keras`) is loaded by the inference script to output real‑time BPM.

### Supporting Utilities
- **ECG Capture** (`record_ecg.py`, `plot_ecg.py`): useful for baseline comparison and debugging.
- **Synthetic Data Generator** (`synth_resting-phase.py`): produces realistic heart‑rate sequences for testing pipelines.
- **Arduino Sketches** (`basic-monitor.ino`, `ECG‑AD8236.ino`, `ECG‑h‑sensor.ino`): firmware for the ESP32 transmitter/receiver and a simple ECG front‑end.

## Development Guidelines (project‑specific)
- Keep the Python environment isolated (virtualenv/conda). Add a `requirements.txt` when new libraries are introduced.
- The ESP32 firmware should be built and flashed via the Docker command above to avoid host‑specific toolchain issues.
- When modifying the DSP pipeline, update the corresponding unit tests (once they exist) and verify inference performance on a small validation set.
- Follow the **Roadmap** in `blueprint/README.md` for the long‑term feature plan; prioritize robust signal processing before expanding ML models.
- All code should retain the existing comment density and naming conventions (snake_case for Python, ALL_CAPS for Arduino constants).

## Repository Structure (quick glance)
```
├─ basic-monitor.ino          # ESP32 Rx firmware with BPM calculation
├─ ECG-AD8236.ino            # Simple ECG acquisition sketch
├─ ECG-h-sensor.ino          # Placeholder for sensor firmware
├─ read_and_process_csi.py   # Core CSI pipeline + inference
├─ train.py                  # LSTM model training script
├─ synth_resting-phase.py     # Synthetic HR generator
├─ record_ecg.py             # Serial ECG logger
├─ plot_ecg.py               # Matplotlib visualizer
├─ media/                    # Architecture diagrams
├─ blueprint/README.md       # Development roadmap & vision
└─ README.md                 # End‑user overview (already present)
```

*Future Claude Code instances* should refer to this file for command shortcuts, architectural context, and the recommended development workflow.
