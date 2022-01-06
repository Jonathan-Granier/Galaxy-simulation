#pragma once

#include "Geometry/CloudVertex.h"
#include "Vulkan/Pipeline.h"

/// @brief
///  Specific pipeline class to render clouds.
class CloudPipeline : public Pipeline
{
public:
	/// @brief
	///  Constructor.
	/// @param iDevice Device to initialize the cloud pipeline with.
	explicit CloudPipeline(const Device &iDevice)
		: Pipeline(iDevice)
	{
	}

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

	const VkVertexInputBindingDescription m_BindingDescription = CloudVertex::GetBindingDescription();
	const std::vector<VkVertexInputAttributeDescription> m_AttributeDescriptions = CloudVertex::GetAttributeDescriptions();
};