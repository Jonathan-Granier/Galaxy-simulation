#include "Renderer.h"

//----------------------------------------------------------------------------------------------------------------------
Renderer::Renderer(Instance &iInstance, VkSurfaceKHR iSurface, uint32_t iWidth, uint32_t iHeight) : m_Device(iInstance, iSurface), m_Swapchain(m_Device, iWidth, iHeight)
{
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateRessources()
{
    CreateSwapchainRessources();
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::ReleaseRessources()
{
    ReleaseSwapchainRessources();
    m_Device.Destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateSwapchainRessources()
{
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::RecreateSwapchainResources(uint32_t iWidth, uint32_t iHeight)
{
    ReleaseSwapchainRessources();

    m_Swapchain.Init(iWidth, iHeight);
    CreateSwapchainRessources();
}
//----------------------------------------------------------------------------------------------------------------------
void Renderer::ReleaseSwapchainRessources()
{
    vkDeviceWaitIdle(m_Device.GetDevice());

    m_Swapchain.Destroy();
}