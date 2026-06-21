# NEOvitals Architecture & Technical Specifications

**NEOvitals** operates via a highly optimized, dual-stage framework that maps imperceptible Wi-Fi signal perturbations directly to physiological cardiac rhythms. This document details the technical specifications of the hardware integration, signal processing, and deep learning stages.

---

## 1. Hardware Integration

The hardware architecture relies on a specialized division of responsibility between the inference system and the ground-truth collection subsystem.

### ESP32 Wi-Fi Node Configuration
- **Spatial Placement:** The system utilizes two ESP32 microcontrollers. The **Transmitter (Tx)** and **Receiver (Rx)** nodes must be positioned securely, typically 1 to 2 meters apart, ensuring the human subject's chest cavity is situated directly within or slightly offset from the Line-of-Sight (LoS) path.
- **Communication:** The Tx node continuously broadcasts CSI packets. The Rx node intercepts these perturbed packets and transmits the raw CSI data over a high-speed serial connection (921600 baud) directly to the host machine running `read_and_process_csi.py`.

### Ground-Truth Subsystem Decoupling
- **Isolated Validation:** An Arduino Nano 33 IoT paired with a MAX30102 pulse oximetry sensor serves *exclusively* for ground-truth data collection during the training phase.
- **Independence:** This subsystem is entirely decoupled from the real-time inference loop. Once the LSTM model is trained, the Arduino/MAX30102 hardware is no longer required for system operation.

---

## 2. The 5-Step Digital Signal Processing (DSP) Pipeline

Raw CSI matrices contain significant environmental noise, high-frequency interference, and phase offsets. The data passes through a mathematical DSP pipeline to isolate the cardiac wave.

### Step 1: Amplitude Conversion
Complex raw CSI subcarriers are represented as real and imaginary components. The first step computes the absolute magnitude to convert these complex pairs into usable amplitude values, stripping phase instability.

### Step 2: Stationary Noise Removal
Static reflections from walls and furniture cause a large DC offset in the signal. A 3rd-order Butterworth band-pass filter is applied to strip out these static elements, isolating only dynamic changes in the environment.

### Step 3: Pulse Extraction
The human resting heart rate generally falls between 48 and 130 BPM (0.8 Hz to 2.17 Hz). A targeted zero-phase Butterworth bandpass filter is utilized to strictly isolate this specific micro-frequency band, suppressing breathing harmonics (which occur at lower frequencies) and general limb movement.

### Step 4: Pulse Shaping
High-frequency jitter and artifacts can obscure the extracted pulse. A Savitzky-Golay smoothing filter is applied to the waveform. This specific filter is chosen for its ability to smooth the signal while preserving the shape and height of the peaks—crucial for periodic heartbeat patterns.

### Step 5: Segmentation & Normalization
The continuous stream of shaped CSI amplitudes is chunked into standardized temporal windows. Specifically, a sliding window of 100 sequential packets is aggregated to form a single tensor of shape `(100, 192)`, ensuring uniform input for the neural network.

---

## 3. Deep Learning Layer Topology

The predictive engine of **NEOvitals** is a Long Short-Term Memory (LSTM) Artificial Neural Network. It is designed to interpret the complex temporal features across 192 subcarriers simultaneously over time.

### Network Architecture
The sequential model maps the continuous `(100, 192)` input tensor to a final scalar Heart Rate Estimate (BPM) via the following topology:

1. **LSTM (64 Units):** 
   - `return_sequences=True`
   - Acts as the primary temporal feature extractor, discovering complex sequences and dependencies across the 100 timesteps and 192 spatial subcarriers.
2. **Dropout (0.2):** 
   - Regularization layer specifically included to prevent overfitting on the limited physiological variance of small training datasets.
3. **LSTM (32 Units):** 
   - `return_sequences=False`
   - Refines the previously extracted features into a concentrated temporal representation, dropping the sequence dimension.
4. **Dropout (0.2):** 
   - Secondary regularization layer for robust model generalization.
5. **Dense (16 Units):** 
   - Dimension reduction layer that compresses the high-dimensional LSTM output into dense, higher-level representations.
6. **Activation (ReLU):** 
   - Introduces non-linear pattern recognition, allowing the network to map the dense representations against non-linear physiological trends.
7. **Dense (1 Unit):** 
   - Final output layer without activation (linear mapping), producing the scalar continuous variable: the final Beats Per Minute (BPM) estimate.
