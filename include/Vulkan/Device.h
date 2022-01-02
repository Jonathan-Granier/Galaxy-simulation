#pragma once

#include "Vulkan/Instance.h"
#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

class MemoryBuffer;

/// @brief
///  Class holding the Vulkan main objects.
class Device
{
public:
    /// @brief
    ///  Class holding the Vulkan queues' indices.
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        std::optional<uint32_t> computeFamily;

        /// @brief
        ///  Checks if the graphics, present & compute queues are all available.
        /// @return True if they are all available, false otherwise.
        bool IsComplete() const
        {
            return graphicsFamily.has_value() && presentFamily.has_value() && computeFamily.has_value();
        }

        /// @brief
        ///  Checks if the compute & graphics queues are the same one.
        /// @return True if they are the same, false otherwise.
        bool IsSameComputeAndGraphics() const { return graphicsFamily.value() == computeFamily.value(); }
    };

    /// @brief
    ///  Constructs the device, recovering Vulkan functions.
    /// @param ioInstance Vulkan instance to initialize the device with.
    /// @param iSurface Vulkan surface to initialise the device with.
    explicit Device(Instance &ioInstance, VkSurfaceKHR iSurface);

    /// @brief
    ///  Default destructor.
    ~Device() = default;

    /// @brief
    ///  Destroys the device.
    void Destroy();

    VkInstance &GetVkInstance() { return m_Instance.GetVkInstance(); }
    VkPhysicalDevice &GetPhysicalDevice() { return m_PhysicalDevice; }
    const VkDevice &GetDevice() const { return m_Device; }
    VkDevice &GetDevice() { return m_Device; }
    VkSurfaceKHR &GetSurface() { return m_Surface; }
    VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
    VkQueue GetPresentQueue() const { return m_PresentQueue; }
    VkQueue GetComputeQueue() const { return m_ComputeQueue; }
    const VkCommandPool &GetCommandPool() const { return m_CommandPool; }

    QueueFamilyIndices &GetQueueIndices() { return m_QueueIndices; }

    /// @brief
    ///  Finds a memory type that supports the given properties.
    /// @param iTypeFilter Type filter.
    /// @param iProperties Memory properties.
    /// @return Found supported memory type.
    uint32_t FindMemoryType(uint32_t iTypeFilter, VkMemoryPropertyFlags iProperties) const;

    /// @brief
    ///  Finds a format that supports the given tiling & features.
    /// @param iCandidates Candidate formats.
    /// @param iTiling Image tiling.
    /// @param iFeatures Feature formats.
    /// @return Found supported format.
    VkFormat FindSupportedFormat(
        const std::vector<VkFormat> &iCandidates, VkImageTiling iTiling, VkFormatFeatureFlags iFeatures);

    VkSampleCountFlagBits GetMaxUsableSampleCount() { return VK_SAMPLE_COUNT_1_BIT; }

    /// @brief
    ///  Create and allocate a buffer.
    /// @param[in] iSize Size in bytes of the buffer to be created.
    /// @param[in] iUsage Specifying allowed usages of the buffer.
    /// @param[in] iProperties Memory type.
    MemoryBuffer CreateMemoryBuffer(VkDeviceSize iSize, VkBufferUsageFlags iUsage, VkMemoryPropertyFlags iProperties) const;

private:
    /// @brief
    ///  Picks a suitable physical device.
    void PickPhysicalDevice();

    /// @brief
    ///  Creates the logical device & recovers the necessary queues.
    void CreateLogicalDevice();

    /// @brief
    ///  Checks if the device is suitable to be picked.
    /// @param iDevice Device to be checked.
    /// @return True if the given device is suitable, false otherwise.
    bool IsDeviceSuitable(VkPhysicalDevice iDevice);

    /// @brief
    ///  Checks that the required extensions are supported by the given device.
    /// @param iDevice Device to be checked.
    /// @return True if the required extensions are supported, false otherwise.
    bool CheckDeviceExtensionSupport(VkPhysicalDevice iDevice);

    /// @brief
    ///  Finds the maximum sample count that the current physical device can handle.
    /// @return Maximum usable sample count.
    VkSampleCountFlagBits FindMaxUsableSampleCount();

    /// @brief
    ///  Finds the necessary queue families.
    /// @param iDevice Device to find the queues from.
    /// @return Indices of the queue families.
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice iDevice);

    void CreateCommandPool();

    Instance &m_Instance;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_Device = VK_NULL_HANDLE;
    VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
    VkQueue m_PresentQueue = VK_NULL_HANDLE;
    VkQueue m_ComputeQueue = VK_NULL_HANDLE;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    QueueFamilyIndices m_QueueIndices{};
    VkCommandPool m_CommandPool = VK_NULL_HANDLE;

    VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;

    const std::vector<const char *> m_DeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};