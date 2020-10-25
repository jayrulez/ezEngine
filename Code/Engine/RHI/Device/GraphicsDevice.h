#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <optional>

/// <summary>
/// A structure describing several common properties of a GraphicsDevice.
/// </summary>
struct EZ_RHI_DLL RHIGraphicsDeviceOptions
{
  /// <summary>
  /// Indicates whether the GraphicsDevice will support debug features, provided they are supported by the host system.
  /// </summary>
  bool Debug = false;

  /// <summary>
  /// Indicates whether the Graphicsdevice will include a "main" Swapchain. If this value is true, then the GraphicsDevice
  /// must be created with one of the overloads that provides Swapchain source information.
  /// </summary>
  bool HasMainSwapchain = false;

  /// <summary>
  /// An optional <see cref="PixelFormat"/> to be used for the depth buffer of the swapchain. If this value is null, then
  /// no depth buffer will be present on the swapchain.
  /// </summary>
  std::optional<ezEnum<RHIPixelFormat>> SwapchainDepthFormat;

  /// <summary>
  /// Indicates whether the main Swapchain will be synchronized to the window system's vertical refresh rate.
  /// </summary>
  bool SyncToVerticalBlank = false;

  /// <summary>
  /// Specifies which model the rendering backend should use for binding resources. This can be overridden per-pipeline
  /// by specifying a value in <see cref="GraphicsPipelineDescription.ResourceBindingModel"/>.
  /// </summary>
  ezEnum<RHIResourceBindingModel> ResourceBindingModel;

  /// <summary>
  /// Indicates whether a 0-to-1 depth range mapping is preferred. For OpenGL, this is not the default, and is not available
  /// on all systems.
  /// </summary>
  bool PreferDepthRangeZeroToOne = false;

  /// <summary>
  /// Indicates whether a bottom-to-top-increasing clip space Y direction is preferred. For Vulkan, this is not the
  /// default, and may not be available on all systems.
  /// </summary>
  bool PreferStandardClipSpaceYDirection = false;

  /// <summary>
  /// Indicates whether the main Swapchain should use an sRGB format. This value is only used in cases where the properties
  /// of the main SwapChain are not explicitly specified with a <see cref="SwapchainDescription"/>. If they are, then the
  /// value of <see cref="SwapchainDescription.ColorSrgb"/> will supercede the value specified here.
  /// </summary>
  bool SwapchainSrgbFormat = false;

  /// <summary>
  /// Constructs a new GraphicsDeviceOptions for a device with no main Swapchain.
  /// </summary>
  /// <param name="debug">Indicates whether the GraphicsDevice will support debug features, provided they are supported by
  /// the host system.</param>
  RHIGraphicsDeviceOptions(bool debug)
  {
    Debug = debug;
    HasMainSwapchain = false;
    SwapchainDepthFormat = std::nullopt;
    SyncToVerticalBlank = false;
    ResourceBindingModel = RHIResourceBindingModel::Default;
    PreferDepthRangeZeroToOne = false;
    PreferStandardClipSpaceYDirection = false;
    SwapchainSrgbFormat = false;
  }

  /// <summary>
  /// Constructs a new GraphicsDeviceOptions for a device with a main Swapchain.
  /// </summary>
  /// <param name="debug">Indicates whether the GraphicsDevice will enable debug features, provided they are supported by
  /// the host system.</param>
  /// <param name="swapchainDepthFormat">An optional <see cref="PixelFormat"/> to be used for the depth buffer of the
  /// swapchain. If this value is null, then no depth buffer will be present on the swapchain.</param>
  /// <param name="syncToVerticalBlank">Indicates whether the main Swapchain will be synchronized to the window system's
  /// vertical refresh rate.</param>
  RHIGraphicsDeviceOptions(bool debug, std::optional<ezEnum<RHIPixelFormat>> swapchainDepthFormat, bool syncToVerticalBlank)
  {
    Debug = debug;
    HasMainSwapchain = true;
    SwapchainDepthFormat = swapchainDepthFormat;
    SyncToVerticalBlank = syncToVerticalBlank;
    ResourceBindingModel = RHIResourceBindingModel::Default;
    PreferDepthRangeZeroToOne = false;
    PreferStandardClipSpaceYDirection = false;
    SwapchainSrgbFormat = false;
  }

