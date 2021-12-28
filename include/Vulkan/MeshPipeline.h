#pragma once
#include "Geometry/MeshVertex.h"
#include "Vulkan/Pipeline.h"

/// @brief
///  Specific pipeline class to render meshes.
class MeshPipeline : public Pipeline
{
public:
    /// @brief
    ///  Constructor.
    /// @param iDevice Device to initialize the mesh pipeline with.
    explicit MeshPipeline(Device &iDevice);

    /// @brief
    ///  Sets the mode to display the meshes' polygons with.
    /// @param iPolyMode New polygon mode.
    void SetPolygonMode(VkPolygonMode iPolyMode);

protected:
    /// @brief
    ///  Gets the mesh pipeline's vertex input information.
    /// @return Mesh pipeline's vertex input information object.
    VkPipelineVertexInputStateCreateInfo GetVertexInputInfo() override;

    /// @brief
    ///  Gets the mesh pipeline's input assembly information.
    /// @return Mesh pipeline's input assembly information object.
    VkPipelineInputAssemblyStateCreateInfo GetInputAssemblyInfo() override;

    /// @brief
    ///  Gets the mesh pipeline's rasterization information.
    /// @return Mesh pipeline's rasterization information object.
    VkPipelineRasterizationStateCreateInfo GetRasterizationInfo() override;

    const VkVertexInputBindingDescription m_BindingDescription = MeshVertex::GetBindingDescription();
    const std::array<VkVertexInputAttributeDescription, 2> m_AttributeDescriptions =
        MeshVertex::GetAttributeDescriptions();

private:
    VkPolygonMode m_PolygonMode = VK_POLYGON_MODE_FILL;
};