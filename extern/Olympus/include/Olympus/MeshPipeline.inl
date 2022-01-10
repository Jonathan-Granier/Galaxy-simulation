namespace olp
{
//----------------------------------------------------------------------------------------------------------------------
template <typename VertexType>
VkPipelineVertexInputStateCreateInfo MeshPipeline<VertexType>::GetVertexInputInfo()
{
    // VERTEX INPUT
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_AttributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &m_BindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = m_AttributeDescriptions.data();
    return vertexInputInfo;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename VertexType>
VkPipelineInputAssemblyStateCreateInfo MeshPipeline<VertexType>::GetInputAssemblyInfo()
{
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    return inputAssembly;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename VertexType>
VkPipelineRasterizationStateCreateInfo MeshPipeline<VertexType>::GetRasterizationInfo()
{
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = m_PolygonMode;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    return rasterizer;
}
}