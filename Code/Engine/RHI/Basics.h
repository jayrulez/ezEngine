#pragma once

#include <RHI/RHIDLL.h>

#include <Foundation/Algorithm/HashableStruct.h>

// Forward declarations

class ezWindowBase;

class ezRHIAdapter;
class ezRHIBindingSet;
class ezRHIBindingSetLayout;
class ezRHICommandList;
class ezRHICommandQueue;
class ezRHIDevice;
class ezRHIFence;
class ezRHIFramebuffer;
class ezRHIInstance;
class ezRHIMemory;
class ezRHIPipeline;
class ezRHIProgram;
class ezRHIQueryHeap;
class ezRHIRenderPass;
class ezRHIResource;
class ezRHIShader;
class ezRHIShaderReflection;
class ezRHISwapChain;
class ezRHIView;

class EZ_RHI_DLL ezRHIQueryInterface : public ezRefCounted
{
public:
  virtual ~ezRHIQueryInterface() = default;

  template <typename T>
  T& As()
  {
    return static_cast<T&>(*this);
  }

  template <typename T>
  const T& As() const
  {
    return static_cast<T&>(*this);
  }
};

// Basic enums
enum class ezRHIMemoryType
{
  Default,
  Upload,
  Readback
};

enum class ezRHICommandListType
{
  Graphics,
  Compute,
  Copy,
};

enum class ezRHIResourceType
{
  Unknown,
  Buffer,
  Texture,
  Sampler,
  AccelerationStructure,
};

enum class ezRHITextureType
{
  Texture1D,
  Texture2D,
  Texture3D,
};

struct EZ_RHI_DLL ezRHIResourceFormat
{
  typedef ezUInt32 StorageType;

  enum Enum
  {
    Unknown,
    R32G32B32A32_FLOAT,
    R32G32B32A32_UINT,
    R32G32B32A32_SINT,

    R32G32B32_FLOAT,
    R32G32B32_UINT,
    R32G32B32_SINT,

    R16G16B16A16_FLOAT,
    R16G16B16A16_UNORM,
    R16G16B16A16_UINT,
    R16G16B16A16_SNORM,
    R16G16B16A16_SINT,

    R32G32_FLOAT,
    R32G32_UINT,
    R32G32_SINT,
    R32G8X24_TYPELESS,    // depth (32-bit) + stencil (8-bit) + shader resource (32-bit)
    D32_FLOAT_S8X24_UINT, // depth (32-bit) + stencil (8-bit)

    R10G10B10A2_UNORM,
    R10G10B10A2_UINT,
    R11G11B10_FLOAT,
    R8G8B8A8_UNORM,
    R8G8B8A8_UNORM_SRGB,
    R8G8B8A8_UINT,
    R8G8B8A8_SNORM,
    R8G8B8A8_SINT,
    B8G8R8A8_UNORM,
    B8G8R8A8_UNORM_SRGB,
    R16G16_FLOAT,
    R16G16_UNORM,
    R16G16_UINT,
    R16G16_SNORM,
    R16G16_SINT,
    R32_TYPELESS, // depth (32-bit) + shader resource (32-bit)
    D32_FLOAT,    // depth (32-bit)
    R32_FLOAT,
    R32_UINT,
    R32_SINT,
    R24G8_TYPELESS,    // depth (24-bit) + stencil (8-bit) + shader resource (24-bit)
    D24_UNORM_S8_UINT, // depth (24-bit) + stencil (8-bit)

    R8G8_UNORM,
    R8G8_UINT,
    R8G8_SNORM,
    R8G8_SINT,
    R16_TYPELESS, // depth (16-bit) + shader resource (16-bit)
    R16_FLOAT,
    D16_UNORM, // depth (16-bit)
    R16_UNORM,
    R16_UINT,
    R16_SNORM,
    R16_SINT,

    R8_UNORM,
    R8_UINT,
    R8_SNORM,
    R8_SINT,

    BC1_UNORM,
    BC1_UNORM_SRGB,
    BC2_UNORM,
    BC2_UNORM_SRGB,
    BC3_UNORM,
    BC3_UNORM_SRGB,
    BC4_UNORM,
    BC4_SNORM,
    BC5_UNORM,
    BC5_SNORM,
    BC6H_UF16,
    BC6H_SF16,
    BC7_UNORM,
    BC7_UNORM_SRGB,

