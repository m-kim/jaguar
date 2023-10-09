#pragma once
#include <webgpu.h>
#include <filesystem>
#include <fstream>

class Shader
{
public:
    Shader(){}
    void create(WGPUDevice device, WGPUTextureFormat swap_chain_format, WGPURenderPipelineDescriptor&);
    WGPUShaderModule loadShader(WGPUDevice device, const std::filesystem::path& path);

    WGPUFragmentState m_frag_state;
    WGPUBlendState m_blend_state;
    WGPUColorTargetState m_color_target;
    std::vector<WGPUVertexAttribute> vertex_attrib;
    std::vector<WGPUVertexBufferLayout> vertex_buffer_layout;
    std::vector<WGPUVertexAttribute> normal_attrib;
    std::vector<WGPUVertexAttribute> color_attrib;

};