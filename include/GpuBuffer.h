#pragma once
#include <vector>
#include "webgpu.h"

template<typename TYPE>
class GpuBuffer
{
public:
    void create(WGPUDevice device, WGPUQueue queue, std::vector<TYPE> &data, WGPUBufferUsageFlags usage){
        m_size = data.size() * sizeof(TYPE);
        m_buffer_desc = {
            .usage = usage,
            .size = m_size,
            .mappedAtCreation = false,
        };

        m_buffer = wgpuDeviceCreateBuffer(device, &m_buffer_desc);
        wgpuQueueWriteBuffer(queue, m_buffer, 0, data.data(), m_size);
    }

    void destroy(){
        wgpuBufferDestroy(m_buffer);
    }

    unsigned int m_size;
    WGPUBuffer m_buffer;
    WGPUBufferDescriptor m_buffer_desc;
};