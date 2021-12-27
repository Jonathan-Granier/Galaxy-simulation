#pragma once

#include "Vulkan/Instance.h"
#include "Vulkan/Device.h"

class Renderer
{
public:
    Renderer(Instance &iInstance, VkSurfaceKHR iSurface, uint32_t iWidth, uint32_t iHeight);
    ~Renderer() = default;

    void ReleaseRessources();

private:
    Device m_Device;
};
