#pragma once
#include <RHI/Instance/BaseTypes.h>
#include <RHI/ShaderReflection/ShaderReflection.h>
#include <RHIVulkan/Pipeline/VKPipeline.h>
#include <RHIVulkan/RenderPass/VKRenderPass.h>
#include <RHIVulkan/RHIVulkanDLL.h>

VkShaderStageFlagBits ExecutionModel2Bit(ShaderKind kind);

class VKDevice;

class VKGraphicsPipeline : public VKPipeline
{
public:
  VKGraphicsPipeline(VKDevice& device, const GraphicsPipelineDesc& desc);
  PipelineType GetPipelineType() const override;

  VkRenderPass GetRenderPass() const;

private:
  void CreateInputLayout(std::vector<VkVertexInputBindingDescription>& binding_desc, std::vector<VkVertexInputAttributeDescription>& attribute_desc);

  GraphicsPipelineDesc m_desc;
  std::vector<VkVertexInputBindingDescription> m_binding_desc;
  std::vector<VkVertexInputAttributeDescription> m_attribute_desc;
};
