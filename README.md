# Image Editor

A modern, cross-platform Image Editor written in **C++23**, built with a clean and modular architecture.  
The project uses **CMake** as its build system and **GTKmm** for the graphical user interface.

The goal of this project is to provide a lightweight, extensible, and maintainable image editing application with a strong focus on code quality and portability.

---

## ✨ Features

- 📂 Open and save images
- 🖌️ Rotate and flip
- 🎨 Apply filters to image
- 📊 Image Histogram 
- 🔍 Zoom and image navigation
- 🧭 GTKmm-based graphical user interface
- 🗂️ Modular and extensible architecture
- ⚙️ Cross-platform build system using CMake
- 🧩 Clean and maintainable C++ codebase


---

## 🖥️ Requirements

The project requires:

- ✅ Operating System:
    - Linux
    - Windows
    - macOS
- ✅ CMake (recent version recommended)
- ✅ Modern C++ compiler (C++17 or newer)
    - GCC
    - Clang
    - MSVC
- ✅ GTKmm installed on your system

---

## 📦 Building the Project

### 1️⃣ Clone the repository

```bash
git clone https://github.com/your-username/image-editor.git
cd image-editor
```

### 2️⃣ Build
```bash
mkdir build
cd build

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

cmake --build build

cmake --install build
```