    ENUM_COUNT,

    Default = R8G8B8A8_UNORM_SRGB //RGBAUByteNormalizedsRGB
  };

  struct FormatInfo
  {
    ezUInt8 BlockSize;
    ezVec3U32 BlockExtent;
    ezUInt8 Components;
  };

  EZ_ALWAYS_INLINE static ezUInt32 GetFormatStride(ezRHIResourceFormat::Enum value);
  EZ_ALWAYS_INLINE static ezUInt32 GetBlockSize(ezRHIResourceFormat::Enum value);
  EZ_ALWAYS_INLINE static ezVec3U32 GetBlockExtent(ezRHIResourceFormat::Enum value);
  EZ_ALWAYS_INLINE static bool IsFormatUnorm(ezRHIResourceFormat::Enum value);
  EZ_ALWAYS_INLINE static bool IsFormatBlockCompressed(ezRHIResourceFormat::Enum value);
  EZ_ALWAYS_INLINE static bool IsFormatStencilSupport(ezRHIResourceFormat::Enum value);
  EZ_ALWAYS_INLINE static void GetInfo(ezUInt32 width,
    ezUInt32 height,
    ezRHIResourceFormat::Enum format,
    ezUInt32& numBytes,
    ezUInt32& rowBytes,
    ezUInt32& numRows,
    ezUInt32 alignment);
  EZ_ALWAYS_INLINE static void GetInfo(ezUInt32 width,
    ezUInt32 height,
    ezRHIResourceFormat::Enum format,
    ezUInt32& numBytes,
    ezUInt32& rowBytes);

private:
  EZ_ALWAYS_INLINE static ezUInt32 Align(ezUInt32 size, ezUInt32 alignment);
};

enum class ezRHIIndexFormat
{
  UInt,
  UShort
};

enum class ezRHISamplerFilter
{
  Anisotropic,
  MinMagMipLinear,
  ComparisonMinMagMipLinear,
};

enum class ezRHISamplerTextureAddressMode
{
  Wrap,
  Clamp
};

enum class ezRHISamplerComparisonFunc
{
  Never,
  Always,
  Less
};

enum class ezRHIViewType
{
  Unknown,
  ConstantBuffer,
  Sampler,
  Texture,
  RWTexture,
  Buffer,
  RWBuffer,
  StructuredBuffer,
  RWStructuredBuffer,
  AccelerationStructure,
  ShadingRateSource,
  RenderTarget,
  DepthStencil
};

enum class ezRHIViewDimension
{
  Unknown,
  Buffer,
  Texture1D,
  Texture1DArray,
  Texture2D,
  Texture2DArray,
  Texture2DMS,
  Texture2DMSArray,
  Texture3D,
  TextureCube,
  TextureCubeArray,
};

enum class ezRHIShaderType
{
  Unknown,
  Vertex,
  Pixel,
  Compute,
  Geometry,
  Amplification,
  Mesh,
  Library,
};

enum class ezRHIRenderPassLoadOp
{
  Load,
  Clear,
  DontCare,
};

enum class ezRHIRenderPassStoreOp
{
  Store = 0,
  DontCare,
};

enum class ezRHIRayTracingShaderGroupType
{
  General,
  TrianglesHitGroup,
  ProceduralHitGroup,
};

enum class ezRHIStencilOp
{
  Keep,
  Zero,
  Replace,
  IncrementSaturated,
  DecrementSaturated,
  Invert,
  Increment,
  Decrement,
};

enum class ezRHIComparisonFunc
{
  Never,
  Less,
  Equal,
  LessEqual,
  Greater,
  NotEqual,
  GreaterEqual,
  Always
};

enum class ezRHIBlend
{
  Zero,
  SrcAlpha,
  InvSrcAlpha,
};

enum class ezRHIBlendOp
{
  Add,
};

enum class ezRHIFillMode
{
  Wireframe,
  Solid
};

enum class ezRHICullMode
{
  None,
  Front,
  Back,
};

enum class ezRHIAccelerationStructureType
{
  TopLevel,
  BottomLevel,
};

enum class ezRHIQueryHeapType
{
  AccelerationStructureCompactedSize
};

