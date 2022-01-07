#include "Vulkan/IntegrationPass.h"
#include <glm/vec4.hpp>
#include <glm/geometric.hpp>
//----------------------------------------------------------------------------------------------------------------------
void IntegrationPass::Destroy()
{
    ComputePass::Destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void IntegrationPass::Create(
    VkDescriptorPool &iDescriptorPool,
    const VkCloud &iGalaxy,
    const olp::UniformBuffer &iOptions,
    const olp::MemoryBuffer &iAccelerationBuffer,
    float iInitialSpeed)
{
    VkDeviceSize nbPoint = iGalaxy.GetCloud().Points.size();
    CreatePipelineLayout();
    CreateDescriptor(iDescriptorPool, iGalaxy, iOptions, iAccelerationBuffer);
    ComputePass::Create("integration", nbPoint);
}

//----------------------------------------------------------------------------------------------------------------------
void IntegrationPass::CreatePipelineLayout()
{
    std::vector<VkDescriptorSetLayoutBinding> descriptorBinding(3);

    // Position storage buffer.
    descriptorBinding[0].binding = 0;
    descriptorBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorBinding[0].descriptorCount = 1;
    descriptorBinding[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    descriptorBinding[0].pImmutableSamplers = nullptr;

    // Acceleration storage buffer
    descriptorBinding[1].binding = 1;
    descriptorBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorBinding[1].descriptorCount = 1;
    descriptorBinding[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    descriptorBinding[1].pImmutableSamplers = nullptr;

    // Smoothing length
    descriptorBinding[2].binding = 2;
    descriptorBinding[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorBinding[2].descriptorCount = 1;
    descriptorBinding[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    descriptorBinding[2].pImmutableSamplers = nullptr;

    m_PipelineLayout.Create(descriptorBinding);
}

//----------------------------------------------------------------------------------------------------------------------
void IntegrationPass::CreateDescriptor(
    VkDescriptorPool &iDescriptorPool, const VkCloud &iGalaxy, const olp::UniformBuffer &iOptions, const olp::MemoryBuffer &iAccelerationBuffer)
{
    m_DescriptorSet.AllocateDescriptorSets(m_PipelineLayout.GetDescriptorLayout(), iDescriptorPool);
    //Vertex Buffer of the galaxy
    VkDescriptorBufferInfo vertexBufferInfo{};
    vertexBufferInfo.buffer = iGalaxy.GetVertexBuffer().Buffer;
    vertexBufferInfo.offset = 0;
    vertexBufferInfo.range = iGalaxy.GetVertexBuffer().Size;

    // Acceleration buffer
    VkDescriptorBufferInfo accelerationBufferInfo{};
    accelerationBufferInfo.buffer = iAccelerationBuffer.Buffer;
    accelerationBufferInfo.offset = 0;
    accelerationBufferInfo.range = iAccelerationBuffer.Size;

    m_DescriptorSet.AddWriteDescriptor(0, vertexBufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
    m_DescriptorSet.AddWriteDescriptor(1, accelerationBufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
    m_DescriptorSet.AddWriteDescriptor(2, iOptions);
    m_DescriptorSet.UpdateDescriptorSets();
}
