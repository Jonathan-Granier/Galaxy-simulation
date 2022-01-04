#include "Vulkan/DisplacementPass.h"
#include <glm/vec4.hpp>
#include <glm/geometric.hpp>
//----------------------------------------------------------------------------------------------------------------------
void DisplacementPass::Destroy()
{

    m_SpeedBuffer.Destroy();
    ComputePass::Destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void DisplacementPass::Create(
    VkDescriptorPool &iDescriptorPool,
    const VkCloud &iGalaxy,
    const UniformBuffer &iOptions,
    const MemoryBuffer &iAccelerationBuffer)
{
    VkDeviceSize nbPoint = iGalaxy.GetCloud().Points.size();
    CreatePipelineLayout();
    CreateBuffers(iGalaxy);
    CreateDescriptor(iDescriptorPool, iGalaxy, iOptions, iAccelerationBuffer);
    ComputePass::Create("displacement", nbPoint);
}

//----------------------------------------------------------------------------------------------------------------------
void DisplacementPass::CreatePipelineLayout()
{
    std::vector<VkDescriptorSetLayoutBinding> descriptorBinding(4);

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

    // Speed buffer.
    descriptorBinding[2].binding = 2;
    descriptorBinding[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorBinding[2].descriptorCount = 1;
    descriptorBinding[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    descriptorBinding[2].pImmutableSamplers = nullptr;

    // Smoothing length
    descriptorBinding[3].binding = 3;
    descriptorBinding[3].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorBinding[3].descriptorCount = 1;
    descriptorBinding[3].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    descriptorBinding[3].pImmutableSamplers = nullptr;

    m_PipelineLayout.Create(descriptorBinding);
}

//----------------------------------------------------------------------------------------------------------------------
void DisplacementPass::CreateBuffers(const VkCloud &iGalaxy)
{

    VkDeviceSize bufferSize = sizeof(glm::vec4) * iGalaxy.GetCloud().Points.size();
    std::vector<glm::vec4> speeds;
    speeds.reserve(iGalaxy.GetCloud().Points.size());

    for (const glm::vec3 &position : iGalaxy.GetCloud().Points)
    {
        glm::vec3 speed = glm::normalize(glm::cross(position, glm::vec3(0.f, 1.f, 0.f))) * 20.0f;
        speeds.emplace_back(glm::vec4(speed, 0));
    }

    MemoryBuffer stagingBuffer = m_Device.CreateMemoryBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    stagingBuffer.TransferDataInBuffer(speeds, bufferSize);

    m_SpeedBuffer = m_Device.CreateMemoryBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    m_SpeedBuffer.CopyFrom(stagingBuffer.Buffer, bufferSize);
    stagingBuffer.Destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void DisplacementPass::CreateDescriptor(
    VkDescriptorPool &iDescriptorPool, const VkCloud &iGalaxy, const UniformBuffer &iOptions, const MemoryBuffer &iAccelerationBuffer)
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

    // Speed buffer
    VkDescriptorBufferInfo speedBufferInfo{};
    speedBufferInfo.buffer = m_SpeedBuffer.Buffer;
    speedBufferInfo.offset = 0;
    speedBufferInfo.range = m_SpeedBuffer.Size;

    m_DescriptorSet.AddWriteDescriptor(0, vertexBufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
    m_DescriptorSet.AddWriteDescriptor(1, accelerationBufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
    m_DescriptorSet.AddWriteDescriptor(2, speedBufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
    m_DescriptorSet.AddWriteDescriptor(3, iOptions);
    m_DescriptorSet.UpdateDescriptorSets();
}
