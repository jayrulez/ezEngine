#pragma once
#include <RHI/View/View.h>
#include <RHIVulkan/Resource/VKResource.h>
#include <RHIVulkan/GPUDescriptorPool/VKGPUDescriptorPoolRange.h>

class VKDevice;

class VKView : public View
{
public:
    VKView(VKDevice& device, const ezSharedPtr<VKResource>& resource, const ViewDesc& view_desc);
    ezSharedPtr<Resource> GetResource() override;
    uint32_t GetDescriptorId() const override;
    uint32_t GetBaseMipLevel() const override;
    uint32_t GetLevelCount() const override;
    uint32_t GetBaseArrayLayer() const override;
    uint32_t GetLayerCount() const override;

    vk::ImageView GetImageView() const;
    vk::WriteDescriptorSet GetDescriptor() const;

private:
    void CreateView();
    void CreateImageView();
    void CreateBufferView();

    VKDevice& m_device;
    ezSharedPtr<VKResource> m_resource;
    ViewDesc m_view_desc;
    vk::UniqueImageView m_image_view;
    vk::UniqueBufferView m_buffer_view;
    ezSharedPtr<VKGPUDescriptorPoolRange> m_range;
    vk::DescriptorImageInfo m_descriptor_image = {};
    vk::DescriptorBufferInfo m_descriptor_buffer = {};
    vk::WriteDescriptorSetAccelerationStructureKHR m_descriptor_acceleration_structure = {};
    vk::WriteDescriptorSet m_descriptor = {};
};
