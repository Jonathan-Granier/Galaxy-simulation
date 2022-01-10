#pragma once

#include <utility>
#include <cstring>
#include <iostream>
#include <vector>
#include "Olympus/Device.h"

namespace olp
{
/// Vulkan buffer and his device memory.
struct MemoryBuffer
{
    /// Constructor.
    MemoryBuffer() = default;

    /// Deleted copy constructor.
    MemoryBuffer(const MemoryBuffer &) = delete;

    /// Move constructor.
    /// @param ioBuffer Buffer to be moved.
    MemoryBuffer(MemoryBuffer &&ioBuffer) noexcept = default;

    /// Deleted copy assignment operator.
    /// @return Reference to the current buffer.
    MemoryBuffer &operator=(const MemoryBuffer &) = delete;

    /// Move assignment operator.
    /// @param ioBuffer Buffer to be moved.
    /// @return Reference to the current buffer.
    MemoryBuffer &operator=(MemoryBuffer &&ioBuffer) noexcept = default;

    void Destroy();

    template <typename T>
    void TransferDataInBuffer(T *iData, uint64_t iDataSize = VK_WHOLE_SIZE, uint64_t iOffset = 0) const
    {
        void *tmpData{};
        vkMapMemory(Device->GetDevice(), Memory, iOffset, iDataSize, 0, &tmpData);
        std::memcpy(tmpData, iData, static_cast<size_t>(iDataSize));
        vkUnmapMemory(Device->GetDevice(), Memory);
    }

    /// Transfer data in a buffer.
    /// @tparam T Type of data.
    /// @param[in] iData Vector to transfer.
    /// @param[in] iDataSize Size in bytes of data.
    /// @param[in] iBuffer Destination buffer.
    template <typename T>
    void TransferDataInBuffer(const std::vector<T> &iData, uint64_t iDataSize = VK_WHOLE_SIZE) const
    {
        TransferDataInBuffer(iData.data(), iDataSize);
    }

    void Map(VkDeviceSize iDataSize = VK_WHOLE_SIZE, VkDeviceSize iOffset = 0);
    void UnMap();
    void Flush(VkDeviceSize iDataSize = VK_WHOLE_SIZE, VkDeviceSize iOffset = 0);
    void CopyFrom(VkBuffer iSrcBuffer, VkDeviceSize iSize) const;

    /// Vulkan device.
    const Device *Device = nullptr;
    /// Handle to the device memory for this buffer.
    VkDeviceMemory Memory = VK_NULL_HANDLE;
    /// The vulkan buffer.
    VkBuffer Buffer = VK_NULL_HANDLE;
    /// Mapped memory.
    void *Mapped = nullptr;

    /// Buffer Size;
    VkDeviceSize Size = 0;
};
}