#include "Olympus/UniformBuffer.h"

namespace olp
{
//----------------------------------------------------------------------------------------------------------------------
UniformBuffer::UniformBuffer(UniformBuffer &&ioBuffer) noexcept
    : m_Buffer{std::move(ioBuffer.m_Buffer)},
      m_Device{ioBuffer.m_Device}
{
}

//----------------------------------------------------------------------------------------------------------------------
UniformBuffer &UniformBuffer::operator=(UniformBuffer &&ioBuffer) noexcept
{
    std::swap(m_Buffer, ioBuffer.m_Buffer);
    m_Device = ioBuffer.m_Device;

    return *this;
}

//----------------------------------------------------------------------------------------------------------------------
void UniformBuffer::Init(uint64_t iSize, const Device &iDevice)
{
    m_Device = &iDevice;
    m_Size = iSize;
    m_Buffer = m_Device->CreateMemoryBuffer(
        iSize,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}
}