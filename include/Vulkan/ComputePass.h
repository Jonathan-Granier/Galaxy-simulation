#pragma once

#include "Vulkan/Olympus/PipelineLayout.h"
#include "Vulkan/Olympus/DescriptorSet.h"
#include "Vulkan/Olympus/Device.h"
#include "Vulkan/Olympus/MemoryBuffer.h"
#include "Geometry/VkCloud.h"

/// @brief
///  Compute pass for the compute star position.
class ComputePass
{
public:
    /// @brief
    ///  Constructor.
    /// @param iDevice Device to initialize the compute pass with.
    explicit ComputePass(const olp::Device &iDevice);

    /// @brief
    ///  Submits the command buffer to the compute queue.
    /// @param[in] iWaitSemaphore Semaphore to wait before execute the pass.
    /// @param[in] iSignalSemaphore Semaphore to signal when the execution is finished.
    void Process(VkSemaphore iWaitSemaphore, VkSemaphore iSignalSemaphore);

    /// @brief
    /// Wait the fence of the compute pass.
    void WaitFence();

    VkSemaphore GetSemaphore() { return m_Semaphore; }
    VkCommandBuffer GetCommandBuffer() { return m_CommandBuffer; }

protected:
    /// @brief
    ///  Destructor.
    virtual void Destroy();

    /// @brief
    ///  Creates the compute pass.
    /// @param iDescriptorPool Descriptor pool to allocate descriptor of the pass.
    /// @param iGalaxy Galaxy cloud.
    /// @param iOptions  Uniform buffer of control parameters.
    virtual void Create(
        std::filesystem::path iShaderName,
        VkDeviceSize iNbPoint);

    /// @brief
    ///  Create the pipeline layout.
    virtual void CreatePipelineLayout() = 0;

    /// @brief
    ///  Create the pipeline.
    void CreatePipeline(std::filesystem::path iShaderName);

    /// @brief
    ///  Create the command pool and the command buffer.
    void CreateCommandPoolAndBuffer();
    /// @brief
    /// Create the sempahore and the fence used for the sync.
    void CreateSemaphore();

    /// @brief
    ///  Build the command buffer.
    /// @param iWidth VertexIndexImage width.
    /// @param iHeight VertexIndexImage height.
    void BuildCommandBuffer(VkDeviceSize iNbPoint);

    /// Vulkan device.
    const olp::Device &m_Device;

    /// Command pool for the compute queue.
    VkCommandPool m_CommandPool;
    /// Command buffer storing the dispatch commands and barriers.
    VkCommandBuffer m_CommandBuffer;
    /// Execution dependency between compute & graphic submission.
    VkSemaphore m_Semaphore;
    /// Synchronisation GPU/CPU. Need to find a better solution.
    VkFence m_Fence;

    /// Layout of the compute pipeline.
    olp::PipelineLayout m_PipelineLayout;
    /// Descriptor of the compute pass.
    olp::DescriptorSet m_DescriptorSet;
    /// Compute pipeline.
    VkPipeline m_Pipeline;
};