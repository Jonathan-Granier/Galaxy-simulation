#include "Geometry/VkCloud.h"
#include "Geometry/CloudVertex.h"
#include <iostream>
#include <glm/geometric.hpp>

//----------------------------------------------------------------------------------------------------------------------
VkCloud::VkCloud(Device &iDevice)
	: m_Device(iDevice)
{
}

//----------------------------------------------------------------------------------------------------------------------
void VkCloud::Init(uint32_t iNbStars, float iGalaxyDiameters, float iGalaxyThickness, float iInitialSpeed)
{
	m_Cloud = Cloud::InitGalaxy(iNbStars, iGalaxyDiameters, iGalaxyThickness);
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
	std::vector<CloudVertex> points;
	points.reserve(m_Cloud.Points.size());

	for (const glm::vec3 &curPt : m_Cloud.Points)
	{
		CloudVertex v;
		v.Pos = curPt;
		v.Speed = glm::vec4(glm::normalize(glm::cross(curPt, glm::vec3(0.f, 1.f, 0.f))) * iInitialSpeed, 0);
		points.emplace_back(v);
	}

	VkDeviceSize bufferSize = sizeof(points[0]) * points.size();

	MemoryBuffer stagingBuffer = m_Device.CreateMemoryBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	stagingBuffer.TransferDataInBuffer(points, bufferSize);

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
	vkCmdDraw(commandBuffer, static_cast<uint32_t>(m_Cloud.Points.size()), 1, 0, 0);
}