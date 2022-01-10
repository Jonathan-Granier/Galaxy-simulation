#pragma once
#include "Olympus/Device.h"
#include "Olympus/CommandBuffer.h"

namespace olp
{
///  Class for create and manage a vulkan image.
class Image
{
public:
    ///  Constructor.
    /// @param iDevice Device to initialize the image with.
    explicit Image(const Device &iDevice);

    ///  Destructor.
    ~Image();

    ///  Initialize the image.
    /// @param[in] iWidth Image's width.
    /// @param[in] iHeight Image's height.
    /// @param[in] iFormat Image's format.
    void Init(uint32_t iWidth, uint32_t iHeight, VkFormat iFormat);

    ///  Destroys the images objects.
    void Destroy();

    ///  Create the image image view of a image.
    /// @param[in] iDevice Vulkan device.
    /// @param[in] iImage Image to generate the view.
    /// @param[in] iFormat Format of the image.
    /// @param[in] iAspectFlags Specifying which aspect(s) of the image are included in the view.
    /// @param[in] iMipLevels First mipmap level accessible to the view.
    static VkImageView CreateImageView(
        const Device &iDevice, VkImage iImage, VkFormat iFormat, VkImageAspectFlags iAspectFlags, uint32_t iMipLevels = 1);

    ///  Create the image image view of the image.
    /// @param[in] iAspectFlags Specifying which aspect(s) of the image are included in the view.
    /// @param[in] iMipLevels First mipmap level accessible to the view.
    void CreateImageView(VkImageAspectFlags iAspectFlags, uint32_t iMipLevels = 1);

    ///  Create the image.
    /// @param[in] iTiling Value specifying the tiling arrangement of the texel blocks in memory.
    /// @param[in] iUsage Describing the intended usage of the image.
    /// @param[in] iProperties Memory property.
    /// @param[in] iMipLevels Number of levels of detail available for minified sampling of the image.
    /// @param[in] iNumSamples Number of samples per texel.
    void CreateImage(
        VkImageTiling iTiling,
        VkImageUsageFlags iUsage,
        VkMemoryPropertyFlags iProperties,
        uint32_t iMipLevels = 1,
        VkSampleCountFlagBits iNumSamples = VK_SAMPLE_COUNT_1_BIT);

    ///  Transform the layout of the image.
    /// @param[in] ioCommandBuffer Command buffer to add the transition.
    /// @param[in] iOldLayout Old image's layout.
    /// @param[in] iNewLayout New image's layout.
    void TransitionImageLayout(CommandBuffer &ioCommandBuffer, VkImageLayout iOldLayout, VkImageLayout iNewLayout);

    ///  Transform the layout of the image.
    /// @param[in] iCommandPool Command pool to allocate the command buffer.
    /// @param[in] iOldLayout Old image's layout.
    /// @param[in] iNewLayout New image's layout.
    void TransitionImageLayout(VkImageLayout iOldLayout, VkImageLayout iNewLayout);

    VkImage GetImage() { return m_Image; }
    VkImageView GetImageView() { return m_ImageView; }
    uint32_t GetWidth() const { return m_Width; }
    uint32_t GetHeight() const { return m_Height; }
    VkFormat GetFormat() const { return m_Format; }

protected:
    /// Vulkan device
    const Device &m_Device;
    /// Image's width.
    uint32_t m_Width;
    /// Image's height.
    uint32_t m_Height;
    /// Image's format.
    VkFormat m_Format = VK_FORMAT_UNDEFINED;
    /// Vulkan Image.
    VkImage m_Image = VK_NULL_HANDLE;
    /// Image view.
    VkImageView m_ImageView = VK_NULL_HANDLE;
    /// Image memory.
    VkDeviceMemory m_ImageMemory = VK_NULL_HANDLE;
};
}