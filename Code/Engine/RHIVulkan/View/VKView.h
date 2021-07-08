#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHIVulkan/VKBasics.h>

#include <RHI/View/View.h>

class ezRHIVKView : public ezRHIView
{
public:
  ezRHIVKView(ezRHIVKDevice& device, ezRHIVKResource* resource, const ezRHIViewDesc& viewDesc);
  ezRHIResource* GetResource() override;
  ezUInt32 GetDescriptorId() const override;
  ezUInt32 GetBaseMipLevel() const override;
  ezUInt32 GetLevelCount() const override;
  ezUInt32 GetBaseArrayLayer() const override;
  ezUInt32 GetLayerCount() const override;

  vk::ImageView GetImageView() const;
  vk::WriteDescriptorSet GetDescriptor() const;

private:
  void CreateView();
  void CreateImageView();
  void CreateBufferView();

  ezRHIVKDevice& m_Device;
  ezRHIVKResource* m_Resource;
  ezRHIViewDesc m_ViewDesc;
  vk::UniqueImageView m_ImageView;
  vk::UniqueBufferView m_BufferView;
 ezRHIVKGPUDescriptorPoolRange* m_Range;
  vk::DescriptorImageInfo m_DescriptorImage = {};
  vk::DescriptorBufferInfo m_DescriptorBuffer = {};
  vk::WriteDescriptorSetAccelerationStructureKHR m_DescriptorAccelerationStructure = {};
  vk::WriteDescriptorSet m_Descriptor = {};
};