enum class ezRHIRaytracingGeometryFlags
{
  None,
  Opaque,
  NoDuplicateAnyHitInvocation
};

enum class ezRHIShadingRate : ezUInt8
{
  ShadingRate1x1 = 0,
  ShadingRate1x2 = 0x1,
  ShadingRate2x1 = 0x4,
  ShadingRate2x2 = 0x5,
  ShadingRate2x4 = 0x6,
  ShadingRate4x2 = 0x9,
  ShadingRate4x4 = 0xa,
};

enum class ezRHIShadingRateCombiner
{
  Passthrough = 0,
  Override = 1,
  Min = 2,
  Max = 3,
  Sum = 4,
};

enum class ezRHICopyAccelerationStructureMode
{
  Clone,
  Compact,
};

enum class ezRHIPipelineType
{
  Graphics,
  Compute,
  RayTracing,
};

enum class ezRHIReturnType
{
  Unknown,
  Float,
  Uint,
  Int,
  Double,
};

enum class ezRHIShaderBlobType
{
  DXIL,
  SPIRV
};

struct ezRHIResourceState
{
  using StorageType = ezUInt32;
  enum Enum
  {
    Unknown = 0,
    Common = 1 << 0,
    VertexAndConstantBuffer = 1 << 1,
    IndexBuffer = 1 << 2,
    RenderTarget = 1 << 3,
    UnorderedAccess = 1 << 4,
    DepthStencilWrite = 1 << 5,
    DepthStencilRead = 1 << 6,
    NonPixelShaderResource = 1 << 7,
    PixelShaderResource = 1 << 8,
    IndirectArgument = 1 << 9,
    CopyDest = 1 << 10,
    CopySource = 1 << 11,
    RaytracingAccelerationStructure = 1 << 12,
    ShadingRateSource = 1 << 13,
    Present = 1 << 14,
    GenericRead =
      VertexAndConstantBuffer |
      IndexBuffer |
      CopySource |
      NonPixelShaderResource |
      PixelShaderResource |
      IndirectArgument,
    kUndefined = 1 << 15,
    Default = Unknown
  };

  struct Bits
  {
  };
};
EZ_DECLARE_FLAGS_OPERATORS(ezRHIResourceState);

template <>
struct ezCompareHelper<ezBitflags<ezRHIResourceState>>
{
  EZ_ALWAYS_INLINE bool Less(const ezBitflags<ezRHIResourceState>& a, const ezBitflags<ezRHIResourceState>& b) const { return a.GetValue() < b.GetValue(); }
  EZ_ALWAYS_INLINE bool Equal(const ezBitflags<ezRHIResourceState>& a, const ezBitflags<ezRHIResourceState>& b) const { return a == b; }
};

struct EZ_RHI_DLL ezRHIBuildAccelerationStructureFlags
{
  using StorageType = ezUInt8;
  enum Enum
  {
    None = 0,
    AllowUpdate = 1 << 0,
    AllowCompaction = 1 << 1,
    PreferFastTrace = 1 << 2,
    PreferFastBuild = 1 << 3,
    MinimizeMemory = 1 << 4,
    Default = None
  };

  struct Bits
  {
  };
};
EZ_DECLARE_FLAGS_OPERATORS(ezRHIBuildAccelerationStructureFlags);

// Basic structs

struct EZ_RHI_DLL ezRHISwapChainCreationDescription
{
  ezWindowBase* m_Window;
  ezUInt32 m_FrameCount;
  bool m_Vsync;
};

struct EZ_RHI_DLL ezRHITextureCreationDescription
{
  ezRHITextureType m_Type;
  ezUInt32 m_BindFlag;
  ezRHIResourceFormat::Enum m_Format;
  ezUInt32 m_SampleCount;
  ezInt32 m_Width;
  ezInt32 m_Height;
  ezInt32 m_Depth;
  ezInt32 m_MipLevels;
};

struct EZ_RHI_DLL ezRHIBufferCreationDescription
{
  ezUInt32 m_BindFlag;
  ezUInt32 m_BufferSize;
};

struct EZ_RHI_DLL ezRHISamplerCreationDescription
{
  ezRHISamplerFilter m_Filter;
  ezRHISamplerTextureAddressMode m_Mode;
  ezRHISamplerComparisonFunc m_ComparisonFunc;
};

