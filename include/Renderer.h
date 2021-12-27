#pragma once

#include "Vulkan/Device.h"
#include "Vulkan/Swapchain.h"

class Renderer
{
public:
    Renderer(Instance &iInstance, VkSurfaceKHR iSurface, uint32_t iWidth, uint32_t iHeight);
    ~Renderer() = default;

    void CreateRessources();
    void ReleaseRessources();
    void CreateSwapchainRessources();
    void RecreateSwapchainResources(uint32_t iWidth, uint32_t iHeight);
    void ReleaseSwapchainRessources();

private:
    Device m_Device;
    Swapchain m_Swapchain;
};
