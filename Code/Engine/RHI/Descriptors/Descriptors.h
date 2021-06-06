#pragma once

#include <Foundation/Algorithm/HashableStruct.h>
#include <Foundation/Containers/HybridArray.h>
#include <Foundation/Containers/StaticArray.h>
#include <Foundation/Math/Color.h>
#include <Foundation/Types/RefCounted.h>
#include <RHI/RHIDLL.h>
#include <RHI/Resources/ResourceFormats.h>
//#include <RendererFoundation/Shader/ShaderByteCode.h>
#include <Texture/Image/ImageEnums.h>

class ezWindowBase;

struct ezRHISwapChainDesc : public ezHashableStruct<ezRHISwapChainDesc>
{
  ezWindowBase* Window = nullptr;
  ezUInt32 Width = 0;
  ezUInt32 Height = 0;
  ezUInt32 BufferCount = 2;
  ezRHIResourceFormat::Enum Format = ezRHIResourceFormat::R10G10B10A2_UNORM;
  bool FullScreen = false;
  bool VSync = true;
  ezColor ClearColor = ezColor::Black;
};

struct ezRHIDeviceDesc
{
  ezRHISwapChainDesc PrimarySwapChainDesc;
  bool DebugDevice = false;
  bool CreatePrimarySwapChain = true;
};

/*
struct ezGALShaderCreationDescription : public ezHashableStruct<ezGALShaderCreationDescription>
{
  ezGALShaderCreationDescription();
  ~ezGALShaderCreationDescription();

  bool HasByteCodeForStage(ezGALShaderStage::Enum Stage) const;

  ezScopedRefPointer<ezGALShaderByteCode> m_ByteCodes[ezGALShaderStage::ENUM_COUNT];
};
*/

struct ezRHIRenderTargetBlendDescription : public ezHashableStruct<ezRHIRenderTargetBlendDescription>
{
  bool BlendEnabled = false;
  ezRHIBlend::Enum SrcBlend = ezRHIBlend::SrcAlpha;
  ezRHIBlend::Enum DestBlend = ezRHIBlend::InvSrcAlpha;
  ezRHIBlendOp::Enum BlendOp = ezRHIBlendOp::Add;
  ezRHIBlend::Enum SrcBlendAlpha = ezRHIBlend::One;
  ezRHIBlend::Enum DestBlendAlpha = ezRHIBlend::One;
  ezRHIBlendOp::Enum BlendOpAlpha = ezRHIBlendOp::Add;
  ezRHIColorWriteMask::Enum RenderTargetWriteMask = ezRHIColorWriteMask::All;
};

struct ezRHIBlendStateCreationDescription : public ezHashableStruct<ezRHIBlendStateCreationDescription>
{
  bool AlphaToCoverageEnable = false;
  bool IndependentBlendEnable = false;
  ezRHIRenderTargetBlendDescription m_RenderTargetBlendDescriptions[EZ_RHI_MAX_RENDERTARGET_COUNT];
};

struct ezRHIStencilOpDescription : public ezHashableStruct<ezRHIStencilOpDescription>
{
  ezEnum<ezRHIStencilOp> StencilFailOp = ezRHIStencilOp::Keep;
  ezEnum<ezRHIStencilOp> StencilDepthFailOp = ezRHIStencilOp::Keep;
  ezEnum<ezRHIStencilOp> StencilPassOp = ezRHIStencilOp::Keep;

  ezEnum<ezRHICompareFunc> StencilFunc = ezRHICompareFunc::Never;
};

struct ezRHIDepthStencilStateCreationDescription : public ezHashableStruct<ezRHIDepthStencilStateCreationDescription>
{
  ezRHIStencilOpDescription FrontFace;
  ezRHIStencilOpDescription BackFace;

  ezEnum<ezRHICompareFunc> DepthFunc = ezRHICompareFunc::Never;
  ezRHIDepthWriteMask::Enum DepthWriteMask = ezRHIDepthWriteMask::Zero;

  bool DepthEnable = false;
  bool StencilEnable = false;
  ezUInt8 StencilReadMask = 0xFF;
  ezUInt8 StencilWriteMask = 0xFF;
};

