#include "Olympus/Device.h"
#include "Olympus/Debug.h"
#include "Olympus/MemoryBuffer.h"
#include <set>
#include <stdexcept>
#include <vector>

namespace olp
{
//----------------------------------------------------------------------------------------------------------------------
Device::Device(const Instance &iInstance, VkSurfaceKHR iSurface)
    : m_Instance(iInstance), m_Surface(iSurface)
{
    PickPhysicalDevice();
    CreateLogicalDevice();
    CreateCommandPool();
}

//----------------------------------------------------------------------------------------------------------------------
void Device::Destroy()
{
    // Window have the ownership on the surface
    m_Surface = VK_NULL_HANDLE;
    if (m_CommandPool)
        vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
    if (m_Device)
        vkDestroyDevice(m_Device, nullptr);
}

//----------------------------------------------------------------------------------------------------------------------
void Device::PickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_Instance.GetVkInstance(), &deviceCount, nullptr);
    if (deviceCount == 0)
        throw std::runtime_error("Failed to find GPUs with Vulkan support !");
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_Instance.GetVkInstance(), &deviceCount, devices.data());

    for (const auto &device : devices)
    {
        if (IsDeviceSuitable(device))
        {
            m_PhysicalDevice = device;
            m_MsaaSamples = FindMaxUsableSampleCount();
            break;
        }
    }

    if (m_PhysicalDevice == VK_NULL_HANDLE)
        throw std::runtime_error("Failed to find a suitable GPU!");
}

//----------------------------------------------------------------------------------------------------------------------
bool Device::IsDeviceSuitable(VkPhysicalDevice iDevice)
{
    QueueFamilyIndices indices = FindQueueFamilies(iDevice);

    bool extensionsSupported = CheckDeviceExtensionSupport(iDevice);
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(iDevice, &supportedFeatures);

    //TODO Check swapchain support.

    return indices.IsComplete() && extensionsSupported && supportedFeatures.samplerAnisotropy && supportedFeatures.fillModeNonSolid && supportedFeatures.largePoints;
}

//----------------------------------------------------------------------------------------------------------------------
bool Device::CheckDeviceExtensionSupport(VkPhysicalDevice iDevice)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(iDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(iDevice, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

    for (const auto &extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

//----------------------------------------------------------------------------------------------------------------------
Device::QueueFamilyIndices Device::FindQueueFamilies(VkPhysicalDevice iDevice)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(iDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(iDevice, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphicsFamily = i;
        if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
            indices.computeFamily = i;

        VkBool32 presentSupport = false;

        vkGetPhysicalDeviceSurfaceSupportKHR(iDevice, i, m_Surface, &presentSupport);

        if (presentSupport)
        {
            indices.presentFamily = i;
        }
        // TODO Remove second test and Add Barrier in Command Buffer
        if (indices.IsComplete() && indices.IsSameComputeAndGraphics())
            break;
        i++;
    }

    return indices;
}
void Device::CreateLogicalDevice()
{
    m_QueueIndices = FindQueueFamilies(m_PhysicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {m_QueueIndices.graphicsFamily.value(),
                                              m_QueueIndices.presentFamily.value(),
                                              m_QueueIndices.computeFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.fillModeNonSolid = VK_TRUE;
    deviceFeatures.largePoints = VK_TRUE;
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.sampleRateShading = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

    VK_CHECK_RESULT(vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device))

    vkGetDeviceQueue(m_Device, m_QueueIndices.graphicsFamily.value(), 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_Device, m_QueueIndices.presentFamily.value(), 0, &m_PresentQueue);
    vkGetDeviceQueue(m_Device, m_QueueIndices.computeFamily.value(), 0, &m_ComputeQueue);
}

//----------------------------------------------------------------------------------------------------------------------
// This function is used to request a device Memory type that supports all the
// property flags we request (e.g. device local, host visibile) Upon success it
// will return the index of the Memory type that fits our requestes Memory
// properties This is necessary as implementations can offer an arbitrary number
// of Memory types with different Memory properties. You can check
// http://vulkan.gpuinfo.org/ for details on different Memory configurations
uint32_t Device::FindMemoryType(uint32_t iTypeFilter, VkMemoryPropertyFlags iProperties) const
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((iTypeFilter & 1) == 1)
        {
            if ((memProperties.memoryTypes[i].propertyFlags & iProperties) == iProperties)
            {
                return i;
            }
        }
        iTypeFilter >>= 1;
    }
    throw std::runtime_error("Could not find a suitable Memory type!");
}

//----------------------------------------------------------------------------------------------------------------------
VkFormat Device::FindSupportedFormat(
    const std::vector<VkFormat> &iCandidates, VkImageTiling iTiling, VkFormatFeatureFlags iFeatures)
{
    for (VkFormat format : iCandidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &props);
        if (iTiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & iFeatures) == iFeatures)
        {
            return format;
        }
        else if (iTiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & iFeatures) == iFeatures)
        {
            return format;
        }
    }
    throw std::runtime_error("Failed to find supported format!");
}

//----------------------------------------------------------------------------------------------------------------------
VkSampleCountFlagBits Device::FindMaxUsableSampleCount()
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &physicalDeviceProperties);

    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT)
        return VK_SAMPLE_COUNT_64_BIT;

    if (counts & VK_SAMPLE_COUNT_32_BIT)
        return VK_SAMPLE_COUNT_32_BIT;

    if (counts & VK_SAMPLE_COUNT_16_BIT)
        return VK_SAMPLE_COUNT_16_BIT;

    if (counts & VK_SAMPLE_COUNT_8_BIT)
        return VK_SAMPLE_COUNT_8_BIT;

    if (counts & VK_SAMPLE_COUNT_4_BIT)
        return VK_SAMPLE_COUNT_4_BIT;

    if (counts & VK_SAMPLE_COUNT_2_BIT)
        return VK_SAMPLE_COUNT_2_BIT;

    return VK_SAMPLE_COUNT_1_BIT;
}

//----------------------------------------------------------------------------------------------------------------------
void Device::CreateCommandPool()
{
    Device::QueueFamilyIndices queueFamilyIndices = GetQueueIndices();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VK_CHECK_RESULT(vkCreateCommandPool(m_Device, &poolInfo, nullptr, &m_CommandPool))
}

//----------------------------------------------------------------------------------------------------------------------
MemoryBuffer Device::CreateMemoryBuffer(
    VkDeviceSize iSize, VkBufferUsageFlags iUsage, VkMemoryPropertyFlags iProperties) const
{
    MemoryBuffer buffer;
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = iSize;
    bufferInfo.usage = iUsage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK_RESULT(
        vkCreateBuffer(m_Device, &bufferInfo, nullptr, &buffer.Buffer))

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_Device, buffer.Buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, iProperties);

    VK_CHECK_RESULT(
        vkAllocateMemory(m_Device, &allocInfo, nullptr, &buffer.Memory))
    VK_CHECK_RESULT(vkBindBufferMemory(m_Device, buffer.Buffer, buffer.Memory, 0));

    buffer.Device = this;
    buffer.Size = iSize;
    return buffer;
}
}