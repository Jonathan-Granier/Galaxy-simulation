#include "Olympus/Swapchain.h"
#include "Olympus/Debug.h"
#include "Olympus/Image.h"
#include <array>

namespace olp
{
//----------------------------------------------------------------------------------------------------------------------
Swapchain::Swapchain(const Device &iDevice, uint32_t iWidth, uint32_t iHeight)
    : m_Device(iDevice), m_Width(iWidth), m_Height(iHeight)
{
    InitPFNvkFunction();
    CreateSwapchain();

    CreateImageViews();
}

//----------------------------------------------------------------------------------------------------------------------
void Swapchain::Init(uint32_t width, uint32_t height)
{
    m_Width = width;
    m_Height = height;
    CreateSwapchain();
    CreateImageViews();
}

//----------------------------------------------------------------------------------------------------------------------
void Swapchain::Destroy()
{
    vkQueueWaitIdle(m_Device.GetPresentQueue());
    for (VkFramebuffer framebuffer : m_Framebuffers)
    {
        vkDestroyFramebuffer(m_Device.GetDevice(), framebuffer, nullptr);
    }
    for (VkImageView imageView : m_ImageViews)
    {
        vkDestroyImageView(m_Device.GetDevice(), imageView, nullptr);
    }
    vkDestroySwapchainKHR(m_Device.GetDevice(), m_Swapchain, nullptr);
}

//----------------------------------------------------------------------------------------------------------------------
void Swapchain::InitPFNvkFunction()
{
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(
        vkGetInstanceProcAddr(m_Device.GetVkInstance(), "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
    vkGetPhysicalDeviceSurfaceFormatsKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(
        vkGetInstanceProcAddr(m_Device.GetVkInstance(), "vkGetPhysicalDeviceSurfaceFormatsKHR"));
    vkGetPhysicalDeviceSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfacePresentModesKHR>(
        vkGetInstanceProcAddr(m_Device.GetVkInstance(), "vkGetPhysicalDeviceSurfacePresentModesKHR"));

    vkCreateSwapchainKHR = reinterpret_cast<PFN_vkCreateSwapchainKHR>(
        vkGetDeviceProcAddr(m_Device.GetDevice(), "vkCreateSwapchainKHR"));
    vkDestroySwapchainKHR = reinterpret_cast<PFN_vkDestroySwapchainKHR>(
        vkGetDeviceProcAddr(m_Device.GetDevice(), "vkDestroySwapchainKHR"));
    vkGetSwapchainImagesKHR = reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(
        vkGetDeviceProcAddr(m_Device.GetDevice(), "vkGetSwapchainImagesKHR"));
    vkAcquireNextImageKHR = reinterpret_cast<PFN_vkAcquireNextImageKHR>(
        vkGetDeviceProcAddr(m_Device.GetDevice(), "vkAcquireNextImageKHR"));
    vkQueuePresentKHR = reinterpret_cast<PFN_vkQueuePresentKHR>(
        vkGetDeviceProcAddr(m_Device.GetDevice(), "vkQueuePresentKHR"));
}

//----------------------------------------------------------------------------------------------------------------------
Swapchain::SwapchainSupport Swapchain::QuerySwapchainSupport()
{
    SwapchainSupport details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Device.GetPhysicalDevice(), m_Device.GetSurface(), &details.Capabilities);
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_Device.GetPhysicalDevice(), m_Device.GetSurface(), &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.Formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            m_Device.GetPhysicalDevice(), m_Device.GetSurface(), &formatCount, details.Formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        m_Device.GetPhysicalDevice(), m_Device.GetSurface(), &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.PresentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            m_Device.GetPhysicalDevice(), m_Device.GetSurface(), &presentModeCount, details.PresentModes.data());
    }

    return details;
}

//----------------------------------------------------------------------------------------------------------------------
VkSurfaceFormatKHR Swapchain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &iAvailableFormats)
{
    for (const auto &availableFormat : iAvailableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }
    return iAvailableFormats[0];
}

