#include "Vulkan/AccelerationPass.h"
#include <glm/vec4.hpp>
//----------------------------------------------------------------------------------------------------------------------
void AccelerationPass::Destroy()
{

    m_AccelerationBuffer.Destroy();
    ComputePass::Destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void AccelerationPass::Create(
    VkDescriptorPool &iDescriptorPool,
    const VkCloud &iGalaxy,
    const olp::UniformBuffer &iOptions)
{
    VkDeviceSize nbPoint = iGalaxy.GetSize();
    CreatePipelineLayout();
    CreateBuffers(nbPoint);
    CreateDescriptor(iDescriptorPool, iGalaxy, iOptions);
    ComputePass::Create("acceleration", nbPoint);
}

//----------------------------------------------------------------------------------------------------------------------
void AccelerationPass::CreatePipelineLayout()
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
void AccelerationPass::CreateBuffers(VkDeviceSize iNbPoint)
{
    VkDeviceSize bufferSize = sizeof(glm::vec4) * iNbPoint;
    m_AccelerationBuffer = m_Device.CreateMemoryBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

//----------------------------------------------------------------------------------------------------------------------
void AccelerationPass::CreateDescriptor(
    VkDescriptorPool &iDescriptorPool, const VkCloud &iGalaxy, const olp::UniformBuffer &iOptions)
{
    m_DescriptorSet.AllocateDescriptorSets(m_PipelineLayout.GetDescriptorLayout(), iDescriptorPool);
    //Vertex Buffer of the galaxy
    VkDescriptorBufferInfo vertexBufferInfo{};
    vertexBufferInfo.buffer = iGalaxy.GetVertexBuffer().Buffer;
    vertexBufferInfo.offset = 0;
    vertexBufferInfo.range = iGalaxy.GetVertexBuffer().Size;

    // Acceleration buffer
    VkDescriptorBufferInfo accelerationBufferInfo{};
    accelerationBufferInfo.buffer = m_AccelerationBuffer.Buffer;
    accelerationBufferInfo.offset = 0;
    accelerationBufferInfo.range = m_AccelerationBuffer.Size;

    m_DescriptorSet.AddWriteDescriptor(0, vertexBufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
    m_DescriptorSet.AddWriteDescriptor(1, accelerationBufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
    m_DescriptorSet.AddWriteDescriptor(2, iOptions);
    m_DescriptorSet.UpdateDescriptorSets();
}
