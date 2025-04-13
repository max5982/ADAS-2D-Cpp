# Surround View Monitoring System (2D-SVM)

A C++-& OpenCV based 2D Surround View Monitoring (SVM) system designed for real-time 360-degree visualization using four fisheye lens cameras.

## Overview

This project fuses four camera streams to produce a stitched top-down bird’s-eye view of the vehicle's surroundings. Key components include:
- Real-time frame capture and synchronization
- Fisheye distortion correction
- Perspective transformation and stitching
- OpenCV-based rendering

## Development Environment

- **Language:** C++
- **Dependencies:**
  - OpenCV 4.6+
- **Build System:** CMake (>= 3.10)
- **Target OS:** Ubuntu 22.04 LTS

## Test System

- **CPU:** Intel 13th Platform
- **GPU:** Anything
- **Camera Setup:** 4x USB cameras (1920x1080@30, fisheye lenses) or 4x MIPI cameras (1920x1080@30, fisheye lenses)
- **Display Resolution:** 1920x1080 or wider (e.g., 2560x1080 for automotive layouts)

## Prerequisites
```bash
sudo apt-get install build-essential cmake
sudo apt-get install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools
sudo apt-get install qtcreator
sudo apt-get install qt5*
sudo apt-get install libv4l-dev
# OpenVINO
wget https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB
sudo gpg --output /etc/apt/trusted.gpg.d/intel.gpg --dearmor GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB
echo "deb https://apt.repos.intel.com/openvino ubuntu22 main" | sudo tee /etc/apt/sources.list.d/intel-openvino.list
sudo apt update
sudo apt install openvino
```

## Installation

```bash
# Clone repository
git clone https://github.com/max5982/ADAS-2D-Cpp.git
cd ADAS-2D-Cpp

# Build
mkdir build && cd build
cmake ..
make -j$(nproc)

# How to run
./ADAS
```
