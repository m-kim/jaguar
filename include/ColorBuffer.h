#pragma once
#include <webgpu.h>
#include <glm/glm.hpp>
#include "PixelBuffer.h"

class ColorBuffer : public PixelBuffer
{
public:

    ColorBuffer( glm::vec4 clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)  )
        : m_clear_color(clear_color)
    {
    }

    void create(WGPUDevice device,WGPURenderPassDescriptor &renderPassDesc, unsigned int width, unsigned int height, WGPUTextureView nextTexture);

    glm::vec4 m_clear_color;
    WGPURenderPassColorAttachment m_render_pass_color_attachment;

};
