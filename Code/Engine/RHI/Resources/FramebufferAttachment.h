#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <RHI/Resources/Texture.h>

#include <Foundation/Algorithm/HashableStruct.h>

/// <summary>
/// Represents a single output of a <see cref="RHIFramebuffer"/>. May be a color or depth attachment.
/// </summary>
class EZ_RHI_DLL RHIFramebufferAttachment : public ezHashableStruct<RHIFramebufferAttachment>
{
public:
  /// <summary>
  /// The target <see cref="Texture"/> which will be rendered to.
  /// </summary>
  virtual RHITexture* GetTarget() const
  {
    return Target;
  }

  /// <summary>
  /// The target array layer.
  /// </summary>
  virtual ezUInt32 GetArrayLayer() const
  {
    return ArrayLayer;
  }

  /// <summary>
  /// The target mip level.
  /// </summary>
  virtual ezUInt32 GetMipLevel() const
  {
    return MipLevel;
  }

  /// <summary>
  /// Constructs a new FramebufferAttachment.
  /// </summary>
  /// <param name="target">The target <see cref="RHITexture"/> which will be rendered to.</param>
  /// <param name="arrayLayer">The target array layer.</param>
  RHIFramebufferAttachment(RHITexture* target, ezUInt32 arrayLayer)
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
  RHIFramebufferAttachment(RHITexture* target, ezUInt32 arrayLayer, ezUInt32 mipLevel)
  {
    Target = target;
    ArrayLayer = arrayLayer;
    MipLevel = mipLevel;
  }

private:
  RHITexture* Target = nullptr;
  ezUInt32 ArrayLayer = 0;
  ezUInt32 MipLevel = 0;
};
