#pragma once

#include "Geometry/Cloud.h"
#include "Vulkan/Olympus/Device.h"
#include "Vulkan/Olympus/CommandBuffer.h"
#include "Vulkan/Olympus/MemoryBuffer.h"
#include <glm/vec3.hpp>

/// @brief
///  Class which holds, allocates and draws a cloud.
class VkCloud
{
public:
	explicit VkCloud(Device &m_Device);
	~VkCloud() = default;

	VkCloud(const VkCloud &) = delete;
	VkCloud(VkCloud &&ioCloud) noexcept = default;
	VkCloud &operator=(const VkCloud &) = delete;
	VkCloud &operator=(VkCloud &&ioCloud) noexcept = default;

	void Init(uint32_t iNbStars, float iGalaxyDiameters, float iGalaxyThickness, float iInitialSpeed);

	void Destroy();
	void Draw(VkCommandBuffer commandBuffer);

	const MemoryBuffer &GetVertexBuffer() const { return m_VertexBuffer; }
	const Cloud &GetCloud() const { return m_Cloud; }

private:
	///  Allocate the cloud in the gpu memory.
	void CreateVertexBuffer(float iInitialSpeed);

	/// Vulkan device.
	Device &m_Device;
	/// Point cloud.
	Cloud m_Cloud;
	/// Vertex buffer.
	MemoryBuffer m_VertexBuffer;
};