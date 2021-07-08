#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHIVulkan/VKBasics.h>

#include <RHIVulkan/Pipeline/VKPipeline.h>

class ezRHIVKGraphicsPipeline : public ezRHIVKPipeline
{
public:
  ezRHIVKGraphicsPipeline(ezRHIVKDevice& device, const ezRHIGraphicsPipelineCreationDescription& desc);
  ezRHIPipelineType GetPipelineType() const override;

  vk::RenderPass GetRenderPass() const;

private:
  void CreateInputLayout(ezDynamicArray<vk::VertexInputBindingDescription>& bindingDesc, ezDynamicArray<vk::VertexInputAttributeDescription>& attributeDesc);

  ezRHIGraphicsPipelineCreationDescription m_Desc;
  ezDynamicArray<vk::VertexInputBindingDescription> m_BindingDesc;
  ezDynamicArray<vk::VertexInputAttributeDescription> m_AttributeDesc;
};
