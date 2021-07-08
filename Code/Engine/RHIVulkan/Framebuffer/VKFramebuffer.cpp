#include <RHIVulkanPCH.h>

#include <RHIVulkan/Framebuffer/VKFramebuffer.h>
#include <RHIVulkan/View/VKView.h>
#include <RHIVulkan/Resource/VKResource.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/RenderPass/VKRenderPass.h>

ezRHIVKFramebuffer::ezRHIVKFramebuffer(ezRHIVKDevice& device, const ezRHIFramebufferCreationDescription& desc)
  : ezRHIFramebufferBase{desc}
  , m_Extent{desc.m_Width, desc.m_Height}
{
  vk::FramebufferCreateInfo framebufferInfo = {};
  ezDynamicArray<vk::ImageView> attachmentViews;
  framebufferInfo.layers = 1;

  auto addView = [&](ezRHIView* view) {
    if (!view)
      return;
    decltype(auto) vkView = view->As<ezRHIVKView>();
    decltype(auto) resource = vkView->GetResource();
    if (!resource)
      return;
    attachmentViews.PushBack(vkView->GetImageView());

    decltype(auto) vkResource = resource->As<ezRHIVKResource>();
    framebufferInfo.layers = ezMath::Max(framebufferInfo.layers, vkResource->m_Image.m_ArrayLayers);
  };

  for (auto& rtv : desc.m_Colors)
  {
    addView(rtv);
  }
  addView(desc.m_DepthStencil);
  addView(desc.m_ShadingRateImage);

  framebufferInfo.width = m_Extent.width;
  framebufferInfo.height = m_Extent.height;
  framebufferInfo.renderPass = desc.m_RenderPass->As<ezRHIVKRenderPass>()->GetRenderPass();
  framebufferInfo.attachmentCount = attachmentViews.GetCount();
  framebufferInfo.pAttachments = attachmentViews.GetData();
  m_Framebuffer = device.GetDevice().createFramebufferUnique(framebufferInfo);
}

vk::Framebuffer ezRHIVKFramebuffer::GetFramebuffer() const
{
  return m_Framebuffer.get();
}

vk::Extent2D ezRHIVKFramebuffer::GetExtent() const
{
  return m_Extent;
}
