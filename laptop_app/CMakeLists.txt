cmake_minimum_required(VERSION 3.16)
project(ImageToPaths LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

add_executable(image_to_paths main.cpp)
target_include_directories(image_to_paths PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(image_to_paths PRIVATE ${OpenCV_LIBS})