set(SOURCE_LIST
        main.cpp        
        )

add_executable(renderer main.cpp)
target_include_directories(renderer PUBLIC ../ ../include )
set_target_properties(renderer PROPERTIES
    CXX_STANDARD 20
    COMPILE_WARNING_AS_ERROR ON
)
target_link_libraries(renderer PRIVATE jaguar wgpu_native userenv ws2_32 d3dcompiler D3D12 DXGI Dwmapi Bcrypt NtDll)