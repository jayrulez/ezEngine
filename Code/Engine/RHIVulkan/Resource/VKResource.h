#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHIVulkan/VKBasics.h>

#include <RHI/Resource/ResourceBase.h>

class ezRHIVKResource : public ezRHIResourceBase
{
public:
  ezRHIVKResource(ezRHIVKDevice& device);

  void CommitMemory(ezRHIMemoryType memoryType) override;
  void BindMemory(ezRHIMemory* pMemory, ezUInt64 offset) override;
  ezUInt64 GetWidth() const override;
  ezUInt32 GetHeight() const override;
  ezUInt16 GetLayerCount() const override;
  ezUInt16 GetLevelCount() const override;
  ezUInt32 GetSampleCount() const override;
  ezUInt64 GetAccelerationStructureHandle() const override;
  void SetName(const ezString& name) override;
  ezUInt8* Map() override;
  void Unmap() override;
  bool AllowCommonStatePromotion(ezBitflags<ezRHIResourceState> stateAfter) override;
  ezRHIMemoryRequirements GetMemoryRequirements() const override;

  struct Image
  {
    vk::Image m_Image;
    vk::UniqueImage m_ImageOwner;
    vk::Format m_Format = vk::Format::eUndefined;
    vk::Extent2D m_Size = {};
    ezUInt32 m_LevelCount = 1;
    ezUInt32 m_SampleCount = 1;
    ezUInt32 m_ArrayLayers = 1;
  } m_Image;

  struct Buffer
  {
    vk::UniqueBuffer m_Buffer;
    ezUInt32 m_Size = 0;
  } m_Buffer;

  struct Sampler
  {
    vk::UniqueSampler m_Sampler;
  } m_Sampler;

  vk::UniqueAccelerationStructureKHR m_AccelerationStructureHandle = {};

private:
  ezRHIVKDevice& m_Device;
  vk::DeviceMemory m_VkMemory;
};
