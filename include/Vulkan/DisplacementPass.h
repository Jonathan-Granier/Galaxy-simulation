#pragma once
#include "Vulkan/ComputePass.h"

class DisplacementPass : public ComputePass
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
        const UniformBuffer &iOptions,
        const MemoryBuffer &iAccelerationBuffer);

private:
    /// @brief
    ///  Create the pipeline layout.
    void CreatePipelineLayout() override;

    /// Create speed and acceleration buffer.
    /// @param iNbPoint Number of point in Galaxy
    void CreateBuffers(const VkCloud &iGalaxy);

    /// @brief
    ///  Create the descriptors.
    /// @param iDescriptorPool Descriptor pool to allocate descriptor of the pass.
    /// @param iGalaxy Galaxy cloud.
    /// @param iOptions Uniform buffer of control parameters.
    void CreateDescriptor(
        VkDescriptorPool &iDescriptorPool,
        const VkCloud &iGalaxy,
        const UniformBuffer &iOptions,
        const MemoryBuffer &iAccelerationBuffer);

    MemoryBuffer m_SpeedBuffer;
};