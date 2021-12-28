#include "Renderer.h"
#include "Vulkan/Debug.h"
#include <iostream>
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

    // TODO Destroy sync

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
    m_Mesh->InitTriangle();
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateUniformBuffers()
{
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreatePipelineLayout()
{
    std::vector<VkDescriptorSetLayoutBinding> descriptorBinding(0);

    // // Model UBO
    // descriptorBinding[0].binding = 0;
    // descriptorBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    // descriptorBinding[0].descriptorCount = 1;
    // descriptorBinding[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    // descriptorBinding[0].pImmutableSamplers = nullptr;

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
    // VkDescriptorPoolSize uniformPoolSize{};
    // uniformPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    // uniformPoolSize.descriptorCount = 6; // ModelInfo + CameraInfo + ScreenSize*2 + Lighting + PointSize

    // VkDescriptorPoolSize imagePoolSize{};
    // imagePoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    // imagePoolSize.descriptorCount = 1; // Vertex Index Image

    // VkDescriptorPoolSize storageBufferPoolSize{};
    // storageBufferPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    // storageBufferPoolSize.descriptorCount = 2; // Shuffled buffer + Reproject buffer

    // std::array<VkDescriptorPoolSize, 3> poolSizes{uniformPoolSize, imagePoolSize, storageBufferPoolSize};

    // VkDescriptorPoolCreateInfo poolInfo{};
    // poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    // poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    // poolInfo.pPoolSizes = poolSizes.data();
    // poolInfo.maxSets = 3;

    // VK_CHECK_RESULT(m_VkFuncs->vkCreateDescriptorPool(m_Device.GetDevice(), &poolInfo, nullptr, &m_DescriptorPool))
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateDescriptorSets()
{
    // m_MainPassDescriptor.AllocateDescriptorSets(m_PipelineLayout.GetDescriptorLayout(), m_DescriptorPool);
    // m_MainPassDescriptor.AddWriteDescriptor(0, m_UniformBuffers.Model);
    // m_MainPassDescriptor.AddWriteDescriptor(1, m_UniformBuffers.Camera);
    // m_MainPassDescriptor.AddWriteDescriptor(2, m_UniformBuffers.Lighting);
    // m_MainPassDescriptor.AddWriteDescriptor(3, m_UniformBuffers.PointSize);
    // m_MainPassDescriptor.UpdateDescriptorSets();

    // m_GradientPassDescriptor.AllocateDescriptorSets(m_GradientPipelineLayout.GetDescriptorLayout(), m_DescriptorPool);
    // m_GradientPassDescriptor.AddWriteDescriptor(0, m_UniformBuffers.ScreenSize);
    // m_GradientPassDescriptor.UpdateDescriptorSets();
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

void Renderer::CreateSyncObjects()
{
}