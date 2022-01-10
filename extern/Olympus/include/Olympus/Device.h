#pragma once

#include "Olympus/Instance.h"
#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

namespace olp
{
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
    /// @param iInstance Vulkan instance to initialize the device with.
    /// @param iSurface Vulkan surface to initialise the device with.
    explicit Device(const Instance &iInstance, VkSurfaceKHR iSurface);

    /// @brief
    ///  Default destructor.
    ~Device() = default;

    /// @brief
    ///  Destroys the device.
    void Destroy();

    const VkInstance &GetVkInstance() const { return m_Instance.GetVkInstance(); }
    const VkPhysicalDevice &GetPhysicalDevice() const { return m_PhysicalDevice; }
    const VkDevice &GetDevice() const { return m_Device; }
    VkDevice &GetDevice() { return m_Device; }
    const VkSurfaceKHR &GetSurface() const { return m_Surface; }
    VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
    VkQueue GetPresentQueue() const { return m_PresentQueue; }
    VkQueue GetComputeQueue() const { return m_ComputeQueue; }
    const VkCommandPool &GetCommandPool() const { return m_CommandPool; }

    const QueueFamilyIndices &GetQueueIndices() const { return m_QueueIndices; }

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

    VkSampleCountFlagBits GetMaxUsableSampleCount() const { return VK_SAMPLE_COUNT_1_BIT; }

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

    /// Vulkan instance.
    const Instance &m_Instance;
    /// Physical device (chose by default).
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    /// Logical device.
    VkDevice m_Device = VK_NULL_HANDLE;
    /// Graphics queue use to draw.
    VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
    /// Present queue use to present image at the screen.
    VkQueue m_PresentQueue = VK_NULL_HANDLE;
    /// Compute queue use to a compute pass.
    VkQueue m_ComputeQueue = VK_NULL_HANDLE;
    /// Surface to render.
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    /// Queue indices.
    QueueFamilyIndices m_QueueIndices{};
    /// Command pool allocate by device.
    VkCommandPool m_CommandPool = VK_NULL_HANDLE;

    /// Multisample anti-aliasing.
    VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;

    /// Device extensions.
    const std::vector<const char *> m_DeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};
}