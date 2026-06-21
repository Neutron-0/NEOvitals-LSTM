# Contributing to NEOvitals

We welcome contributions to **NEOvitals**! This document provides the guidelines and workflow for contributing to the project to ensure a high-quality, professional standard across all codebase components.

## Code Style & Standards

To maintain an enterprise-grade repository, all code must adhere to the following standards:

- **Python Scripts (Machine Learning & Data Processing):**
  - All Python code (`read_and_process_csi.py`, `train.py`, etc.) must strictly adhere to the [PEP 8](https://peps.python.org/pep-0008/) style guide.
  - Ensure all functions and classes are fully documented with docstrings detailing parameters, return types, and operational logic.
  - Utilize appropriate type hinting to improve code maintainability.

- **C++ Firmware (Arduino & ESP32):**
  - All C++ firmware code must be organized into modular, well-commented structures.
  - Keep hardware-specific implementations encapsulated and separate from core logic.
  - Maintain consistent naming conventions (e.g., camelCase for variables/functions, PascalCase for classes).

## Workflow

1. **Opening Issues:**
   - Before starting major work, please open an Issue outlining the bug, feature request, or architectural change. This allows the maintainers to provide feedback early in the process.

2. **Branching Convention:**
   - Create a new branch for your work from `main`.
   - Use the following naming prefixes for clarity:
     - `feature/` for new capabilities (e.g., `feature/improved-noise-removal`)
     - `fix/` for bug fixes (e.g., `fix/serial-parsing-error`)
     - `docs/` for documentation updates
     - `refactor/` for code structural changes

3. **Submitting Pull Requests (PRs):**
   - Ensure your commits are atomic and have descriptive messages.
   - Reference any relevant issues in your PR description.
   - Wait for a core maintainer to review and approve your PR before merging.

## Testing Requirements

Because **NEOvitals** operates as a continuous, real-time physiological inference pipeline, stability is critical:
- **CSI Data Processing Changes:** Any modifications to the 5-step Digital Signal Processing (DSP) pipeline (Amplitude Conversion, Stationary Noise Removal, Pulse Extraction, Pulse Shaping, Segmentation/Normalization) must be fully documented. You must verify that the altered pipeline still cleanly feeds into the fixed `(100, 192)` LSTM input shape.
- **Model Architecture Changes:** If proposing updates to the deep learning architecture, include a summary of the new layer topology, the rationale, and the impact on inference latency or BPM accuracy.
- **Real-Time Verification:** Ensure that any changes do not break the real-time inference loop (`read_and_process_csi.py`). The system must be able to process 100 sequential CSI packets smoothly without memory leaks or timing bottlenecks.
