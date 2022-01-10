#include "Olympus/ImGUI.h"
#include "Olympus/MemoryBuffer.h"
#include "Olympus/Debug.h"
#include "Olympus/Shader.h"

namespace olp
{
//----------------------------------------------------------------------------------------------------------------------
ImGUI::ImGUI(const Device &iDevice)
    : m_Device(iDevice),
      m_FontTexture(iDevice),
      m_DescriptorSet(iDevice)
{
}

//----------------------------------------------------------------------------------------------------------------------
ImGUI::~ImGUI()
{
}

//----------------------------------------------------------------------------------------------------------------------
void ImGUI::Destroy()
{
    m_VertexBuffer.Destroy();
    m_IndexBuffer.Destroy();

    m_FontTexture.Destroy();
    vkDestroyPipeline(m_Device.GetDevice(), m_Pipeline, nullptr);
    vkDestroyPipelineLayout(m_Device.GetDevice(), m_PipelineLayout, nullptr);
    vkDestroyDescriptorPool(m_Device.GetDevice(), m_DescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(m_Device.GetDevice(), m_DescriptorSetLayout, nullptr);
}

//----------------------------------------------------------------------------------------------------------------------
void ImGUI::CreateResources(VkRenderPass iRenderPass)
{
    ImGuiIO &io = ImGui::GetIO();

    // Create font texture
    unsigned char *fontData = nullptr;
    int texWidth, texHeight;
    io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);

    m_FontTexture.Init(fontData, texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM);
    m_FontTexture.GetDescriptor();
    CreateDescriptors();
    CreatePipeline(iRenderPass);
}

//----------------------------------------------------------------------------------------------------------------------
void ImGUI::CreatePipeline(VkRenderPass iRenderPass)
{
    // Pipeline layout
    // Push constants for UI rendering parameters

    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PushConstBlock);

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &m_DescriptorSetLayout;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

    VK_CHECK_RESULT(vkCreatePipelineLayout(m_Device.GetDevice(), &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout));

    // Setup graphics m_Pipeline for UI rendering
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyState.flags = 0;
    inputAssemblyState.primitiveRestartEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo rasterizationState{};
    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationState.cullMode = VK_CULL_MODE_NONE; // DIFF
    rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationState.flags = 0;
    rasterizationState.depthClampEnable = VK_FALSE;
    rasterizationState.lineWidth = 1.0f;

    // Enable blending
    VkPipelineColorBlendAttachmentState blendAttachmentState{};
    blendAttachmentState.blendEnable = VK_TRUE;
    blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlendState{};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &blendAttachmentState;

    VkPipelineDepthStencilStateCreateInfo depthStencilState{};
    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilState.depthTestEnable = VK_FALSE;
    depthStencilState.depthWriteEnable = VK_FALSE;
    depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
    viewportState.flags = 0;

    VkPipelineMultisampleStateCreateInfo multisampleState{};
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleState.flags = 0;

    std::vector<VkDynamicState> dynamicStateEnables = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pDynamicStates = dynamicStateEnables.data();
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
    dynamicState.flags = 0;

    std::filesystem::path path = std::filesystem::path(OLYMPUS_SHADERS) / "ui";
    std::filesystem::path vertexPath = path;
    std::filesystem::path fragPath = path;
    vertexPath += "_vert.spv";
    fragPath += "_frag.spv";
    Shader vertShader(m_Device);
    Shader fragShader(m_Device);
    vertShader.Load(vertexPath);
    fragShader.Load(fragPath);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShader.GetShaderModule();
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShader.GetShaderModule();
    fragShaderStageInfo.pName = "main";

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages = {vertShaderStageInfo, fragShaderStageInfo};

    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.layout = m_PipelineLayout;
    pipelineCreateInfo.renderPass = iRenderPass;
    pipelineCreateInfo.flags = 0;
    pipelineCreateInfo.basePipelineIndex = -1;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineCreateInfo.pStages = shaderStages.data();

    // Vertex bindings an attributes based on ImGui vertex definition
    std::vector<VkVertexInputBindingDescription> vertexInputBindings(1);
    VkVertexInputBindingDescription vInputBindDescription{};
    vertexInputBindings[0].binding = 0;
    vertexInputBindings[0].stride = sizeof(ImDrawVert);
    vertexInputBindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::vector<VkVertexInputAttributeDescription> vertexInputAttributes(3);
    vertexInputAttributes[0].location = 0;
    vertexInputAttributes[0].binding = 0;
    vertexInputAttributes[0].format = VK_FORMAT_R32G32_SFLOAT;
    vertexInputAttributes[0].offset = offsetof(ImDrawVert, pos);

    vertexInputAttributes[1].location = 1;
    vertexInputAttributes[1].binding = 0;
    vertexInputAttributes[1].format = VK_FORMAT_R32G32_SFLOAT;
    vertexInputAttributes[1].offset = offsetof(ImDrawVert, uv);

    vertexInputAttributes[2].location = 2;
    vertexInputAttributes[2].binding = 0;
    vertexInputAttributes[2].format = VK_FORMAT_R8G8B8A8_UNORM;
    vertexInputAttributes[2].offset = offsetof(ImDrawVert, col);

