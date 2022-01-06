#pragma once

#include "Vulkan/VulkanObject/CommandBuffer.h"
#include "Vulkan/VulkanObject/Device.h"
#include "Vulkan/VulkanObject/Image.h"

///  Class which holds and manage a image like a texture. (Not working).
class Texture
{
public:
    /// @brief
    ///  Constructor.
    /// @param iDevice Device to initialize the texture with.
    explicit Texture(const Device &iDevice);
    /// Destructor.
    ~Texture();

    /// Init the texture with a buffer.
    void Init(const uint8_t *iData, uint32_t iWidth, uint32_t iHeight, VkFormat iFormat);
    /// Destroys the texture.
    void Destroy();
    /// Get descriptor information.
    /// @return A Descriptor image info.
    VkDescriptorImageInfo GetDescriptor() { return m_Descriptor; }

protected:
    /// Copy the buffer to a vulkan image.
    /// @param ioCommandBuffer Buffer to add the copy command.
    /// @param iBuffer Buffer to copy.
    void CopyBufferToImage(CommandBuffer &ioCommandBuffer, const VkBuffer &iBuffer);

    /// Create the sampler associate to the vulkan image.
    /// @param iMaxAnisotropy Anisotropy value (unable by default).
    void CreateSampler(float iMaxAnisotropy = 1.0f);

    /// Vulkan device.
    const Device &m_Device;
    /// Vulkan Image.
    Image m_Image;
    /// Associate sampler.
    VkSampler m_TextureSampler = VK_NULL_HANDLE;
    /// Associate descriptor.
    VkDescriptorImageInfo m_Descriptor;
};