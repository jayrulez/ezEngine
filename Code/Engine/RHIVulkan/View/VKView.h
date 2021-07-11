#pragma once
#include <RHI/View/View.h>
#include <RHIVulkan/Resource/VKResource.h>
#include <RHIVulkan/GPUDescriptorPool/VKGPUDescriptorPoolRange.h>

class VKDevice;

class VKView : public View
{
public:
    VKView(VKDevice& device, const std::shared_ptr<VKResource>& resource, const ViewDesc& view_desc);
  ~VKView();
    std::shared_ptr<Resource> GetResource() override;
    uint32_t GetDescriptorId() const override;
    uint32_t GetBaseMipLevel() const override;
    uint32_t GetLevelCount() const override;
    uint32_t GetBaseArrayLayer() const override;
    uint32_t GetLayerCount() const override;

    VkImageView GetImageView() const;
    VkWriteDescriptorSet GetDescriptor() const;

private:
    void CreateView();
    void CreateImageView();
    void CreateBufferView();

    VKDevice& m_device;
    std::shared_ptr<VKResource> m_resource;
    ViewDesc m_view_desc;
    VkImageView m_image_view = VK_NULL_HANDLE;
    VkBufferView m_buffer_view = VK_NULL_HANDLE;
    std::shared_ptr<VKGPUDescriptorPoolRange> m_range;
    VkDescriptorImageInfo m_descriptor_image = {};
    VkDescriptorBufferInfo m_descriptor_buffer = {};
    VkWriteDescriptorSetAccelerationStructureKHR m_descriptor_acceleration_structure = {};
    VkWriteDescriptorSet m_descriptor = {};
};
