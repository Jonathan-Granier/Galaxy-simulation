#pragma once

#include <glm/glm.hpp>
#include "Vulkan/Device.h"
#include "Vulkan/Swapchain.h"
#include "Vulkan/PipelineLayout.h"
#include "Vulkan/MeshPipeline.h"
#include "Vulkan/Image.h"
#include "Vulkan/DescriptorSet.h"
#include "Vulkan/CommandBuffer.h"
#include "Vulkan/UniformBuffer.h"
#include "Vulkan/ImGUI.h"
#include "Geometry/VkMesh.h"

struct ModelInfo
{
    glm::mat4 Model;
    glm::mat4 View;
    glm::mat4 Proj;
};

struct UniformBuffers
{
    UniformBuffer Model;
};

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
    void CreateResources();
    ///  Releases Vulkan resources.
    void ReleaseResources();

    ///  Recreates swapchain resources.
    /// @param iWidth New swapchain width.
    /// @param iHeight New swapchain height.
    void RecreateSwapchainResources(uint32_t iWidth, uint32_t iHeight);

    ///  Releases swapchain resources.
    void ReleaseSwapchainResources();

    ///  Renders the next frame.
    void DrawNextFrame();

private:
    /// Init ImGUI vulkan ressources.
    void InitImGUI();
    ///  Creates swapchain resources (pipelines, framebuffers, descriptors, ...).
    void CreateSwapchainResources();

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

    ///  Creates the depth buffer.
    void CreateDepthBuffer();

    ///  Finds the appropriate depth format.
    /// @return Chosen depth format.
    VkFormat FindDepthFormat();

    ///  Builds the command buffer at the given index.
    /// @param iIndex Index of the command buffer to build.
    void BuildCommandBuffer(uint32_t iIndex);

    ///  Updates uniform buffers.
    void UpdateUniformBuffers();

    /// Vulkan device that contains instance, physical device, device and queue.
    Device m_Device;
    /// Swapchain.
    Swapchain m_Swapchain;

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

    /// Depth buffer image.
    Image m_DepthBuffer;

    /// Mesh to draw.
    std::unique_ptr<VkMesh> m_Mesh;

    /// Maximum number of frames to calculate in parallel.
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    /// Semaphore to know if the current image is available. Already presented by the swapchain.
    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> m_ImageAvailableSemaphores{};
    /// Semaphore to know if the rendering is finished for current image.
    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> m_RenderFinishedSemaphores{};
    /// Fence to synchronize GPU/CPU for update uniform buffer.
    std::array<VkFence, MAX_FRAMES_IN_FLIGHT> m_InFlightFences{};
    std::vector<VkFence> m_ImagesInFlight{};

    /// Current frame index in the swapchain
    size_t m_CurrentFrame = 0;

    /// Uniform buffers.
    UniformBuffers m_UniformBuffers;

    /// ImGUI
    std::unique_ptr<ImGUI> m_ImGUI;
};
