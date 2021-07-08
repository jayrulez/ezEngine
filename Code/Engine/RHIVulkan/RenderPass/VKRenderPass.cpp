#include <RHIVulkanPCH.h>

#include <RHIVulkan/RenderPass/VKRenderPass.h>
#include <RHIVulkan/Device/VKDevice.h>

vk::AttachmentLoadOp Convert(ezRHIRenderPassLoadOp op)
{
  switch (op)
  {
    case ezRHIRenderPassLoadOp::Load:
      return vk::AttachmentLoadOp::eLoad;
    case ezRHIRenderPassLoadOp::Clear:
      return vk::AttachmentLoadOp::eClear;
    case ezRHIRenderPassLoadOp::DontCare:
      return vk::AttachmentLoadOp::eDontCare;
  }
  assert(false);
  return vk::AttachmentLoadOp::eLoad;
}

vk::AttachmentStoreOp Convert(ezRHIRenderPassStoreOp op)
{
  switch (op)
  {
    case ezRHIRenderPassStoreOp::Store:
      return vk::AttachmentStoreOp::eStore;
    case ezRHIRenderPassStoreOp::DontCare:
      return vk::AttachmentStoreOp::eDontCare;
  }
  assert(false);
  return vk::AttachmentStoreOp::eStore;
}

ezRHIVKRenderPass::ezRHIVKRenderPass(ezRHIVKDevice& device, const ezRHIRenderPassCreationDescription& desc)
  : m_Desc{desc}
{
  while (!m_Desc.m_Colors.IsEmpty() && m_Desc.m_Colors.PeekBack().m_Format == ezRHIResourceFormat::Unknown)
  {
    m_Desc.m_Colors.PopBack();
  }

  ezDynamicArray<vk::AttachmentDescription2> attachmentDescriptions;

  auto addAttachment = [&](vk::AttachmentReference2& reference, ezRHIResourceFormat::Enum format, vk::ImageLayout layout, ezRHIRenderPassLoadOp loadOp, ezRHIRenderPassStoreOp storeOp) {
    if (format == ezRHIResourceFormat::Unknown)
    {
      reference.attachment = VK_ATTACHMENT_UNUSED;
      return;
    }
    vk::AttachmentDescription2& description = attachmentDescriptions.ExpandAndGetRef();
    description.format = VKUtilities::ToVKFormat(format);
    description.samples = static_cast<vk::SampleCountFlagBits>(m_Desc.m_SampleCount);
    description.loadOp = Convert(loadOp);
    description.storeOp = Convert(storeOp);
    description.initialLayout = layout;
    description.finalLayout = layout;

    reference.attachment = attachmentDescriptions.GetCount() - 1;
    reference.layout = layout;
  };

  vk::SubpassDescription2 subPass = {};
  subPass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

  ezDynamicArray<vk::AttachmentReference2> colorAttachmentReferences;
  for (auto& rtv : m_Desc.m_Colors)
  {
    addAttachment(colorAttachmentReferences.ExpandAndGetRef(), rtv.m_Format, vk::ImageLayout::eColorAttachmentOptimal, rtv.m_LoadOp, rtv.m_StoreOp);
  }

  subPass.colorAttachmentCount = colorAttachmentReferences.GetCount();
  subPass.pColorAttachments = colorAttachmentReferences.GetData();

  vk::AttachmentReference2 depthAttachmentReference = {};
  if (m_Desc.m_DepthStencil.m_Format != ezRHIResourceFormat::Unknown)
  {
    addAttachment(depthAttachmentReference, m_Desc.m_DepthStencil.m_Format, vk::ImageLayout::eDepthStencilAttachmentOptimal, m_Desc.m_DepthStencil.m_DepthLoadOp, m_Desc.m_DepthStencil.m_DepthStoreOp);
    if (depthAttachmentReference.attachment != VK_ATTACHMENT_UNUSED)
    {
      vk::AttachmentDescription2& description = attachmentDescriptions[depthAttachmentReference.attachment];
      description.stencilLoadOp = Convert(m_Desc.m_DepthStencil.m_StencilLoadOp);
      description.stencilStoreOp = Convert(m_Desc.m_DepthStencil.m_StencilStoreOp);
    }
    subPass.pDepthStencilAttachment = &depthAttachmentReference;
  }

  if (m_Desc.m_ShadingRateFormat != ezRHIResourceFormat::Unknown)
  {
    vk::AttachmentReference2 shadingRateImageAttachmentReference = {};
    addAttachment(
      shadingRateImageAttachmentReference,
      m_Desc.m_ShadingRateFormat,
      vk::ImageLayout::eFragmentShadingRateAttachmentOptimalKHR,
      ezRHIRenderPassLoadOp::Load,
      ezRHIRenderPassStoreOp::Store);

    vk::FragmentShadingRateAttachmentInfoKHR fragmentShadingRateAttachmentInfo = {};
    fragmentShadingRateAttachmentInfo.pFragmentShadingRateAttachment = &shadingRateImageAttachmentReference;
    fragmentShadingRateAttachmentInfo.shadingRateAttachmentTexelSize.width = device.GetCapabilities().m_ShadingRateImageTileSize;
    fragmentShadingRateAttachmentInfo.shadingRateAttachmentTexelSize.height = device.GetCapabilities().m_ShadingRateImageTileSize;
    subPass.pNext = &fragmentShadingRateAttachmentInfo;
  }

  vk::RenderPassCreateInfo2 renderPassInfo = {};
  renderPassInfo.attachmentCount = attachmentDescriptions.GetCount();
  renderPassInfo.pAttachments = attachmentDescriptions.GetData();
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subPass;

  m_RenderPass = device.GetDevice().createRenderPass2Unique(renderPassInfo);
}

const ezRHIRenderPassCreationDescription& ezRHIVKRenderPass::GetDescription() const
{
  return m_Desc;
}

vk::RenderPass ezRHIVKRenderPass::GetRenderPass() const
{
  return m_RenderPass.get();
}
