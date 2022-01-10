#include "Olympus/Texture.h"
#include "Olympus/Debug.h"
#include "Olympus/MemoryBuffer.h"

namespace olp
{
//----------------------------------------------------------------------------------------------------------------------
Texture::Texture(const Device &iDevice)
    : m_Device(iDevice),
      m_Image(iDevice)
{
}

//----------------------------------------------------------------------------------------------------------------------
Texture::~Texture()
{
    Destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void Texture::Init(const uint8_t *iData, uint32_t iWidth, uint32_t iHeight, VkFormat iFormat)
{

    // Move data in buffer.
    VkDeviceSize imageSize = iWidth * iHeight * 4; // TODO Use format to find this factor.
    MemoryBuffer stagingBuffer = m_Device.CreateMemoryBuffer(
        imageSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    stagingBuffer.TransferDataInBuffer(iData, imageSize);

    // Create Image
    m_Image.Init(iWidth, iHeight, iFormat);
    m_Image.CreateImage(
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    m_Image.CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT, 1);

    CommandBuffer copyCmd(m_Device);
    copyCmd.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    // Prepare for transfer
    m_Image.TransitionImageLayout(copyCmd, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    CopyBufferToImage(copyCmd, stagingBuffer.Buffer);
    m_Image.TransitionImageLayout(
        copyCmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    copyCmd.EndAndRun(m_Device.GetGraphicsQueue()); // TODO Use copy queue.

    stagingBuffer.Destroy();
    CreateSampler();

    m_Descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    m_Descriptor.imageView = m_Image.GetImageView();
    m_Descriptor.sampler = m_TextureSampler;
}

//----------------------------------------------------------------------------------------------------------------------
void Texture::Destroy()
{
    m_Image.Destroy();
    if (m_TextureSampler != VK_NULL_HANDLE)
    {
        vkDestroySampler(m_Device.GetDevice(), m_TextureSampler, nullptr);
        m_TextureSampler = VK_NULL_HANDLE;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Texture::CopyBufferToImage(CommandBuffer &ioCommandBuffer, const VkBuffer &iBuffer)
{
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {m_Image.GetWidth(), m_Image.GetHeight(), 1};

    vkCmdCopyBufferToImage(
        ioCommandBuffer.GetBuffer(), iBuffer, m_Image.GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

//----------------------------------------------------------------------------------------------------------------------
void Texture::CreateSampler(float iMaxAnisotropy)
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV = samplerInfo.addressModeU;
    samplerInfo.addressModeW = samplerInfo.addressModeU;
    samplerInfo.maxAnisotropy = iMaxAnisotropy;
    if (iMaxAnisotropy != 1.0f)
        samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;
    VK_CHECK_RESULT(
        vkCreateSampler(m_Device.GetDevice(), &samplerInfo, nullptr, &m_TextureSampler))
}
}