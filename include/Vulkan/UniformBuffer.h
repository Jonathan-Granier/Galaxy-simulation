#pragma once

#include "Vulkan/MemoryBuffer.h"
#include "Vulkan/Device.h"
#include <cstdint>

/// TODO Remove this class and use memoryBuffer

/// @brief
///  Uniform buffer. Small block of data send to the shaders.
class UniformBuffer
{
public:
    /// @brief
    /// Constructor.
    UniformBuffer() = default;

    /// @brief
    ///  Constructor.
    /// @param[in] iSize Size of the buffer.
    /// @param[in] iFactory Buffer factory used for allocate and destroy the buffer.
    UniformBuffer(uint64_t iSize, const Device &iDevice) { Init(iSize, iDevice); }

    /// @brief
    ///  Deleted copy constructor.
    UniformBuffer(const UniformBuffer &) = delete;

    /// @brief
    ///  Move constructor.
    /// @param ioBuffer Buffer to be moved.
    UniformBuffer(UniformBuffer &&ioBuffer) noexcept;
    ~UniformBuffer() { Release(); }

    /// @brief
    /// Deleted copy assignment operator.
    /// @return Reference to the current buffer.
    UniformBuffer &operator=(const UniformBuffer &) = delete;

    /// @brief
    ///  Move assignment operator.
    /// @param ioBuffer Buffer to be moved.
    /// @return Reference to the current buffer.
    UniformBuffer &operator=(UniformBuffer &&ioBuffer) noexcept;

    const MemoryBuffer &GetMemoryBuffer() const { return m_Buffer; }
    uint64_t GetSize() const { return m_Size; }

    /// @brief
    ///  Initialize the buffer.
    /// @param[in] iSize Size of the buffer.
    /// @param[in] iDevice Device used for allocate the buffer.
    void Init(uint64_t iSize, const Device &iDevice);

    /// @brief
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

    /// @brief
    ///  Release the buffer.
    void Release() { m_Buffer.Destroy(); }

private:
    /// Buffer.
    MemoryBuffer m_Buffer{};
    /// Buffer size.
    uint64_t m_Size{0};

    /// Vulkan Device.
    const Device *m_Device{};
};