#pragma once
#include <RHI/Resource/ResourceBase.h>
#include <RHIVulkan/RHIVulkanDLL.h>
#include <map>

static bool operator<(const VkImageSubresourceRange& lhs, const VkImageSubresourceRange& rhs)
{
    return std::tie(lhs.aspectMask, lhs.baseArrayLayer, lhs.baseMipLevel, lhs.layerCount, lhs.levelCount) <
        std::tie(rhs.aspectMask, rhs.baseArrayLayer, rhs.baseMipLevel, rhs.layerCount, rhs.levelCount);
};

class VKDevice;

class VKResource : public ResourceBase
{
public:
    VKResource(VKDevice& device);

    void CommitMemory(MemoryType memory_type) override;
    void BindMemory(const std::shared_ptr<Memory>& memory, uint64_t offset) override;
    uint64_t GetWidth() const override;
    uint32_t GetHeight() const override;
    uint16_t GetLayerCount() const override;
    uint16_t GetLevelCount() const override;
    uint32_t GetSampleCount() const override;
    uint64_t GetAccelerationStructureHandle() const override;
    void SetName(const std::string& name) override;
    uint8_t* Map() override;
    void Unmap() override;
    bool AllowCommonStatePromotion(ResourceState state_after) override;
    MemoryRequirements GetMemoryRequirements() const override;

    struct Image
    {
        VkImage res;
        VkImage res_owner;
        VkFormat format = VkFormat::VK_FORMAT_UNDEFINED;
        VkExtent2D size = {};
        uint32_t level_count = 1;
        uint32_t sample_count = 1;
        uint32_t array_layers = 1;
    } image;

    struct Buffer
    {
        VkBuffer res;
        uint32_t size = 0;
    } buffer;

    struct Sampler
    {
        VkSampler res;
    } sampler;

    VkAccelerationStructureKHR acceleration_structure_handle = {};

private:
    VKDevice& m_device;
    VkDeviceMemory m_vk_memory;
};
