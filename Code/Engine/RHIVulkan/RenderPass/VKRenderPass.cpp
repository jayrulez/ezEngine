#include <RHIVulkan/RenderPass/VKRenderPass.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/View/VKView.h>
#include <RHIVulkan/Utilities/VKUtility.h>

VkAttachmentLoadOp Convert(RenderPassLoadOp op)
{
    switch (op)
    {
    case RenderPassLoadOp::kLoad:
        return VkAttachmentLoadOp::eLoad;
    case RenderPassLoadOp::kClear:
        return VkAttachmentLoadOp::eClear;
    case RenderPassLoadOp::kDontCare:
        return VkAttachmentLoadOp::eDontCare;
    }
    assert(false);
    return VkAttachmentLoadOp::eLoad;
}

VkAttachmentStoreOp Convert(RenderPassStoreOp op)
{
    switch (op)
    {
    case RenderPassStoreOp::kStore:
        return VkAttachmentStoreOp::eStore;
    case RenderPassStoreOp::kDontCare:
        return VkAttachmentStoreOp::eDontCare;
    }
    assert(false);
    return VkAttachmentStoreOp::eStore;
}

VKRenderPass::VKRenderPass(VKDevice& device, const RenderPassDesc& desc)
    : m_desc(desc)
{
    while (!m_desc.colors.empty() && m_desc.colors.back().format == ezRHIResourceFormat::UNKNOWN)
    {
        m_desc.colors.pop_back();
    }

    std::vector<VkAttachmentDescription2> attachment_descriptions;
    auto add_attachment = [&](VkAttachmentReference2& reference, ezRHIResourceFormat::Enum format, VkImageLayout layout, RenderPassLoadOp load_op, RenderPassStoreOp store_op)
    {
      if (format == ezRHIResourceFormat::UNKNOWN)
        {
            reference.attachment = VK_ATTACHMENT_UNUSED;
            return;
        }
        VkAttachmentDescription2& description = attachment_descriptions.emplace_back();
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
    sub_pass.pipelineBindPoint = VkPipelineBindPoint::eGraphics;

    std::vector<VkAttachmentReference2> color_attachment_references;
    for (auto& rtv : m_desc.colors)
    {
        add_attachment(color_attachment_references.emplace_back(), rtv.format, VkImageLayout::eColorAttachmentOptimal, rtv.load_op, rtv.store_op);
    }

    sub_pass.colorAttachmentCount = (ezUInt32)color_attachment_references.size();
    sub_pass.pColorAttachments = color_attachment_references.data();

    VkAttachmentReference2 depth_attachment_reference = {};
    if (m_desc.depth_stencil.format != ezRHIResourceFormat::UNKNOWN)
    {
        add_attachment(depth_attachment_reference, m_desc.depth_stencil.format, VkImageLayout::eDepthStencilAttachmentOptimal, m_desc.depth_stencil.depth_load_op, m_desc.depth_stencil.depth_store_op);
        if (depth_attachment_reference.attachment != VK_ATTACHMENT_UNUSED)
        {
            VkAttachmentDescription2& description = attachment_descriptions[depth_attachment_reference.attachment];
            description.stencilLoadOp = Convert(m_desc.depth_stencil.stencil_load_op);
            description.stencilStoreOp = Convert(m_desc.depth_stencil.stencil_store_op);
        }
        sub_pass.pDepthStencilAttachment = &depth_attachment_reference;
    }

    if (m_desc.shading_rate_format != ezRHIResourceFormat::UNKNOWN)
    {
        VkAttachmentReference2 shading_rate_image_attachment_reference = {};
        add_attachment(
            shading_rate_image_attachment_reference,
            m_desc.shading_rate_format,
            VkImageLayout::eFragmentShadingRateAttachmentOptimalKHR,
            RenderPassLoadOp::kLoad,
            RenderPassStoreOp::kStore
        );

        VkFragmentShadingRateAttachmentInfoKHR fragment_shading_rate_attachment_info = {};
        fragment_shading_rate_attachment_info.pFragmentShadingRateAttachment = &shading_rate_image_attachment_reference;
        fragment_shading_rate_attachment_info.shadingRateAttachmentTexelSize.width = device.GetShadingRateImageTileSize();
        fragment_shading_rate_attachment_info.shadingRateAttachmentTexelSize.height = device.GetShadingRateImageTileSize();
        sub_pass.pNext = &fragment_shading_rate_attachment_info;
    }

    VkRenderPassCreateInfo2 render_pass_info = {};
    render_pass_info.attachmentCount = (ezUInt32)attachment_descriptions.size();
    render_pass_info.pAttachments = attachment_descriptions.data();
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &sub_pass;

    m_render_pass = device.GetDevice().createRenderPass2Unique(render_pass_info);
}

const RenderPassDesc& VKRenderPass::GetDesc() const
{
    return m_desc;
}

VkRenderPass VKRenderPass::GetRenderPass() const
{
    return m_render_pass.get();
}