struct EZ_RHI_DLL ezRHIViewDesc
{
  ezRHIViewType m_ViewType = ezRHIViewType::Unknown;
  ezRHIViewDimension m_Dimension = ezRHIViewDimension::Unknown;
  ezUInt32 m_BaseMipLevel = 0;
  ezUInt32 m_LevelCount = static_cast<ezUInt32>(-1);
  ezUInt32 m_BaseArray_layer = 0;
  ezUInt32 m_LayerCount = static_cast<ezUInt32>(-1);
  ezUInt32 m_PlaneSlice = 0;
  ezUInt32 m_Offset = 0;
  ezUInt32 m_StructureStride = 0;
  ezUInt64 m_BufferSize = static_cast<ezUInt64>(-1);
  ezRHIResourceFormat::Enum m_BufferFormat = ezRHIResourceFormat::Unknown;
  bool m_Bindless = false;
};

struct EZ_RHI_DLL ezRHIBindKeyDescription : public ezHashableStruct<ezRHIBindKeyDescription>
{
  ezRHIShaderType m_ShaderType = ezRHIShaderType::Unknown;
  ezRHIViewType m_ViewType = ezRHIViewType::Unknown;
  ezUInt32 m_Slot = 0;
  ezUInt32 m_Space = 0;
  ezUInt32 m_Count = 0;

  ezRHIBindKeyDescription() = default;

  ezRHIBindKeyDescription(ezRHIShaderType shaderType,
    ezRHIViewType viewType,
    ezUInt32 slot,
    ezUInt32 space,
    ezUInt32 count)
    : m_ShaderType{shaderType}
    , m_ViewType{viewType}
    , m_Slot{slot}
    , m_Space{space}
    , m_Count{count}
  {
  }

  /*
  bool operator==(const ezRHIBindKeyDescription& other) const {
    return CalculateHash() == other.CalculateHash();
  }*/
};

template <>
struct ezCompareHelper<ezRHIBindKeyDescription>
{
  EZ_ALWAYS_INLINE bool Less(const ezRHIBindKeyDescription& a, const ezRHIBindKeyDescription& b) const { return a.CalculateHash() < b.CalculateHash(); }
  EZ_ALWAYS_INLINE bool Equal(const ezRHIBindKeyDescription& a, const ezRHIBindKeyDescription& b) const { return a.CalculateHash() == b.CalculateHash(); }
};

struct EZ_RHI_DLL ezRHIBindingDescription
{
  ezRHIBindKeyDescription m_BindKey;
  ezRHIView* m_View;
};

struct EZ_RHI_DLL ezRHIRenderPassColorDescription
{
  ezRHIResourceFormat::Enum m_Format = ezRHIResourceFormat::Unknown;
  ezRHIRenderPassLoadOp m_LoadOp = ezRHIRenderPassLoadOp::Load;
  ezRHIRenderPassStoreOp m_StoreOp = ezRHIRenderPassStoreOp::Store;
};

struct EZ_RHI_DLL ezRHIRenderPassDepthStencilDescription
{
  ezRHIResourceFormat::Enum m_Format = ezRHIResourceFormat::Unknown;
  ezRHIRenderPassLoadOp m_DepthLoadOp = ezRHIRenderPassLoadOp::Load;
  ezRHIRenderPassStoreOp m_DepthStoreOp = ezRHIRenderPassStoreOp::Store;
  ezRHIRenderPassLoadOp m_StencilLoadOp = ezRHIRenderPassLoadOp::Load;
  ezRHIRenderPassStoreOp m_StencilStoreOp = ezRHIRenderPassStoreOp::Store;
};

struct EZ_RHI_DLL ezRHIRenderPassCreationDescription
{
  ezDynamicArray<ezRHIRenderPassColorDescription> m_Colors;
  ezRHIRenderPassDepthStencilDescription m_DepthStencil;
  ezRHIResourceFormat::Enum m_ShadingRateFormat = ezRHIResourceFormat::Unknown;
  ezUInt32 m_SampleCount = 1;
};

struct EZ_RHI_DLL ezRHIFramebufferCreationDescription
{
  ezRHIRenderPass* m_RenderPass;
  ezUInt32 m_Width;
  ezUInt32 m_Height;
  ezDynamicArray<ezRHIView*> m_Colors;
  ezRHIView* m_DepthStencil;
  ezRHIView* m_ShadingRateImage;
};

