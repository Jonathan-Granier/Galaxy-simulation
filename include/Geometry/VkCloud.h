#pragma once

#include "Geometry/Cloud.h"
#include "Vulkan/Device.h"
#include "Vulkan/CommandBuffer.h"
#include "Vulkan/MemoryBuffer.h"
#include <glm/vec3.hpp>

/// @brief
///  Class which holds, allocates and draws a mesh.
class VkCloud
{
public:
	explicit VkCloud(Device &m_Device);
	~VkCloud() = default;

	VkCloud(const VkCloud &) = delete;
	VkCloud(VkCloud &&ioCloud) noexcept = default;
	VkCloud &operator=(const VkCloud &) = delete;
	VkCloud &operator=(VkCloud &&ioCloud) noexcept = default;

	void Init();
	void InitAxisX();
	void InitAxisY();
	void InitAxisZ();

	void Destroy();
	void Draw(VkCommandBuffer commandBuffer);

private:
	/// @brief
	///  Allocate the cloud in the gpu memory.
	void CreateVertexBuffer(const glm::vec3 &color = glm::vec3(1.0, 0, 0));

	Device *m_Device;
	Cloud m_Cloud;

	MemoryBuffer m_VertexBuffer;
};