#pragma once

#include "Olympus/PipelineLayout.h"
#include "Olympus/DescriptorSet.h"
#include "Olympus/Device.h"
#include "Olympus/MemoryBuffer.h"
#include "Geometry/VkCloud.h"
#include <filesystem>

/// @brief
///  Compute pass for the compute star position.
class ComputePass
{
public:
    /// Virtual desctructor.
    virtual ~ComputePass() = default;

    ///  Constructor.
    /// @param iDevice Device to initialize the compute pass with.
    explicit ComputePass(const olp::Device &iDevice);

    ///  Submits the command buffer to the compute queue.
    /// @param[in] iWaitSemaphore Semaphore to wait before execute the pass.
    /// @param[in] iSignalSemaphore Semaphore to signal when the execution is finished.
    void Process(VkSemaphore iWaitSemaphore, VkSemaphore iSignalSemaphore);

    /// Wait the fence of the compute pass.
    void WaitFence();

    VkSemaphore GetSemaphore() { return m_Semaphore; }
    VkCommandBuffer GetCommandBuffer() { return m_CommandBuffer; }

protected:
    ///  Destructor.
    virtual void Destroy();

    ///  Creates the compute pass.
    /// @param iDescriptorPool Descriptor pool to allocate descriptor of the pass.
    /// @param iGalaxy Galaxy cloud.
    /// @param iOptions  Uniform buffer of control parameters.
    void Create(
        std::filesystem::path iShaderName,
        VkDeviceSize iNbPoint);

    ///  Create the pipeline layout.
    virtual void CreatePipelineLayout() = 0;

    ///  Create the pipeline.
    void CreatePipeline(std::filesystem::path iShaderName);

    ///  Create the command pool and the command buffer.
    void CreateCommandPoolAndBuffer();
    /// Create the sempahore and the fence used for the sync.
    void CreateSemaphore();

    ///  Build the command buffer.
    /// @param[in] iWidth VertexIndexImage width.
    /// @param[in] iHeight VertexIndexImage height.
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