#include "Vulkan/BufferFactory.h"
#include "Vulkan/CommandBuffer.h"
#include "Vulkan/Debug.h"

//----------------------------------------------------------------------------------------------------------------------
BufferFactory::BufferFactory(Device &ioDevice, VkCommandPool iCommandPool)
    : m_Device(ioDevice), m_CommandPool(iCommandPool)
{
}

//----------------------------------------------------------------------------------------------------------------------
MemoryBuffer BufferFactory::CreateMemoryBuffer(
    VkDeviceSize iSize, VkBufferUsageFlags iUsage, VkMemoryPropertyFlags iProperties) const
{
    MemoryBuffer buffer;
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = iSize;
    bufferInfo.usage = iUsage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK_RESULT(
        vkCreateBuffer(m_Device.GetDevice(), &bufferInfo, nullptr, &buffer.Buffer))

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_Device.GetDevice(), buffer.Buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = m_Device.FindMemoryType(memRequirements.memoryTypeBits, iProperties);

    VK_CHECK_RESULT(
        vkAllocateMemory(m_Device.GetDevice(), &allocInfo, nullptr, &buffer.Memory))
    VK_CHECK_RESULT(vkBindBufferMemory(m_Device.GetDevice(), buffer.Buffer, buffer.Memory, 0));

    return buffer;
}

//----------------------------------------------------------------------------------------------------------------------
void BufferFactory::CopyBuffer(VkBuffer iSrcBuffer, VkBuffer iDstBuffer, VkDeviceSize iSize) const
{
    const CommandBuffer commandBuffer(m_Device, m_CommandPool);

    commandBuffer.Begin();
    commandBuffer.CopyBuffer(iSrcBuffer, iDstBuffer, iSize, 0, 0);
    commandBuffer.EndAndRun(m_Device.GetGraphicsQueue());
}

//----------------------------------------------------------------------------------------------------------------------
void BufferFactory::ReleaseBuffer(MemoryBuffer &ioBuffer) const
{
    if (ioBuffer.Buffer == VK_NULL_HANDLE)
        return;

    vkDestroyBuffer(m_Device.GetDevice(), ioBuffer.Buffer, nullptr);
    vkFreeMemory(m_Device.GetDevice(), ioBuffer.Memory, nullptr);

    ioBuffer.Buffer = VK_NULL_HANDLE;
    ioBuffer.Memory = VK_NULL_HANDLE;
}