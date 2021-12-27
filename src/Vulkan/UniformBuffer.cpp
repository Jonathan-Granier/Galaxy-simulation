#include "Vulkan/UniformBuffer.h"

//----------------------------------------------------------------------------------------------------------------------
UniformBuffer::UniformBuffer(UniformBuffer &&ioBuffer) noexcept
    : m_Buffer{std::move(ioBuffer.m_Buffer)}, m_Factory{ioBuffer.m_Factory}
{
}

//----------------------------------------------------------------------------------------------------------------------
UniformBuffer &UniformBuffer::operator=(UniformBuffer &&ioBuffer) noexcept
{
    std::swap(m_Buffer, ioBuffer.m_Buffer);
    m_Factory = ioBuffer.m_Factory;

    return *this;
}

//----------------------------------------------------------------------------------------------------------------------
void UniformBuffer::Init(uint64_t iSize, const BufferFactory &iFactory)
{
    m_Factory = &iFactory;
    m_Size = iSize;
    m_Buffer = m_Factory->CreateMemoryBuffer(
        iSize,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}