#include "Olympus/Pipeline.h"
#include "Olympus/Debug.h"
namespace olp
{
//----------------------------------------------------------------------------------------------------------------------
Pipeline::Pipeline(const Device &iDevice)
    : m_Device(iDevice),
      m_VertShader(iDevice),
      m_FragShader(iDevice)
{
}

//----------------------------------------------------------------------------------------------------------------------
void Pipeline::LoadShader(const std::filesystem::path &iShaderPath)
{
    std::filesystem::path path = iShaderPath;
    std::filesystem::path vertexPath = path;
    std::filesystem::path fragPath = path;
    vertexPath += "_vert.spv";
    fragPath += "_frag.spv";
    m_VertShader.Load(vertexPath);
    m_FragShader.Load(fragPath);
}

//----------------------------------------------------------------------------------------------------------------------
static std::vector<VkPipelineColorBlendAttachmentState> GetColorBlendAttachment(uint32_t iNbAttachment)
{
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    return std::vector<VkPipelineColorBlendAttachmentState>(iNbAttachment, colorBlendAttachment);
}

//----------------------------------------------------------------------------------------------------------------------
void Pipeline::Create(
    VkPipelineLayout iPipelineLayout,
    VkRenderPass iRenderPass,
    uint32_t iSubpassIndex,
    const std::filesystem::path &iShaderPath,
    uint32_t iSwapchainWidth,
    uint32_t iSwapchainHeight,
    uint32_t iAttachmentCount)
{
    Destroy();

    LoadShader(iShaderPath);

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
    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = GetVertexInputInfo();
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = GetInputAssemblyInfo();
    VkPipelineRasterizationStateCreateInfo rasterizer = GetRasterizationInfo();

    // Viewports
    VkViewport viewport;
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = static_cast<float>(iSwapchainWidth);
    viewport.height = static_cast<float>(iSwapchainHeight);
    viewport.minDepth = 0;
    viewport.maxDepth = 1;

    VkRect2D scissor;
    scissor.offset = {0, 0};
    scissor.extent.width = iSwapchainWidth;
    scissor.extent.height = iSwapchainHeight;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    // Spec say must be 1
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.pScissors = &scissor;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = m_Device.GetMaxUsableSampleCount();
    //multisampling.minSampleShading = .2f;

    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments = GetColorBlendAttachment(iAttachmentCount);

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
    colorBlending.pAttachments = colorBlendAttachments.data();
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    //	depthStencil.front // Optional
    //	depthStencil.back // Optional

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.layout = iPipelineLayout;
    pipelineInfo.renderPass = iRenderPass;
    pipelineInfo.subpass = iSubpassIndex;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    VK_CHECK_RESULT(vkCreateGraphicsPipelines(
        m_Device.GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline))

    m_FragShader.Destroy();
    m_VertShader.Destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void Pipeline::Destroy()
{
    if (m_Pipeline == VK_NULL_HANDLE)
        return;

    vkDestroyPipeline(m_Device.GetDevice(), m_Pipeline, nullptr);
    m_Pipeline = VK_NULL_HANDLE;
}
}