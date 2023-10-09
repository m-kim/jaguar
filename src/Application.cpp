#include "Application.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>


#include "webgpu.h"
#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "ColorBuffer.h"

void onWindowResize(GLFWwindow* m_window, int , int) {
	auto pApp = reinterpret_cast<Application*>(glfwGetWindowUserPointer(m_window));
	if (pApp != nullptr) pApp->m_cam.onResize();
}

void onMouseMove(GLFWwindow* m_window, double x, double y)
{
    auto pApp = reinterpret_cast<Application*>(glfwGetWindowUserPointer(m_window));
	if (pApp != nullptr) pApp->m_cam.motion(x,y);
}

void onMouseCallback(GLFWwindow* m_window, int button, int action, int)
{
    double x,y;         
    glfwGetCursorPos(m_window, &x, &y);
    auto pApp = reinterpret_cast<Application*>(glfwGetWindowUserPointer(m_window));
	if (pApp != nullptr) pApp->m_cam.mouse(button, action, x,y);
}

void onMouseScroll(GLFWwindow* m_window, double x, double y)
{
    auto pApp = reinterpret_cast<Application*>(glfwGetWindowUserPointer(m_window));
	if (pApp != nullptr) pApp->m_cam.mouse(3, GLFW_PRESS, x, y);
}


