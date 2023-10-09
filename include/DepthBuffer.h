#pragma once

#include "PixelBuffer.h"

#include <webgpu.h>
class DepthBuffer : public PixelBuffer
{
public:
    DepthBuffer( float clear_depth = 0.0f, unsigned char clear_stencil = 0 )
        : m_clear_depth(clear_depth), m_clear_stencil(clear_stencil) 
    {
    }
    ~DepthBuffer()
    {
        wgpuTextureViewRelease(m_depth_texture_view);
        wgpuTextureDestroy(m_depth_texture);
        wgpuTextureRelease(m_depth_texture);

    }
    void create(WGPUDevice device, unsigned int width, unsigned int height);

    void createDerivedViews( WGPUDevice Device, WGPURenderPipelineDescriptor& );
    float m_clear_depth;
    unsigned char m_clear_stencil;

    WGPUTexture m_depth_texture;
    WGPUTextureView m_depth_texture_view;
    WGPUDepthStencilState m_depth_stencil_state;
};