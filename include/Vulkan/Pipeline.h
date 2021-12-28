#pragma once

#include "Vulkan/Device.h"
#include "Vulkan/Shader.h"

/// @brief
///  Graphics pipeline used to render a scene.
class Pipeline
{
public:
    /// @brief
    ///  Constructor.
    /// @param ioDevice Device to initialize the pipeline with.
    explicit Pipeline(Device &ioDevice);

    /// @brief
    ///  Deleted copy constructor.
    Pipeline(const Pipeline &) = delete;

    /// @brief
    ///  Move constructor.
    /// @param ioPipeline Pipeline to be moved.
    Pipeline(Pipeline &&ioPipeline) noexcept;

    /// @brief
    ///  Destructor.
    virtual ~Pipeline() { Destroy(); }

    /// @brief
    ///  Deleted copy assignment operator.
    /// @return Reference to the current pipeline.
    Pipeline &operator=(const Pipeline &) = delete;

    /// @brief
    ///  Move assignment operator.
    /// @param ioPipeline Pipeline to be moved.
    /// @return Reference to the current pipeline.
    Pipeline &operator=(Pipeline &&ioPipeline) noexcept;

    VkPipeline GetPipeline() { return m_Pipeline; }

    /// @brief
    ///  Creates the pipeline.
    /// @param iPipelineLayout Layout of the pipeline to create.
    /// @param iRenderPass Render pass to create the pipeline with.
    /// @param iSubpassIndex Index of the subpass to create the pipeline with.
    /// @param iShaderName Name of the shaders to be loaded (<name>_vert.spv & <name>_frag.spv).
    /// @param iSwapchainWidth Swapchain width.
    /// @param iSwapchainHeight Swapchain height.
    /// @param iAttachmentCount Attachment count.
    void Create(
        VkPipelineLayout iPipelineLayout,
        VkRenderPass iRenderPass,
        uint32_t iSubpassIndex,
        const std::filesystem::path &iShaderName,
        uint32_t iSwapchainWidth,
        uint32_t iSwapchainHeight,
        uint32_t iAttachmentCount);

    /// @brief
    ///  Destroys the pipeline object.
    void Destroy();

protected:
    /// @brief
    ///  Loads the vertex & fragment shaders from the given file name.
    /// @param iShaderName Name of the shaders to be loaded (<name>_vert.spv & <name>_frag.spv).
    void LoadShader(const std::filesystem::path &iShaderName);

    /// @brief
    ///  Gets the pipeline's corresponding vertex input information.
    /// @return Pipeline's vertex input information object.
    virtual VkPipelineVertexInputStateCreateInfo GetVertexInputInfo() = 0;

    /// @brief
    ///  Gets the pipeline's corresponding input assembly information.
    /// @return Pipeline's input assembly information object.
    virtual VkPipelineInputAssemblyStateCreateInfo GetInputAssemblyInfo() = 0;

    /// @brief
    ///  Gets the pipeline's corresponding rasterization information.
    /// @return Pipeline's rasterization information object.
    virtual VkPipelineRasterizationStateCreateInfo GetRasterizationInfo() = 0;

    Device *m_Device;
    VkPipeline m_Pipeline = VK_NULL_HANDLE;
    Shader m_VertShader;
    Shader m_FragShader;
};