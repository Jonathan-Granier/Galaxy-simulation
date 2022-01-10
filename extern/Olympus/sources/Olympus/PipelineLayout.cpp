#include "Olympus/PipelineLayout.h"
#include "Olympus/Debug.h"

namespace olp
{
//----------------------------------------------------------------------------------------------------------------------
PipelineLayout::PipelineLayout(const Device &iDevice)
    : m_Device(iDevice)
{
}

//----------------------------------------------------------------------------------------------------------------------
void PipelineLayout::Destroy()
{
    if (m_DescLayout != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorSetLayout(m_Device.GetDevice(), m_DescLayout, nullptr);
        m_DescLayout = VK_NULL_HANDLE;
    }

    if (m_Layout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(m_Device.GetDevice(), m_Layout, nullptr);
        m_Layout = VK_NULL_HANDLE;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void PipelineLayout::Create(const std::vector<VkDescriptorSetLayoutBinding> &iDescLayoutBinding)
{
    if (iDescLayoutBinding.empty())
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        VK_CHECK_RESULT(vkCreatePipelineLayout(
            m_Device.GetDevice(), &pipelineLayoutInfo, nullptr, &m_Layout))
    }
    else
    {
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(iDescLayoutBinding.size());
        layoutInfo.pBindings = iDescLayoutBinding.data();

        VK_CHECK_RESULT(vkCreateDescriptorSetLayout(
            m_Device.GetDevice(), &layoutInfo, nullptr, &m_DescLayout))

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &m_DescLayout;
        VK_CHECK_RESULT(vkCreatePipelineLayout(
            m_Device.GetDevice(), &pipelineLayoutInfo, nullptr, &m_Layout))
    }
}
}