struct ezRHIRasterizerStateCreationDescription : public ezHashableStruct<ezRHIRasterizerStateCreationDescription>
{
  ezEnum<ezRHIFillMode> FillMode = ezRHIFillMode::Solid;
  ezEnum<ezRHICullMode> CullMode = ezRHICullMode::None;
  bool FrontCounterClockwise = false;
  ezInt32 DepthBias = 0;
  float DepthBiasClamp = 0.0f;
  float SlopeScaledDepthBias = 0.0f;
  bool DepthClipEnable = false;
  bool MultisampleEnable = false;
  bool AntialiasedLineEnable = false;
  bool ConservativeRasterizationEnable = false;
  ezUInt32 ForcedSampleCount = 0;
};

struct ezRHISamplerStateDesc : public ezHashableStruct<ezRHISamplerStateDesc>
{
  ezEnum<ezRHITextureFilterMode> Filter = ezRHITextureFilterMode::MinMagMipPoint;
  ezEnum<ezRHITextureAddressMode> AddressU = ezRHITextureAddressMode::Clamp;
  ezEnum<ezRHITextureAddressMode> AddressV = ezRHITextureAddressMode::Clamp;
  ezEnum<ezRHITextureAddressMode> AddressW = ezRHITextureAddressMode::Clamp;
  float MipLODBias = 0.0f;
  ezUInt32 MaxAnisotropy = 0;
  ezEnum<ezRHICompareFunc> ComparisonFunc = ezRHICompareFunc::Never;
  ezColor BorderColor = ezColor::Black;
  float MinLOD = 0.0f;
  float MaxLOD = ezMath::MaxValue<float>();
};

struct ezRHIGPUBufferDesc : public ezHashableStruct<ezRHIGPUBufferDesc>
{
  ezUInt32 ByteWidth = 0;
  ezRHIResourceUsage::Enum Usage = ezRHIResourceUsage::Default;
  ezUInt32 BindFlags = 0;
  ezUInt32 CPUAccessFlags = 0;
  ezUInt32 MiscFlags = 0;
  ezUInt32 StructureByteStride = 0;                                // needed for typed and structured buffer types!
  ezRHIResourceFormat::Enum Format = ezRHIResourceFormat::UNKNOWN; // only needed for typed buffer!
};

struct ezRHITextureDesc : public ezHashableStruct<ezRHITextureDesc>
{
  ezRHITextureType::Enum Type = ezRHITextureType::Texture2D;
  ezUInt32 Width = 0;
  ezUInt32 Height = 0;
  ezUInt32 Depth = 0;
  ezUInt32 ArraySize = 1;
  ezUInt32 MipLevels = 1;
  ezRHIResourceFormat::Enum Format = ezRHIResourceFormat::UNKNOWN;
  ezRHIMSAASampleCount::Enum SampleCount = ezRHIMSAASampleCount::None;
  ezRHIResourceUsage::Enum Usage = ezRHIResourceUsage::Default;
  ezBitflags<ezRHIBindFlags> BindFlags = ezRHIBindFlags::Default;
  ezBitflags<ezRHICPUAccessFlags> CPUAccessFlags = ezRHICPUAccessFlags::Default;
  ezBitflags<ezRHIResourceMiscFlags> MiscFlags = ezRHIResourceMiscFlags::Default;
  ezRHIClearValue Clear = {};
  ezRHIImageLayout::Enum Layout = ezRHIImageLayout::ShaderResource;
};

struct ezRHIResourceViewDesc : public ezHashableStruct<ezRHIResourceViewDesc>
{

};

struct ezRHIRenderTargetViewDesc : public ezHashableStruct<ezRHIRenderTargetViewDesc>
{
};

struct ezRHIUnorderedAccessViewDesc : public ezHashableStruct<ezRHIUnorderedAccessViewDesc>
{
};

struct ezRHIViewport
{
  float TopLeftX = 0.0f;
  float TopLeftY = 0.0f;
  float Width = 0.0f;
  float Height = 0.0f;
  float MinDepth = 0.0f;
  float MaxDepth = 1.0f;
};

struct ezRHIInputLayout
{
  static const ezUInt32 AppendAlignedElement = 0xffffffff; // automatically figure out AlignedByteOffset depending on Format