  /// <summary>
  /// Constructs a new GraphicsDeviceOptions for a device with a main Swapchain.
  /// </summary>
  /// <param name="debug">Indicates whether the GraphicsDevice will enable debug features, provided they are supported by
  /// the host system.</param>
  /// <param name="swapchainDepthFormat">An optional <see cref="PixelFormat"/> to be used for the depth buffer of the
  /// swapchain. If this value is null, then no depth buffer will be present on the swapchain.</param>
  /// <param name="syncToVerticalBlank">Indicates whether the main Swapchain will be synchronized to the window system's
  /// vertical refresh rate.</param>
  /// <param name="resourceBindingModel">Specifies which model the rendering backend should use for binding resources.</param>
  RHIGraphicsDeviceOptions(
    bool debug,
    std::optional<ezEnum<RHIPixelFormat>> swapchainDepthFormat,
    bool syncToVerticalBlank,
    ezEnum<RHIResourceBindingModel> resourceBindingModel)
  {
    Debug = debug;
    HasMainSwapchain = true;
    SwapchainDepthFormat = swapchainDepthFormat;
    SyncToVerticalBlank = syncToVerticalBlank;
    ResourceBindingModel = resourceBindingModel;
    PreferDepthRangeZeroToOne = false;
    PreferStandardClipSpaceYDirection = false;
    SwapchainSrgbFormat = false;
  }

  /// <summary>
  /// Constructs a new GraphicsDeviceOptions for a device with a main Swapchain.
  /// </summary>
  /// <param name="debug">Indicates whether the GraphicsDevice will enable debug features, provided they are supported by
  /// the host system.</param>
  /// <param name="swapchainDepthFormat">An optional <see cref="PixelFormat"/> to be used for the depth buffer of the
  /// swapchain. If this value is null, then no depth buffer will be present on the swapchain.</param>
  /// <param name="syncToVerticalBlank">Indicates whether the main Swapchain will be synchronized to the window system's
  /// vertical refresh rate.</param>
  /// <param name="resourceBindingModel">Specifies which model the rendering backend should use for binding resources.</param>
  /// <param name="preferDepthRangeZeroToOne">Indicates whether a 0-to-1 depth range mapping is preferred. For OpenGL,
  /// this is not the default, and is not available on all systems.</param>
  RHIGraphicsDeviceOptions(
    bool debug,
    std::optional<ezEnum<RHIPixelFormat>> swapchainDepthFormat,
    bool syncToVerticalBlank,
    ezEnum<RHIResourceBindingModel> resourceBindingModel,
    bool preferDepthRangeZeroToOne)
  {
    Debug = debug;
    HasMainSwapchain = true;
    SwapchainDepthFormat = swapchainDepthFormat;
    SyncToVerticalBlank = syncToVerticalBlank;
    ResourceBindingModel = resourceBindingModel;
    PreferDepthRangeZeroToOne = preferDepthRangeZeroToOne;
    PreferStandardClipSpaceYDirection = false;
    SwapchainSrgbFormat = false;
  }

