#pragma once

#include "GpuResource.h"

class PixelBuffer : public GpuResource
{
public:
    PixelBuffer() : m_width(0), m_height(0), m_array_size(0)
    {

    }

    int m_width, m_height, m_array_size;
};