//----------------------------------------------------------------------------------------------------------------------
VkPresentModeKHR Swapchain::ChoosePresentMode(const std::vector<VkPresentModeKHR> &iAvailablePresentModes)
{
    for (const auto &availablePresentMode : iAvailablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

//----------------------------------------------------------------------------------------------------------------------
VkExtent2D Swapchain::ChooseExtent(const VkSurfaceCapabilitiesKHR &iCapabilities)
{
    if (iCapabilities.currentExtent.width != UINT32_MAX)
    {
        return iCapabilities.currentExtent;
    }
    else
    {
        VkExtent2D actualExtent = {m_Width, m_Height};

        actualExtent.width = std::max(
            iCapabilities.minImageExtent.width, std::min(iCapabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(
            iCapabilities.minImageExtent.height, std::min(iCapabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Swapchain::CreateSwapchain()
{
    SwapchainSupport swapchainSupport = QuerySwapchainSupport();

    VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(swapchainSupport.Formats);
    VkPresentModeKHR presentMode = ChoosePresentMode(swapchainSupport.PresentModes);
    VkExtent2D extent = ChooseExtent(swapchainSupport.Capabilities);

    uint32_t imageCount = swapchainSupport.Capabilities.minImageCount + 1;

    if (swapchainSupport.Capabilities.maxImageCount > 0 && imageCount > swapchainSupport.Capabilities.maxImageCount)
    {
        imageCount = swapchainSupport.Capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_Device.GetSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    // Use VK_IMAGE_USAGE_TRANSFER_DST_BIT for deferred rendering.

    Device::QueueFamilyIndices indices = m_Device.GetQueueIndices();
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                     indices.presentFamily.value(),
                                     indices.computeFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 3;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;     // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }
    createInfo.preTransform = swapchainSupport.Capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VK_CHECK_RESULT(vkCreateSwapchainKHR(m_Device.GetDevice(), &createInfo, nullptr, &m_Swapchain))

    vkGetSwapchainImagesKHR(m_Device.GetDevice(), m_Swapchain, &imageCount, nullptr);
    m_Images.resize(imageCount);
    vkGetSwapchainImagesKHR(m_Device.GetDevice(), m_Swapchain, &imageCount, m_Images.data());

    //TODO Refactor ?
    m_ImageFormat = surfaceFormat.format;
    m_Extent = extent;
}

//----------------------------------------------------------------------------------------------------------------------
void Swapchain::CreateImageViews()
{
    m_ImageViews.resize(m_Images.size());
    for (size_t i = 0; i < m_Images.size(); ++i)
    {
        m_ImageViews[i] = Image::CreateImageView(m_Device, m_Images[i], m_ImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    }
}
//----------------------------------------------------------------------------------------------------------------------
void Swapchain::CreateFrameBuffers(VkRenderPass iRenderPass, VkImageView iDepthImageView)
{
    m_Framebuffers.resize(m_Images.size());

    for (size_t i = 0; i < m_Framebuffers.size(); i++)
    {
        std::array<VkImageView, 2> attachments = {m_ImageViews[i], iDepthImageView};
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = iRenderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = m_Extent.width;
        framebufferInfo.height = m_Extent.height;
        framebufferInfo.layers = 1;

        VK_CHECK_RESULT(
            vkCreateFramebuffer(m_Device.GetDevice(), &framebufferInfo, nullptr, &m_Framebuffers[i]))
    }
}

//----------------------------------------------------------------------------------------------------------------------
VkResult Swapchain::GetNextImage(VkSemaphore &iImageAvailableSemaphore, uint32_t &oImageIndex)
{
    return vkAcquireNextImageKHR(
        m_Device.GetDevice(), m_Swapchain, UINT64_MAX, iImageAvailableSemaphore, VK_NULL_HANDLE, &oImageIndex);
}

//----------------------------------------------------------------------------------------------------------------------
VkResult Swapchain::PresentNextImage(VkSemaphore *iSignalSemaphores, uint32_t iImageIndex)
{
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = iSignalSemaphores;

    VkSwapchainKHR swapchains[] = {m_Swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &iImageIndex;
    presentInfo.pResults = nullptr; // Optional

    return vkQueuePresentKHR(m_Device.GetPresentQueue(), &presentInfo);
}
}