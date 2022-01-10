#pragma once
#include "Olympus/Device.h"

namespace olp
{
/// @brief
///  Pipeline layout used to create a pipeline.
class PipelineLayout
{
public:
    ///  Constructor.
    /// @param iDevice Device to initialize the pipeline layout with.
    explicit PipelineLayout(const Device &iDevice);

    ///  Deleted copy constructor.
    PipelineLayout(const PipelineLayout &) = delete;

    ///  Move constructor.
    /// @param ioPipeline Pipeline layout to be moved.
    PipelineLayout(PipelineLayout &&ioLayout) noexcept = default;

    ///  Destructor.
    ~PipelineLayout() { Destroy(); }

    /// Deleted copy assignment operator.
    /// @return Reference to the current pipeline layout.
    PipelineLayout &operator=(const PipelineLayout &) = delete;

    ///  Move assignment operator.
    /// @param ioPipeline Pipeline layout to be moved.
    /// @return Reference to the current pipeline layout.
    PipelineLayout &operator=(PipelineLayout &&ioLayout) noexcept = default;

    VkPipelineLayout GetLayout() { return m_Layout; }
    VkDescriptorSetLayout GetDescriptorLayout() { return m_DescLayout; }

    ///  Creates the pipeline layout.
    /// @param iDescLayoutBinding Descriptor layout binding attached to the pipeline layout.
    void Create(const std::vector<VkDescriptorSetLayoutBinding> &iDescLayoutBinding);

    ///  Destroys the pipeline layout object.
    void Destroy();

private:
    /// Vulkan device.
    const Device &m_Device;
    /// Pipeline layout.
    VkPipelineLayout m_Layout = VK_NULL_HANDLE;
    /// Descriptor layout corresponding to the pipeline layout.
    VkDescriptorSetLayout m_DescLayout = VK_NULL_HANDLE;
};
}