  struct Element
  {
    ezString SemanticName;
    ezUInt32 SemanticIndex = 0;
    ezRHIResourceFormat::Enum Format = ezRHIResourceFormat::UNKNOWN;
    ezUInt32 InputSlot = 0;
    ezUInt32 AlignedByteOffset = AppendAlignedElement;
    ezRHIInputClassification::Enum InputSlotClass = ezRHIInputClassification::PerVertexData;
  };
  ezDynamicArray<Element> Elements;
};

struct ezRHIGPUQueryHeapDesc
{
  ezRHIGPUQueryType::Enum Type = ezRHIGPUQueryType::Timestamp;
  ezUInt32 QueryCount = 0;
};

struct ezRHIPipelineStateDesc
{
  const ezRHIShader* VertexShader = nullptr;
  const ezRHIShader* PixelShader = nullptr;
  const ezRHIShader* HullShader = nullptr;
  const ezRHIShader* DomainShader = nullptr;
  const ezRHIShader* GeometryShader = nullptr;
  const ezRHIShader* MeshShader = nullptr;
  const ezRHIShader* AmplificationShader = nullptr;
  const ezRHIBlendState* BlendState = nullptr;
  const ezRHIRasterizerState* RasterizerState = nullptr;
  const ezRHIDepthStencilState* DepthStencilState = nullptr;
  const ezRHIInputLayout* InputLayout = nullptr;
  ezRHIPrimitiveTopology::Enum PrimitiveTopology = ezRHIPrimitiveTopology::TriangleList;
  ezUInt32 SampleMask = 0xFFFFFFFF;
};

struct ezRHIGPUBarrierDesc
{
  enum class BarrierType
  {
    Memory, // UAV accesses
    Image,  // image layout transition
    Buffer, // buffer state transition
  } Type = BarrierType::Memory;

  struct MemoryDesc
  {
    const ezRHIResourceBase* Resource;
  };
  struct ImageDesc
  {
    const ezRHITexture* Texture;
    ezRHIImageLayout::Enum LayoutBefore;
    ezRHIImageLayout::Enum LayoutAfter;
    ezInt32 Mip;
    ezInt32 Slice;
  };
  struct BufferDesc
  {
    const ezRHIGPUBuffer* Buffer;
    ezRHIBufferState::Enum StateBefore;
    ezRHIBufferState::Enum StateAfter;
  };
  union
  {
    MemoryDesc Memory;
    ImageDesc Image;
    BufferDesc Buffer;
  };

  static ezRHIGPUBarrierDesc CreateMemoryBarrier(const ezRHIResourceBase* resource = nullptr)
  {
    ezRHIGPUBarrierDesc barrier;
    barrier.Type = BarrierType::Memory;
    barrier.Memory.Resource = resource;
    return barrier;
  }
  static ezRHIGPUBarrierDesc CreateImageBarrier(const ezRHITexture* texture, ezRHIImageLayout::Enum before, ezRHIImageLayout::Enum after,
    ezInt32 mip = -1, ezInt32 slice = -1)
  {
    ezRHIGPUBarrierDesc barrier;
    barrier.Type = BarrierType::Image;
    barrier.Image.Texture = texture;
    barrier.Image.LayoutBefore = before;
    barrier.Image.LayoutAfter = after;
    barrier.Image.Mip = mip;
    barrier.Image.Slice = slice;
    return barrier;
  }
  static ezRHIGPUBarrierDesc CreateBufferBarrier(const ezRHIGPUBuffer* buffer, ezRHIBufferState::Enum before, ezRHIBufferState::Enum after)
  {
    ezRHIGPUBarrierDesc barrier;
    barrier.Type = BarrierType::Buffer;
    barrier.Buffer.Buffer = buffer;
    barrier.Buffer.StateBefore = before;
    barrier.Buffer.StateAfter = after;
    return barrier;
  }
};

struct ezRHIRenderPassAttachment
{
  enum class AttachmentType
  {
    RenderTarget,
    DepthStencil,
    Resolve,
    ShadingRateSource
  } Type = AttachmentType::RenderTarget;

  enum class LoadOperationType
  {
    Load,
    Clear,
    DontCare,
  } LoadOperation = LoadOperationType::Load;

