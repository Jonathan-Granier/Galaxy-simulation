#pragma once

#include "Olympus/Device.h"

namespace olp
{
///  Class for create and manage the vulkan swapchain.
class Swapchain
{
public:
    ///  Constructor.
    /// @param iDevice Device to initialize the swapchain with.
    /// @param iWidth Width of the surface to draw.
    /// @param iHeight Height of the surface to draw.
    Swapchain(const Device &iDevice, uint32_t iWidth, uint32_t iHeight);

    ///  Destructor.
    ~Swapchain() = default;

    ///  Initialize the swapchain.
    /// @param iWidth Width of the surface to draw.
    /// @param iHeight Height of the surface to draw.
    void Init(uint32_t width, uint32_t height);

    ///  Destroys the swapchain, the framebuffer and image view objects.
    void Destroy();

    ///  Create the framebuffers used by the swapchain.
    /// @param[in] iRenderPass Render pass to attache to the framebuffers.
    /// @param[in] iDepthImageView Depth attachment used by the render pass.
    void CreateFrameBuffers(VkRenderPass iRenderPass, VkImageView iDepthImageView);

    /// @brief
    ///  Get the next image to draw.
    /// @param[in] iImageAvailableSemaphore Semaphore to signal when the next image is get.
    /// @param[in] oImageIndex Index of the next image.
    /// @return The result of the vulkan's function.
    VkResult GetNextImage(VkSemaphore &iImageAvailableSemaphore, uint32_t &oImageIndex);

    ///  Present the next image in the surface.
    /// @param[in] signalSemaphores Semaphore to signal when the next image has been presented.
    /// @param[in] oImageIndex Index of the next image.
    VkResult PresentNextImage(VkSemaphore *iSignalSemaphores, uint32_t iImageIndex);

    VkFormat GetColorFormat() { return m_ImageFormat; }
    VkExtent2D GetImageSize() { return m_Extent; }
    size_t GetImageCount() { return m_Images.size(); }
    VkFramebuffer GetFramebuffer(uint32_t i) { return m_Framebuffers[i]; }
    VkSwapchainKHR GetSwapchain() { return m_Swapchain; }

private:
    struct SwapchainSupport
    {
        VkSurfaceCapabilitiesKHR Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentModes;
    };

    ///  Init all PFN vulkan function.
    void InitPFNvkFunction();

    ///  Query the support of the swapchain.
    SwapchainSupport QuerySwapchainSupport();

    ///  Select the format of the surface.
    /// @param[in] iAvailableFormats Available formats of the surface.
    /// @return The format selected.
    static VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &iAvailableFormats);

    ///  Select the present mode.
    /// @param[in] iAvailablePresentModes Available present mode.
    /// @return The present mode selected.
    static VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR> &iAvailablePresentModes);

    ///  Choose the extent of the swapchain.
    /// @param[in] iCapabilities Capabilities of the surface
    /// @return The extend selected.
    VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR &iCapabilities);

    ///  Create the swapchain.
    void CreateSwapchain();

    ///  Create images views.
    void CreateImageViews();

    /// Vulkan device.
    const Device &m_Device;

    /// Vulkan Swapchain.
    VkSwapchainKHR m_Swapchain;
    /// Image of the swapchain.
    std::vector<VkImage> m_Images;
    /// Imageview of the swapchain.
    std::vector<VkImageView> m_ImageViews;
    /// Format of the swapchain.
    VkFormat m_ImageFormat;
    /// Extent of the surface.
    VkExtent2D m_Extent;
    /// Framebuffer of the swapchain.
    std::vector<VkFramebuffer> m_Framebuffers;

    // TODO remove.(Duplicate with m_Extent.)
    uint32_t m_Width = 0;
    uint32_t m_Height = 0;

    // PFN Function
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR{nullptr};
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR{nullptr};
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR{nullptr};
    PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR{nullptr};
    PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR{nullptr};
    PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR{nullptr};
    PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR{nullptr};
    PFN_vkQueuePresentKHR vkQueuePresentKHR{nullptr};
};
}