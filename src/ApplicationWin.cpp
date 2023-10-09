#include "ApplicationWin.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <iostream>

WGPUSurface ApplicationWin::getWGPUSurface(GLFWwindow* window, WGPUInstance instance)
{

    WGPUChainedStruct chain{
                    .next = NULL,
                    .sType = WGPUSType_SurfaceDescriptorFromWindowsHWND,
        };

    HWND hwnd = glfwGetWin32Window(window);
    HINSTANCE hinstance = GetModuleHandle(NULL);

    WGPUSurfaceDescriptorFromWindowsHWND surface_desc_HWND{
        .chain = chain,
        .hinstance = hinstance,
        .hwnd = hwnd,
    };
    WGPUSurfaceDescriptor surface_desc{
        .nextInChain = (const WGPUChainedStruct*)&surface_desc_HWND,
        .label=NULL, 
    };
    
    return wgpuInstanceCreateSurface(
        instance,
        &surface_desc);
}
