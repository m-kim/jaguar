set(SOURCE_LIST
    Application.cpp
    ApplicationWin.cpp
    Camera.cpp
    ColorBuffer.cpp
    DepthBuffer.cpp        
    Shader.cpp
)

set(HEADER_LIST
../include/Application.h
../include/Camera.h  
../include/ColorBuffer.h             
../include/GpuBuffer.h        
../include/GpuResource.h                
../include/PixelBuffer.h
../include/Shader.h  
)

set(LIBRARY_LIST glfw glm::glm)

add_library(jaguar ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(jaguar PUBLIC ../ ../include  /Users/mark/AppData/Local/wgpu-windows-x86_64-release/ )
set_target_properties(jaguar PROPERTIES CXX_STANDARD 20)
target_link_libraries(jaguar
        ${LIBRARY_LIST})