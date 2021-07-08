#pragma once
#include <RHI/RenderPass/RenderPass.h>
#include <RHIVulkan/RHIVulkanDLL.h>

class VKDevice;

class VKRenderPass : public RenderPass
{
public:
    VKRenderPass(VKDevice& device, const RenderPassDesc& desc);
    const RenderPassDesc& GetDesc() const override;

    VkRenderPass GetRenderPass() const;

private:
    RenderPassDesc m_desc;
    VkRenderPass m_render_pass;
};
