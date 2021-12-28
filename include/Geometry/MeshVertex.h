#pragma once

#include <glm/vec3.hpp>
#include <vulkan/vulkan.h>
#include <array>
/// @brief
///  A mesh vertex, that is 32 bytes (with padding).
struct MeshVertex
{
    /// Position.
    glm::vec3 Pos{};
    /// Normal.
    glm::vec3 Normal{};

    static VkVertexInputBindingDescription GetBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();
};