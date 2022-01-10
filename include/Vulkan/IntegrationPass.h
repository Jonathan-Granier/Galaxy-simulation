#pragma once
#include "Vulkan/ComputePass.h"

/// Integration compute pass for update position of each star.
class IntegrationPass : public ComputePass
{
public:
    using ComputePass::ComputePass;

    /// Destroy all vulkan element used by the compute pass.
    void Destroy() override;

    ///  Creates the compute pass.
    /// @param[in] iDescriptorPool      Descriptor pool to allocate descriptor of the pass.
    /// @param[in] iGalaxy              Galaxy cloud.
    /// @param[in] iOptions             Uniform buffer of control parameters.
    /// @param[in] iAccelerationBuffer  Buffer that store the acceleration of each star.
    void Create(
        VkDescriptorPool &iDescriptorPool,
        const VkCloud &iGalaxy,
        const olp::UniformBuffer &iOptions,
        const olp::MemoryBuffer &iAccelerationBuffer);

private:
    ///  Create the pipeline layout.
    void CreatePipelineLayout() override;

    ///  Create the descriptors.
    /// @param[in] iDescriptorPool  Descriptor pool to allocate descriptor of the pass.
    /// @param[in] iGalaxy          Galaxy cloud.
    /// @param[in] iOptions         Uniform buffer of control parameters.
    void CreateDescriptor(
        VkDescriptorPool &iDescriptorPool,
        const VkCloud &iGalaxy,
        const olp::UniformBuffer &iOptions,
        const olp::MemoryBuffer &iAccelerationBuffer);
};