void Application::loadMesh(std::vector<float> &vertex_data, std::vector<float> &normal_data, std::vector<float> &color_data, std::vector<int> &index_data)
{
    createBuffers();
    m_vertex_buffer.create(m_device, m_queue, vertex_data, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex);
    m_normal_buffer.create(m_device, m_queue, normal_data, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex);
    m_color_buffer.create(m_device, m_queue, color_data, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex);
    m_index_buffer.create(m_device, m_queue, index_data, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index);

}
bool Application::initGLFW()
{
    if(!glfwInit()){
        std::cout << "Could not initialize glfw!" << std::endl;

        return 0;        
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_window = glfwCreateWindow(640, 480, "WebGPU Anari", NULL, NULL);
    
    if (!m_window) {
        std::cout << "Could not open window!" << std::endl;
        glfwTerminate();
        return 0;
    }

    glfwSetWindowUserPointer(m_window, this);
    glfwSetMouseButtonCallback(m_window, &onMouseCallback);
    glfwSetCursorPosCallback(m_window, &onMouseMove);
    glfwSetScrollCallback(m_window, &onMouseScroll);
    return true;
}

bool Application::onInit(){
    
    if (!initGLFW())
        return false;
    m_instance_desc = {};
    m_instance_desc.nextInChain = nullptr;
    m_instance = wgpuCreateInstance(&m_instance_desc);

    if (!m_instance) {
        std::cout << "Could not initialize WebGPU!" << std::endl;
        return false;
    }
    std::cout << "WGPU instance: " << m_instance << std::endl;

    m_surface = getWGPUSurface(m_window, m_instance);

    
    m_adapter_opts = {
        .nextInChain = nullptr,
        .compatibleSurface = m_surface,
    };
    m_adapter = getAdapter(m_instance, &m_adapter_opts);

    std::cout << "Requesting device..." << std::endl;

    m_device_desc = {
        .label = "Device",
        .requiredFeaturesCount = 0,
        .requiredLimits = 0,
        .defaultQueue{
            .nextInChain = nullptr,
            .label = "default queue",}
    };

    m_device = requestDevice(m_adapter, &m_device_desc);
    std::cout << "Got device: " << m_device << std::endl;

    m_queue = wgpuDeviceGetQueue(m_device);
    auto onQueueWorkDone = [](WGPUQueueWorkDoneStatus status, void*) {
        std::cout << "Queued work finished with status: " << status << std::endl;
    };
    wgpuQueueOnSubmittedWorkDone(m_queue, onQueueWorkDone, nullptr);
    WGPUTextureFormat swapChainFormat = wgpuSurfaceGetPreferredFormat(m_surface, m_adapter);
    m_swapchain_desc = {
        .usage = WGPUTextureUsage_RenderAttachment,
        .format = swapChainFormat,
        .width = 640,
        .height = 480,
        .presentMode = WGPUPresentMode_Fifo,
    };
    m_swapchain = wgpuDeviceCreateSwapChain(m_device, m_surface, &m_swapchain_desc);

    buildPipeline(swapChainFormat);
    
    return 1;
}

bool Application::render()
{
    if (glfwWindowShouldClose(m_window))
        return false;

    glfwPollEvents();
    m_cam.m_uniforms.time = static_cast<float>(glfwGetTime()); // glfwGetTime returns a double
    m_cam.update(m_queue);
    WGPUTextureView nextTexture = wgpuSwapChainGetCurrentTextureView(m_swapchain);
    if (!nextTexture) {
        std::cerr << "Cannot acquire next swap chain texture" << std::endl;
        return false;
    }

    WGPUCommandEncoderDescriptor commandEncoderDesc = {};
    commandEncoderDesc.nextInChain = nullptr;
    commandEncoderDesc.label = "Command Encoder";
    WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(m_device, &commandEncoderDesc);
    

    WGPURenderPassDescriptor render_pass_desc = {};
    
    ColorBuffer color({ 0.9, 0.1, 0.2, 1.0 });
    color.create(m_device, render_pass_desc, 640, 480, nextTexture);

    WGPURenderPassDepthStencilAttachment depth_stencil_attachment;
    depth_stencil_attachment.view = m_dp.m_depth_texture_view;
    depth_stencil_attachment.depthClearValue = 1.0f;
    depth_stencil_attachment.depthLoadOp = WGPULoadOp_Clear;
    depth_stencil_attachment.depthStoreOp = WGPUStoreOp_Store;
    depth_stencil_attachment.depthReadOnly = false;
    depth_stencil_attachment.stencilClearValue = 0;
    depth_stencil_attachment.stencilLoadOp = WGPULoadOp_Clear;
    depth_stencil_attachment.stencilStoreOp = WGPUStoreOp_Store;
    depth_stencil_attachment.stencilReadOnly = true;
    render_pass_desc.depthStencilAttachment = &depth_stencil_attachment;

    render_pass_desc.timestampWriteCount = 0;
    render_pass_desc.timestampWrites = nullptr;

    render_pass_desc.nextInChain = nullptr;

    WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &render_pass_desc);
    wgpuRenderPassEncoderSetPipeline(renderPass, m_pipeline);
    wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, m_vertex_buffer.m_buffer, 0, m_vertex_buffer.m_size);
    wgpuRenderPassEncoderSetVertexBuffer(renderPass, 1, m_normal_buffer.m_buffer, 0, m_normal_buffer.m_size);
    wgpuRenderPassEncoderSetVertexBuffer(renderPass, 2, m_color_buffer.m_buffer, 0, m_color_buffer.m_size);
    wgpuRenderPassEncoderSetIndexBuffer(renderPass, m_index_buffer.m_buffer, WGPUIndexFormat_Uint32, 0, m_index_buffer.m_size);
    wgpuRenderPassEncoderSetBindGroup(renderPass, 0, m_bind_group, 0, nullptr);
    wgpuRenderPassEncoderDrawIndexed(renderPass, m_index_buffer.m_size/sizeof(int), 1, 0, 0, 0);
    wgpuRenderPassEncoderEnd(renderPass);
    wgpuTextureViewRelease(nextTexture);

    WGPUCommandBufferDescriptor cmd_buffer_desc = {.nextInChain = nullptr,.label = "Command buffer",};
    WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmd_buffer_desc);
    wgpuQueueSubmit(m_queue, 1, &command);

    wgpuSwapChainPresent(m_swapchain);
    return true;
}
void Application::buildPipeline(WGPUTextureFormat swap_chain_format){

    std::cout << "Creating render pipeline..." << std::endl;

    WGPURenderPipelineDescriptor pipeline_desc;
    Shader shader;
    shader.create(m_device, swap_chain_format, pipeline_desc);

    m_dp.createDerivedViews(m_device, pipeline_desc);



    pipeline_desc.multisample.count = 1;

    pipeline_desc.multisample.mask = ~0u;
    pipeline_desc.multisample.alphaToCoverageEnabled = false;
    pipeline_desc.layout = nullptr;

    m_binding_layout = { //Default
        .buffer{
            .nextInChain = nullptr,
            .type = WGPUBufferBindingType_Undefined,
            .hasDynamicOffset = false,
        },
        .sampler{
            .nextInChain = nullptr,
            .type = WGPUSamplerBindingType_Undefined,
        },
        .texture{
            .nextInChain = nullptr,
            .sampleType = WGPUTextureSampleType_Undefined,
            .viewDimension = WGPUTextureViewDimension_Undefined,
            .multisampled = false,
        },
        .storageTexture{
            .nextInChain = nullptr,
            .access = WGPUStorageTextureAccess_Undefined,
            .format = WGPUTextureFormat_Undefined,
            .viewDimension = WGPUTextureViewDimension_Undefined,
        },
    };
    m_binding_layout.binding = 0;
    m_binding_layout.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
    m_binding_layout.buffer.type = WGPUBufferBindingType_Uniform;
    m_binding_layout.buffer.minBindingSize = sizeof(MyUniforms);

    m_bind_group_layout_desc = {};
    m_bind_group_layout_desc.entryCount = 1;
    m_bind_group_layout_desc.entries = &m_binding_layout;


    m_bind_group_layout = wgpuDeviceCreateBindGroupLayout(m_device, &m_bind_group_layout_desc);
    m_layout_desc = {
        .bindGroupLayoutCount = 1,
        .bindGroupLayouts = (WGPUBindGroupLayout*)&m_bind_group_layout,
    };

    WGPUPipelineLayout layout = wgpuDeviceCreatePipelineLayout(m_device, &m_layout_desc);
    pipeline_desc.layout = layout;


    std::cout << "render pipeline complete" << std::endl;
    m_pipeline = wgpuDeviceCreateRenderPipeline(m_device, &pipeline_desc);
    
}