  const ezRHITexture* Texture = nullptr;
  ezInt32 Subresource = -1;
  enum StoreOperationType
  {
    Store,
    DontCare,
  } StoreOperation = StoreOperationType::Store;

  ezRHIImageLayout::Enum InitialLayout = ezRHIImageLayout::Undefined; // layout before the render pass
  ezRHIImageLayout::Enum SubpassLayout = ezRHIImageLayout::Undefined; // layout within the render pass
  ezRHIImageLayout::Enum FinalLayout = ezRHIImageLayout::Undefined;   // layout after the render pass

  static ezRHIRenderPassAttachment RenderTarget(
    const ezRHITexture* resource = nullptr,
    LoadOperationType loadOperation = LoadOperationType::Load,
    StoreOperationType storeOperation = StoreOperationType::Store,
    ezRHIImageLayout::Enum initialLayout = ezRHIImageLayout::ShaderResource,
    ezRHIImageLayout::Enum subpassLayout = ezRHIImageLayout::RenderTarget,
    ezRHIImageLayout::Enum finalLayout = ezRHIImageLayout::ShaderResource)
  {
    ezRHIRenderPassAttachment attachment;
    attachment.Type = AttachmentType::RenderTarget;
    attachment.Texture = resource;
    attachment.LoadOperation = loadOperation;
    attachment.StoreOperation = storeOperation;
    attachment.InitialLayout = initialLayout;
    attachment.SubpassLayout = subpassLayout;
    attachment.FinalLayout = finalLayout;
    return attachment;
  }

  static ezRHIRenderPassAttachment DepthStencil(
    const ezRHITexture* resource = nullptr,
    LoadOperationType loadOperation = LoadOperationType::Load,
    StoreOperationType storeOperation = StoreOperationType::Store,
    ezRHIImageLayout::Enum initialLayout = ezRHIImageLayout::DepthStencil,
    ezRHIImageLayout::Enum subpassLayout = ezRHIImageLayout::DepthStencil,
    ezRHIImageLayout::Enum finalLayout = ezRHIImageLayout::DepthStencil)
  {
    ezRHIRenderPassAttachment attachment;
    attachment.Type = AttachmentType::DepthStencil;
    attachment.Texture = resource;
    attachment.LoadOperation = loadOperation;
    attachment.StoreOperation = storeOperation;
    attachment.InitialLayout = initialLayout;
    attachment.SubpassLayout = subpassLayout;
    attachment.FinalLayout = finalLayout;
    return attachment;
  }

  static ezRHIRenderPassAttachment Resolve(
    const ezRHITexture* resource = nullptr,
    ezRHIImageLayout::Enum initialLayout = ezRHIImageLayout::ShaderResource,
    ezRHIImageLayout::Enum finalLayout = ezRHIImageLayout::ShaderResource)
  {
    ezRHIRenderPassAttachment attachment;
    attachment.Type = AttachmentType::Resolve;
    attachment.Texture = resource;
    attachment.InitialLayout = initialLayout;
    attachment.FinalLayout = finalLayout;
    return attachment;
  }

  static ezRHIRenderPassAttachment ShadingRateSource(
    const ezRHITexture* resource = nullptr,
    ezRHIImageLayout::Enum initialLayout = ezRHIImageLayout::ShadingRateSrc,
    ezRHIImageLayout::Enum finalLayout = ezRHIImageLayout::ShadingRateSrc)
  {
    ezRHIRenderPassAttachment attachment;
    attachment.Type = AttachmentType::ShadingRateSource;
    attachment.Texture = resource;
    attachment.InitialLayout = initialLayout;
    attachment.SubpassLayout = ezRHIImageLayout::ShadingRateSrc;
    attachment.FinalLayout = finalLayout;
    return attachment;
  }
};

struct ezRHIRenderPassFlags
{
  typedef ezUInt8 StorageType;
  enum Enum
  {
    Empty = 0,
    AllowUAVWrites = 1 << 0,
  };
  struct Bits
  {
  };
};
EZ_DECLARE_FLAGS_OPERATORS(ezRHIRenderPassFlags);

