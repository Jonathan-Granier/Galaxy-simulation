#pragma once

#include "Olympus/Device.h"
#include "Olympus/Texture.h"
#include "Olympus/DescriptorSet.h"
#include "Olympus/MemoryBuffer.h"
#include <array>
#include <imgui/imgui.h>

namespace olp
{
class ImGUI
{

public:
    // UI params are set via push constants
    struct PushConstBlock
    {
        ImVec2 scale;
        ImVec2 translate;
    } pushConstBlock;

    /// Constructor.
    /// @param iDevice Device to initialize ImGUI resources with.
    ImGUI(const Device &iDevice);

    ~ImGUI();

    // Initialize all Vulkan resources used by the ui
    void CreateResources(VkRenderPass iRenderPass);

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
    const Device &m_Device;

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
}