    VkPipelineVertexInputStateCreateInfo vertexInputState{};
    vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindings.size());
    vertexInputState.pVertexBindingDescriptions = vertexInputBindings.data();
    vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributes.size());
    vertexInputState.pVertexAttributeDescriptions = vertexInputAttributes.data();

    pipelineCreateInfo.pVertexInputState = &vertexInputState;

    VK_CHECK_RESULT(vkCreateGraphicsPipelines(m_Device.GetDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_Pipeline));

    fragShader.Destroy();
    vertShader.Destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void ImGUI::CreateDescriptors()
{
    // Descriptor pool
    std::vector<VkDescriptorPoolSize> poolSizes = {{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1}};
    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolInfo.pPoolSizes = poolSizes.data();
    descriptorPoolInfo.maxSets = 2;

    VK_CHECK_RESULT(vkCreateDescriptorPool(m_Device.GetDevice(), &descriptorPoolInfo, nullptr, &m_DescriptorPool));

    // Descriptor set layout
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings(1);
    setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    setLayoutBindings[0].binding = 0;
    setLayoutBindings[0].descriptorCount = 1;

    VkDescriptorSetLayoutCreateInfo descriptorLayout{};
    descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayout.pBindings = setLayoutBindings.data();
    descriptorLayout.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());

    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device.GetDevice(), &descriptorLayout, nullptr, &m_DescriptorSetLayout));
    m_DescriptorSet.AllocateDescriptorSets(m_DescriptorSetLayout, m_DescriptorPool, 1);
    m_DescriptorSet.AddWriteDescriptor(0, m_FontTexture.GetDescriptor(), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
    m_DescriptorSet.UpdateDescriptorSets();
}

//----------------------------------------------------------------------------------------------------------------------
void ImGUI::Update()
{
    ImDrawData *imDrawData = ImGui::GetDrawData();

    // Note: Alignment is done inside buffer creation
    VkDeviceSize vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
    VkDeviceSize indexBufferSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

    if ((vertexBufferSize == 0) || (indexBufferSize == 0))
    {
        return;
    }

    // Update buffers only if vertex or index count has been changed compared to current buffer size
    // Vertex buffer
    if ((m_VertexBuffer.Buffer == VK_NULL_HANDLE) || (m_VertexCount != imDrawData->TotalVtxCount))
    {
        m_VertexBuffer.Destroy();
        m_VertexBuffer = m_Device.CreateMemoryBuffer(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

        m_VertexCount = imDrawData->TotalVtxCount;
        m_VertexBuffer.Map();
    }

    // Index buffer
    if ((m_IndexBuffer.Buffer == VK_NULL_HANDLE) || (m_IndexCount < imDrawData->TotalIdxCount))
    {
        m_IndexBuffer.Destroy();
        m_IndexBuffer = m_Device.CreateMemoryBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

        m_IndexCount = imDrawData->TotalIdxCount;
        m_IndexBuffer.Map();
    }

    // Upload data
    ImDrawVert *vtxDst = (ImDrawVert *)m_VertexBuffer.Mapped;
    ImDrawIdx *idxDst = (ImDrawIdx *)m_IndexBuffer.Mapped;

    for (int n = 0; n < imDrawData->CmdListsCount; n++)
    {
        const ImDrawList *cmd_list = imDrawData->CmdLists[n];
        memcpy(vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
        vtxDst += cmd_list->VtxBuffer.Size;
        idxDst += cmd_list->IdxBuffer.Size;
    }

    // Flush to make writes visible to GPU
    m_VertexBuffer.Flush();
    m_IndexBuffer.Flush();
}

//----------------------------------------------------------------------------------------------------------------------
void ImGUI::Draw(VkCommandBuffer commandBuffer)
{
    ImGuiIO &io = ImGui::GetIO();

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSet.GetDescriptorSet(), 0, nullptr);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);

    VkViewport viewport{};
    viewport.width = ImGui::GetIO().DisplaySize.x;
    viewport.height = ImGui::GetIO().DisplaySize.y;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    // UI scale and translate via push constants
    pushConstBlock.scale = ImVec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
    pushConstBlock.translate = ImVec2(-1.0f, -1.0f);
    vkCmdPushConstants(commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstBlock), &pushConstBlock);

    // Render commands
    ImDrawData *imDrawData = ImGui::GetDrawData();
    int32_t vertexOffset = 0;
    int32_t indexOffset = 0;

    if (imDrawData->CmdListsCount > 0)
    {

        VkDeviceSize offsets[1] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_VertexBuffer.Buffer, offsets);
        vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT16);

        for (int32_t i = 0; i < imDrawData->CmdListsCount; i++)
        {
            const ImDrawList *cmd_list = imDrawData->CmdLists[i];
            for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
            {
                const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[j];
                VkRect2D scissorRect;
                scissorRect.offset.x = std::max((int32_t)(pcmd->ClipRect.x), 0);
                scissorRect.offset.y = std::max((int32_t)(pcmd->ClipRect.y), 0);
                scissorRect.extent.width = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
                scissorRect.extent.height = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y);
                vkCmdSetScissor(commandBuffer, 0, 1, &scissorRect);
                vkCmdDrawIndexed(commandBuffer, pcmd->ElemCount, 1, indexOffset, vertexOffset, 0);
                indexOffset += pcmd->ElemCount;
            }
            vertexOffset += cmd_list->VtxBuffer.Size;
        }
    }
}
}