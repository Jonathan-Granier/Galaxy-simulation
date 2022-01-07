#pragma once

#include "Geometry/CloudVertex.h"
#include "Vulkan/VulkanObject/Pipeline.h"

/// @brief
///  Specific pipeline class to render clouds.
template <typename VertexType>
class CloudPipeline : public Pipeline
{
public:
	using Pipeline::Pipeline;

protected:
	/// @brief
	///  Gets the cloud pipeline's vertex input information.
	/// @return Cloud pipeline's vertex input information object.
	VkPipelineVertexInputStateCreateInfo GetVertexInputInfo() override;

	/// @brief
	///  Gets the cloud pipeline's input assembly information.
	/// @return Cloud pipeline's input assembly information object.
	VkPipelineInputAssemblyStateCreateInfo GetInputAssemblyInfo() override;

	/// @brief
	///  Gets the cloud pipeline's rasterization information.
	/// @return Cloud pipeline's rasterization information object.
	VkPipelineRasterizationStateCreateInfo GetRasterizationInfo() override;

	const VkVertexInputBindingDescription m_BindingDescription = VertexType::GetBindingDescription();
	const std::vector<VkVertexInputAttributeDescription> m_AttributeDescriptions = VertexType::GetAttributeDescriptions();
};

#include "Vulkan/VulkanObject/CloudPipeline.inl"