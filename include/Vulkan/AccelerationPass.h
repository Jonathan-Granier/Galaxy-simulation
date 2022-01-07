#pragma once

#include "Vulkan/ComputePass.h"

class AccelerationPass : public ComputePass
{
public:
    using ComputePass::ComputePass;

    void Destroy() override;

    /// @brief
    ///  Creates the compute pass.
    /// @param iDescriptorPool Descriptor pool to allocate descriptor of the pass.
    /// @param iGalaxy Galaxy cloud.
    /// @param iOptions  Uniform buffer of control parameters.
    void Create(
        VkDescriptorPool &iDescriptorPool,
        const VkCloud &iGalaxy,
        const olp::UniformBuffer &iOptions);

    const olp::MemoryBuffer &GetAccelerationBuffer() const { return m_AccelerationBuffer; }

private:
    /// @brief
    ///  Create the pipeline layout.
    void CreatePipelineLayout() override;

    /// Create speed and acceleration buffer.
    /// @param iNbPoint Number of point in Galaxy
    void CreateBuffers(VkDeviceSize iNbPoint);

    /// @brief
    ///  Create the descriptors.
    /// @param iDescriptorPool Descriptor pool to allocate descriptor of the pass.
    /// @param iGalaxy Galaxy cloud.
    /// @param iOptions Uniform buffer of control parameters.
    void CreateDescriptor(
        VkDescriptorPool &iDescriptorPool,
        const VkCloud &iGalaxy,
        const olp::UniformBuffer &iOptions);

    olp::MemoryBuffer m_AccelerationBuffer;
};