struct ezRHIRenderPassDesc
{
  ezBitflags<ezRHIRenderPassFlags> Flags = ezRHIRenderPassFlags::Empty;
  ezDynamicArray<ezRHIRenderPassAttachment> Attachments;
};

struct ezRHIIndirectDrawArgsInstanced
{
  ezUInt32 VertexCountPerInstance = 0;
  ezUInt32 InstanceCount = 0;
  ezUInt32 StartVertexLocation = 0;
  ezUInt32 StartInstanceLocation = 0;
};
struct ezRHIIndirectDrawArgsIndexedInstanced
{
  ezUInt32 IndexCountPerInstance = 0;
  ezUInt32 InstanceCount = 0;
  ezUInt32 StartIndexLocation = 0;
  ezInt32 BaseVertexLocation = 0;
  ezUInt32 StartInstanceLocation = 0;
};
struct ezRHIIndirectDispatchArgs
{
  ezUInt32 ThreadGroupCountX = 0;
  ezUInt32 ThreadGroupCountY = 0;
  ezUInt32 ThreadGroupCountZ = 0;
};
struct ezRHISubresourceData
{
  const void* pSysMem = nullptr;
  ezUInt32 SysMemPitch = 0;
  ezUInt32 SysMemSlicePitch = 0;
};
struct ezRHIRect
{
  ezInt32 Left = 0;
  ezInt32 Top = 0;
  ezInt32 Right = 0;
  ezInt32 Bottom = 0;
};

struct ezRHIMappingFlags
{
  typedef ezUInt8 StorageType;
  enum Enum
  {
    Empty = 0,
    Read = 1 << 0,
    Write = 1 << 1
  };
  struct Bits
  {
  };
};
EZ_DECLARE_FLAGS_OPERATORS(ezRHIMappingFlags);

struct ezRHIMapping
{
  ezBitflags<ezRHIMappingFlags> Flags = ezRHIMappingFlags::Empty;
  ezUInt64 Offset = 0;
  ezUInt64 Size = 0;
  ezUInt32 RowPitch = 0; // output
  void* Data = nullptr;  // output
};

// Ray tracing

struct ezRHIRayTracingASLevel
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    Bottom,
    TOp,

    ENUM_COUNT,
    Default = Bottom
  };
};

struct ezRHIRayTracingASFlags
{
  typedef ezUInt8 StorageType;
  enum Enum
  {
    Empty = 0,
    AllowUpdate = 1 << 0,
    AllowCompaction = 1 << 1,
    PreferFastTrace = 1 << 2,
    PreferFastBuild = 1 << 3,
    MinimizeMemory = 1 << 4
  };
  struct Bits
  {
  };
};
EZ_DECLARE_FLAGS_OPERATORS(ezRHIRayTracingASFlags);

struct ezRHIBottomLevelASGeometryFlags
{
  typedef ezUInt8 StorageType;
  enum Enum
  {
    Empty = 0,
    Opaque = 1 << 0,
    NoDuplicateAnyHitInvocation = 1 << 1,
    UseTransform = 1 << 2
  };
  struct Bits
  {
  };
};
EZ_DECLARE_FLAGS_OPERATORS(ezRHIBottomLevelASGeometryFlags);

struct ezRHIBottomLevelASGeometryType
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    Triangles,
    ProceduralAABBs,

    ENUM_COUNT
  };
};

struct ezRHIBottomLevelASTrianglesDesc
{
  ezRHIGPUBuffer* VertexBuffer;
  ezRHIGPUBuffer* IndexBuffer;
  ezUInt32 IndexCount = 0;
  ezUInt32 IndexOffset = 0;
  ezUInt32 VertexCount = 0;
  ezUInt32 VertexByteOffset = 0;
  ezUInt32 VertexStride = 0;
  ezRHIIndexType::Enum IndexFormat = ezRHIIndexType::UInt;
  ezRHIResourceFormat::Enum VertexFormat = ezRHIResourceFormat::R32G32B32_FLOAT;
  ezRHIGPUBuffer* Transform3x4Buffer;
  ezUInt32 Transform3x4BufferOffset = 0;
};

