#pragma once
#include <RHI/Instance/BaseTypes.h>
#include <RHI/ShaderReflection/ShaderReflection.h>
#include <RHIVulkan/Pipeline/VKPipeline.h>
#include <RHIVulkan/RenderPass/VKRenderPass.h>
#include <vulkan/vulkan.hpp>

vk::ShaderStageFlagBits ExecutionModel2Bit(ShaderKind kind);

class VKDevice;

class VKGraphicsPipeline : public VKPipeline
{
public:
  VKGraphicsPipeline(VKDevice& device, const GraphicsPipelineDesc& desc);
  PipelineType GetPipelineType() const override;

  vk::RenderPass GetRenderPass() const;

private:
  void CreateInputLayout(std::vector<vk::VertexInputBindingDescription>& binding_desc, std::vector<vk::VertexInputAttributeDescription>& attribute_desc);

  GraphicsPipelineDesc m_desc;
  std::vector<vk::VertexInputBindingDescription> m_binding_desc;
  std::vector<vk::VertexInputAttributeDescription> m_attribute_desc;
};
