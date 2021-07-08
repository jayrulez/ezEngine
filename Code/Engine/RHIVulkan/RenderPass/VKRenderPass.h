#pragma once
#include <RHI/RenderPass/RenderPass.h>
#include <RHIVulkan/RHIVulkanDLL.h>

class VKDevice;

class VKRenderPass : public RenderPass
{
public:
    VKRenderPass(VKDevice& device, const RenderPassDesc& desc);
  ~VKRenderPass();
    const RenderPassDesc& GetDesc() const override;

    VkRenderPass GetRenderPass() const;

private:
    VKDevice& m_device;
    RenderPassDesc m_desc;
    VkRenderPass m_render_pass;
};
