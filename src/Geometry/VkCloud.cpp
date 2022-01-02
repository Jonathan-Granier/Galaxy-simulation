#include "Geometry/VkCloud.h"
#include "Geometry/CloudVertex.h"
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
VkCloud::VkCloud(Device& iDevice)
	: m_Device(&iDevice)
{}

//----------------------------------------------------------------------------------------------------------------------
VkCloud::~VkCloud()
{
	Destroy();
}


//----------------------------------------------------------------------------------------------------------------------
void VkCloud::Init()
{
	m_Cloud = Cloud::InitGalaxy();
	CreateVertexBuffer(glm::vec3(0.0, 0, 1.0));
}

//----------------------------------------------------------------------------------------------------------------------
void VkCloud::InitAxisX()
{
	m_Cloud = Cloud::InitAxisX();
	CreateVertexBuffer(glm::vec3(1.0, 0, 0));
}

//----------------------------------------------------------------------------------------------------------------------
void VkCloud::InitAxisY()
{
	m_Cloud = Cloud::InitAxisY();
	CreateVertexBuffer(glm::vec3(0, 1.0, 0));
}

//----------------------------------------------------------------------------------------------------------------------
void VkCloud::InitAxisZ()
{
	m_Cloud = Cloud::InitAxisY();
	CreateVertexBuffer(glm::vec3(0, 0, 1.0));
}

//----------------------------------------------------------------------------------------------------------------------
void VkCloud::Destroy()
{
    m_VertexBuffer.Destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void VkCloud::CreateVertexBuffer(const glm::vec3& color)
{
	std::vector<CloudVertex> points;
	points.reserve(m_Cloud.Points.size());

	for(const glm::vec3& curPt : m_Cloud.Points )
	{
		CloudVertex v;
		v.Pos = curPt;
		v.Color = color;
		points.emplace_back(v);
	}

	VkDeviceSize bufferSize = sizeof(points[0]) * points.size();

	MemoryBuffer stagingBuffer = m_Device->CreateMemoryBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	stagingBuffer.TransferDataInBuffer(points, bufferSize);

	m_VertexBuffer = m_Device->CreateMemoryBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	m_VertexBuffer.CopyFrom(stagingBuffer.Buffer, bufferSize);
	stagingBuffer.Destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void VkCloud::Draw(VkCommandBuffer commandBuffer)
{
	VkBuffer vertexBuffers[] = { m_VertexBuffer.Buffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdDraw(commandBuffer, static_cast<uint32_t>(m_Cloud.Points.size()), 1, 0, 0);
}