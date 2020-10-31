#include <RHI/Resources/Framebuffer.h>

RHIFramebuffer::RHIFramebuffer(const std::optional<RHIFramebufferAttachmentDescription>& depthTargetDesc, const ezDynamicArray<RHIFramebufferAttachmentDescription>& colorTargetDescs)
{
  if (depthTargetDesc)
  {
    RHIFramebufferAttachmentDescription depthAttachment = depthTargetDesc.value();
    DepthTarget = RHIFramebufferAttachment(depthAttachment.Target, depthAttachment.ArrayLayer, depthAttachment.MipLevel);
  }

  for (ezUInt32 i = 0; i < colorTargetDescs.GetCount(); i++)
  {
    ColorTargets.PushBack(RHIFramebufferAttachment(colorTargetDescs[i].Target, colorTargetDescs[i].ArrayLayer, colorTargetDescs[i].MipLevel));
  }

  RHITexture* dimTex = nullptr;
  ezUInt32 mipLevel;

  if (ColorTargets.GetCount() > 0)
  {
    dimTex = ColorTargets[0].GetTarget();
    mipLevel = ColorTargets[0].GetMipLevel();
  }
  else
  {
    EZ_VERIFY(DepthTarget.has_value(), "DepthTarget is not set.");
    dimTex = DepthTarget.value().GetTarget();
    dimTex = DepthTarget.value().GetTarget();
  }

  ezUInt32 mipWidth = 0;
  ezUInt32 mipHeight = 0;

  // TODO: implement this
  //Util.GetMipDimensions(dimTex, mipLevel, out uint mipWidth, out uint mipHeight, out _);

  Width = mipWidth;
  Height = mipHeight;

  OutputDescription = RHIOutputDescription::CreateFromFramebuffer(this);
}

/// <summary>
/// Gets the depth attachment associated with this instance. May be null if no depth texture is used.
/// </summary>

std::optional<RHIFramebufferAttachment> RHIFramebuffer::GetDepthTarget() const
{
  return DepthTarget;
}

/// <summary>
/// Gets the collection of color attachments associated with this instance. May be empty.
/// </summary>

ezDynamicArray<RHIFramebufferAttachment> RHIFramebuffer::GetColorTargets() const
{
  return ColorTargets;
}

/// <summary>
/// Gets an <see cref="RHIOutputDescription"/> which describes the number and formats of the depth and color targets
/// in this instance.
/// </summary>

RHIOutputDescription RHIFramebuffer::GetOutputDescription() const
{
  return OutputDescription;
}

/// <summary>
/// Gets the width of the <see cref="RHIFramebuffer"/>.
/// </summary>

ezUInt32 RHIFramebuffer::GetWidth() const
{
  return Width;
}

/// <summary>
/// Gets the height of the <see cref="RHIFramebuffer"/>.
/// </summary>

ezUInt32 RHIFramebuffer::GetHeight() const
{
  return Height;
}
