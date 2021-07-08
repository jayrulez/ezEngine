#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHIVulkan/VKBasics.h>

#include <RHI/Framebuffer/FramebufferBase.h>

class ezRHIVKFramebuffer : public ezRHIFramebufferBase
{
public:
  ezRHIVKFramebuffer(ezRHIVKDevice& device, const ezRHIFramebufferCreationDescription& desc);

  vk::Framebuffer GetFramebuffer() const;
  vk::Extent2D GetExtent() const;

private:
  vk::UniqueFramebuffer m_Framebuffer;
  vk::Extent2D m_Extent;
};