void Application::createBuffers()
{
    
    
    m_dp.create(m_device, 640, 480);
    m_cam.init(m_device, m_queue);

    WGPUBindGroupEntry binding{
	    .binding = 0,
	    .buffer = m_cam.m_uniform_buffer,
	    .offset = 0,
	    .size = sizeof(MyUniforms),
    };
    WGPUBindGroupDescriptor bind_group_desc{
	    .layout = m_bind_group_layout,
	    .entryCount = m_bind_group_layout_desc.entryCount,
	    .entries = &binding,
    };
	m_bind_group = wgpuDeviceCreateBindGroup(m_device, &bind_group_desc);
}

WGPUAdapter Application::getAdapter(WGPUInstance instance, WGPURequestAdapterOptions const * options) {
    struct UserData {
        WGPUAdapter adapter = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

    auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const * message, void * pUserData) {
        UserData& userData = *reinterpret_cast<UserData*>(pUserData);
        if (status == WGPURequestAdapterStatus_Success) {
            userData.adapter = adapter;
        } else {
            std::cout << "Could not get WebGPU adapter: " << message << std::endl;
        }
        userData.requestEnded = true;
    };

    wgpuInstanceRequestAdapter(
        instance,
        options,
        onAdapterRequestEnded,
        (void*)&userData
    );

    return userData.adapter;
}



WGPUDevice Application::requestDevice(WGPUAdapter adapter, WGPUDeviceDescriptor const * descriptor) {
    struct UserData {
        WGPUDevice device = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

    auto onDeviceRequestEnded = [](WGPURequestDeviceStatus status, WGPUDevice device, char const * message, void * pUserData) {
        UserData& userData = *reinterpret_cast<UserData*>(pUserData);
        if (status == WGPURequestDeviceStatus_Success) {
            userData.device = device;
        } else {
            std::cout << "Could not get WebGPU device: " << message << std::endl;
        }
        userData.requestEnded = true;
    };

    wgpuAdapterRequestDevice(
        adapter,
        descriptor,
        onDeviceRequestEnded,
        (void*)&userData
    );

    

    return userData.device;
}

bool Application::onFinish()
{
    m_vertex_buffer.destroy();
    m_index_buffer.destroy();
    m_color_buffer.destroy();
    m_normal_buffer.destroy();

    wgpuSwapChainRelease(m_swapchain);
    wgpuDeviceRelease(m_device);
    wgpuAdapterRelease(m_adapter);
    wgpuInstanceRelease(m_instance);
    wgpuSurfaceRelease(m_surface);

    glfwDestroyWindow(m_window);
    glfwTerminate();

    return 1;
}