#include "Geometry/VkMesh.h"
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
VkMesh::VkMesh(Device &iDevice)
    : m_Device(&iDevice)
{
}

//----------------------------------------------------------------------------------------------------------------------
void VkMesh::InitCube()
{
    m_Mesh = Mesh::InitCube();
    m_Mesh.Translate({0, 0, 2.0});

    CreateVertexBuffer();
    CreateIndexBuffer();
}

//----------------------------------------------------------------------------------------------------------------------
void VkMesh::InitQuad()
{
    m_Mesh = Mesh::InitQuad();
    CreateVertexBuffer();
    CreateIndexBuffer();
}

//----------------------------------------------------------------------------------------------------------------------
void VkMesh::InitTriangle()
{
    m_Mesh = Mesh::InitTriangle();
    CreateVertexBuffer();
    CreateIndexBuffer();
}

//----------------------------------------------------------------------------------------------------------------------
void VkMesh::Destroy()
{
    m_VertexBuffer.Destroy();
    m_IndexBuffer.Destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void VkMesh::CreateVertexBuffer()
{
    VkDeviceSize bufferSize = sizeof(m_Mesh.Vertices[0]) * m_Mesh.Vertices.size();

    MemoryBuffer stagingBuffer = m_Device->CreateMemoryBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    stagingBuffer.TransferDataInBuffer(m_Mesh.Vertices, bufferSize);

    m_VertexBuffer = m_Device->CreateMemoryBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    m_VertexBuffer.CopyFrom(stagingBuffer.Buffer, bufferSize);
    stagingBuffer.Destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void VkMesh::CreateIndexBuffer()
{
    VkDeviceSize bufferSize = sizeof(m_Mesh.Indices[0]) * m_Mesh.Indices.size();

    MemoryBuffer stagingBuffer = m_Device->CreateMemoryBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    stagingBuffer.TransferDataInBuffer(m_Mesh.Indices, bufferSize);

    m_IndexBuffer = m_Device->CreateMemoryBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    m_IndexBuffer.CopyFrom(stagingBuffer.Buffer, bufferSize);
    stagingBuffer.Destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void VkMesh::Draw(VkCommandBuffer commandBuffer)
{
    VkBuffer vertexBuffers[] = {m_VertexBuffer.Buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_Mesh.Indices.size()), 1, 0, 0, 0);
}