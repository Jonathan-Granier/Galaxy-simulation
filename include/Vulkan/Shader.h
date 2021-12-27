#pragma once

#include "Vulkan/Device.h"
#include <filesystem>

/// @brief
///  Class for create and manage a vulkan shader.
class Shader
{
public:
    /// @brief
    ///  Constructor.
    /// @param ioDevice Device to initialize the shader with.
    explicit Shader(Device &ioDevice);

    /// @brief
    ///  Deleted copy constructor.
    Shader(const Shader &) = delete;

    /// @brief
    ///  Move constructor.
    /// @param ioShader Shader to be moved.
    Shader(Shader &&ioShader) noexcept;

    /// @brief
    ///  Destructor.
    ~Shader() { Destroy(); }

    /// @brief
    ///  Deleted copy assignment operator.
    /// @return Reference to the current shader.
    Shader &operator=(const Shader &) = delete;

    /// @brief
    ///  Move assignment operator.
    /// @param ioShader Shader to be moved.
    /// @return Reference to the current shader.
    Shader &operator=(Shader &&ioShader) noexcept;

    VkShaderModule GetShaderModule() { return m_ShaderModule; }
    bool IsValid() { return m_ShaderModule != VK_NULL_HANDLE; }

    /// @brief
    ///  Load of shader from a SPIR-V file.
    /// @param[in] iPath Path of the shaders to be loaded.
    void Load(const std::filesystem::path &iPath);

    /// @brief
    ///  Destroys the shader object.
    void Destroy();

private:
    Device *m_Device;
    VkShaderModule m_ShaderModule = VK_NULL_HANDLE;
};