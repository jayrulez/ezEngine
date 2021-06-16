#pragma once

#include <RendererVk/Implementation/VulkanHelpers.h>

namespace ezInternal::Vk
{
  class Pipeline
  {
  public:
    Pipeline(const PipelineStateDesc& desc);
    inline virtual ~Pipeline() {}
    EZ_ALWAYS_INLINE PipelineType GetPipelineType() const;
    EZ_ALWAYS_INLINE const PipelineStateDesc& GetDescription() const;
    ezResult Init(ezGALDevice* pDevice);
    ezResult Destroy(ezGALDevice* pDevice);

  private:
    PipelineStateDesc m_Description;
    VkPipelineLayout m_VkPipelineLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_VkDescriptorSetLayout = VK_NULL_HANDLE;
  };

#include <RendererVk/Device/Implementation/Pipeline_inl.h>
} // namespace ezInternal::Vk
