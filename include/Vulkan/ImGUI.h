#pragma once

#include "Vulkan/Device.h"
#include "Vulkan/Texture.h"
#include "Vulkan/BufferFactory.h"
#include <array>
#include <glm/glm.hpp>
#include <imgui/imgui.h>

struct UISettings
{
    bool displayModels = true;
    bool displayLogos = true;
    bool displayBackground = true;
    bool animateLight = false;
    float lightSpeed = 0.25f;
    std::array<float, 50> frameTimes{};
    float frameTimeMin = 9999.0f, frameTimeMax = 0.0f;
    float lightTimer = 0.0f;
};

static UISettings uiSettings;

class ImGUI
{
private:
    // Vulkan resources for rendering the UI
    Device &m_Device;
    VkCommandPool m_CommandPool;
    BufferFactory &m_BufferFactory;

    Texture m_FontTexture;

        MemoryBuffer m_VertexBuffer;
    MemoryBuffer m_IndexBuffer;

    int32_t vertexCount = 0;
    int32_t indexCount = 0;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;

    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout m_DescriptorSetLayout;
    VkDescriptorSet m_DescriptorSet;

public:
    // UI params are set via push constants
    struct PushConstBlock
    {
        glm::vec2 scale;
        glm::vec2 translate;
    } pushConstBlock;

    ImGUI(Device &ioDevice, VkCommandPool ioCommandPool, BufferFactory &ioBufferFactory);

    ~ImGUI();
    // Initialize styles, keys, etc.
    void init(float width, float height);
    // Initialize all Vulkan resources used by the ui
    void initResources();

    void CreatePipeline(VkRenderPass renderPass);

    // Starts a new imGui frame and sets up windows and ui elements
    void newFrame(bool updateFrameGraph);

    // Update vertex and index buffer containing the imGui elements when required
    void updateBuffers();

    // Draw current imGui frame into a command buffer
    void drawFrame(VkCommandBuffer commandBuffer);
};