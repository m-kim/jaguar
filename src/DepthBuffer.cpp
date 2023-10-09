#include "DepthBuffer.h"

void DepthBuffer::create(WGPUDevice device, unsigned int width, unsigned int height)
{

    WGPUTextureFormat depth_texture_format = WGPUTextureFormat_Depth24Plus;
    WGPUTextureDescriptor depth_texture_desc{
	    .usage = WGPUTextureUsage_RenderAttachment,
	    .dimension = WGPUTextureDimension_2D,
	    .size = {width, height, 1},
	    .format = depth_texture_format,
	    .mipLevelCount = 1,
	    .sampleCount = 1,
	    .viewFormatCount = 1,
	    .viewFormats = (WGPUTextureFormat*)&depth_texture_format,
    };
	m_depth_texture = wgpuDeviceCreateTexture(device, &depth_texture_desc);

    WGPUTextureViewDescriptor depth_texture_view_desc{
	    .format = depth_texture_format,
	    .dimension = WGPUTextureViewDimension_2D,
	    .baseMipLevel = 0,
	    .mipLevelCount = 1,
	    .baseArrayLayer = 0,
	    .arrayLayerCount = 1,
	    .aspect = WGPUTextureAspect_DepthOnly,
    };

	m_depth_texture_view = wgpuTextureCreateView(m_depth_texture, &depth_texture_view_desc);
}

void DepthBuffer::createDerivedViews( WGPUDevice Device, WGPURenderPipelineDescriptor &pipeline_desc )
{
    WGPUStencilFaceState default_stencil_face_state{
        .compare = WGPUCompareFunction_Always,
        .failOp = WGPUStencilOperation_Keep,
        .depthFailOp = WGPUStencilOperation_Keep,
        .passOp = WGPUStencilOperation_Keep,
    };
    m_depth_stencil_state = {
        .format = WGPUTextureFormat_Undefined,
        .depthWriteEnabled = false,
        .depthCompare = WGPUCompareFunction_Always,
        .stencilFront = default_stencil_face_state,
        .stencilBack = default_stencil_face_state,
        .stencilReadMask = 0xFFFFFFFF,
        .stencilWriteMask = 0xFFFFFFFF,
        .depthBias = 0,
        .depthBiasSlopeScale = 0,
        .depthBiasClamp = 0,
    };

    m_depth_stencil_state.depthCompare = WGPUCompareFunction_Less;
    m_depth_stencil_state.depthWriteEnabled = true;
    WGPUTextureFormat depthTextureFormat = WGPUTextureFormat_Depth24Plus;
    m_depth_stencil_state.format = depthTextureFormat;
    m_depth_stencil_state.stencilReadMask = 0;
	m_depth_stencil_state.stencilWriteMask = 0;
    pipeline_desc.depthStencil = &m_depth_stencil_state;
}