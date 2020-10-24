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

/// <summary>
/// Enumerates the optional features supported by a given <see cref="GraphicsDevice"/>.
/// </summary>
class RHIGraphicsDeviceFeatures
{
public:
  /// <summary>
  /// Indicates whether Compute Shaders can be used.
  /// </summary>
  bool ComputeShaderSupported() const { return ComputeShader; }

  /// <summary>
  /// Indicates whether Geometry Shaders can be used.
  /// </summary>
  bool GeometryShaderSupported() const { return GeometryShader; }

  /// <summary>
  /// Indicates whether Tessellation Shaders can be used.
  /// </summary>
  bool TessellationShadersSupported() const { return TessellationShaders; }

  /// <summary>
  /// Indicates whether multiple independent viewports can be set simultaneously.
  /// If this is not supported, then only the first Viewport index will be used for all render outputs.
  /// </summary>
  bool MultipleViewportsSupported() const { return MultipleViewports; }

  /// <summary>
  /// Indicates whether <see cref="SamplerDescription.LodBias"/> can be non-zero.
  /// If false, it is an error to attempt to use a non-zero bias value.
  /// </summary>
  bool SamplerLodBiasSupported() const { return SamplerLodBias; }

  /// <summary>
  /// Indicates whether a non-zero "vertexStart" value can be used in
  /// <see cref="CommandList.Draw(uint, uint, uint, uint)"/> and
  /// <see cref="CommandList.DrawIndexed(uint, uint, uint, int, uint)"/>.
  /// </summary>
  bool DrawBaseVertexSupported() const { return DrawBaseVertex; }

  /// <summary>
  /// Indicates whether a non-zero "instanceStart" value can be used in
  /// <see cref="CommandList.Draw(uint, uint, uint, uint)"/> and
  /// <see cref="CommandList.DrawIndexed(uint, uint, uint, int, uint)"/>.
  /// </summary>
  bool DrawBaseInstanceSupported() const { return DrawBaseInstance; }

  /// <summary>
  /// Indicates whether indirect draw commands can be issued.
  /// </summary>
  bool DrawIndirectSupported() const { return DrawIndirect; }

  /// <summary>
  /// Indicates whether indirect draw structures stored in an Indirect DeviceBuffer can contain
  /// a non-zero FirstInstance value.
  /// </summary>
  bool DrawIndirectBaseInstanceSupported() const { return DrawIndirectBaseInstance; }

  /// <summary>
  /// Indicates whether <see cref="PolygonFillMode.Wireframe"/> is supported.
  /// </summary>
  bool FillModeWireframeSupported() const { return FillModeWireframe; }

  /// <summary>
  /// Indicates whether <see cref="SamplerFilter.Anisotropic"/> is supported.
  /// </summary>
  bool SamplerAnisotropySupported() const { return SamplerAnisotropy; }

  /// <summary>
  /// Indicates whether <see cref="RasterizerStateDescription.DepthClipEnabled"/> can be set to false.
  /// </summary>
  bool DepthClipDisableSupported() const { return DepthClipDisable; }

  /// <summary>
  /// Indicates whether a <see cref="Texture"/> can be created with <see cref="TextureType.Texture1D"/>.
  /// </summary>
  bool Texture1DSupported() const { return Texture1D; }

  /// <summary>
  /// Indicates whether a <see cref="BlendStateDescription"/> can be used which has multiple different
  /// <see cref="BlendAttachmentDescription"/> values for each attachment. If false, all attachments must have the same
  /// blend state.
  /// </summary>
  bool IndependentBlendSupported() const { return IndependentBlend; }

  /// <summary>
  /// Indicates whether <see cref="BufferUsage.StructuredBufferReadOnly"/> and
  /// <see cref="BufferUsage.StructuredBufferReadWrite"/> can be used. If false, structured buffers cannot be created.
  /// </summary>
  bool StructuredBufferSupported() const { return StructuredBuffer; }

  /// <summary>
  /// Indicates whether a <see cref="TextureView"/> can be created which does not view the full set of mip levels and array
  /// layers contained in its target Texture, or uses a different <see cref="PixelFormat"/> from the underlying Texture.
  /// </summary>
  bool SubsetTextureViewSupported() const { return SubsetTextureView; }

  /// <summary>
  /// Indicates whether <see cref="CommandList"/> instances created with this device support the
  /// <see cref="CommandList.PushDebugGroup(string)"/>, <see cref="CommandList.PopDebugGroup"/>, and
  /// <see cref="CommandList.InsertDebugMarker(string)"/> methods. If not, these methods will have no effect.
  /// </summary>
  bool CommandListDebugMarkersSupported() const { return CommandListDebugMarkers; }

  /// <summary>
  /// Indicates whether uniform and structured buffers can be bound with an offset and a size. If false, buffer resources
  /// must be bound with their full range.
  /// </summary>
  bool BufferRangeBindingSupported() const { return BufferRangeBinding; }

  /// <summary>
  /// Indicates whether 64-bit floating point integers can be used in shaders.
  /// </summary>
  bool ShaderFloat64Supported() const { return ShaderFloat64Supported; }

  RHIGraphicsDeviceFeatures(bool computeShader,
    bool geometryShader,
    bool tessellationShaders,
    bool multipleViewports,
    bool samplerLodBias,
    bool drawBaseVertex,
    bool drawBaseInstance,
    bool drawIndirect,
    bool drawIndirectBaseInstance,
    bool fillModeWireframe,
    bool samplerAnisotropy,
    bool depthClipDisable,
    bool texture1D,
    bool independentBlend,
    bool structuredBuffer,
    bool subsetTextureView,
    bool commandListDebugMarkers,
    bool bufferRangeBinding,
    bool shaderFloat64)
  {

    ComputeShader = computeShader;
    GeometryShader = geometryShader;
    TessellationShaders = tessellationShaders;
    MultipleViewports = multipleViewports;
    SamplerLodBias = samplerLodBias;
    DrawBaseVertex = drawBaseVertex;
    DrawBaseInstance = drawBaseInstance;
    DrawIndirect = drawIndirect;
    DrawIndirectBaseInstance = drawIndirectBaseInstance;
    FillModeWireframe = fillModeWireframe;
    SamplerAnisotropy = samplerAnisotropy;
    DepthClipDisable = depthClipDisable;
    Texture1D = texture1D;
    IndependentBlend = independentBlend;
    StructuredBuffer = structuredBuffer;
    SubsetTextureView = subsetTextureView;
    CommandListDebugMarkers = commandListDebugMarkers;
    BufferRangeBinding = bufferRangeBinding;
    ShaderFloat64 = shaderFloat64;
  }

private:
  bool ComputeShader = false;
  bool GeometryShader = false;
  bool TessellationShaders = false;
  bool MultipleViewports = false;
  bool SamplerLodBias = false;
  bool DrawBaseVertex = false;
  bool DrawBaseInstance = false;
  bool DrawIndirect = false;
  bool DrawIndirectBaseInstance = false;
  bool FillModeWireframe = false;
  bool SamplerAnisotropy = false;
  bool DepthClipDisable = false;
  bool Texture1D = false;
  bool IndependentBlend = false;
  bool StructuredBuffer = false;
  bool SubsetTextureView = false;
  bool CommandListDebugMarkers = false;
  bool BufferRangeBinding = false;
  bool ShaderFloat64 = false;
};
