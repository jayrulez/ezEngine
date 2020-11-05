#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <RHI/Descriptors/OutputDescription.h>
#include <RHI/Descriptors/FramebufferAttachmentDescription.h>

#include <RHI/Resources/Resource.h>
#include <RHI/Resources/FramebufferAttachment.h>

class RHITexture;

/// <summary>
/// A device resource used to control which color and depth textures are rendered to.
/// See <see cref="RHIFramebufferDescription"/>.
/// </summary>
class EZ_RHI_DLL RHIFramebuffer : public RHIResource
{
protected:
  RHIFramebuffer() = default;

  RHIFramebuffer(const std::optional<RHIFramebufferAttachmentDescription>& depthTargetDesc, const ezDynamicArray<RHIFramebufferAttachmentDescription>& colorTargetDescs);

public:
  /// <summary>
  /// Gets the depth attachment associated with this instance. May be null if no depth texture is used.
  /// </summary>
  virtual std::optional<RHIFramebufferAttachment> GetDepthTarget() const;

  /// <summary>
  /// Gets the collection of color attachments associated with this instance. May be empty.
  /// </summary>
  virtual ezDynamicArray<RHIFramebufferAttachment> GetColorTargets() const;

  /// <summary>
  /// Gets an <see cref="RHIOutputDescription"/> which describes the number and formats of the depth and color targets
  /// in this instance.
  /// </summary>
  virtual RHIOutputDescription GetOutputDescription() const;

  /// <summary>
  /// Gets the width of the <see cref="RHIFramebuffer"/>.
  /// </summary>
  virtual ezUInt32 GetWidth() const;

  /// <summary>
  /// Gets the height of the <see cref="RHIFramebuffer"/>.
  /// </summary>
  virtual ezUInt32 GetHeight() const;
private:
  std::optional<RHIFramebufferAttachment> DepthTarget;
  ezDynamicArray<RHIFramebufferAttachment> ColorTargets;
  RHIOutputDescription OutputDescription;
  ezUInt32 Width = 0;
  ezUInt32 Height = 0;
};
