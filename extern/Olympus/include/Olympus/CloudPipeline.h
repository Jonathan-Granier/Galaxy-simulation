#pragma once

#include "Geometry/CloudVertex.h"
#include "Olympus/Pipeline.h"

namespace olp
{
///  Specific pipeline class to render clouds.
template <typename VertexType>
class CloudPipeline : public Pipeline
{
public:
    using Pipeline::Pipeline;

protected:
    ///  Gets the cloud pipeline's vertex input information.
    /// @return Cloud pipeline's vertex input information object.
    VkPipelineVertexInputStateCreateInfo GetVertexInputInfo() override;

    ///  Gets the cloud pipeline's input assembly information.
    /// @return Cloud pipeline's input assembly information object.
    VkPipelineInputAssemblyStateCreateInfo GetInputAssemblyInfo() override;

    ///  Gets the cloud pipeline's rasterization information.
    /// @return Cloud pipeline's rasterization information object.
    VkPipelineRasterizationStateCreateInfo GetRasterizationInfo() override;

    const VkVertexInputBindingDescription m_BindingDescription = VertexType::GetBindingDescription();
    const std::vector<VkVertexInputAttributeDescription> m_AttributeDescriptions =
        VertexType::GetAttributeDescriptions();
};
}
#include "Olympus/CloudPipeline.inl"