#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/RenderPass/VKRenderPass.h>
#include <RHIVulkan/Utilities/VKUtility.h>
#include <RHIVulkan/View/VKView.h>

VkAttachmentLoadOp Convert(RenderPassLoadOp op)
{
  switch (op)
  {
    case RenderPassLoadOp::kLoad:
      return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_LOAD;
    case RenderPassLoadOp::kClear:
      return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
    case RenderPassLoadOp::kDontCare:
      return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  }
  assert(false);
  return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_LOAD;
}

VkAttachmentStoreOp Convert(RenderPassStoreOp op)
{
  switch (op)
  {
    case RenderPassStoreOp::kStore:
      return VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
    case RenderPassStoreOp::kDontCare:
      return VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
  }
  assert(false);
  return VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
}

VKRenderPass::VKRenderPass(VKDevice& device, const RenderPassDesc& desc)
  : m_desc(desc)
  , m_device{device}
{
  while (!m_desc.colors.empty() && m_desc.colors.back().format == ezRHIResourceFormat::UNKNOWN)
  {
    m_desc.colors.pop_back();
  }

  std::vector<VkAttachmentDescription2> attachment_descriptions;
  auto add_attachment = [&](VkAttachmentReference2& reference, ezRHIResourceFormat::Enum format, VkImageLayout layout, RenderPassLoadOp load_op, RenderPassStoreOp store_op) {
    reference.sType = VkStructureType::VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
    if (format == ezRHIResourceFormat::UNKNOWN)
    {
      reference.attachment = VK_ATTACHMENT_UNUSED;
      return;
    }
    VkAttachmentDescription2& description = attachment_descriptions.emplace_back();
    description.sType = VkStructureType::VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
    description.format = VKUtils::ToVkFormat(format);
    description.samples = static_cast<VkSampleCountFlagBits>(m_desc.sample_count);
    description.loadOp = Convert(load_op);
    description.storeOp = Convert(store_op);
    description.initialLayout = layout;
    description.finalLayout = layout;

    reference.attachment = (ezUInt32)(attachment_descriptions.size() - 1);
    reference.layout = layout;
  };

  VkSubpassDescription2 sub_pass = {};
  sub_pass.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2;
  sub_pass.pipelineBindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;

  std::vector<VkAttachmentReference2> color_attachment_references;
  for (auto& rtv : m_desc.colors)
  {
    add_attachment(color_attachment_references.emplace_back(), rtv.format, VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, rtv.load_op, rtv.store_op);
  }

  sub_pass.colorAttachmentCount = (ezUInt32)color_attachment_references.size();
  sub_pass.pColorAttachments = color_attachment_references.data();

  VkAttachmentReference2 depth_attachment_reference = {};
  depth_attachment_reference.sType = VkStructureType::VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
  if (m_desc.depth_stencil.format != ezRHIResourceFormat::UNKNOWN)
  {
    add_attachment(depth_attachment_reference, m_desc.depth_stencil.format, VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, m_desc.depth_stencil.depth_load_op, m_desc.depth_stencil.depth_store_op);
    if (depth_attachment_reference.attachment != VK_ATTACHMENT_UNUSED)
    {
      VkAttachmentDescription2& description = attachment_descriptions[depth_attachment_reference.attachment];
      description.sType = VkStructureType::VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
      description.stencilLoadOp = Convert(m_desc.depth_stencil.stencil_load_op);
      description.stencilStoreOp = Convert(m_desc.depth_stencil.stencil_store_op);
    }
    sub_pass.pDepthStencilAttachment = &depth_attachment_reference;
  }

  if (m_desc.shading_rate_format != ezRHIResourceFormat::UNKNOWN)
  {
    VkAttachmentReference2 shading_rate_image_attachment_reference = {};
    shading_rate_image_attachment_reference.sType = VkStructureType::VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
    add_attachment(
      shading_rate_image_attachment_reference,
      m_desc.shading_rate_format,
      VkImageLayout::VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR,
      RenderPassLoadOp::kLoad,
      RenderPassStoreOp::kStore);

    VkFragmentShadingRateAttachmentInfoKHR fragment_shading_rate_attachment_info = {};
    fragment_shading_rate_attachment_info.pFragmentShadingRateAttachment = &shading_rate_image_attachment_reference;
    fragment_shading_rate_attachment_info.shadingRateAttachmentTexelSize.width = device.GetShadingRateImageTileSize();
    fragment_shading_rate_attachment_info.shadingRateAttachmentTexelSize.height = device.GetShadingRateImageTileSize();
    sub_pass.pNext = &fragment_shading_rate_attachment_info;
  }

  VkRenderPassCreateInfo2 render_pass_info = {};
  render_pass_info.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2;
  render_pass_info.attachmentCount = (ezUInt32)attachment_descriptions.size();
  render_pass_info.pAttachments = attachment_descriptions.data();
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &sub_pass;

  vkCreateRenderPass2(device.GetDevice(), &render_pass_info, nullptr, &m_render_pass);
}

VKRenderPass::~VKRenderPass()
{
  if (m_render_pass != VK_NULL_HANDLE)
  {
    vkDestroyRenderPass(m_device.GetDevice(), m_render_pass, nullptr);
  }
}

const RenderPassDesc& VKRenderPass::GetDesc() const
{
  return m_desc;
}

VkRenderPass VKRenderPass::GetRenderPass() const
{
  return m_render_pass;
}
