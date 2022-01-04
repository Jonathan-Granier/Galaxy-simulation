
#include "Vulkan/ComputePass.h"
#include "Vulkan/Debug.h"
#include "Vulkan/Shader.h"
#include <cmath>

//----------------------------------------------------------------------------------------------------------------------
ComputePass::ComputePass(Device &device)
    : m_Device(device), m_PipelineLayout(device)
{
    m_DescriptorSet.Init(device);
}

//----------------------------------------------------------------------------------------------------------------------
void ComputePass::Destroy()
{
    m_PipelineLayout.Destroy();
    m_AccelerationBuffer.Destroy();
    m_SpeedBuffer.Destroy();
    vkDestroyPipeline(m_Device.GetDevice(), m_Pipeline, nullptr);
    vkDestroySemaphore(m_Device.GetDevice(), m_Semaphore, nullptr);
    vkDestroyCommandPool(m_Device.GetDevice(), m_CommandPool, nullptr);
    vkDestroyFence(m_Device.GetDevice(), m_Fence, nullptr);
}
//----------------------------------------------------------------------------------------------------------------------
void ComputePass::Create(
    VkDescriptorPool &iDescriptorPool,
    const VkCloud &iGalaxy,
    const UniformBuffer &iOptions)
{
    VkDeviceSize nbPoint = iGalaxy.GetCloud().Points.size();
    CreatePipelineLayout();
    CreateBuffers(nbPoint);
    CreateDescriptor(iDescriptorPool, iGalaxy, iOptions);
    CreatePipeline();
    CreateCommandPoolAndBuffer();
    CreateSemaphore();
    BuildCommandBuffer(nbPoint);
}

//----------------------------------------------------------------------------------------------------------------------
void ComputePass::CreatePipelineLayout()
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
void ComputePass::CreateBuffers(VkDeviceSize iNbPoint)
{
    VkDeviceSize size = sizeof(glm::vec3) * iNbPoint;
    m_AccelerationBuffer = m_Device.CreateMemoryBuffer(
        iNbPoint,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    m_SpeedBuffer = m_Device.CreateMemoryBuffer(
        iNbPoint,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

//----------------------------------------------------------------------------------------------------------------------
void ComputePass::CreateDescriptor(
    VkDescriptorPool &iDescriptorPool, const VkCloud &iGalaxy, const UniformBuffer &iOptions)
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

//----------------------------------------------------------------------------------------------------------------------
void ComputePass::CreatePipeline()
{
    Shader shader(m_Device);
    std::filesystem::path shaderPath = OLYMPUS_ROOT;
    shaderPath += "shaders/displacement_comp.spv";
    shader.Load(shaderPath);

    VkPipelineShaderStageCreateInfo shaderStageInfo{};
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStageInfo.module = shader.GetShaderModule();
    shaderStageInfo.pName = "main";

    VkComputePipelineCreateInfo pipelineCreateInfo;
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.layout = m_PipelineLayout.GetLayout();
    pipelineCreateInfo.flags = 0;
    pipelineCreateInfo.stage = shaderStageInfo;
    pipelineCreateInfo.pNext = nullptr;
    VK_CHECK_RESULT(vkCreateComputePipelines(
        m_Device.GetDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_Pipeline))
}

//----------------------------------------------------------------------------------------------------------------------
void ComputePass::CreateCommandPoolAndBuffer()
{
    Device::QueueFamilyIndices queueFamilyIndices = m_Device.GetQueueIndices();
    VkCommandPoolCreateInfo cmdPoolInfo{};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.queueFamilyIndex = queueFamilyIndices.computeFamily.value();
    cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VK_CHECK_RESULT(
        vkCreateCommandPool(m_Device.GetDevice(), &cmdPoolInfo, nullptr, &m_CommandPool))

    // Create a command buffer for compute operations
    VkCommandBufferAllocateInfo cmdBufAllocateInfo{};
    cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufAllocateInfo.commandPool = m_CommandPool;
    cmdBufAllocateInfo.commandBufferCount = 1;
    cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    VK_CHECK_RESULT(vkAllocateCommandBuffers(
        m_Device.GetDevice(), &cmdBufAllocateInfo, &m_CommandBuffer))
}

//----------------------------------------------------------------------------------------------------------------------
void ComputePass::CreateSemaphore()
{
    // Semaphore for compute & graphics sync
    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VK_CHECK_RESULT(vkCreateSemaphore(
        m_Device.GetDevice(), &semaphoreCreateInfo, nullptr, &m_Semaphore))

    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    VK_CHECK_RESULT(vkCreateFence(m_Device.GetDevice(), &fenceCreateInfo, nullptr, &m_Fence))
}

//----------------------------------------------------------------------------------------------------------------------
void ComputePass::BuildCommandBuffer(VkDeviceSize iNbPoint)
{
    VkCommandBufferBeginInfo cmdBufInfo{};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    VK_CHECK_RESULT(vkBeginCommandBuffer(m_CommandBuffer, &cmdBufInfo))
    vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_Pipeline);
    // Bind descriptor here.
    vkCmdBindDescriptorSets(
        m_CommandBuffer,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        m_PipelineLayout.GetLayout(),
        0,
        1,
        &m_DescriptorSet.GetDescriptorSet(),
        0,
        nullptr);

    uint32_t x = static_cast<uint32_t>(std::ceil(iNbPoint / 256.0));

    vkCmdDispatch(m_CommandBuffer, x, 1, 1);

    vkEndCommandBuffer(m_CommandBuffer);
}

//----------------------------------------------------------------------------------------------------------------------
void ComputePass::Process(VkSemaphore iWaitSemaphore, VkSemaphore iSignalSemaphore)
{
    // Wait for rendering finished
    VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    // Submit compute commands
    VkSubmitInfo computeSubmitInfo{};
    computeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    computeSubmitInfo.commandBufferCount = 1;
    computeSubmitInfo.pCommandBuffers = &m_CommandBuffer;
    computeSubmitInfo.waitSemaphoreCount = 1;
    computeSubmitInfo.pWaitSemaphores = &iWaitSemaphore;
    computeSubmitInfo.pWaitDstStageMask = &waitStageMask;
    computeSubmitInfo.signalSemaphoreCount = 1;
    computeSubmitInfo.pSignalSemaphores = &iSignalSemaphore;
    vkResetFences(m_Device.GetDevice(), 1, &m_Fence);
    VK_CHECK_RESULT(vkQueueSubmit(m_Device.GetComputeQueue(), 1, &computeSubmitInfo, m_Fence))
}

void ComputePass::WaitFence()
{
    // Wait for fence to ensure that compute buffer writes have finished
    vkWaitForFences(m_Device.GetDevice(), 1, &m_Fence, VK_TRUE, UINT64_MAX);
}