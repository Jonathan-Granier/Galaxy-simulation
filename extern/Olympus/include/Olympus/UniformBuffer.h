#pragma once

#include "Olympus/MemoryBuffer.h"
#include "Olympus/Device.h"
#include <cstdint>

namespace olp
{
/// @brief
///  Uniform buffer. Small block of data send to the shaders.
class UniformBuffer
{
public:
    /// Constructor.
    UniformBuffer() = default;

    /// Destructor.
    ~UniformBuffer() { Destroy(); }

    ///  Constructor.
    /// @param[in] iDevice Device to initialize the buffer with.
    /// @param[in] iSize Size of the buffer.
    UniformBuffer(const Device &iDevice, uint64_t iSize) { Init(iSize, iDevice); }

    ///  Deleted copy constructor.
    UniformBuffer(const UniformBuffer &) = delete;

    ///  Move constructor.
    /// @param[in,out] ioBuffer Buffer to be moved.
    UniformBuffer(UniformBuffer &&ioBuffer) noexcept;

    /// Deleted copy assignment operator.
    /// @return Reference to the current buffer.
    UniformBuffer &operator=(const UniformBuffer &) = delete;

    ///  Move assignment operator.
    /// @param[in,out] ioBuffer Buffer to be moved.
    /// @return Reference to the current buffer.
    UniformBuffer &operator=(UniformBuffer &&ioBuffer) noexcept;

    const MemoryBuffer &GetMemoryBuffer() const { return m_Buffer; }
    uint64_t GetSize() const { return m_Size; }

    ///  Initialize the buffer.
    /// @param[in] iSize Size of the buffer.
    /// @param[in] iDevice Device used for allocate the buffer.
    void Init(uint64_t iSize, const Device &iDevice);

    ///  Send data to the buffer.
    /// @tparam T Type of the data.
    /// @param[in] iData Data to send to the buffer.
    /// @param[in] iOffset Offset of the data in the buffer.
    /// @param[in] iDataSize Size of the data.
    template <typename T>
    void SendData(const T &iData, uint64_t iDataSize = VK_WHOLE_SIZE, uint64_t iOffset = 0)
    {
        m_Buffer.TransferDataInBuffer(iData, iDataSize, iOffset);
    }

    ///  Release the buffer.
    void Destroy() { m_Buffer.Destroy(); }

private:
    /// Buffer.
    MemoryBuffer m_Buffer{};
    /// Buffer size.
    uint64_t m_Size{0};
    /// Vulkan Device.
    const Device *m_Device{};
};
}