#pragma once

#include "Olympus/Device.h"
#include <filesystem>

namespace olp
{
///  Class for create and manage a vulkan shader.
class Shader
{
public:
    ///  Constructor.
    /// @param iDevice Device to initialize the shader with.
    explicit Shader(const Device &iDevice);

    ///  Deleted copy constructor.
    Shader(const Shader &) = delete;

    ///  Move constructor.
    /// @param ioShader Shader to be moved.
    Shader(Shader &&ioShader) noexcept = default;

    ///  Destructor.
    ~Shader() { Destroy(); }

    ///  Deleted copy assignment operator.
    /// @return Reference to the current shader.
    Shader &operator=(const Shader &) = delete;

    ///  Move assignment operator.
    /// @param ioShader Shader to be moved.
    /// @return Reference to the current shader.
    Shader &operator=(Shader &&ioShader) noexcept = default;

    VkShaderModule GetShaderModule() { return m_ShaderModule; }
    bool IsValid() { return m_ShaderModule != VK_NULL_HANDLE; }

    ///  Load of shader from a SPIR-V file.
    /// @param[in] iPath Path of the shaders to be loaded.
    void Load(const std::filesystem::path &iPath);

    ///  Destroys the shader object.
    void Destroy();

private:
    /// Vulkan device.
    const Device &m_Device;
    /// Vulkan shader module.
    VkShaderModule m_ShaderModule = VK_NULL_HANDLE;
};
}