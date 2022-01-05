#pragma once
#include "Vulkan/ComputePass.h"

class DisplacementPass : public ComputePass
{
public:
    using ComputePass::ComputePass;

    /// Destroy all vulkan element used by the compute pass.
    void Destroy() override;

    /// @brief
    ///  Creates the compute pass.
    /// @param iDescriptorPool      Descriptor pool to allocate descriptor of the pass.
    /// @param iGalaxy              Galaxy cloud.
    /// @param iOptions             Uniform buffer of control parameters.
    /// @param iAccelerationBuffer  Buffer that store the acceleration of each star.
    /// @param iInitialSpeed        Stars' initial speed.
    void Create(
        VkDescriptorPool &iDescriptorPool,
        const VkCloud &iGalaxy,
        const UniformBuffer &iOptions,
        const MemoryBuffer &iAccelerationBuffer,
        float iInitialSpeed);

private:
    /// @brief
    ///  Create the pipeline layout.
    void CreatePipelineLayout() override;

    /// Create speed and acceleration buffer.
    /// @param iNbPoint         Number of point in Galaxy.
    /// @param iInitialSpeed    Stars' initial speed.
    void CreateBuffers(const VkCloud &iGalaxy, float iInitialSpeed);

    /// @brief
    ///  Create the descriptors.
    /// @param iDescriptorPool  Descriptor pool to allocate descriptor of the pass.
    /// @param iGalaxy          Galaxy cloud.
    /// @param iOptions         Uniform buffer of control parameters.
    void CreateDescriptor(
        VkDescriptorPool &iDescriptorPool,
        const VkCloud &iGalaxy,
        const UniformBuffer &iOptions,
        const MemoryBuffer &iAccelerationBuffer);

    /// Buffer to store stars' speed.
    MemoryBuffer m_SpeedBuffer;
};