cmake_minimum_required(VERSION 3.15)
project(oneko VERSION 1.0.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
pkg_check_modules(WAYLAND REQUIRED wayland-client wayland-protocols)

# Create protocol library
add_library(wayland-protocols STATIC
    wlr-layer-shell-unstable-v1-protocol.c
    xdg-shell-protocol.c
)

# Set C compiler flags for protocol library
set_target_properties(wayland-protocols PROPERTIES
    COMPILE_FLAGS "-fPIC"
    C_VISIBILITY_PRESET "default"  # Changed from "hidden" to "default"
)

target_include_directories(wayland-protocols PUBLIC
    ${WAYLAND_INCLUDE_DIRS}
)

# Add main executable
add_executable(oneko
    oneko.cc
)

# Link libraries
target_link_libraries(oneko PRIVATE
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${WAYLAND_LIBRARIES}
    wayland-protocols
)

# Include directories
target_include_directories(oneko PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${WAYLAND_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set warning levels
if(MSVC)
    target_compile_options(oneko PRIVATE /W4)
else()
    target_compile_options(oneko PRIVATE -Wall -Wextra -Wpedantic)
endif()
