#pragma once

#include "Olympus/CommandBuffer.h"
#include "Olympus/Device.h"
#include "Olympus/Image.h"

namespace olp
{
///  Class which holds and manage a image like a texture. (Not working).
class Texture
{
public:
    ///  Constructor.
    /// @param[in] iDevice Device to initialize the texture with.
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
    /// @param[in] ioCommandBuffer Buffer to add the copy command.
    /// @param[in] iBuffer Buffer to copy.
    void CopyBufferToImage(CommandBuffer &ioCommandBuffer, const VkBuffer &iBuffer);

    /// Create the sampler associate to the vulkan image.
    /// @param[in] iMaxAnisotropy Anisotropy value (unable by default).
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
}