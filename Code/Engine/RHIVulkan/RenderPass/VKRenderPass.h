#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHIVulkan/VKBasics.h>

#include <RHI/RenderPass/RenderPass.h>

class ezRHIVKRenderPass : public ezRHIRenderPass
{
public:
  ezRHIVKRenderPass(ezRHIVKDevice& device, const ezRHIRenderPassCreationDescription& desc);
  const ezRHIRenderPassCreationDescription& GetDescription() const override;

  vk::RenderPass GetRenderPass() const;

private:
  ezRHIRenderPassCreationDescription m_Desc;
  vk::UniqueRenderPass m_RenderPass;
};
