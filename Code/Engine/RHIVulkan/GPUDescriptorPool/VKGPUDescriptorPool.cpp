#include <RHIVulkan/GPUDescriptorPool/VKGPUDescriptorPool.h>
#include <RHIVulkan/Device/VKDevice.h>

VKGPUDescriptorPool::VKGPUDescriptorPool(VKDevice& device)
    : m_device(device)
{
}

VkDescriptorPool VKGPUDescriptorPool::CreateDescriptorPool(const std::map<VkDescriptorType, size_t>& count)
{
    std::vector<VkDescriptorPoolSize> pool_sizes;
    for (auto & x : count)
    {
        pool_sizes.emplace_back();
        VkDescriptorPoolSize& pool_size = pool_sizes.back();
        pool_size.type = x.first;
        pool_size.descriptorCount = (ezUInt32)x.second;
    }

    // TODO: fix me
    if (count.empty())
    {
        pool_sizes.emplace_back();
        VkDescriptorPoolSize& pool_size = pool_sizes.back();
        pool_size.type = VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLER;
        pool_size.descriptorCount = 1;
    }

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = (ezUInt32)pool_sizes.size();
    pool_info.pPoolSizes = pool_sizes.data();
    pool_info.maxSets = 1;
    pool_info.flags = VkDescriptorPoolCreateFlagBits::VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    VkDescriptorPool descriptorPool = {};
    vkCreateDescriptorPool(m_device.GetDevice(), &pool_info, nullptr, &descriptorPool);
    return descriptorPool;
}

DescriptorSetPool VKGPUDescriptorPool::AllocateDescriptorSet(const VkDescriptorSetLayout& set_layout, const std::map<VkDescriptorType, size_t>& count)
{
    DescriptorSetPool res = {};
    res.pool = CreateDescriptorPool(count);

    VkDescriptorSetAllocateInfo alloc_info = {};
    alloc_info.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = res.pool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &set_layout;

    VkDescriptorSet descriptorSet = {};
    vkAllocateDescriptorSets(m_device.GetDevice(), &alloc_info, &descriptorSet);
    res.set = std::move(descriptorSet);

    return res;
}
