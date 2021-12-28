#include "Renderer.h"
#include "Vulkan/Debug.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

//----------------------------------------------------------------------------------------------------------------------
Renderer::Renderer(Instance &iInstance, VkSurfaceKHR iSurface, uint32_t iWidth, uint32_t iHeight)
    : m_Device(iInstance, iSurface),
      m_Swapchain(m_Device, iWidth, iHeight),
      m_PipelineLayout(m_Device),
      m_MeshPipeline(m_Device)
{
    CreateRessources();
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateRessources()
{
    std::cout << "Create ressources" << std::endl;

    CreateCommandPool();

    m_BufferFactory = std::make_unique<BufferFactory>(m_Device, m_CommandPool);

    m_MainPassDescriptor.Init(m_Device);

    InitGeometry();
    CreateSwapchainRessources();
    CreateSyncObjects();
    CreateCommandBuffers();
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::ReleaseRessources()
{
    std::cout << "Release ressources" << std::endl;
    ReleaseSwapchainRessources();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        vkDestroySemaphore(m_Device.GetDevice(), m_RenderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(m_Device.GetDevice(), m_ImageAvailableSemaphores[i], nullptr);
        vkDestroyFence(m_Device.GetDevice(), m_InFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(m_Device.GetDevice(), m_CommandPool, nullptr);
    m_Mesh->Destroy();

    m_BufferFactory.reset();
    m_Device.Destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateSwapchainRessources()
{
    std::cout << "Create swapchain ressources" << std::endl;

    CreateRenderPass();

    m_Swapchain.CreateFrameBuffers(m_RenderPass);

    CreatePipelineLayout();
    CreatePipeline();
    CreateUniformBuffers();
    CreateDescriptorPool();
    CreateDescriptorSets();
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::RecreateSwapchainRessources(uint32_t iWidth, uint32_t iHeight)
{
    std::cout << "Recreate swapchain ressources" << std::endl;
    ReleaseSwapchainRessources();

    m_Swapchain.Init(iWidth, iHeight);
    CreateSwapchainRessources();
    CreateCommandBuffers();
}
//----------------------------------------------------------------------------------------------------------------------
void Renderer::ReleaseSwapchainRessources()
{
    std::cout << "Release swapchain ressources" << std::endl;
    vkDeviceWaitIdle(m_Device.GetDevice());

    for (CommandBuffer &commandBuffer : m_CommandBuffers)
        commandBuffer.Free();

    m_UniformBuffers.Model.Release();

    vkDestroyDescriptorPool(m_Device.GetDevice(), m_DescriptorPool, nullptr);

    m_MeshPipeline.Destroy();
    m_PipelineLayout.Destroy();
    vkDestroyRenderPass(m_Device.GetDevice(), m_RenderPass, nullptr);
    m_Swapchain.Destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateCommandPool()
{
    Device::QueueFamilyIndices queueFamilyIndices = m_Device.GetQueueIndices();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VK_CHECK_RESULT(vkCreateCommandPool(m_Device.GetDevice(), &poolInfo, nullptr, &m_CommandPool))
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateCommandBuffers()
{
    m_CommandBuffers.clear();
    m_CommandBuffers.reserve(m_Swapchain.GetImageCount());

    for (size_t i = 0; i < m_CommandBuffers.capacity(); ++i)
    {
        m_CommandBuffers.emplace_back(m_Device, m_CommandPool);
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::InitGeometry()
{
    m_Mesh = std::make_unique<VkMesh>(m_Device, *m_BufferFactory);
    m_Mesh->InitCube();
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateUniformBuffers()
{
    m_UniformBuffers.Model.Init(sizeof(ModelInfo), *m_BufferFactory);
}

void Renderer::UpdateUniformBuffers()
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    ModelInfo modelUbo{};
    modelUbo.Model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    modelUbo.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    modelUbo.Proj = glm::perspective(glm::radians(45.0f), static_cast<float>(m_Swapchain.GetImageSize().width) / static_cast<float>(m_Swapchain.GetImageSize().height), 0.1f, 10.0f);
    modelUbo.Proj[1][1] *= -1;

    m_BufferFactory->TransferDataInBuffer(modelUbo, sizeof(modelUbo), m_UniformBuffers.Model.GetMemoryBuffer());
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreatePipelineLayout()
{
    std::vector<VkDescriptorSetLayoutBinding> descriptorBinding(1);

    // Model UBO
    descriptorBinding[0].binding = 0;
    descriptorBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorBinding[0].descriptorCount = 1;
    descriptorBinding[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    descriptorBinding[0].pImmutableSamplers = nullptr;

    m_PipelineLayout.Create(descriptorBinding);
}

void Renderer::CreatePipeline()
{
    m_MeshPipeline.Create(
        m_PipelineLayout.GetLayout(),
        m_RenderPass,
        0,
        "mesh",
        m_Swapchain.GetImageSize().width,
        m_Swapchain.GetImageSize().height,
        1);
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateDescriptorPool()
{
    VkDescriptorPoolSize uniformPoolSize{};
    uniformPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformPoolSize.descriptorCount = 1; // ModelInfo

    std::array<VkDescriptorPoolSize, 1> poolSizes{uniformPoolSize};

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = 3;

    VK_CHECK_RESULT(vkCreateDescriptorPool(m_Device.GetDevice(), &poolInfo, nullptr, &m_DescriptorPool))
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateDescriptorSets()
{
    m_MainPassDescriptor.AllocateDescriptorSets(m_PipelineLayout.GetDescriptorLayout(), m_DescriptorPool);
    m_MainPassDescriptor.AddWriteDescriptor(0, m_UniformBuffers.Model);
    m_MainPassDescriptor.UpdateDescriptorSets();
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateRenderPass()
{
    std::array<VkAttachmentDescription, 1> attachments{};

    // Color
    attachments[0].format = m_Swapchain.GetColorFormat();
    attachments[0].samples = m_Device.GetMaxUsableSampleCount();
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorReference{0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
    //VkAttachmentReference depthReference{2, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

    std::array<VkSubpassDescription, 1> subpasses{};
    // Gradient subpass
    subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpasses[0].colorAttachmentCount = 1;
    subpasses[0].pColorAttachments = &colorReference;

    std::array<VkSubpassDependency, 1> dependencies{};
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = 0;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
    renderPassInfo.pSubpasses = subpasses.data();
    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    renderPassInfo.pDependencies = dependencies.data();

    VK_CHECK_RESULT(vkCreateRenderPass(m_Device.GetDevice(), &renderPassInfo, nullptr, &m_RenderPass))
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateSyncObjects()
{
    m_ImagesInFlight.resize(m_Swapchain.GetImageCount(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        VK_CHECK_RESULT(
            vkCreateSemaphore(m_Device.GetDevice(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]))
        VK_CHECK_RESULT(
            vkCreateSemaphore(m_Device.GetDevice(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]))
        VK_CHECK_RESULT(vkCreateFence(m_Device.GetDevice(), &fenceInfo, nullptr, &m_InFlightFences[i]))
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::BuildCommandBuffer(uint32_t iIndex)
{
    const CommandBuffer &commandBuffer = m_CommandBuffers[iIndex];
    commandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    const VkExtent2D imageSize = m_Swapchain.GetImageSize();

    std::array<VkClearValue, 1> clearValues{};
    clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_RenderPass;
    renderPassInfo.framebuffer = m_Swapchain.GetFramebuffer(static_cast<uint32_t>(iIndex));
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = {imageSize};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer.GetBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(
        commandBuffer.GetBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_MeshPipeline.GetPipeline());

    vkCmdBindDescriptorSets(
        commandBuffer.GetBuffer(),
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_PipelineLayout.GetLayout(),
        0,
        1,
        &m_MainPassDescriptor.GetDescriptorSet(),
        0,
        nullptr);

    m_Mesh->Draw(commandBuffer.GetBuffer());

    vkCmdEndRenderPass(commandBuffer.GetBuffer());

    commandBuffer.End();
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::DrawNextFrame()
{
    vkWaitForFences(m_Device.GetDevice(), 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = m_Swapchain.GetNextImage(m_ImageAvailableSemaphores[m_CurrentFrame], imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapchainRessources(m_Swapchain.GetImageSize().width, m_Swapchain.GetImageSize().height);
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // Check if a previous frame is using this image (i.e. there is its fence to wait on)
    if (m_ImagesInFlight[imageIndex] != VK_NULL_HANDLE)
    {
        vkWaitForFences(m_Device.GetDevice(), 1, &m_ImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }

    // Mark the image as now being in use by this frame
    m_ImagesInFlight[imageIndex] = m_InFlightFences[m_CurrentFrame];

    BuildCommandBuffer(imageIndex);
    UpdateUniformBuffers();

    std::array<VkPipelineStageFlags, 1> waitStages = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    std::array<VkSemaphore, 1> waitSemaphores = {m_ImageAvailableSemaphores[m_CurrentFrame]};
    std::array<VkSemaphore, 1> signalSemaphores = {m_RenderFinishedSemaphores[m_CurrentFrame]};

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStages.data();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_CommandBuffers[imageIndex].GetBuffer();
    submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
    submitInfo.pSignalSemaphores = signalSemaphores.data();

    vkResetFences(m_Device.GetDevice(), 1, &m_InFlightFences[m_CurrentFrame]);

    VK_CHECK_RESULT(
        vkQueueSubmit(m_Device.GetGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_CurrentFrame]))

    result = m_Swapchain.PresentNextImage(&m_RenderFinishedSemaphores[m_CurrentFrame], imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        RecreateSwapchainRessources(m_Swapchain.GetImageSize().width, m_Swapchain.GetImageSize().height);
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }

    m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}