#pragma once

#include "Vulkan/CommandBuffer.h"
#include "Vulkan/Device.h"
#include "Vulkan/Image.h"

/// @brief
///  Class which holds and manage a image like a texture. (Not working).
class Texture
{
public:
    explicit Texture(Device &iDevice);
    ~Texture();

    void Init(const uint8_t *iData, uint32_t iWidth, uint32_t iHeight, VkFormat iFormat);
    void Destroy();
    VkDescriptorImageInfo GetDescriptor() { return m_Descriptor; }

protected:
    void CopyBufferToImage(CommandBuffer &ioCommandBuffer, VkBuffer &iBuffer);
    void CreateSampler(float iMaxAnisotropy = 1.0f);

    Device &m_Device;
    Image m_Image;
    VkSampler m_TextureSampler = VK_NULL_HANDLE;
    VkDescriptorImageInfo m_Descriptor;
};