#pragma once
#include <vulkan/vulkan.h>
#include <string>

/// Instance use to setup the vulkan instance and validation layer.
class Instance
{
public:
    /// Constructor.
    Instance() = default;
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
    /// Destroy vulkan instance.
    void Destroy();

    /// Get vulkan instance.
    /// @return Vulkan instance.
    VkInstance &GetVkInstance() { return m_Instance; }

private:
    /// Vulkan instance
    VkInstance m_Instance = VK_NULL_HANDLE;
    /// Validation layer.
    VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
};