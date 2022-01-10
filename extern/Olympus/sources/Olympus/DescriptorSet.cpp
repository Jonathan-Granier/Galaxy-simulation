#include "Olympus/DescriptorSet.h"
#include "Olympus/Debug.h"
#include "Olympus/UniformBuffer.h"
namespace olp
{
//----------------------------------------------------------------------------------------------------------------------
DescriptorSet::DescriptorSet(const Device &iDevice) : m_Device(iDevice)
{
}

//----------------------------------------------------------------------------------------------------------------------
void DescriptorSet::AllocateDescriptorSets(VkDescriptorSetLayout iLayout, VkDescriptorPool iDescPool, uint64_t iDescSetCount)
{
    const std::vector<VkDescriptorSetLayout> layouts(iDescSetCount, iLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = iDescPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(iDescSetCount);
    allocInfo.pSetLayouts = layouts.data();

    m_DescriptorSets.resize(iDescSetCount);
    VK_CHECK_RESULT(vkAllocateDescriptorSets(
        m_Device.GetDevice(), &allocInfo, m_DescriptorSets.data()))
}

//----------------------------------------------------------------------------------------------------------------------
void DescriptorSet::AddWriteDescriptor(uint32_t iBinding, const UniformBuffer &iBuffer, uint64_t iDescSetIndex)
{
    VkDescriptorBufferInfo &bufferInfo = m_BufferInfos.emplace_back();
    bufferInfo.buffer = iBuffer.GetMemoryBuffer().Buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = iBuffer.GetSize();

    VkWriteDescriptorSet &writeDescriptorSet = m_WriteDescriptorSets.emplace_back();
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = m_DescriptorSets[iDescSetIndex];
    writeDescriptorSet.dstBinding = iBinding;
    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.pBufferInfo = &bufferInfo;
}

//TODO Refactor Buffer , add descriptor info
void DescriptorSet::AddWriteDescriptor(
    uint32_t iBinding, VkDescriptorBufferInfo iBufferInfo, VkDescriptorType iType, uint64_t iDescSetIndex)
{
    VkDescriptorBufferInfo &bufferInfo = m_BufferInfos.emplace_back(iBufferInfo);
    VkWriteDescriptorSet &writeDescriptorSet = m_WriteDescriptorSets.emplace_back(VkWriteDescriptorSet{});
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = m_DescriptorSets[iDescSetIndex];
    writeDescriptorSet.dstBinding = iBinding;
    writeDescriptorSet.descriptorType = iType;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.pBufferInfo = &bufferInfo;
}

//----------------------------------------------------------------------------------------------------------------------
void DescriptorSet::AddWriteDescriptor(
    uint32_t iBinding, VkDescriptorImageInfo iImage, VkDescriptorType iType, uint64_t iDescSetIndex)
{
    VkDescriptorImageInfo &imageInfo = m_ImageInfos.emplace_back(iImage);

    VkWriteDescriptorSet &writeDescriptorSet = m_WriteDescriptorSets.emplace_back(VkWriteDescriptorSet{});
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = m_DescriptorSets[iDescSetIndex];
    writeDescriptorSet.dstBinding = iBinding;
    writeDescriptorSet.descriptorType = iType;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.pImageInfo = &imageInfo;
}

//----------------------------------------------------------------------------------------------------------------------
void DescriptorSet::UpdateDescriptorSets()
{
    vkUpdateDescriptorSets(
        m_Device.GetDevice(), static_cast<uint32_t>(m_WriteDescriptorSets.size()), m_WriteDescriptorSets.data(), 0, nullptr);
    m_WriteDescriptorSets.clear();
    m_ImageInfos.clear();
    m_BufferInfos.clear();
}
}