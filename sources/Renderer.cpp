#include "Renderer.h"
#include "Vulkan/Olympus/Debug.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

//----------------------------------------------------------------------------------------------------------------------
Renderer::Renderer(const Instance &iInstance, VkSurfaceKHR iSurface, uint32_t iWidth, uint32_t iHeight)
    : m_Device(iInstance, iSurface),
      m_Swapchain(m_Device, iWidth, iHeight),
      m_PipelineLayout(m_Device),
      m_CloudPipeline(m_Device),
      m_DepthBuffer(m_Device),
      m_AccelerationPass(m_Device),
      m_IntegrationPass(m_Device),
      m_MainPassDescriptor(m_Device)
{
    CreateResources();
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateResources()
{
    std::cout << "Create ressources" << std::endl;

    m_ImGUI = std::make_unique<ImGUI>(m_Device);

    CreatePipelineLayout();
    CreateSwapchainResources();

    CreateUniformBuffers();

    CreateSyncObjects();
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::ReleaseResources()
{
    std::cout << "Release ressources" << std::endl;
    ReleaseSwapchainResources();

    ReleaseGalaxy();
    m_UniformBuffers.Model.Destroy();
    m_UniformBuffers.Acceleration.Destroy();
    m_UniformBuffers.Displacement.Destroy();

    m_PipelineLayout.Destroy();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        vkDestroySemaphore(m_Device.GetDevice(), m_RenderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(m_Device.GetDevice(), m_ImageAvailableSemaphores[i], nullptr);
        vkDestroyFence(m_Device.GetDevice(), m_InFlightFences[i], nullptr);
    }

    m_Device.Destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::InitializeGalaxy(uint32_t iNbStars, float iGalaxyDiameters, float iGalaxyThickness, float iInitialSpeed, float iBlackHoleMass)
{
    CreateDescriptorPool();
    CreateDescriptorSets();

    VkCloud &galaxy = m_Clouds.emplace_back(m_Device);
    galaxy.Init(iNbStars, iGalaxyDiameters, iGalaxyThickness, iInitialSpeed);

    m_AccelerationInfo.NbPoint = galaxy.GetCloud().Points.size();
    m_DisplacementInfo.NbPoint = m_AccelerationInfo.NbPoint;
    m_AccelerationInfo.BlackHoleMass = iBlackHoleMass;

    m_AccelerationPass.Create(m_DescriptorPool, galaxy, m_UniformBuffers.Acceleration);

    m_IntegrationPass.Create(
        m_DescriptorPool,
        galaxy,
        m_UniformBuffers.Displacement,
        m_AccelerationPass.GetAccelerationBuffer(), iInitialSpeed);
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::ReleaseGalaxy()
{
    vkDeviceWaitIdle(m_Device.GetDevice());

    m_IntegrationPass.Destroy();
    m_AccelerationPass.Destroy();

    vkDestroyDescriptorPool(m_Device.GetDevice(), m_DescriptorPool, nullptr);

    for (VkCloud &c : m_Clouds)
        c.Destroy();
    m_Clouds.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateSwapchainResources()
{
    std::cout << "Create swapchain ressources" << std::endl;

    CreateDepthBuffer();
    CreateRenderPass();

    m_Swapchain.CreateFrameBuffers(m_RenderPass, m_DepthBuffer.GetImageView());

    m_ImGUI->CreateResources(m_RenderPass);
    CreatePipeline();
    CreateCommandBuffers();
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::RecreateSwapchainResources(uint32_t iWidth, uint32_t iHeight)
{
    std::cout << "Recreate swapchain ressources" << std::endl;
    ReleaseSwapchainResources();

    m_Swapchain.Init(iWidth, iHeight);
    CreateSwapchainResources();
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::ReleaseSwapchainResources()
{
    std::cout << "Release swapchain ressources" << std::endl;
    vkDeviceWaitIdle(m_Device.GetDevice());

    m_CloudPipeline.Destroy();
    m_ImGUI->Destroy();

    for (CommandBuffer &commandBuffer : m_CommandBuffers)
        commandBuffer.Free();

    vkDestroyRenderPass(m_Device.GetDevice(), m_RenderPass, nullptr);
    m_DepthBuffer.Destroy();
    m_Swapchain.Destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateCommandBuffers()
{
    m_CommandBuffers.clear();
    m_CommandBuffers.reserve(m_Swapchain.GetImageCount());

    for (size_t i = 0; i < m_CommandBuffers.capacity(); ++i)
    {
        m_CommandBuffers.emplace_back(m_Device);
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateUniformBuffers()
{
    m_UniformBuffers.Model.Init(sizeof(ModelInfo), m_Device);
    m_UniformBuffers.Displacement.Init(sizeof(DisplacementInfo), m_Device);
    m_UniformBuffers.Acceleration.Init(sizeof(AccelerationInfo), m_Device);
}

void Renderer::UpdateUniformBuffers(const glm::mat4 &iView, const glm::mat4 &iProj)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    ModelInfo modelUbo{};
    modelUbo.Model = glm::mat4(1.0);
    modelUbo.View = iView;
    modelUbo.Proj = iProj;
    modelUbo.Proj[1][1] *= -1;

    m_UniformBuffers.Model.SendData(&modelUbo, sizeof(ModelInfo));

    m_UniformBuffers.Displacement.SendData(&m_DisplacementInfo, sizeof(DisplacementInfo));
    m_UniformBuffers.Acceleration.SendData(&m_AccelerationInfo, sizeof(AccelerationInfo));
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
    m_CloudPipeline.Create(
        m_PipelineLayout.GetLayout(),
        m_RenderPass,
        0,
        "galaxy",
        m_Swapchain.GetImageSize().width,
        m_Swapchain.GetImageSize().height,
        1);
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateDescriptorPool()
{
    VkDescriptorPoolSize uniformPoolSize{};
    uniformPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformPoolSize.descriptorCount = 3; // ModelInfo + AccelerationInfo + DisplacementInfo

    VkDescriptorPoolSize storageBufferPoolSize{};
    storageBufferPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    storageBufferPoolSize.descriptorCount = 4; // Position Buffer*2 + Acceleration buffer*2

    std::array<VkDescriptorPoolSize, 2> poolSizes{uniformPoolSize, storageBufferPoolSize};

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
VkFormat Renderer::FindDepthFormat()
{
    return m_Device.FindSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateDepthBuffer()
{
    VkFormat depthFormat = FindDepthFormat();

    m_DepthBuffer.Init(m_Swapchain.GetImageSize().width, m_Swapchain.GetImageSize().height, depthFormat);
    m_DepthBuffer.CreateImage(
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        1,
        m_Device.GetMaxUsableSampleCount());
    m_DepthBuffer.CreateImageView(VK_IMAGE_ASPECT_DEPTH_BIT);
    m_DepthBuffer.TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateRenderPass()
{
    std::array<VkAttachmentDescription, 2> attachments{};

    // Color
    attachments[0].format = m_Swapchain.GetColorFormat();
    attachments[0].samples = m_Device.GetMaxUsableSampleCount();
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    //Depth
    attachments[1].format = FindDepthFormat();
    attachments[1].samples = m_Device.GetMaxUsableSampleCount();
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorReference{0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
    VkAttachmentReference depthReference{1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

    std::array<VkSubpassDescription, 1> subpasses{};
    // Gradient subpass
    subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpasses[0].colorAttachmentCount = 1;
    subpasses[0].pColorAttachments = &colorReference;
    subpasses[0].pDepthStencilAttachment = &depthReference;

    std::array<VkSubpassDependency, 1> dependencies{};
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependencies[0].srcAccessMask = 0;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

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
    CommandBuffer &commandBuffer = m_CommandBuffers[iIndex];
    commandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    const VkExtent2D imageSize = m_Swapchain.GetImageSize();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_RenderPass;
    renderPassInfo.framebuffer = m_Swapchain.GetFramebuffer(static_cast<uint32_t>(iIndex));
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = {imageSize};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    m_ImGUI->Update();

    vkCmdBeginRenderPass(commandBuffer.GetBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(
        commandBuffer.GetBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_CloudPipeline.GetPipeline());

    vkCmdBindDescriptorSets(
        commandBuffer.GetBuffer(),
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_PipelineLayout.GetLayout(),
        0,
        1,
        &m_MainPassDescriptor.GetDescriptorSet(),
        0,
        nullptr);

    for (VkCloud &cloud : m_Clouds)
        cloud.Draw(commandBuffer.GetBuffer());

    m_ImGUI->Draw(commandBuffer.GetBuffer());

    vkCmdEndRenderPass(commandBuffer.GetBuffer());

    commandBuffer.End();
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::DrawNextFrame(const glm::mat4 &iView, const glm::mat4 &iProj)
{
    // TODO remove ?
    m_IntegrationPass.WaitFence();
    m_AccelerationPass.WaitFence();

    vkWaitForFences(m_Device.GetDevice(), 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = m_Swapchain.GetNextImage(m_ImageAvailableSemaphores[m_CurrentFrame], imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapchainResources(m_Swapchain.GetImageSize().width, m_Swapchain.GetImageSize().height);
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
    UpdateUniformBuffers(iView, iProj);

    std::array<VkPipelineStageFlags, 1> waitStages = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    std::array<VkSemaphore, 1> waitSemaphores = {m_ImageAvailableSemaphores[m_CurrentFrame]};
    std::array<VkSemaphore, 1> signalSemaphores = {m_AccelerationPass.GetSemaphore()};

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

    m_AccelerationPass.Process(m_AccelerationPass.GetSemaphore(), m_IntegrationPass.GetSemaphore());
    m_IntegrationPass.Process(m_IntegrationPass.GetSemaphore(), m_RenderFinishedSemaphores[m_CurrentFrame]);

    result = m_Swapchain.PresentNextImage(&m_RenderFinishedSemaphores[m_CurrentFrame], imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        RecreateSwapchainResources(m_Swapchain.GetImageSize().width, m_Swapchain.GetImageSize().height);
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }

    m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}