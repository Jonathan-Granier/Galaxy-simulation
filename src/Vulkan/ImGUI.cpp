#include "Vulkan/ImGUI.h"
#include "Vulkan/MemoryBuffer.h"
#include "Vulkan/Debug.h"
#include "Vulkan/Shader.h"

ImGUI::ImGUI(Device &ioDevice, VkCommandPool ioCommandPool, BufferFactory &ioBufferFactory)
    : m_Device(ioDevice),
      m_CommandPool(ioCommandPool), // TODO move command pool in device ?
      m_FontImage(m_Device),
      m_BufferFactory(ioBufferFactory)
{
    ImGui::CreateContext();
}

ImGUI::~ImGUI()
{
    ImGui::DestroyContext();
    // Release all Vulkan resources required for rendering imGui
    m_BufferFactory.ReleaseBuffer(m_VertexBuffer);
    m_BufferFactory.ReleaseBuffer(m_IndexBuffer);

    m_FontImage.Destroy();
    vkDestroySampler(m_Device.GetDevice(), m_Sampler, nullptr);
    vkDestroyPipelineCache(m_Device.GetDevice(), pipelineCache, nullptr);
    vkDestroyPipeline(m_Device.GetDevice(), pipeline, nullptr);
    vkDestroyPipelineLayout(m_Device.GetDevice(), pipelineLayout, nullptr);
    vkDestroyDescriptorPool(m_Device.GetDevice(), descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(m_Device.GetDevice(), m_DescriptorSetLayout, nullptr);
}

void ImGUI::init(float width, float height)
{
    // Color scheme
    ImGuiStyle &style = ImGui::GetStyle();
    style.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.6f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_Header] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    // Dimensions
    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2(width, height);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
}