/*
struct EZ_RHI_DLL ezRHIShaderCreationDescription
{
  ezDynamicArray<ezUInt8> m_Blob;
  ezRHIShaderReflection* m_Reflection;
  ezString m_ShaderPath;
  ezString m_EntryPoint;
  ezRHIShaderType m_Type;
  ezString m_Model;
  ezMap<ezString, ezString> m_Defines;

  ezRHIShaderCreationDescription(const ezString& shaderPath, const ezString& entryPoint, ezRHIShaderType type, const ezString& model)
    : m_ShaderPath{shaderPath}
    , m_EntryPoint{entryPoint}
    , m_Type{type}
    , m_Model{model}
  {
  }
};
*/

struct EZ_RHI_DLL ezRHIShaderCreationDescription
{
  ezDynamicArray<ezUInt8> m_Blob;
  ezRHIShaderReflection* m_Reflection;
  ezString m_EntryPoint;
  ezRHIShaderType m_Type;
  ezString m_Model;
  ezMap<ezString, ezString> m_Defines;

  ezRHIShaderCreationDescription(const ezString& shaderPath, const ezString& entryPoint, ezRHIShaderType type, const ezString& model, ezDynamicArray<ezUInt8> blob, ezRHIShaderReflection* reflection)
    : m_EntryPoint{entryPoint}
    , m_Type{type}
    , m_Model{model}
    , m_Blob{blob}
    , m_Reflection{reflection}
  {
  }
};

struct EZ_RHI_DLL ezRHIInputLayoutDescription
{
  ezUInt32 m_Slot = 0;
  ezString m_SemanticName;
  ezRHIResourceFormat::Enum m_Format = ezRHIResourceFormat::Unknown;
  ezUInt32 m_Stride = 0;
};

struct EZ_RHI_DLL ezRHIStencilOpDescription
{
  ezRHIStencilOp m_FailOp = ezRHIStencilOp::Keep;
  ezRHIStencilOp m_DepthFailOp = ezRHIStencilOp::Keep;
  ezRHIStencilOp m_PassOp = ezRHIStencilOp::Keep;
  ezRHIComparisonFunc m_Func = ezRHIComparisonFunc::Always;
};

struct EZ_RHI_DLL ezRHIDepthStencilDescription
{
  bool m_DepthTestEnable = true;
  ezRHIComparisonFunc m_DepthFunc = ezRHIComparisonFunc::Less;
  bool m_DepthWriteEnable = true;
  bool m_DepthBoundsTestEnable = false;
  bool m_StencilEnable = false;
  ezUInt8 m_StencilReadMask = 0xff;
  ezUInt8 m_StencilWriteMask = 0xff;
  ezRHIStencilOpDescription m_FrontFace = {};
  ezRHIStencilOpDescription m_BackFace = {};
};

struct EZ_RHI_DLL ezRHIBlendDescription
{
  bool m_BlendEnable = false;
  ezRHIBlend m_BlendSrc;
  ezRHIBlend m_BlendDest;
  ezRHIBlendOp m_BlendOp;
  ezRHIBlend m_BlendSrcAlpha;
  ezRHIBlend m_BlendDestApha;
  ezRHIBlendOp m_BlendOpAlpha;
};

struct EZ_RHI_DLL ezRHIRasterizerDesc
{
  ezRHIFillMode m_FillMode = ezRHIFillMode::Solid;
  ezRHICullMode m_CullMode = ezRHICullMode::Back;
  ezUInt32 m_DepthBias = 0;
};

struct EZ_RHI_DLL ezRHIGraphicsPipelineCreationDescription
{
  ezRHIProgram* m_Program;
  ezRHIBindingSetLayout* m_Layout;
  ezDynamicArray<ezRHIInputLayoutDescription> m_Input;
  ezRHIRenderPass* m_RenderPass;
  ezRHIDepthStencilDescription m_DepthStencilDesc;
  ezRHIBlendDescription m_BlendDesc;
  ezRHIRasterizerDesc m_RasterizerDesc;
};

struct EZ_RHI_DLL ezRHIComputePipelineCreationDescription
{
  ezRHIProgram* m_Program;
  ezRHIBindingSetLayout* m_Layout;
};

