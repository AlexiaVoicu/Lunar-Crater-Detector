# Automated Lunar Crater Detection from LROC PDS/RDR Images

We present a **C++ pipeline** designed for the automated detection and categorization of lunar craters using high-resolution TIFF images from the *Lunar Reconnaissance Orbiter Camera (LROC)*. The system utilizes the **Circle Hough Transform (CHT)** to identify circular features, categorizes them by size, and manages large-scale planetary data (files up to 3GB+) through an optimized downsampling and re-scaling approach.

**Author**: Voicu Alexia  
**Contact**: alexia.voicu.97@gmail.com   
 **License**: MIT

---

## 1. Requirements & Environment

To ensure reproducibility, the project was developed and tested under the following environment:

* **Operating System**: macOS (tested on Apple Silicon) / Linux (Ubuntu 22.04).
* **Compiler**: GCC/G++ version 9.0+ supporting the **C++17 standard**.
* **Libraries**: **OpenCV 4.x** (modules: `imgcodecs`, `highgui`, `imgproc`).
* **Hardware**: Minimum 8GB RAM recommended for handling large TIFF I/O operations.

---

## 2. Project Structure

```text
.
├── crater_detector.cpp      # Main source code (C++)
├── README.md                # Project documentation
├── LICENSE                  # MIT License file
├── Outputs/                 # Folder containing detection results
│   ├── detected_31.jpg      # Visual verification sample
│   └── crater_detection_results.csv # Compiled database
└── moon_images/             # Input directory (Place .TIF files here)
```
---

## 3. General Structure & Logic

### Detection Modules
The logic is split into specialized functions based on crater radii (scaled for 1/8 resolution). *Note: Functions were renamed from Romanian to English for consistency with the documentation.*
* `detectSmallCraters()`: Targets craters with a scaled radius of 20-80 px.
* `detectMediumCraters()`: Targets features with a scaled radius of 80-150 px.
* `detectLargeCraters()`: Targets features with a scaled radius of 150-200 px.
* `detectExtraLargeCraters()`: Targets massive structures with a scaled radius of 200-500 px.

### Coordinate & Scale Management
Since detection is performed on downsampled images (1/8 scale) to optimize RAM usage:
* **Scaling Factor**: All centroids (X, Y) are automatically **re-scaled by a factor of 8** before export.
* **Accuracy**: Radii and Diameters are converted back to original pixel units to ensure spatial accuracy relative to the source TIFF.

---

## 4. Main Pipeline Steps

1.  **Memory Optimization**: Configures `OPENCV_IO_MAX_IMAGE_PIXELS` to bypass limits and allows processing of images up to 2GB of total pixels.
2.  **Sub-sampling**: Loads images at 1/8 resolution using `IMREAD_REDUCED_COLOR_8` to ensure compatibility with standard hardware.
3.  **Pre-processing**: 
    * **Grayscale Conversion**: Standardizes input for the Hough Transform.
    * **Min-Max Normalization**: Crucial for DTM/SHADE images to enhance crater rim contrast.
    * **Gaussian Blurring**: A $9 \times 9$ kernel reduces sensor noise to prevent false positives.
4.  **Multi-Stage Detection**: Runs four concurrent Hough Circle passes with adaptive thresholds.
5.  **Data Export**: 
    * Saves detection overlays as `detected_X.jpg` for visual verification.
    * Exports a global `crater_detection_results.csv` file.

---

## 5. Usage Instructions

1. **Preparation**: Create a `moon_images/` folder and add your LROC `.TIF` files.
2. **Compilation**:
    ```bash
    g++ -std=c++17 crater_detector.cpp -o crater_detector `pkg-config --cflags --libs opencv4`
    ```
3. **Execution**: 
    ```bash
    ./crater_detector
    ```
4.  **Output**: Results will be generated in the root folder (or `Sample_Results/` if configured).

---

## 6. Limitations & Known Issues

* **False Positives**: High-contrast shadows near steep mountain ridges or rilles may be incorrectly flagged as craters.
* **Overlapping Features**: The Circle Hough Transform (CHT) may struggle to distinguish between heavily degraded or overlapping crater rims.
* **Lighting Sensitivity**: Detection accuracy is higher on **DTM (Digital Terrain Models)** than on Shaded Relief maps due to sun-angle artifacts.
* **Geometric Bias**: The algorithm is optimized for circular features; elliptical impact sites or irregular volcanic calderas may be missed.

---

## 7. Example of Results

The system generates a report where all measurements are mapped back to the **original image resolution**:

| Image | Category | X_Original | Y_Original | Radius_Original | Diameter_Original |
| :--- | :--- | :--- | :--- | :--- | :--- |
| NAC_DTM_NASSAU03.TIF | Mic | 4840 | 15872 | 412 | 824 |
| NAC_POLE_SOUTH.TIF | Mediu | 12450 | 8920 | 960 | 1920 |

---

## 8. Contribution & Contact

This is an academic project developed for planetary surface analysis. For questions, suggestions, or contributions, please open an issue or contact the author at alexia.voicu.97@gmail.com .