void ImGUI::initResources(VkRenderPass renderPass)
{
    ImGuiIO &io = ImGui::GetIO();

    // Create font texture
    unsigned char *fontData = nullptr;
    int texWidth, texHeight;
    io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);
    VkDeviceSize uploadSize = texWidth * texHeight * 4 * sizeof(char);
    // Create target image for copy
    m_FontImage.Init(texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM);
    m_FontImage.CreateImage(VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, VK_SAMPLE_COUNT_1_BIT);
    m_FontImage.CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT, 1);

    MemoryBuffer stagingBuffer = m_BufferFactory.CreateMemoryBuffer(
        uploadSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    {
        void *tmpData{};
        vkMapMemory(m_Device.GetDevice(), stagingBuffer.Memory, 0, uploadSize, 0, &tmpData);
        memcpy(tmpData, fontData, static_cast<size_t>(uploadSize));
        vkUnmapMemory(m_Device.GetDevice(), stagingBuffer.Memory);
    }

    //m_BufferFactory.TransferDataInBuffer(fontData, uploadSize, stagingBuffer); #DAFUQ TODO
    // Copy buffer data to font image
    CommandBuffer copyCmd(m_Device, m_CommandPool);
    copyCmd.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    // Prepare for transfer
    m_FontImage.TransitionImageLayout(copyCmd, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    // VK_PIPELINE_STAGE_HOST_BIT or VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT.

    // Copy
    VkBufferImageCopy bufferCopyRegion = {};
    bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    bufferCopyRegion.imageSubresource.layerCount = 1;
    bufferCopyRegion.imageExtent.width = texWidth;
    bufferCopyRegion.imageExtent.height = texHeight;
    bufferCopyRegion.imageExtent.depth = 1;

    vkCmdCopyBufferToImage(
        copyCmd.GetBuffer(),
        stagingBuffer.Buffer,
        m_FontImage.GetImage(),
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &bufferCopyRegion);

    // Prepare for shader read
    m_FontImage.TransitionImageLayout(copyCmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    copyCmd.EndAndRun(m_Device.GetGraphicsQueue()); // TODO Use copy queue.
    m_BufferFactory.ReleaseBuffer(stagingBuffer);

    // Font texture Sampler
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.maxAnisotropy = 1.0f;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    VK_CHECK_RESULT(vkCreateSampler(m_Device.GetDevice(), &samplerInfo, nullptr, &m_Sampler));
    // Descriptor pool
    std::vector<VkDescriptorPoolSize> poolSizes = {{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1}};
    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolInfo.pPoolSizes = poolSizes.data();
    descriptorPoolInfo.maxSets = 2;
    VK_CHECK_RESULT(vkCreateDescriptorPool(m_Device.GetDevice(), &descriptorPoolInfo, nullptr, &descriptorPool));

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

    // TODO use descriptor set

    // Descriptor set

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.pSetLayouts = &m_DescriptorSetLayout;
    allocInfo.descriptorSetCount = 1;
    VK_CHECK_RESULT(vkAllocateDescriptorSets(m_Device.GetDevice(), &allocInfo, &m_DescriptorSet));

    VkDescriptorImageInfo fontDescriptor{};
    fontDescriptor.sampler = m_Sampler;
    fontDescriptor.imageView = m_FontImage.GetImageView();
    fontDescriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    std::vector<VkWriteDescriptorSet> writeDescriptorSets(1);
    writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[0].dstSet = m_DescriptorSet;
    writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSets[0].dstBinding = 0;
    writeDescriptorSets[0].pImageInfo = &fontDescriptor;
    writeDescriptorSets[0].descriptorCount = 1;

    vkUpdateDescriptorSets(m_Device.GetDevice(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);

    // Pipeline cache
    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    VK_CHECK_RESULT(vkCreatePipelineCache(m_Device.GetDevice(), &pipelineCacheCreateInfo, nullptr, &pipelineCache));

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

    VK_CHECK_RESULT(vkCreatePipelineLayout(m_Device.GetDevice(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout));

    // Setup graphics pipeline for UI rendering
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyState.flags = 0;
    inputAssemblyState.primitiveRestartEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo rasterizationState{};
    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationState.cullMode = VK_CULL_MODE_NONE;
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

    std::filesystem::path path = std::filesystem::path(OLYMPUS_ROOT) / std::filesystem::path("shaders") / "ui";
    std::filesystem::path vertexPath = path;
    std::filesystem::path fragPath = path;
    vertexPath += "_vert.spv";
    fragPath += "_frag.spv";
    Shader m_VertShader(m_Device);
    Shader m_FragShader(m_Device);
    m_VertShader.Load(vertexPath);
    m_FragShader.Load(fragPath);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = m_VertShader.GetShaderModule();
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = m_FragShader.GetShaderModule();
    fragShaderStageInfo.pName = "main";

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages = {vertShaderStageInfo, fragShaderStageInfo};

    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.layout = pipelineLayout;
    pipelineCreateInfo.renderPass = renderPass;
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

    VK_CHECK_RESULT(vkCreateGraphicsPipelines(m_Device.GetDevice(), pipelineCache, 1, &pipelineCreateInfo, nullptr, &pipeline));

    m_FragShader.Destroy();
    m_VertShader.Destroy();
}

void ImGUI::newFrame(bool updateFrameGraph)
{
    ImGui::NewFrame();

    // Init imGui windows and elements

    ImVec4 clear_color = ImColor(114, 144, 154);
    static float f = 0.0f;
    ImGui::TextUnformatted("Olympus");
    ImGui::TextUnformatted("TRUC");

    // Update frame time display
    if (updateFrameGraph)
    {
        std::rotate(uiSettings.frameTimes.begin(), uiSettings.frameTimes.begin() + 1, uiSettings.frameTimes.end());
        float frameTime = 1000.0f / (12 * 1000.0f);
        uiSettings.frameTimes.back() = frameTime;
        if (frameTime < uiSettings.frameTimeMin)
        {
            uiSettings.frameTimeMin = frameTime;
        }
        if (frameTime > uiSettings.frameTimeMax)
        {
            uiSettings.frameTimeMax = frameTime;
        }
    }

    ImGui::PlotLines("Frame Times", &uiSettings.frameTimes[0], 50, 0, "", uiSettings.frameTimeMin, uiSettings.frameTimeMax, ImVec2(0, 80));

    ImGui::Text("Camera");
    //ImGui::InputFloat3("position", 10.0f, 2);
    //ImGui::InputFloat3("rotation", 20.0f, 2);

    ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);
    ImGui::Begin("Example settings");
    ImGui::Checkbox("Render models", &uiSettings.displayModels);
    ImGui::Checkbox("Display logos", &uiSettings.displayLogos);
    ImGui::Checkbox("Display background", &uiSettings.displayBackground);
    ImGui::Checkbox("Animate light", &uiSettings.animateLight);
    ImGui::SliderFloat("Light speed", &uiSettings.lightSpeed, 0.1f, 1.0f);
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
    //ImGui::ShowDemoWindow();

    // Render to generate draw buffers
    ImGui::Render();
}

void ImGUI::updateBuffers()
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
    if ((m_VertexBuffer.Buffer == VK_NULL_HANDLE) || (vertexCount != imDrawData->TotalVtxCount))
    {
        m_BufferFactory.ReleaseBuffer(m_VertexBuffer);
        m_VertexBuffer = m_BufferFactory.CreateMemoryBuffer(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

        vertexCount = imDrawData->TotalVtxCount;
        m_BufferFactory.Map(m_VertexBuffer);
    }

    // Index buffer
    if ((m_IndexBuffer.Buffer == VK_NULL_HANDLE) || (indexCount < imDrawData->TotalIdxCount))
    {
        m_BufferFactory.ReleaseBuffer(m_IndexBuffer);
        m_IndexBuffer = m_BufferFactory.CreateMemoryBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

        indexCount = imDrawData->TotalIdxCount;
        m_BufferFactory.Map(m_IndexBuffer);
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
    m_BufferFactory.Flush(m_VertexBuffer);
    m_BufferFactory.Flush(m_IndexBuffer);
}

void ImGUI::drawFrame(VkCommandBuffer commandBuffer)
{
    ImGuiIO &io = ImGui::GetIO();

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &m_DescriptorSet, 0, nullptr);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    VkViewport viewport{};
    viewport.width = ImGui::GetIO().DisplaySize.x;
    viewport.height = ImGui::GetIO().DisplaySize.y;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    // UI scale and translate via push constants
    pushConstBlock.scale = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
    pushConstBlock.translate = glm::vec2(-1.0f);
    vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstBlock), &pushConstBlock);

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