# Procedural Terrain Generation Project

## 📌 Overview
This project generates procedural terrain using input Digital Elevation Model (DEM) files. It segments and analyzes terrain data before generating new terrain using noise functions. The final result is a smoothly merged DEM file that can be evaluated using similarity metrics.

## 📂 Contents

1. [Data](#data)  
2. [Segment](#segment)  
3. [Analyze](#analyze)  
4. [Generate](#generate)  
5. [Finalize](#finalize)  
6. [Evaluate](#evaluate)  
7. [How to Run](#how-to-run)

---

### 1️⃣ Data
📁 **Folder:** `Data/`  
This folder stores the input DEM files. Two example files are provided:  
- `bryce_canyon_input.asc`  
- `kings_canyon_input.asc`  

Sources for obtaining DEM files:  
- [United States Geological Survey (USGS)](https://www.usgs.gov/)  
- [ELVIS - Elevation Information System](https://elevation.fsdf.org.au/)  

---

### 2️⃣ Segment
📁 **Folder:** `Segment/`  
Contains the segmentation algorithm implementation using Python’s `skimage` package.  
- Identifies terrain segments  
- Resizes them for infinite terrain generation  
- Exports segments into multiple CSV files  

---

### 3️⃣ Analyze
📁 **Folder:** `Analyze/`  
Analyzes the CSV files of each segment and generates a height distribution function for terrain generation.

---

### 4️⃣ Generate
📁 **Folder:** `Generate/`  
Houses the noise function implementations for terrain generation:  
- `valuenoise.cpp` → Fractal Brownian Motion & Value Noise  
- `simplexnoise.cpp` → Simplex noise algorithm  
- `main.cpp` → Calls the noise algorithms and produces DEM files  

---

### 5️⃣ Finalize
📁 **Folder:** `Finalize/`  
Merges all generated DEM files into a final terrain. A Gaussian blur smoothing function is used to ensure seamless merging, but it can be deactivated if needed.

---

### 6️⃣ Evaluate
📁 **Folder:** `Evaluate/`  
Contains similarity metric implementations for evaluating the generated terrain.  
- A Jupyter Notebook can be used to compare two terrain files.  
- Modify the terrain file names in the second cell and run the notebook.  

---

## 🚀 How to Run

> **This program is only compatible with Windows.**

### ✅ Install Dependencies
1. Install **C++**
2. Install **Conda**
3. Set up the Conda environment:  
   ```sh
   ./setup_conda_env.ps1
   ```

### ▶️ Run the Program
1. Adjust parameters in `runner.ps1`
2. Execute:  
   ```sh
   ./runner.ps1
   ```
3. Evaluate terrain similarity using:  
   ```sh
   python similarity.py
   ```

---

### 📢 Notes
- Ensure all required dependencies are installed before running the program.
- The Gaussian blur function in `Finalize/` is optional and can be disabled.
- DEM files from external sources should match the expected format.

---
