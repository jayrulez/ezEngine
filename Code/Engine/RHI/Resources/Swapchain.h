#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <optional>

#include <RHI/Resources/DeviceResource.h>
#include <RHI/Resources/Framebuffer.h>

/// <summary>
/// A platform-specific object representing a renderable surface.
/// A SwapchainSource can be created with one of several static factory methods.
/// A SwapchainSource is used to describe a Swapchain (see <see cref="SwapchainDescription"/>).
/// </summary>
class EZ_RHI_DLL RHISwapchainSource
{
public:
  RHISwapchainSource() = default;

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements are equal; false otherswise.</returns>
  virtual bool operator==(const RHISwapchainSource& other) const
  {
    return true;
  }
};

/// <summary>
/// Describes a <see cref="Swapchain"/>, for creation via a <see cref="ResourceFactory"/>.
/// </summary>
struct EZ_RHI_DLL RHISwapchainDescription : public ezHashableStruct<RHISwapchainDescription>
{
  /// <summary>
  /// The <see cref="SwapchainSource"/> which will be used as the target of rendering operations.
  /// This is a window-system-specific object which differs by platform.
  /// </summary>
  RHISwapchainSource Source;

  /// <summary>
  /// The initial width of the Swapchain surface.
  /// </summary>
  ezUInt32 Width = 0;

  /// <summary>
  /// The initial height of the Swapchain surface.
  /// </summary>
  ezUInt32 Height = 0;

  /// <summary>
  /// The optional format of the depth target of the Swapchain's Framebuffer.
  /// If non-null, this must be a valid depth Texture format.
  /// If null, then no depth target will be created.
  /// </summary>
  std::optional<ezEnum<RHIPixelFormat>> DepthFormat;

  /// <summary>
  /// Indicates whether presentation of the Swapchain will be synchronized to the window system's vertical refresh rate.
  /// </summary>
  bool SyncToVerticalBlank = false;

  /// <summary>
  /// Indicates whether the color target of the Swapchain will use an sRGB PixelFormat.
  /// </summary>
  bool ColorSrgb = false;

  /// <summary>
  /// Constructs a new SwapchainDescription.
  /// </summary>
  /// <param name="source">The <see cref="SwapchainSource"/> which will be used as the target of rendering operations.
  /// This is a window-system-specific object which differs by platform.</param>
  /// <param name="width">The initial width of the Swapchain surface.</param>
  /// <param name="height">The initial height of the Swapchain surface.</param>
  /// <param name="depthFormat">The optional format of the depth target of the Swapchain's Framebuffer.
  /// If non-null, this must be a valid depth Texture format.
  /// If null, then no depth target will be created.</param>
  /// <param name="syncToVerticalBlank">Indicates whether presentation of the Swapchain will be synchronized to the window
  /// system's vertical refresh rate.</param>
  RHISwapchainDescription(
    RHISwapchainSource source,
    ezUInt32 width,
    ezUInt32 height,
    std::optional<ezEnum<RHIPixelFormat>> depthFormat,
    bool syncToVerticalBlank)
  {
    Source = source;
    Width = width;
    Height = height;
    DepthFormat = depthFormat;
    SyncToVerticalBlank = syncToVerticalBlank;
    ColorSrgb = false;
  }

  /// <summary>
  /// Constructs a new SwapchainDescription.
  /// </summary>
  /// <param name="source">The <see cref="SwapchainSource"/> which will be used as the target of rendering operations.
  /// This is a window-system-specific object which differs by platform.</param>
  /// <param name="width">The initial width of the Swapchain surface.</param>
  /// <param name="height">The initial height of the Swapchain surface.</param>
  /// <param name="depthFormat">The optional format of the depth target of the Swapchain's Framebuffer.
  /// If non-null, this must be a valid depth Texture format.
  /// If null, then no depth target will be created.</param>
  /// <param name="syncToVerticalBlank">Indicates whether presentation of the Swapchain will be synchronized to the window
  /// system's vertical refresh rate.</param>
  /// <param name="colorSrgb">Indicates whether the color target of the Swapchain will use an sRGB PixelFormat.</param>
  RHISwapchainDescription(
    RHISwapchainSource source,
    ezUInt32 width,
    ezUInt32 height,
    std::optional<ezEnum<RHIPixelFormat>> depthFormat,
    bool syncToVerticalBlank,
    bool colorSrgb)
  {
    Source = source;
    Width = width;
    Height = height;
    DepthFormat = depthFormat;
    SyncToVerticalBlank = syncToVerticalBlank;
    ColorSrgb = colorSrgb;
  }

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements are equal; false otherswise.</returns>
  bool operator==(const RHISwapchainDescription& other) const
  {
    return Source == other.Source &&
           Width == other.Width &&
           Height == other.Height &&
           DepthFormat == other.DepthFormat &&
           SyncToVerticalBlank == other.SyncToVerticalBlank &&
           ColorSrgb == other.ColorSrgb;
  }
};

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
