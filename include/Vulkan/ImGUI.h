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

    ///  Destroys the all vulkan resources object.
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

    /// Texture (Image + Sampler) to store the ImGUI Font.
    Texture m_FontTexture;
    /// ImGUI vertex buffer.
    MemoryBuffer m_VertexBuffer;
    /// ImGUI Index buffer (triangle).
    MemoryBuffer m_IndexBuffer;

    /// Vertex buffer size..
    int32_t m_VertexCount = 0;
    /// Index buffer size
    int32_t m_IndexCount = 0;
    /// Pipeline layout to draw ImGUI mesh.
    VkPipelineLayout m_PipelineLayout;
    /// Pipeleine to draw ImGUI mesh.
    VkPipeline m_Pipeline;

    /// Descriptor pool.
    VkDescriptorPool m_DescriptorPool;
    /// Descriptor layout.
    VkDescriptorSetLayout m_DescriptorSetLayout;
    /// Descriptor set to use the font texture.
    DescriptorSet m_DescriptorSet;
};