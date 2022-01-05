#pragma once
#include "Vulkan/VulkanObject/Device.h"

/// @brief
///  Pipeline layout used to create a pipeline.
class PipelineLayout
{
public:
    /// @brief
    ///  Constructor.
    /// @param ioDevice Device to initialize the pipeline layout with.
    explicit PipelineLayout(Device &device);

    /// @brief
    ///  Deleted copy constructor.
    PipelineLayout(const PipelineLayout &) = delete;

    /// @brief
    ///  Move constructor.
    /// @param ioPipeline Pipeline layout to be moved.
    PipelineLayout(PipelineLayout &&ioLayout) noexcept;

    /// @brief
    ///  Destructor.
    ~PipelineLayout() { Destroy(); }

    /// @brief
    /// Deleted copy assignment operator.
    /// @return Reference to the current pipeline layout.
    PipelineLayout &operator=(const PipelineLayout &) = delete;

    /// @brief
    ///  Move assignment operator.
    /// @param ioPipeline Pipeline layout to be moved.
    /// @return Reference to the current pipeline layout.
    PipelineLayout &operator=(PipelineLayout &&ioLayout) noexcept;

    VkPipelineLayout GetLayout() { return m_Layout; }
    VkDescriptorSetLayout GetDescriptorLayout() { return m_DescLayout; }

    /// @brief
    ///  Creates the pipeline layout.
    /// @param iDescLayoutBinding Descriptor layout binding attached to the pipeline layout.
    void Create(const std::vector<VkDescriptorSetLayoutBinding> &iDescLayoutBinding);
    /// @brief
    ///  Destroys the pipeline layout object.
    void Destroy();

private:
    /// Vulkan device.
    Device *m_Device;
    /// Pipeline layout.
    VkPipelineLayout m_Layout = VK_NULL_HANDLE;
    /// Descriptor layout corresponding to the pipeline layout.
    VkDescriptorSetLayout m_DescLayout = VK_NULL_HANDLE;
};