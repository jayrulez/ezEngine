#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Framebuffer/VKFramebuffer.h>
#include <RHIVulkan/Pipeline/VKGraphicsPipeline.h>
#include <RHIVulkan/View/VKView.h>

VKFramebuffer::VKFramebuffer(VKDevice& device, const FramebufferDesc& desc)
  : FramebufferBase(desc)
  , m_device{device}
  , m_extent{desc.width, desc.height}
{
  VkFramebufferCreateInfo framebuffer_info = {};
  framebuffer_info.sType = VkStructureType::VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  std::vector<VkImageView> attachment_views;
  framebuffer_info.layers = 1;
  auto add_view = [&](const std::shared_ptr<View>& view) {
    if (!view)
      return;
    decltype(auto) vk_view = view->As<VKView>();
    decltype(auto) resource = vk_view.GetResource();
    if (!resource)
      return;
    attachment_views.emplace_back(vk_view.GetImageView());

    decltype(auto) vk_resource = resource->As<VKResource>();
    framebuffer_info.layers = ezMath::Max(framebuffer_info.layers, vk_resource.image.array_layers);
  };
  for (auto& rtv : desc.colors)
  {
    add_view(rtv);
  }
  add_view(desc.depth_stencil);
  add_view(desc.shading_rate_image);

  framebuffer_info.width = m_extent.width;
  framebuffer_info.height = m_extent.height;
  framebuffer_info.renderPass = desc.render_pass->As<VKRenderPass>().GetRenderPass();
  framebuffer_info.attachmentCount = (ezUInt32)attachment_views.size();
  framebuffer_info.pAttachments = attachment_views.data();

  vkCreateFramebuffer(device.GetDevice(), &framebuffer_info, nullptr, &m_framebuffer);
}

VKFramebuffer::~VKFramebuffer()
{
  vkDestroyFramebuffer(m_device.GetDevice(), m_framebuffer, nullptr);
}

VkFramebuffer VKFramebuffer::GetFramebuffer() const
{
  return m_framebuffer;
}

VkExtent2D VKFramebuffer::GetExtent() const
{
  return m_extent;
}
