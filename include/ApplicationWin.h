#pragma once

#include <webgpu.h>
#include "Application.h"


class ApplicationWin : public Application
{
public:
    ApplicationWin(){

    }

    WGPUSurface getWGPUSurface(GLFWwindow* window, WGPUInstance instance);
};