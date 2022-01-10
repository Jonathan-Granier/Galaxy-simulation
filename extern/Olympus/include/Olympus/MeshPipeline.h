#pragma once
#include "Geometry/MeshVertex.h"
#include "Olympus/Pipeline.h"

namespace olp
{
///  Specific pipeline class to render meshes.
template <typename VertexType>
class MeshPipeline : public Pipeline
{
public:
    using Pipeline::Pipeline;

    ///  Sets the mode to display the meshes' polygons with.
    /// @param iPolyMode New polygon mode.
    void SetPolygonMode(VkPolygonMode iPolyMode) { m_PolygonMode = iPolyMode; }

protected:
    ///  Gets the mesh pipeline's vertex input information.
    /// @return Mesh pipeline's vertex input information object.
    VkPipelineVertexInputStateCreateInfo GetVertexInputInfo() override;

    ///  Gets the mesh pipeline's input assembly information.
    /// @return Mesh pipeline's input assembly information object.
    VkPipelineInputAssemblyStateCreateInfo GetInputAssemblyInfo() override;

    ///  Gets the mesh pipeline's rasterization information.
    /// @return Mesh pipeline's rasterization information object.
    VkPipelineRasterizationStateCreateInfo GetRasterizationInfo() override;

    const VkVertexInputBindingDescription m_BindingDescription = VertexType::GetBindingDescription();
    const std::array<VkVertexInputAttributeDescription, 2> m_AttributeDescriptions =
        VertexType::GetAttributeDescriptions();

private:
    /// Polygon mode to draw the mesh.
    VkPolygonMode m_PolygonMode = VK_POLYGON_MODE_FILL;
};
}
#include "MeshPipeline.inl"