cmake_minimum_required(VERSION 4.0)
project(ImageEditor)

set(CMAKE_CXX_STANDARD 23)

add_executable(ImageEditor src/main.cpp
        include/Core/Application.hpp
        src/Core/Application.cpp
        include/Core/Image.hpp
        src/Core/Image.cpp
        include/Filters/Filter.hpp
        src/Filter/Filter.cpp
        "include/Filters/Edge Detection.hpp"
        include/Filters/Sobolev.hpp
        include/Core/Pixel.hpp
        thirdparty/stb_image.cpp
        include/Core/Window.hpp
        src/Core/Window.cpp
)
