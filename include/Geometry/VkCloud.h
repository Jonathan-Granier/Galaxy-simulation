#pragma once

#include "Olympus/Device.h"
#include "Olympus/CommandBuffer.h"
#include "Olympus/MemoryBuffer.h"
#include "Geometry/CloudVertex.h"
#include <glm/vec3.hpp>
/// @brief
///  Class which holds, allocates and draws a cloud.
class VkCloud
{
public:
    explicit VkCloud(olp::Device &m_Device);
    ~VkCloud() = default;

    VkCloud(const VkCloud &) = delete;
    VkCloud(VkCloud &&ioCloud) noexcept = default;
    VkCloud &operator=(const VkCloud &) = delete;
    VkCloud &operator=(VkCloud &&ioCloud) noexcept = default;

    void Init(uint32_t iNbStars, float iGalaxyDiameters, float iGalaxyThickness, float iInitialSpeed);

    void Destroy();
    void Draw(VkCommandBuffer commandBuffer);

    const olp::MemoryBuffer &GetVertexBuffer() const { return m_VertexBuffer; }
    uint32_t GetSize() const { return static_cast<uint32_t>(m_Cloud.size()); }

private:
    ///  Allocate the cloud in the gpu memory.
    void CreateVertexBuffer();

    /// Vulkan device.
    olp::Device &m_Device;
    /// Point cloud.
    std::vector<CloudVertex> m_Cloud;
    /// Vertex buffer.
    olp::MemoryBuffer m_VertexBuffer;
};