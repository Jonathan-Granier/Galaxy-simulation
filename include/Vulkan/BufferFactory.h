#pragma once

#include "Vulkan/Device.h"
#include "Vulkan/MemoryBuffer.h"
#include <cstring>

// TODO Remove this class , move create in Device and other method in MemoryBuffer

/// @brief
///  Factory for create and manage buffers.
class BufferFactory
{
public:
    /// @brief
    ///  Constructor.
    /// @param[in] ioDevice Device to initialize the pipeline with.
    BufferFactory(Device &ioDevice);

    /// @brief
    ///  Destructor.
    ~BufferFactory() = default;

    /// @brief
    ///  Create and allocate a buffer.
    /// @param[in] iSize Size in bytes of the buffer to be created.
    /// @param[in] iUsage Specifying allowed usages of the buffer.
    /// @param[in] iProperties Memory type.
    MemoryBuffer CreateMemoryBuffer(VkDeviceSize iSize, VkBufferUsageFlags iUsage, VkMemoryPropertyFlags iProperties) const;

    /// @brief
    ///  Copy a buffer to another buffer.
    /// @param[in] iSrcBuffer Buffer to copy.
    /// @param[in] iDstBuffer Destination buffer.
    /// @param[in] iSize Size in bytes of the buffer.
    void CopyBuffer(VkBuffer iSrcBuffer, VkBuffer iDstBuffer, VkDeviceSize iSize) const;

    /// @brief
    ///  Release a buffer.
    /// @param[in] ioBuffer Buffer to release.
    void ReleaseBuffer(MemoryBuffer &ioBuffer) const;

    template <typename T>
    void TransferDataInBuffer(T *iData, uint64_t iDataSize, const MemoryBuffer &iBuffer) const
    {
        TransferDataInBuffer(iData, 0, iDataSize, iBuffer);
    }

    template <typename T>
    void TransferDataInBuffer(T *iData, uint64_t iOffset, uint64_t iDataSize, const MemoryBuffer &iBuffer) const
    {
        void *tmpData{};
        vkMapMemory(m_Device.GetDevice(), iBuffer.Memory, iOffset, iDataSize, 0, &tmpData);
        std::memcpy(tmpData, iData, static_cast<size_t>(iDataSize));
        vkUnmapMemory(m_Device.GetDevice(), iBuffer.Memory);
    }

    /// @brief
    ///  Transfer data in a buffer.
    /// @tparam T Type of data.
    /// @param[in] iData Vector to transfer.
    /// @param[in] iDataSize Size in bytes of data.
    /// @param[in] iBuffer Destination buffer.
    template <typename T>
    void TransferDataInBuffer(const std::vector<T> &iData, uint64_t iDataSize, const MemoryBuffer &iBuffer) const
    {
        void *tmpData{};
        vkMapMemory(m_Device.GetDevice(), iBuffer.Memory, 0, iDataSize, 0, &tmpData);
        std::memcpy(tmpData, iData.data(), static_cast<size_t>(iDataSize));
        vkUnmapMemory(m_Device.GetDevice(), iBuffer.Memory);
    }

    /// @brief
    ///  Transfer data in a buffer.
    /// @tparam T Type of data.
    /// @param[in] iData Data to transfer.
    /// @param[in] iDataSize Size in bytes of data.
    /// @param[in] iBuffer Destination buffer.
    template <typename T>
    void TransferDataInBuffer(const T &iData, uint64_t iDataSize, const MemoryBuffer &iBuffer) const
    {
        TransferDataInBuffer(iData, 0, iDataSize, iBuffer);
    }

    /// @brief
    ///  Transfer data in a buffer.
    /// @tparam T Type of data.
    /// @param[in] iData Data to transfer.
    /// @param[in] iOffset Offset of the data.
    /// @param[in] iDataSize Size in bytes of data.
    /// @param[in] iBuffer Destination buffer.
    template <typename T>
    void TransferDataInBuffer(const T &iData, uint64_t iOffset, uint64_t iDataSize, const MemoryBuffer &iBuffer) const
    {
        void *tmpData{};
        vkMapMemory(m_Device.GetDevice(), iBuffer.Memory, iOffset, iDataSize, 0, &tmpData);
        std::memcpy(tmpData, &iData, static_cast<size_t>(iDataSize));
        vkUnmapMemory(m_Device.GetDevice(), iBuffer.Memory);
    }

    // TODO DOC & Refactor
    void Map(MemoryBuffer &iBuffer, VkDeviceSize iOffset = 0, VkDeviceSize iDataSize = VK_WHOLE_SIZE)
    {
        vkMapMemory(m_Device.GetDevice(), iBuffer.Memory, iOffset, iDataSize, 0, &iBuffer.Mapped);
    }

    void UnMap(MemoryBuffer &iBuffer)
    {
        if (iBuffer.Mapped)
        {
            vkUnmapMemory(m_Device.GetDevice(), iBuffer.Memory);
            iBuffer.Mapped = nullptr;
        }
    }

    void Flush(MemoryBuffer &iBuffer, VkDeviceSize iOffset = 0, VkDeviceSize iDataSize = VK_WHOLE_SIZE)
    {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = iBuffer.Memory;
        mappedRange.offset = iOffset;
        mappedRange.size = iDataSize;
        vkFlushMappedMemoryRanges(m_Device.GetDevice(), 1, &mappedRange);
    }

protected:
    /// Vulkan device.
    Device &m_Device;
};