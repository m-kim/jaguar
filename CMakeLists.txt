cmake_minimum_required(VERSION 3.0...3.25)
project(
    jaguar 
    VERSION 0.1.0 
    LANGUAGES CXX 
)

link_directories(/Users/mark/AppData/Local/wgpu-windows-x86_64-release/)


find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
add_subdirectory(src)
add_subdirectorY(app)

