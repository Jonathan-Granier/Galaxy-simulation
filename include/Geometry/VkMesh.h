#pragma once

#include "Geometry/Mesh.h"
#include "Vulkan/Device.h"
#include "Vulkan/MemoryBuffer.h"

/// @brief
///  Class which holds, allocates and draws a mesh.
class VkMesh
{
public:
    VkMesh(Device &iDevice);
    ~VkMesh() = default;
    VkMesh(const VkMesh &) = delete;
    VkMesh(VkMesh &&ioCloud) noexcept = default;
    VkMesh &operator=(const VkMesh &) = delete;
    VkMesh &operator=(VkMesh &&ioCloud) noexcept = default;

    void InitSphere();
    void InitCube();
    void InitQuad();
    void InitTriangle();
    void Destroy();
    VkBuffer GetVertexBuffer() const { return m_VertexBuffer.Buffer; }
    VkBuffer GetIndexBuffer() const { return m_IndexBuffer.Buffer; }
    uint32_t GetIndexCount() const { return static_cast<uint32_t>(m_Mesh.Indices.size() * 3); }

    void Draw(VkCommandBuffer commandBuffer);

private:
    /// @brief
    ///  Allocate the mesh vertices in the gpu memory.
    void CreateVertexBuffer();
    /// @brief
    ///  Allocate the mesh indices (triangle) in the gpu memory.
    void CreateIndexBuffer();

    Device *m_Device;
    Mesh m_Mesh;

    MemoryBuffer m_VertexBuffer;
    MemoryBuffer m_IndexBuffer;
};