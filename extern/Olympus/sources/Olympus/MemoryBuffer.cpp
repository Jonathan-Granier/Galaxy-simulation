#include "Olympus/MemoryBuffer.h"
#include "Olympus/CommandBuffer.h"

namespace olp
{
//----------------------------------------------------------------------------------------------------------------------
void MemoryBuffer::Destroy()
{
    if (Buffer == VK_NULL_HANDLE)
        return;

    vkDestroyBuffer(Device->GetDevice(), Buffer, nullptr);
    vkFreeMemory(Device->GetDevice(), Memory, nullptr);

    Buffer = VK_NULL_HANDLE;
    Memory = VK_NULL_HANDLE;
    Size = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void MemoryBuffer::Map(VkDeviceSize iDataSize, VkDeviceSize iOffset)
{
    vkMapMemory(Device->GetDevice(), Memory, iOffset, iDataSize, 0, &Mapped);
}

//----------------------------------------------------------------------------------------------------------------------
void MemoryBuffer::UnMap()
{
    if (Mapped)
    {
        vkUnmapMemory(Device->GetDevice(), Memory);
        Mapped = nullptr;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void MemoryBuffer::Flush(VkDeviceSize iDataSize, VkDeviceSize iOffset)
{
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = Memory;
    mappedRange.offset = iOffset;
    mappedRange.size = iDataSize;
    vkFlushMappedMemoryRanges(Device->GetDevice(), 1, &mappedRange);
}

//----------------------------------------------------------------------------------------------------------------------
void MemoryBuffer::CopyFrom(VkBuffer iSrcBuffer, VkDeviceSize iSize) const
{
    CommandBuffer commandBuffer(*Device);

    commandBuffer.Begin();
    commandBuffer.CopyBuffer(iSrcBuffer, Buffer, iSize, 0, 0);
    commandBuffer.EndAndRun(Device->GetGraphicsQueue());
}
}