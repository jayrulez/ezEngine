#include <RHI/Resources/FramebufferAttachment.h>

/// <summary>
/// The target <see cref="Texture"/> which will be rendered to.
/// </summary>

RHITexture* RHIFramebufferAttachment::GetTarget() const
{
  return Target;
}

/// <summary>
/// The target array layer.
/// </summary>

ezUInt32 RHIFramebufferAttachment::GetArrayLayer() const
{
  return ArrayLayer;
}

/// <summary>
/// The target mip level.
/// </summary>

ezUInt32 RHIFramebufferAttachment::GetMipLevel() const
{
  return MipLevel;
}

/// <summary>
/// Constructs a new FramebufferAttachment.
/// </summary>
/// <param name="target">The target <see cref="RHITexture"/> which will be rendered to.</param>
/// <param name="arrayLayer">The target array layer.</param>

RHIFramebufferAttachment::RHIFramebufferAttachment(RHITexture* target, ezUInt32 arrayLayer)
{
  Target = target;
  ArrayLayer = arrayLayer;
  MipLevel = 0;
}

/// <summary>
/// Constructs a new FramebufferAttachment.
/// </summary>
/// <param name="target">The target <see cref="RHITexture"/> which will be rendered to.</param>
/// <param name="arrayLayer">The target array layer.</param>
/// <param name="mipLevel">The target mip level.</param>

RHIFramebufferAttachment::RHIFramebufferAttachment(RHITexture* target, ezUInt32 arrayLayer, ezUInt32 mipLevel)
{
  Target = target;
  ArrayLayer = arrayLayer;
  MipLevel = mipLevel;
}
