#include "Geometry/VkCloud.h"
#include <iostream>
#include <glm/geometric.hpp>
#include "MathHelper.h"
//----------------------------------------------------------------------------------------------------------------------
VkCloud::VkCloud(olp::Device &iDevice)
    : m_Device(iDevice)
{
}

//----------------------------------------------------------------------------------------------------------------------
void VkCloud::Init(uint32_t iNbStars, float iGalaxyDiameters, float iGalaxyThickness, float iInitialSpeed)
{
    m_Cloud.resize(iNbStars);

    for (CloudVertex &vertex : m_Cloud)
    {
        vertex.Pos = Spherical(RandomFloat(0.0f, iGalaxyDiameters * 0.5f), RandomFloat(0.0, 2 * PI), RandomFloat(0.0f, PI));
        vertex.Pos.y *= iGalaxyThickness / iGalaxyDiameters;
        vertex.Speed = glm::vec4(glm::normalize(glm::cross(vertex.Pos, glm::vec3(0.f, 1.f, 0.f))) * iInitialSpeed, 0);
    }
    CreateVertexBuffer(iInitialSpeed);
}

//----------------------------------------------------------------------------------------------------------------------
void VkCloud::Destroy()
{
    m_VertexBuffer.Destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void VkCloud::CreateVertexBuffer(float iInitialSpeed)
{
    VkDeviceSize bufferSize = sizeof(m_Cloud[0]) * m_Cloud.size();

    olp::MemoryBuffer stagingBuffer = m_Device.CreateMemoryBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    stagingBuffer.TransferDataInBuffer(m_Cloud, bufferSize);

    m_VertexBuffer = m_Device.CreateMemoryBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    m_VertexBuffer.CopyFrom(stagingBuffer.Buffer, bufferSize);
    stagingBuffer.Destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void VkCloud::Draw(VkCommandBuffer commandBuffer)
{
    const VkBuffer vertexBuffers[] = {m_VertexBuffer.Buffer};
    const VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdDraw(commandBuffer, static_cast<uint32_t>(m_Cloud.size()), 1, 0, 0);
}