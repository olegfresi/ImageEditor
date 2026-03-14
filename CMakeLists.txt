cmake_minimum_required(VERSION 3.20)
project(ImageEditor)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-4.0)

add_executable(ImageEditor
        include/Core/Profiler.hpp

        src/Core/Application.cpp
        src/Core/Image.cpp
        src/Core/FileManager.cpp
        src/Core/Document.cpp
        src/Core/Window.cpp
        src/Core/Utils.cpp
        src/Core/Pixel.cpp
        src/Core/Processing.cpp

        src/Filter/Blur.cpp
        src/Filter/Sharpen.cpp
        src/Filter/EdgeDetection.cpp
        src/Filter/Emboss.cpp
        src/Filter/ColorInversion.cpp

        src/Math/Convolution.cpp
        src/Math/Gaussian.cpp

        src/Widget/Histogram.cpp
        src/Widget/ToneCurve.cpp

        src/main.cpp
        thirdparty/stb_image.cpp
)

target_link_libraries(ImageEditor PRIVATE PkgConfig::GTKMM)

