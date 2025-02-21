cmake_minimum_required(VERSION 3.30.0)
project(oneko VERSION 1.0.0 LANGUAGES CXX)

# set c++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

add_executable(oneko
    oneko.cc
)

target_link_libraries(oneko PRIVATE
    SDL2::SDL2
)

target_include_directories(oneko PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(MSVC)
    target_compile_options(oneko PRIVATE /W4)
else()
    target_compile_options(oneko PRIVATE -Wall -Wextra -pedantic)
endif()
