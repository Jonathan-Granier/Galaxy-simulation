#pragma once

#include "Olympus/Device.h"
#include "Olympus/Shader.h"

namespace olp
{
///  Graphics pipeline used to render a scene.
class Pipeline
{
public:
    ///  Constructor.
    /// @param[in] iDevice Device to initialize the pipeline with.
    explicit Pipeline(const Device &iDevice);

    ///  Deleted copy constructor.
    Pipeline(const Pipeline &) = delete;

    ///  Move constructor.
    /// @param ioPipeline Pipeline to be moved.
    Pipeline(Pipeline &&ioPipeline) noexcept = default;

    ///  Destructor.
    virtual ~Pipeline() { Destroy(); }

    ///  Deleted copy assignment operator.
    /// @return Reference to the current pipeline.
    Pipeline &operator=(const Pipeline &) = delete;

    ///  Move assignment operator.
    /// @param ioPipeline Pipeline to be moved.
    /// @return Reference to the current pipeline.
    Pipeline &operator=(Pipeline &&ioPipeline) noexcept = default;

    VkPipeline GetPipeline() { return m_Pipeline; }

    ///  Creates the pipeline.
    /// @param[in] iPipelineLayout Layout of the pipeline to create.
    /// @param[in] iRenderPass Render pass to create the pipeline with.
    /// @param[in] iSubpassIndex Index of the subpass to create the pipeline with.
    /// @param[in] iShaderPath Path of the shaders to be loaded (<path/to/shaders/name>_vert.spv & <path/to/shaders/name>_frag.spv).
    /// @param[in] iSwapchainWidth Swapchain width.
    /// @param[in] iSwapchainHeight Swapchain height.
    /// @param[in] iAttachmentCount Attachment count.
    void Create(
        VkPipelineLayout iPipelineLayout,
        VkRenderPass iRenderPass,
        uint32_t iSubpassIndex,
        const std::filesystem::path &iShaderPath,
        uint32_t iSwapchainWidth,
        uint32_t iSwapchainHeight,
        uint32_t iAttachmentCount);

    /// @brief
    ///  Destroys the pipeline object.
    void Destroy();

protected:
    ///  Loads the vertex & fragment shaders from the given file name.
    /// @param[in] iShaderPath Path of the shaders to be loaded (<path/to/shaders/name>_vert.spv & <path/to/shaders/name>_frag.spv).
    void LoadShader(const std::filesystem::path &iShaderPath);

    ///  Gets the pipeline's corresponding vertex input information.
    /// @return Pipeline's vertex input information object.
    virtual VkPipelineVertexInputStateCreateInfo GetVertexInputInfo() = 0;

    ///  Gets the pipeline's corresponding input assembly information.
    /// @return Pipeline's input assembly information object.
    virtual VkPipelineInputAssemblyStateCreateInfo GetInputAssemblyInfo() = 0;

    ///  Gets the pipeline's corresponding rasterization information.
    /// @return Pipeline's rasterization information object.
    virtual VkPipelineRasterizationStateCreateInfo GetRasterizationInfo() = 0;

    /// Vulkan device.
    const Device &m_Device;
    /// Vulkan pipeline.
    VkPipeline m_Pipeline = VK_NULL_HANDLE;
    /// Vertex shader.
    Shader m_VertShader;
    /// Fragment shader.
    Shader m_FragShader;
};
}