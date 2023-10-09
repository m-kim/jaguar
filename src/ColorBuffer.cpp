#include "ColorBuffer.h"

void ColorBuffer::create(WGPUDevice device,WGPURenderPassDescriptor &renderPassDesc, unsigned int width, unsigned int height, WGPUTextureView nextTexture)
{

    m_render_pass_color_attachment = {
        .view = nextTexture,
        .resolveTarget = nullptr,
        .loadOp = WGPULoadOp_Clear,
        .storeOp = WGPUStoreOp_Store,
        .clearValue = WGPUColor{ m_clear_color[0], m_clear_color[1], m_clear_color[2], m_clear_color[3] },
    };
    renderPassDesc.colorAttachmentCount = 1;
    renderPassDesc.colorAttachments = &m_render_pass_color_attachment;
}