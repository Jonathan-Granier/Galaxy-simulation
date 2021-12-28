#include "Geometry/VkMesh.h"
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
VkMesh::VkMesh(Device &device, const BufferFactory &bufferFactory)
    : m_Device(&device), m_BufferFactory(&bufferFactory)
{
}

//----------------------------------------------------------------------------------------------------------------------
void VkMesh::InitCube()
{
    m_Mesh = Mesh::InitCube();
    m_Mesh.Translate({10, 0, 0});

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
void VkMesh::Destroy()
{
    m_BufferFactory->ReleaseBuffer(m_VertexBuffer);
    m_BufferFactory->ReleaseBuffer(m_IndexBuffer);
}

//----------------------------------------------------------------------------------------------------------------------
void VkMesh::CreateVertexBuffer()
{
    VkDeviceSize bufferSize = sizeof(m_Mesh.Vertices[0]) * m_Mesh.Vertices.size();

    MemoryBuffer stagingBuffer = m_BufferFactory->CreateMemoryBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    m_BufferFactory->TransferDataInBuffer(m_Mesh.Vertices, bufferSize, stagingBuffer);

    m_VertexBuffer = m_BufferFactory->CreateMemoryBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    m_BufferFactory->CopyBuffer(stagingBuffer.Buffer, m_VertexBuffer.Buffer, bufferSize);
    m_BufferFactory->ReleaseBuffer(stagingBuffer);
}

//----------------------------------------------------------------------------------------------------------------------
void VkMesh::CreateIndexBuffer()
{
    VkDeviceSize bufferSize = sizeof(m_Mesh.Indices[0]) * m_Mesh.Indices.size();

    MemoryBuffer stagingBuffer = m_BufferFactory->CreateMemoryBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    m_BufferFactory->TransferDataInBuffer(m_Mesh.Indices, bufferSize, stagingBuffer);

    m_IndexBuffer = m_BufferFactory->CreateMemoryBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    m_BufferFactory->CopyBuffer(stagingBuffer.Buffer, m_IndexBuffer.Buffer, bufferSize);
    m_BufferFactory->ReleaseBuffer(stagingBuffer);
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