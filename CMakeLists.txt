cmake_minimum_required(VERSION 3.16)
project(oneko)

# Force Qt6 path
set(Qt6_DIR "/usr/lib/x86_64-linux-gnu/cmake/Qt6")
set(Qt6Widgets_DIR "/usr/lib/x86_64-linux-gnu/cmake/Qt6Widgets")
set(Qt6Core_DIR "/usr/lib/x86_64-linux-gnu/cmake/Qt6Core")

# Basic setup
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)

# Find Qt packages explicitly
find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)

add_executable(oneko WIN32
    main.cc 
    oneko.cc
)

target_link_libraries(oneko PRIVATE Qt6::Core Qt6::Widgets)