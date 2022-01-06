#pragma once

#include <glm/glm.hpp>
#include "Vulkan/VulkanObject/Device.h"
#include "Vulkan/VulkanObject/Swapchain.h"
#include "Vulkan/IntegrationPass.h"
#include "Vulkan/AccelerationPass.h"
#include "Vulkan/VulkanObject/PipelineLayout.h"
#include "Vulkan/CloudPipeline.h"
#include "Vulkan/VulkanObject/Image.h"
#include "Vulkan/VulkanObject/DescriptorSet.h"
#include "Vulkan/VulkanObject/CommandBuffer.h"
#include "Vulkan/VulkanObject/UniformBuffer.h"
#include "Vulkan/VulkanObject/ImGUI.h"
#include "Geometry/VkCloud.h"

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

    /// Initialize Galaxy and ComputePass.
    /// @param iNbStars Number of stars in galaxy.
    /// @param iGalaxyDiameter Galaxy's diamater.
    /// @param iGalaxyThickness Galaxy's thickness.
    /// @param iInitialSpeed Stars' initial speed.
    /// @param iBlackHoleMass Mass of the black hole in the center of the galaxy.
    void InitializeGalaxy(uint32_t iNbStars, float iGalaxyDiameters, float iGalaxyThickness, float iInitialSpeed, float iBlackHoleMass);

    /// Release Galaxy and ComputePass.
    void ReleaseGalaxy();

    ///  Recreates swapchain resources.
    /// @param iWidth New swapchain width.
    /// @param iHeight New swapchain height.
    void RecreateSwapchainResources(uint32_t iWidth, uint32_t iHeight);

    ///  Releases swapchain resources.
    void ReleaseSwapchainResources();

    ///  Renders the next frame.
    /// @param iView View matrix of the scene.
    /// @param iProj Projection matrix of the scene.
    void DrawNextFrame(const glm::mat4 &iView, const glm::mat4 &iProj);

    void SetStep(float iStep) { m_DisplacementInfo.Step = iStep; };
    void SetInteractionRate(float iInteractionRate) { m_AccelerationInfo.InteractionRate = iInteractionRate; };
    void SetSmoothLenght(float iSmoothLenght) { m_AccelerationInfo.SmoothLenght = iSmoothLenght; };

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
    /// @param iView View matrix of the scene.
    /// @param iProj Projection matrix of the scene.
    void UpdateUniformBuffers(const glm::mat4 &iView, const glm::mat4 &iProj);

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
    /// Cloud pipeline.
    CloudPipeline m_CloudPipeline;

    /// Graphics render pass.
    VkRenderPass m_RenderPass = VK_NULL_HANDLE;

    // Compute Pass
    /// Pass to compute the stars acceleration
    AccelerationPass m_AccelerationPass;
    /// Pass to calculate the new position and speed of each stars.
    IntegrationPass m_IntegrationPass;

    /// Command pool for the graphics queue.
    VkCommandPool m_CommandPool = VK_NULL_HANDLE;
    /// Command buffer for the graphics queue. ( 1 by framebuffer of the swapchain).
    std::vector<CommandBuffer> m_CommandBuffers{};

    /// Depth buffer image.
    Image m_DepthBuffer;

    /// Mesh to draw.
    std::vector<VkCloud> m_Clouds;

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

    /// ImGUI
    std::unique_ptr<ImGUI> m_ImGUI;

    struct ModelInfo
    {
        glm::mat4 Model;
        glm::mat4 View;
        glm::mat4 Proj;
    };

    struct DisplacementInfo
    {
        float Step = 0;
        uint32_t NbPoint = 0;
    } m_DisplacementInfo;

    struct AccelerationInfo
    {
        float BlackHoleMass = 1000.0;
        float InteractionRate = 0;
        float SmoothLenght = 0;
        uint32_t NbPoint = 0;
    } m_AccelerationInfo;

    /// Uniform buffers.
    struct UniformBuffers
    {
        UniformBuffer Model;
        UniformBuffer Displacement;
        UniformBuffer Acceleration;
    } m_UniformBuffers;
};
