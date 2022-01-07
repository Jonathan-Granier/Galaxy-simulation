#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vulkan/vulkan.h>
#include <vector>

/// @brief
///  A cloud vertex, a 32-byte object.
struct CloudVertex
{
	/// Position
	glm::vec3 Pos{};
	/// Padding (necessary for the use in compute shader).
	float pad1;
	/// star speed
	glm::vec4 Speed{};

	static VkVertexInputBindingDescription GetBindingDescription();
	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
};