struct ezRHIBottomLevelASProceduralAABBsDesc
{
  ezRHIGPUBuffer* AABBBuffer;
  ezUInt32 Offset = 0;
  ezUInt32 Count = 0;
  ezUInt32 Stride = 0;
};

struct ezRHIBottomLevelASGeometryDesc
{
  ezBitflags<ezRHIBottomLevelASGeometryFlags> Flags = ezRHIBottomLevelASGeometryFlags::Empty;
  ezRHIBottomLevelASGeometryType::Enum Type = ezRHIBottomLevelASGeometryType::Triangles;
  ezRHIBottomLevelASTrianglesDesc Triangles;
  ezRHIBottomLevelASProceduralAABBsDesc AABBs;
};

struct ezRHIBottomLevelASDesc
{
  ezDynamicArray<ezRHIBottomLevelASGeometryDesc> Geometries;
};

struct ezRHITopLevelASInstanceFlags
{
  typedef ezUInt8 StorageType;
  enum Enum
  {
    Empty = 0,
    TriangleCullDisable = 1 << 0,
    TriangleFrontCounterClockwise = 1 << 1,
    ForceOpaque = 1 << 2,
    ForceNonOpaque = 1 << 3
  };
  struct Bits
  {
  };
};
EZ_DECLARE_FLAGS_OPERATORS(ezRHITopLevelASInstanceFlags);

struct ezRHITopLevelASInstanceDesc
{
  ezRHIFloat3X4 Transform;
  ezUInt32 InstanceID : 24;
  ezUInt32 InstanceMask : 8;
  ezUInt32 InstanceContributionToHitGroupIndex : 24;
  ezUInt32 Flags : 8;
  ezRHIResourceBase* BottomLevel;
};

struct ezRHITopLevelASDesc
{
  ezRHIGPUBuffer* InstanceBuffer;
  ezUInt32 Offset = 0;
  ezUInt32 Count = 0;
};

struct ezRHIRaytracingAccelerationStructureDesc
{
  ezBitflags<ezRHIRayTracingASFlags> Flags = ezRHIRayTracingASFlags::Empty;

  ezRHIRayTracingASLevel::Enum Type = ezRHIRayTracingASLevel::Bottom;

  ezRHIBottomLevelASDesc BottomLevel;

  ezRHITopLevelASDesc TopLevel;
};

struct ezRHIShaderLibraryType
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    RayGeneration, 
    Miss,
    ClosestHit,
    AnyHit,
    Intersection,

    ENUM_COUNT
  };
};

struct ezRHIShaderLibrary
{
  ezRHIShaderLibraryType::Enum Type = ezRHIShaderLibraryType::RayGeneration;
  const ezRHIShader* Shader = nullptr;
  ezString FunctionName;
};

struct ezRHIShaderHitGroupType
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    General, // raygen or miss
    Triangles,
    Procedural,

    ENUM_COUNT
  };
};

struct ezRHIShaderHitGroup
{
  ezRHIShaderHitGroupType::Enum Type = ezRHIShaderHitGroupType::Triangles;
  ezString Name;
  ezUInt32 GeneralShader = ~0;
  ezUInt32 ClosestHitShader = ~0;
  ezUInt32 AnyHitShader = ~0;
  ezUInt32 IntersectionShader = ~0;
};
struct ezRHIRaytracingPipelineStateDesc
{
  ezDynamicArray<ezRHIShaderLibrary> ShaderLibraries;
  ezDynamicArray<ezRHIShaderHitGroup> HitGroups;
  ezUInt32 MaxTraceRecursionDepth = 1;
  ezUInt32 MaxAttributeSizeInBytes = 0;
  ezUInt32 MaxPayloadSizeInBytes = 0;
};

struct ezRHIShaderTable
{
  const ezRHIGPUBuffer* Buffer = nullptr;
  ezUInt64 Offset = 0;
  ezUInt64 Size = 0;
  ezUInt64 Stride = 0;
};
struct ezRHIDispatchRaysDesc
{
  ezRHIShaderTable RayGeneration;
  ezRHIShaderTable Miss;
  ezRHIShaderTable HitGroup;
  ezRHIShaderTable Callable;
  ezUInt32 Width = 1;
  ezUInt32 Height = 1;
  ezUInt32 Depth = 1;
};
