#include "Shader.h"
#include <iostream>
#include <vector>

void Shader::create(WGPUDevice device,  WGPUTextureFormat swap_chain_format, WGPURenderPipelineDescriptor &pipeline_desc)
{
    
    auto shader_module = loadShader(device,std::filesystem::path("shaders/shader.wgsl").make_preferred() );


	std::cout << "Shader module: " << shader_module << std::endl;

	std::cout << "Creating render pipeline..." << std::endl;
    vertex_attrib.resize(1);
    vertex_buffer_layout.resize(3);
    normal_attrib.resize(1);
    color_attrib.resize(1);

    vertex_attrib[0] = {
            .format = WGPUVertexFormat_Float32x3,
            .offset = 0,
            .shaderLocation = 0,
        };

    vertex_buffer_layout[0] = {
        .arrayStride = 3 * sizeof(float),
        .stepMode = WGPUVertexStepMode_Vertex,
        .attributeCount = vertex_attrib.size(),
        .attributes = vertex_attrib.data(),
    };

    normal_attrib[0] = {
        .format = WGPUVertexFormat_Float32x3,
        .offset = 0,
        .shaderLocation = 1,
    };

    vertex_buffer_layout[1] = {
        .arrayStride = 3 * sizeof(float),
        .stepMode = WGPUVertexStepMode_Vertex,
        .attributeCount = normal_attrib.size(), 
        .attributes = normal_attrib.data(),
    };

    color_attrib[0] = {
        .format = WGPUVertexFormat_Float32x3,
        .offset = 0,
        .shaderLocation = 2,
    };

    vertex_buffer_layout[2] = {
        .arrayStride = 3 * sizeof(float),
        .stepMode = WGPUVertexStepMode_Vertex,
        .attributeCount = color_attrib.size(), 
        .attributes = color_attrib.data(),
    };

    pipeline_desc = {
        .nextInChain = nullptr, .layout = nullptr,
        .vertex{
                .module = shader_module,
                .entryPoint = "vs_main",
                .constantCount = 0,
                .constants = nullptr,
                .bufferCount = vertex_buffer_layout.size(),
                .buffers = vertex_buffer_layout.data(),
            },
        .primitive{
            .topology = WGPUPrimitiveTopology_TriangleList,
            .stripIndexFormat = WGPUIndexFormat_Undefined,
            .frontFace = WGPUFrontFace_CCW,
            .cullMode = WGPUCullMode_None,
        },
    };

    m_frag_state = {
	    .nextInChain = nullptr,
        .module = shader_module,
        .entryPoint = "fs_main",
        .constantCount = 0,
        .constants = nullptr,};
    pipeline_desc.fragment = &m_frag_state;


    m_blend_state = {	
        .color{
            .operation = WGPUBlendOperation_Add,
            .srcFactor = WGPUBlendFactor_SrcAlpha,
            .dstFactor = WGPUBlendFactor_OneMinusSrcAlpha,
            },
        .alpha{
            .operation = WGPUBlendOperation_Add,
            .srcFactor = WGPUBlendFactor_Zero,
            .dstFactor = WGPUBlendFactor_One,
            },
    };

    m_color_target = {
	    .nextInChain = nullptr,
	    .format = swap_chain_format,
	    .blend = &m_blend_state,
	    .writeMask = WGPUColorWriteMask_All,
    };
    m_frag_state.targetCount = 1;
	m_frag_state.targets = &m_color_target;

    pipeline_desc.multisample.count = 1;

	pipeline_desc.multisample.mask = ~0u;
	pipeline_desc.multisample.alphaToCoverageEnabled = false;
	pipeline_desc.layout = nullptr;
}

WGPUShaderModule Shader::loadShader( WGPUDevice device, const std::filesystem::path& path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		return nullptr;
	}
	file.seekg(0, std::ios::end);
	size_t size = file.tellg();
	std::string shaderSource(size, ' ');
	file.seekg(0);
	file.read(shaderSource.data(), size);

	WGPUShaderModuleWGSLDescriptor shader_code_desc{
	    .chain{
            .next = nullptr,
	        .sType = WGPUSType_ShaderModuleWGSLDescriptor,
        },
	    .code = shaderSource.c_str(),
    };
	WGPUShaderModuleDescriptor shader_desc{
        .nextInChain = &shader_code_desc.chain,
        .hintCount = 0,
        .hints = nullptr,
    };

	return wgpuDeviceCreateShaderModule(device, &shader_desc);
}
