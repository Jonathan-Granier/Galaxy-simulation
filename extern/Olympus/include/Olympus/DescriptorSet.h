#pragma once

#include "Olympus/Device.h"
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
namespace olp
{
class UniformBuffer;

/// @brief
///  Class for create and manage a vulkan descriptor set.
class DescriptorSet
{
public:
    /// @brief
    ///  Constructor.
    /// @param iDevice Device to initialize the descriptor with.
    DescriptorSet(const Device &iDevice);

    /// @brief
    ///  Deleted copy constructor.
    DescriptorSet(const DescriptorSet &) = delete;

    /// @brief
    ///  Move constructor.
    DescriptorSet(DescriptorSet &&) noexcept = default;

    /// @brief
    ///  Destructor.
    ~DescriptorSet() = default;

    /// @brief
    ///  Deleted copy assignment operator.
    /// @return Reference to the current descriptor.
    DescriptorSet &operator=(const DescriptorSet &) = delete;

    /// @brief
    ///  Move assignment operator.
    /// @return Reference to the current descriptor.
    DescriptorSet &operator=(DescriptorSet &&) noexcept = default;

    const VkDescriptorSet &GetDescriptorSet(std::size_t iSetIndex = 0) const { return m_DescriptorSets[iSetIndex]; }

    /// @brief
    ///  Allocate a set of descriptor with the same layout.
    /// @param[in] iLayout Layout of the descriptor.
    /// @param[in] iDescPool Pool used for the allocation.
    /// @param[in] iDescSetCount Number of descriptor to allocate.
    void AllocateDescriptorSets(VkDescriptorSetLayout iLayout, VkDescriptorPool iDescPool, uint64_t iDescSetCount = 1);

    /// @brief
    ///  Add a uniform buffer to the descriptor
    /// @param[in] iBinding Descriptor binding within that set.
    /// @param[in] iBuffer Uniform buffer to add.
    /// @param[in] iDescSetIndex Index of destination descriptor.
    void AddWriteDescriptor(uint32_t iBinding, const UniformBuffer &iBuffer, uint64_t iDescSetIndex = 0);

    /// @brief
    ///  Add a buffer to the descriptor
    /// @param[in] iBinding Descriptor binding within that set.
    /// @param[in] iBuffer Buffer to add.
    /// @param[in] iType Type of descriptor to add.
    /// @param[in] iDescSetIndex Index of destination descriptor.
    void AddWriteDescriptor(
        uint32_t iBinding, VkDescriptorBufferInfo iBufferInfo, VkDescriptorType iType, uint64_t iDescSetIndex = 0);

    /// @brief
    ///  Add a image to the descriptor
    /// @param[in] iBinding Descriptor binding within that set.
    /// @param[in] iImageInfo Image to add.
    /// @param[in] iType Type of descriptor to add.
    /// @param[in] iDescSetIndex Index of destination descriptor.
    void AddWriteDescriptor(
        uint32_t iBinding, VkDescriptorImageInfo iImageInfo, VkDescriptorType iType, uint64_t iDescSetIndex = 0);

    /// @brief
    ///  Update descriptor with the previous WriteDescriptor add.
    void UpdateDescriptorSets();

private:
    /// A set of Descriptor set.
    std::vector<VkDescriptorSet> m_DescriptorSets{};
    /// A set of write descriptor with a Image of Buffer information.
    std::vector<VkWriteDescriptorSet> m_WriteDescriptorSets{};
    /// Storage image information used by update.
    std::vector<VkDescriptorImageInfo> m_ImageInfos{};
    /// Storage buffer information used by update.
    std::vector<VkDescriptorBufferInfo> m_BufferInfos{};
    /// Vulkan device.
    const Device m_Device;
};
}