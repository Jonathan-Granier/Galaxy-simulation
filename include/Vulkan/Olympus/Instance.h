#pragma once
#include <vulkan/vulkan.h>
#include <string>

namespace olp
{
/// Instance use to setup the vulkan instance and validation layer.
class Instance
{
public:
    /// Constructor.
    Instance() = default;

    /// Destroy vulkan instance.
    void Destroy();
    /// Create vulkan instance.
    /// @param iAppName Name of the application.
    /// @param iEnabledExtensionNames Extension to activate.
    /// @param iEnabledExtensionCount Number of extension.
    void CreateInstance(
        std::string iAppName,
        const char *const *iEnabledExtensionNames,
        uint32_t iEnabledExtensionCount);

    /// Setup validation layer.
    void SetupDebugMessenger();

    /// Get vulkan instance.
    /// @return Vulkan instance.
    const VkInstance &GetVkInstance() const { return m_Instance; }

private:
    /// Vulkan instance
    VkInstance m_Instance = VK_NULL_HANDLE;
    /// Validation layer.
    VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
};
}