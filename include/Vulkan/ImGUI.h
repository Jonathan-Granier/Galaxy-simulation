#pragma once

#include "Vulkan/Device.h"
#include "Vulkan/Texture.h"
#include "Vulkan/DescriptorSet.h"
#include "Vulkan/MemoryBuffer.h"
#include <array>
#include <glm/glm.hpp>
#include <imgui/imgui.h>

class ImGUI
{

public:
    // UI params are set via push constants
    struct PushConstBlock
    {
        glm::vec2 scale;
        glm::vec2 translate;
    } pushConstBlock;

    ImGUI(Device &ioDevice);

    ~ImGUI();

    // Initialize all Vulkan resources used by the ui
    void CreateRessources(VkRenderPass iRenderPass);
    void Destroy();

    // Update vertex and index buffer containing the imGui elements when required
    void Update();

    // Draw current imGui frame into a command buffer
    void Draw(VkCommandBuffer commandBuffer);

private:
    void CreateDescriptors();
    void CreatePipeline(VkRenderPass renderPass);
    // Vulkan resources for rendering the UI
    Device &m_Device;

    Texture m_FontTexture;

    MemoryBuffer m_VertexBuffer;
    MemoryBuffer m_IndexBuffer;

    int32_t m_VertexCount = 0;
    int32_t m_IndexCount = 0;
    VkPipelineLayout m_PipelineLayout;
    VkPipeline m_Pipeline;

    VkDescriptorPool m_DescriptorPool;
    VkDescriptorSetLayout m_DescriptorSetLayout;
    DescriptorSet m_DescriptorSet;

    float m_Scale = 1.0f;
};