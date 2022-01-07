
#include "Vulkan/ComputePass.h"
#include "Vulkan/VulkanObject/Debug.h"
#include "Vulkan/VulkanObject/Shader.h"
#include <cmath>

//----------------------------------------------------------------------------------------------------------------------
ComputePass::ComputePass(const Device &iDevice)
    : m_Device(iDevice),
      m_PipelineLayout(iDevice),
      m_DescriptorSet(iDevice)
{
}

//----------------------------------------------------------------------------------------------------------------------
void ComputePass::Destroy()
{
    m_PipelineLayout.Destroy();
    vkDestroyPipeline(m_Device.GetDevice(), m_Pipeline, nullptr);
    vkDestroySemaphore(m_Device.GetDevice(), m_Semaphore, nullptr);
    vkDestroyCommandPool(m_Device.GetDevice(), m_CommandPool, nullptr);
    vkDestroyFence(m_Device.GetDevice(), m_Fence, nullptr);
}
//----------------------------------------------------------------------------------------------------------------------
void ComputePass::Create(
    std::filesystem::path iShaderName,
    VkDeviceSize iNbPoint)
{
    CreatePipeline(iShaderName);
    CreateCommandPoolAndBuffer();
    CreateSemaphore();
    BuildCommandBuffer(iNbPoint);
}

//----------------------------------------------------------------------------------------------------------------------
void ComputePass::CreatePipeline(std::filesystem::path iShaderName)
{
    Shader shader(m_Device);
    std::filesystem::path shaderPath = OLYMPUS_ROOT;
    shaderPath += "shaders/build" / iShaderName;
    shaderPath += "_comp.spv";
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