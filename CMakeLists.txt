cmake_minimum_required(VERSION 3.20)
project(ImageEditor)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-4.0)

add_executable(ImageEditor
        src/main.cpp
        src/Core/Application.cpp
        src/Core/Image.cpp
        src/Filter/Filter.cpp
        thirdparty/stb_image.cpp
        src/Core/Window.cpp
        src/Core/Pixel.cpp
        src/Core/Event.cpp
)

target_link_libraries(ImageEditor PRIVATE PkgConfig::GTKMM)