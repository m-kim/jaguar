#pragma once
#include <webgpu.h>
#include "GpuBuffer.h"
#include "DepthBuffer.h"
#include "Camera.h"

#include <iostream>

class GLFWwindow;
class Application
{
    public:
    Application(){}
    ~Application(){}

    virtual bool onInit();
    virtual bool initGLFW();
    virtual bool render();
    virtual bool onFinish();
    virtual void buildPipeline(WGPUTextureFormat swap_chain_format);
    virtual void createBuffers();
    virtual void loadMesh(std::vector<float> &, std::vector<float> &, std::vector<float> &, std::vector<int> &);
    virtual WGPUSurface getWGPUSurface(GLFWwindow* window, WGPUInstance instance) = 0;
    virtual WGPUAdapter getAdapter(WGPUInstance instance, WGPURequestAdapterOptions const * options);
    virtual WGPUDevice requestDevice(WGPUAdapter adapter, WGPUDeviceDescriptor const * descriptor);

    GLFWwindow* m_window;
    WGPUInstanceDescriptor m_instance_desc;
    WGPURequestAdapterOptions m_adapter_opts;
    WGPUBindGroupLayoutEntry m_binding_layout;
    WGPUInstance m_instance;
    WGPUAdapter m_adapter;
    WGPUDevice m_device;
    WGPUQueue m_queue;
    WGPUSurface m_surface;
    DepthBuffer m_dp;
    WGPUSwapChain m_swapchain;
    WGPUSwapChainDescriptor m_swapchain_desc;
    WGPUDeviceDescriptor m_device_desc;
    WGPUBindGroupLayout m_bind_group_layout;
    WGPUPipelineLayoutDescriptor m_layout_desc;
    WGPUBindGroupLayoutDescriptor m_bind_group_layout_desc;
    WGPURenderPipeline  m_pipeline;
    WGPUBindGroup m_bind_group;

    Camera m_cam;
    GpuBuffer<float> m_vertex_buffer, m_normal_buffer, m_color_buffer;
    GpuBuffer<int> m_index_buffer;
};