struct EZ_RHI_DLL ezRHIRayTracingShaderGroup
{
  ezRHIRayTracingShaderGroupType m_Type = ezRHIRayTracingShaderGroupType::General;
  ezUInt64 m_General = 0;
  ezUInt64 m_ClosestHit = 0;
  ezUInt64 m_AnyHit = 0;
  ezUInt64 m_Intersection = 0;
};

struct EZ_RHI_DLL ezRHIRayTracingPipelineCreationDescription
{
  ezRHIProgram* m_Program;
  ezRHIBindingSetLayout* m_Layout;
  ezDynamicArray<ezRHIRayTracingShaderGroup> m_Groups;
};

struct EZ_RHI_DLL ezRHIRaytracingASPrebuildInfo
{
  ezUInt64 m_AccelerationStructureSize = 0;
  ezUInt64 m_BuildScratchDataSize = 0;
  ezUInt64 m_UpdateScratchDataSize = 0;
};

struct EZ_RHI_DLL ezRHIRaytracingGeometryDescription
{
  ezRHIBufferCreationDescription m_Vertex;
  ezRHIBufferCreationDescription m_Index;
  ezRHIRaytracingGeometryFlags flags = ezRHIRaytracingGeometryFlags::None;
};

struct EZ_RHI_DLL ezRHIRayTracingShaderTable
{
  ezRHIResource* m_Resource;
  ezUInt64 m_Offset;
  ezUInt64 m_Size;
  ezUInt64 m_Stride;
};

struct EZ_RHI_DLL ezRHIRayTracingShaderTables
{
  ezRHIRayTracingShaderTable m_Raygen;
  ezRHIRayTracingShaderTable m_Miss;
  ezRHIRayTracingShaderTable m_Hit;
  ezRHIRayTracingShaderTable m_Callable;
};

struct EZ_RHI_DLL ezRHIResourceBarrierDescription
{
  ezRHIResource* m_Resource;
  ezBitflags<ezRHIResourceState> m_StateBefore;
  ezBitflags<ezRHIResourceState> m_StateAfter;
  ezUInt32 m_BaseMipLevel = 0;
  ezUInt32 m_LevelCount = 1;
  ezUInt32 m_BaseArrayLayer = 0;
  ezUInt32 m_LayerCount = 1;
};

struct EZ_RHI_DLL ezRHIResourceBindingDescription
{
  ezString m_Name;
  ezRHIViewType m_Type;
  ezUInt32 m_Slot;
  ezUInt32 m_Space;
  ezUInt32 m_Count;
  ezRHIViewDimension m_Dimension;
  ezRHIReturnType m_ReturnType;
  uint32_t StructureStride;
};

struct EZ_RHI_DLL ezRHIClearDescription
{
  ezDynamicArray<ezColor> m_Colors;
  float m_Depth = 1.0f;
  ezUInt8 m_Stencil = 0;
};

struct EZ_RHI_DLL ezRHITextureOffset
{
  ezInt32 x;
  ezInt32 y;
  ezInt32 z;
};

struct EZ_RHI_DLL ezRHITextureExtent3D
{
  ezUInt32 m_Width;
  ezUInt32 m_Height;
  ezUInt32 m_Depth;
};

struct EZ_RHI_DLL ezRHIBufferToTextureCopyRegion
{
  ezUInt64 m_BufferOffset;
  ezUInt32 m_BufferRowPitch;
  ezUInt32 m_TextureMipLevel;
  ezUInt32 m_TextureArrayLayer;
  ezRHITextureOffset m_TextureOffset;
  ezRHITextureExtent3D m_TextureExtent;
};

struct EZ_RHI_DLL ezRHITextureCopyRegion
{
  ezRHITextureExtent3D m_Extent;
  ezUInt32 m_SrcMipLevel;
  ezUInt32 m_SrcArrayLayer;
  ezRHITextureOffset m_SrcOffset;
  ezUInt32 m_DstMipLevel;
  ezUInt32 m_DstArrayLayer;
  ezRHITextureOffset m_DstOffset;
};

struct EZ_RHI_DLL ezRHIBufferCopyRegion
{
  ezUInt64 m_SrcOffset;
  ezUInt64 m_DstOffset;
  ezUInt64 m_NumBytes;
};

#include <RHI/Basics_inl.h>
