#include "Vulkan/CommandBuffer.h"
#include "Vulkan/Debug.h"

//----------------------------------------------------------------------------------------------------------------------
CommandBuffer::CommandBuffer(const Device &iDevice)
    : m_Device{&iDevice}
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_Device->GetCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    VK_CHECK_RESULT(
        vkAllocateCommandBuffers(m_Device->GetDevice(), &allocInfo, &m_CommandBuffer))
}

//----------------------------------------------------------------------------------------------------------------------
CommandBuffer::CommandBuffer(CommandBuffer &&ioBuffer) noexcept
    : m_CommandBuffer{std::exchange(ioBuffer.m_CommandBuffer, {})},
      m_Device{ioBuffer.m_Device}
{
}

//----------------------------------------------------------------------------------------------------------------------
void CommandBuffer::Begin(VkCommandBufferUsageFlags iFlags) const
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = iFlags;
    beginInfo.pInheritanceInfo = nullptr;

    VK_CHECK_RESULT(vkBeginCommandBuffer(m_CommandBuffer, &beginInfo))
}

//----------------------------------------------------------------------------------------------------------------------
void CommandBuffer::CopyBuffer(
    VkBuffer iSrcBuffer, VkBuffer iDstBuffer, uint64_t iSize, uint64_t iSrcOffset, uint64_t iDstOffset) const
{
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = iDstOffset;
    copyRegion.dstOffset = iSrcOffset;
    copyRegion.size = iSize;

    vkCmdCopyBuffer(m_CommandBuffer, iSrcBuffer, iDstBuffer, 1, &copyRegion);
}

//----------------------------------------------------------------------------------------------------------------------
CommandBuffer &CommandBuffer::operator=(CommandBuffer &&ioBuffer) noexcept
{
    std::swap(m_CommandBuffer, ioBuffer.m_CommandBuffer);
    std::swap(m_Device, ioBuffer.m_Device);

    return *this;
}

//----------------------------------------------------------------------------------------------------------------------
void CommandBuffer::End() const
{
    VK_CHECK_RESULT(vkEndCommandBuffer(m_CommandBuffer))
}

//----------------------------------------------------------------------------------------------------------------------
void CommandBuffer::Free()
{
    if (m_CommandBuffer == VK_NULL_HANDLE)
        return;

    vkFreeCommandBuffers(m_Device->GetDevice(), m_Device->GetCommandPool(), 1, &m_CommandBuffer);
    m_CommandBuffer = VK_NULL_HANDLE;
}

//----------------------------------------------------------------------------------------------------------------------
void CommandBuffer::EndAndRun(VkQueue iQueue) const
{
    End();
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_CommandBuffer;

    vkQueueSubmit(iQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(iQueue); // TODO Replace With a fence.
}