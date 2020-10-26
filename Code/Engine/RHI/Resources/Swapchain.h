#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/DeviceResource.h>
#include <RHI/Resources/Framebuffer.h>

/// <summary>
/// A device resource providing the ability to present rendered images to a visible surface.
/// See <see cref="SwapchainDescription"/>.
/// </summary>
class EZ_RHI_DLL RHISwapchain : public RHIDeviceResource
{
public:
  /// <summary>
  /// Gets a <see cref="Framebuffer"/> representing the render targets of this instance.
  /// </summary>
  virtual RHIFramebuffer* GetFramebuffer() const = 0;

  /// <summary>
  /// Resizes the renderable Textures managed by this instance to the given dimensions.
  /// </summary>
  /// <param name="width">The new width of the Swapchain.</param>
  /// <param name="height">The new height of the Swapchain.</param>
  virtual void Resize(ezUInt32 width, ezUInt32 height) = 0;

  /// <summary>
  /// Gets or sets whether presentation of this Swapchain will be synchronized to the window system's vertical refresh
  /// rate.
  /// </summary>
  virtual bool GetSyncToVerticalBlank() const = 0;
  virtual void SetSyncToVerticalBlank(bool value) = 0;
};
