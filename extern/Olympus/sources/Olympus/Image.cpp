#include "Olympus/Image.h"
#include "Olympus/Debug.h"
#include "Olympus/CommandBuffer.h"
namespace olp
{
//----------------------------------------------------------------------------------------------------------------------
Image::Image(const Device &iDevice)
    : m_Device(iDevice)
{
}

//----------------------------------------------------------------------------------------------------------------------
Image::~Image()
{
    Destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void Image::Init(uint32_t iWidth, uint32_t iHeight, VkFormat iFormat)
{
    m_Width = iWidth;
    m_Height = iHeight;
    m_Format = iFormat;
}

//----------------------------------------------------------------------------------------------------------------------
void Image::Destroy()
{
    if (m_Image != VK_NULL_HANDLE)
    {
        vkDestroyImage(m_Device.GetDevice(), m_Image, nullptr);
        m_Image = VK_NULL_HANDLE;
    }
    if (m_ImageMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(m_Device.GetDevice(), m_ImageMemory, nullptr);
        m_ImageMemory = VK_NULL_HANDLE;
    }
    if (m_ImageView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(m_Device.GetDevice(), m_ImageView, nullptr);
        m_ImageView = VK_NULL_HANDLE;
    }
}

//----------------------------------------------------------------------------------------------------------------------
VkImageView Image::CreateImageView(
    const Device &iDevice, VkImage iImage, VkFormat iFormat, VkImageAspectFlags iAspectFlags, uint32_t iMipLevels)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = iImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = iFormat;

    viewInfo.subresourceRange.aspectMask = iAspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = iMipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    VK_CHECK_RESULT(vkCreateImageView(iDevice.GetDevice(), &viewInfo, nullptr, &imageView))

    return imageView;
}

//----------------------------------------------------------------------------------------------------------------------
void Image::CreateImageView(VkImageAspectFlags aspectFlags, uint32_t mipLevels)
{
    m_ImageView = Image::CreateImageView(m_Device, m_Image, m_Format, aspectFlags, mipLevels);
}

//----------------------------------------------------------------------------------------------------------------------
void Image::CreateImage(
    VkImageTiling iTiling,
    VkImageUsageFlags iUsage,
    VkMemoryPropertyFlags iProperties,
    uint32_t iMipLevels,
    VkSampleCountFlagBits iNumSamples)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = m_Width;
    imageInfo.extent.height = m_Height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = iMipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = m_Format;
    imageInfo.tiling = iTiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = iUsage;
    imageInfo.samples = iNumSamples;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK_RESULT(vkCreateImage(m_Device.GetDevice(), &imageInfo, nullptr, &m_Image))

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_Device.GetDevice(), m_Image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = m_Device.FindMemoryType(memRequirements.memoryTypeBits, iProperties);

    VK_CHECK_RESULT(
        vkAllocateMemory(m_Device.GetDevice(), &allocInfo, nullptr, &m_ImageMemory))

    vkBindImageMemory(m_Device.GetDevice(), m_Image, m_ImageMemory, 0);
}

//----------------------------------------------------------------------------------------------------------------------
static bool HasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

//----------------------------------------------------------------------------------------------------------------------
void Image::TransitionImageLayout(VkImageLayout iOldLayout, VkImageLayout iNewLayout)
{
    CommandBuffer cmd(m_Device);
    cmd.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    TransitionImageLayout(cmd, iOldLayout, iNewLayout);
    cmd.EndAndRun(m_Device.GetGraphicsQueue());
}

//----------------------------------------------------------------------------------------------------------------------
void Image::TransitionImageLayout(CommandBuffer &ioCommandBuffer, VkImageLayout iOldLayout, VkImageLayout iNewLayout)
{
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = iOldLayout;
    barrier.newLayout = iNewLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_Image;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    if (iNewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        if (HasStencilComponent(m_Format))
        {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }
    else
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (iOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && iNewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (iOldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && iNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (iOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && iNewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else if (iOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && iNewLayout == VK_IMAGE_LAYOUT_GENERAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = 0;
        sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    }
    else
    {
        throw std::invalid_argument("Unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        ioCommandBuffer.GetBuffer(), sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}
}