# Automated Lunar Crater Detection from LROC PDS/RDR Images

We present a **C++ pipeline** designed for the automated detection and categorization of lunar craters using high-resolution TIFF images from the *Lunar Reconnaissance Orbiter Camera (LROC)*. The system utilizes the **Circle Hough Transform (CHT)** to identify circular features, categorizes them by size, and manages large-scale planetary data (files up to 3GB+) through an optimized downsampling and re-scaling approach.

**Keywords**: OpenCV, C++, Lunar Crater Detection, LROC PDS, Hough Circles, Planetary Data, Image Processing.

---

## Purpose of the Project

* **Automated Identification**: Detecting craters across varying lunar terrains (DTM, SHADE, NAC).
* **Size Categorization**: Classifying craters into four main categories: Small, Medium, Large, and Extra-Large.
* **Big Data Handling**: Efficiently processing massive TIFF files (3GB+) that exceed standard RAM limits.
* **Data Export**: Generating a structured CSV database containing coordinates (X, Y), radii, and diameters for further geological analysis.

---

## General Structure

### 1. main2.cpp – Project Entry Point & Batch Processor
This module manages the entire workflow:
* Configures the environment to bypass standard OpenCV pixel limits (`OPENCV_IO_MAX_IMAGE_PIXELS`).
* Automatically scans the `imagini_luna/` directory for all `.TIF` files.
* Implements a `try-catch` safety block to prevent program crashes on corrupted or unsupported files.
* Uses `IMREAD_REDUCED_COLOR_8` for memory-efficient loading (1/8 scale).

### 2. Detection Modules (Categorization Functions)
The detection logic is split into specialized functions based on crater radii:
* `CratereMici()`: Detects small craters (scaled radius 20-80 px).
* `CratereMijlocii()`: Detects medium features (scaled radius 80-150 px).
* `CratereMari()`: Detects large craters (scaled radius 150-200 px).
* `CratereFoarteMari()`: Detects massive lunar structures (scaled radius 200-500 px).

### 3. Coordinate & Scale Management
Since detection is performed on downsampled images (1/8 scale) for performance:
* Centroids (X, Y) are automatically **re-scaled by a factor of 8** before export.
* Radii and Diameters are converted back to original pixel units for spatial accuracy.

---

## Input Requirements

* **Source**: LROC PDS (RDR) Datasets.
* **Format**: `.TIF` / `.tiff` (Digital Terrain Models or Shaded Relief images).
* **File Size**: Supports files from 20MB up to **4GB**.

---

## Main Steps

1.  **Memory Optimization**: Setting `setenv` to allow processing of images up to 2GB of total pixels.
2.  **Sub-sampling**: Loading images at 1/8 resolution to ensure compatibility with standard hardware (e.g., MacBook RAM).
3.  **Pre-processing**: 
    * Grayscale conversion.
    * **Min-Max Normalization**: Crucial for DTM/SHADE images to enhance crater rim contrast.
    * **Gaussian Blurring**: Reducing sensor noise to prevent false positive detections.
4.  **Multi-Stage Detection**: Running four concurrent Hough Circle passes with adaptive thresholds.
5.  **Data Scaling**: Transforming local pixel coordinates to global image-space coordinates.
6.  **Output Generation**:
    * Saving detection overlays as `detectat_X.jpg` for visual verification.
    * Exporting a global `rezultate_cratere.csv` file.

---

## Example of Results

Sample detection images and the full CSV database containing the processed results can be found in the [Sample_Results/](./Sample_Results/) directory.

### Sample CSV Data Structure:
The system generates a report where all measurements are mapped back to the **original image resolution**:

| Imagine | Categorie | Centru_X_Original | Centru_Y_Original | Raza_Originala_Pixeli | Diametru_Original_Pixeli |
| :--- | :--- | :--- | :--- | :--- | :--- |
| NAC_DTM_NASSAU03.TIF | Mic | 4840 | 15872 | 412 | 824 |
| NAC_POLE_SOUTH.TIF | Mediu | 12450 | 8920 | 960 | 1920 |

---

## How to Compile and Run

To compile the project on a system with **OpenCV 4** installed:

```bash
# Compile
g++ main2.cpp -o main1 `pkg-config --cflags --libs opencv4`

# Run
./main1
