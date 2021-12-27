#pragma once

#include <vulkan/vulkan.h>
#include <utility>

/// @brief
/// Vulkan buffer and his device memory.
struct MemoryBuffer
{
    /// @brief
    ///  Constructor.
    MemoryBuffer() = default;

    /// @brief
    ///  Deleted copy constructor.
    MemoryBuffer(const MemoryBuffer &) = delete;

    /// @brief
    ///  Move constructor.
    /// @param ioBuffer Buffer to be moved.
    MemoryBuffer(MemoryBuffer &&ioBuffer) noexcept
        : Memory{std::exchange(ioBuffer.Memory, {VK_NULL_HANDLE})}, Buffer{std::exchange(ioBuffer.Buffer, {VK_NULL_HANDLE})}
    {
    }

    /// @brief
    ///  Deleted copy assignment operator.
    /// @return Reference to the current buffer.
    MemoryBuffer &operator=(const MemoryBuffer &) = delete;

    /// @brief
    ///  Move assignment operator.
    /// @param ioBuffer Buffer to be moved.
    /// @return Reference to the current buffer.
    MemoryBuffer &operator=(MemoryBuffer &&ioBuffer) noexcept
    {
        std::swap(Memory, ioBuffer.Memory);
        std::swap(Buffer, ioBuffer.Buffer);

        return *this;
    }

    /// Handle to the device memory for this buffer
    VkDeviceMemory Memory = VK_NULL_HANDLE;
    /// The vulkan buffer.
    VkBuffer Buffer = VK_NULL_HANDLE;
};