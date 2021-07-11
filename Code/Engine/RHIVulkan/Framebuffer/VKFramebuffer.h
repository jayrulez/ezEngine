#pragma once
#include <RHI/Framebuffer/FramebufferBase.h>
#include <RHIVulkan/RHIVulkanDLL.h>

class VKDevice;
class VKGraphicsPipeline;

class VKFramebuffer : public FramebufferBase
{
public:
    VKFramebuffer(VKDevice& device, const FramebufferDesc& desc);
  ~VKFramebuffer();

    VkFramebuffer GetFramebuffer() const;
    VkExtent2D GetExtent() const;

private:
    VKDevice& m_device;
    VkFramebuffer m_framebuffer;
    VkExtent2D m_extent;
};
