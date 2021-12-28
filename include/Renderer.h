#pragma once

#include "Vulkan/Device.h"
#include "Vulkan/Swapchain.h"
#include "Vulkan/PipelineLayout.h"
#include "Vulkan/MeshPipeline.h"
#include "Vulkan/BufferFactory.h"
#include "Vulkan/DescriptorSet.h"
#include "Vulkan/CommandBuffer.h"
#include "Geometry/VkMesh.h"
class Renderer
{
public:
    ///  Constructs the renderer, creating the necessary objects & initializing resources.
    /// @param iInstance Vulkan instance to initialize the device with.
    /// @param iSurface Vulkan surface to initialize the device with.
    /// @param iWidth Swapchain width.
    /// @param iHeight Swapchain height.
    Renderer(Instance &iInstance, VkSurfaceKHR iSurface, uint32_t iWidth, uint32_t iHeight);
    ~Renderer() = default;

    ///  Create Vulkan resources.
    void CreateRessources();
    ///  Releases Vulkan resources.
    void ReleaseRessources();

    ///  Recreates swapchain resources.
    /// @param iWidth New swapchain width.
    /// @param iHeight New swapchain height.
    void RecreateSwapchainRessources(uint32_t iWidth, uint32_t iHeight);

    ///  Releases swapchain resources.
    void ReleaseSwapchainRessources();

private:
    ///  Creates swapchain resources (pipelines, framebuffers, descriptors, ...).
    void CreateSwapchainRessources();

    ///  Initializes the default scene's geometry.
    void InitGeometry();

    ///  Creates the pipeline layout.
    void CreatePipeline();

    ///  Creates the pipelines.
    void CreatePipelineLayout();

    ///  Creates the main render pass.
    void CreateRenderPass();

    ///  Creates the uniform buffers.
    void CreateUniformBuffers();

    ///  Creates the descriptor pool.
    void CreateDescriptorPool();

    ///  Creates the descriptor sets.
    void CreateDescriptorSets();

    ///  Creates the command pool.
    void CreateCommandPool();

    ///  Create the command buffers.
    void CreateCommandBuffers();

    ///  Creates the synchronization objets.
    void CreateSyncObjects();

    /// Vulkan device that contains instance, physical device, device and queue.
    Device m_Device;
    /// Swapchain.
    Swapchain m_Swapchain;

    /// Buffer factory used for create buffer.
    std::unique_ptr<BufferFactory> m_BufferFactory;

    /// Descriptor of the main render pass.
    DescriptorSet m_MainPassDescriptor;
    /// Descriptor pool.
    VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

    /// Pipeline layout of the main render pass.
    PipelineLayout m_PipelineLayout;
    /// Mesh pipeline.
    MeshPipeline m_MeshPipeline;

    /// Graphics render pass.
    VkRenderPass m_RenderPass = VK_NULL_HANDLE;

    /// Command pool for the graphics queue.
    VkCommandPool m_CommandPool = VK_NULL_HANDLE;
    /// Command buffer for the graphics queue. ( 1 by framebuffer of the swapchain).
    std::vector<CommandBuffer> m_CommandBuffers{};

    /// Mesh to draw.
    std::unique_ptr<VkMesh> m_Mesh;
};
