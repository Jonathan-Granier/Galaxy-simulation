#pragma once

#include "Olympus/Device.h"

namespace olp
{
///  Class for create and manage a command buffer.
class CommandBuffer
{
public:
    ///  Constructor.
    /// @param[in] iDevice Device to initialize the command buffer with.
    CommandBuffer(const Device &iDevice);

    ///  Deleted copy constructor.
    CommandBuffer(const CommandBuffer &) = delete;

    ///  Move constructor.
    /// @param[in,out] ioBuffer Buffer to be moved.
    CommandBuffer(CommandBuffer &&ioBuffer) noexcept = default;

    ///  Destructor.
    ~CommandBuffer() { Free(); }

    /// Deleted copy assignment operator.
    /// @return Reference to the current buffer.
    CommandBuffer &operator=(const CommandBuffer &) = delete;

    ///  Move assignment operator.
    /// @param[in,out] ioBuffer Buffer to be moved.
    /// @return Reference to the current buffer.
    CommandBuffer &operator=(CommandBuffer &&ioBuffer) noexcept = default;

    const VkCommandBuffer &GetBuffer() const { return m_CommandBuffer; }

    ///  Begin a record on the command buffer.
    /// @param[in] iFlags Specifying usage behavior for the command buffer.
    void Begin(VkCommandBufferUsageFlags iFlags = 0);

    ///  Copy a buffer to a another buffer.
    /// @param[in] iSrcBuffer Buffer to copy.
    /// @param[in] iDstBuffer Destination buffer.
    /// @param[in] iSize Size of the buffer.
    /// @param[in] iSrcOffset Offset of the source buffer.
    /// @param[in] iDstOffset Offset of the destination buffer.
    void CopyBuffer(VkBuffer iSrcBuffer, VkBuffer iDstBuffer, uint64_t iSize, uint64_t iSrcOffset = 0,
                    uint64_t iDstOffset = 0);

    ///  End the record on the command buffer.
    void End();

    ///  End and run the command buffer with the given queue.
    /// @param[in] iQueue Queue to submit the buffer.
    void EndAndRun(VkQueue iQueue);

    ///  Free the command buffer.
    void Free();

private:
    /// Command buffer.
    VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
    /// Vulkan device.
    const Device &m_Device;
};
}