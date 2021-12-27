#include "Renderer.h"

//----------------------------------------------------------------------------------------------------------------------
Renderer::Renderer(Instance &iInstance, VkSurfaceKHR iSurface, uint32_t iWidth, uint32_t iHeight) : m_Device(iInstance, iSurface)
{
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::ReleaseRessources()
{
    m_Device.Destroy();
}