  /// <summary>
  /// Constructs a new GraphicsDeviceOptions for a device with a main Swapchain.
  /// </summary>
  /// <param name="debug">Indicates whether the GraphicsDevice will enable debug features, provided they are supported by
  /// the host system.</param>
  /// <param name="swapchainDepthFormat">An optional <see cref="PixelFormat"/> to be used for the depth buffer of the
  /// swapchain. If this value is null, then no depth buffer will be present on the swapchain.</param>
  /// <param name="syncToVerticalBlank">Indicates whether the main Swapchain will be synchronized to the window system's
  /// vertical refresh rate.</param>
  /// <param name="resourceBindingModel">Specifies which model the rendering backend should use for binding resources.</param>
  /// <param name="preferDepthRangeZeroToOne">Indicates whether a 0-to-1 depth range mapping is preferred. For OpenGL,
  /// this is not the default, and is not available on all systems.</param>
  /// <param name="preferStandardClipSpaceYDirection">Indicates whether a bottom-to-top-increasing clip space Y direction
  /// is preferred. For Vulkan, this is not the default, and is not available on all systems.</param>
  RHIGraphicsDeviceOptions(
    bool debug,
    std::optional<ezEnum<RHIPixelFormat>> swapchainDepthFormat,
    bool syncToVerticalBlank,
    ezEnum<RHIResourceBindingModel> resourceBindingModel,
    bool preferDepthRangeZeroToOne,
    bool preferStandardClipSpaceYDirection)
  {
    Debug = debug;
    HasMainSwapchain = true;
    SwapchainDepthFormat = swapchainDepthFormat;
    SyncToVerticalBlank = syncToVerticalBlank;
    ResourceBindingModel = resourceBindingModel;
    PreferDepthRangeZeroToOne = preferDepthRangeZeroToOne;
    PreferStandardClipSpaceYDirection = preferStandardClipSpaceYDirection;
    SwapchainSrgbFormat = false;
  }

  /// <summary>
  /// Constructs a new GraphicsDeviceOptions for a device with a main Swapchain.
  /// </summary>
  /// <param name="debug">Indicates whether the GraphicsDevice will enable debug features, provided they are supported by
  /// the host system.</param>
  /// <param name="swapchainDepthFormat">An optional <see cref="PixelFormat"/> to be used for the depth buffer of the
  /// swapchain. If this value is null, then no depth buffer will be present on the swapchain.</param>
  /// <param name="syncToVerticalBlank">Indicates whether the main Swapchain will be synchronized to the window system's
  /// vertical refresh rate.</param>
  /// <param name="resourceBindingModel">Specifies which model the rendering backend should use for binding resources.</param>
  /// <param name="preferDepthRangeZeroToOne">Indicates whether a 0-to-1 depth range mapping is preferred. For OpenGL,
  /// this is not the default, and is not available on all systems.</param>
  /// <param name="preferStandardClipSpaceYDirection">Indicates whether a bottom-to-top-increasing clip space Y direction
  /// is preferred. For Vulkan, this is not the default, and is not available on all systems.</param>
  /// <param name="swapchainSrgbFormat">Indicates whether the main Swapchain should use an sRGB format. This value is only
  /// used in cases where the properties of the main SwapChain are not explicitly specified with a
  /// <see cref="SwapchainDescription"/>. If they are, then the value of <see cref="SwapchainDescription.ColorSrgb"/> will
  /// supercede the value specified here.</param>
  RHIGraphicsDeviceOptions(
    bool debug,
    std::optional<ezEnum<RHIPixelFormat>> swapchainDepthFormat,
    bool syncToVerticalBlank,
    ezEnum<RHIResourceBindingModel> resourceBindingModel,
    bool preferDepthRangeZeroToOne,
    bool preferStandardClipSpaceYDirection,
    bool swapchainSrgbFormat)
  {
    Debug = debug;
    HasMainSwapchain = true;
    SwapchainDepthFormat = swapchainDepthFormat;
    SyncToVerticalBlank = syncToVerticalBlank;
    ResourceBindingModel = resourceBindingModel;
    PreferDepthRangeZeroToOne = preferDepthRangeZeroToOne;
    PreferStandardClipSpaceYDirection = preferStandardClipSpaceYDirection;
    SwapchainSrgbFormat = swapchainSrgbFormat;
  }
};
