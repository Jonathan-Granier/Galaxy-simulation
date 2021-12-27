#pragma once
#include <vulkan/vulkan.h>
#include <string>

class Instance
{
public:
    Instance() = default;
    void CreateInstance(std::string appName, const char *const *ppEnabledExtensionNames, uint32_t enabledExtensionCount);
    void SetupDebugMessenger();
    void Destroy();

    VkInstance &GetVkInstance() { return m_Instance; }

private:
    VkInstance m_Instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
};