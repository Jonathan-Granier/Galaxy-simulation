#include "Vulkan/Shader.h"
#include "Vulkan/Debug.h"
#include <fstream>

//----------------------------------------------------------------------------------------------------------------------
Shader::Shader(const Device &iDevice)
    : m_Device{&iDevice}
{
}

//----------------------------------------------------------------------------------------------------------------------
Shader::Shader(Shader &&ioShader) noexcept
    : m_Device{ioShader.m_Device}, m_ShaderModule{std::exchange(ioShader.m_ShaderModule, {VK_NULL_HANDLE})}
{
}

//----------------------------------------------------------------------------------------------------------------------
Shader &Shader::operator=(Shader &&ioShader) noexcept
{
    m_Device = ioShader.m_Device;
    std::swap(m_ShaderModule, ioShader.m_ShaderModule);

    return *this;
}

//----------------------------------------------------------------------------------------------------------------------
void Shader::Load(const std::filesystem::path &iPath)
{
    std::ifstream file(iPath, std::ios::in | std::ios::binary | std::ios::ate);

    if (!file)
    {
        std::cerr << "Failed to read shader " << iPath.c_str() << std::endl;
        return;
    }

    const std::size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> shaderData(fileSize);
    file.read(shaderData.data(), fileSize);

    file.close();

    VkShaderModuleCreateInfo shaderInfo;
    shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderInfo.codeSize = static_cast<size_t>(fileSize);
    shaderInfo.pCode = reinterpret_cast<const uint32_t *>(shaderData.data());
    shaderInfo.pNext = nullptr;
    shaderInfo.flags = 0;
    VK_CHECK_RESULT(
        vkCreateShaderModule(m_Device->GetDevice(), &shaderInfo, nullptr, &m_ShaderModule))
}

//----------------------------------------------------------------------------------------------------------------------
void Shader::Destroy()
{
    if (IsValid())
    {
        vkDestroyShaderModule(m_Device->GetDevice(), m_ShaderModule, nullptr);
        m_ShaderModule = VK_NULL_HANDLE;
    }
}