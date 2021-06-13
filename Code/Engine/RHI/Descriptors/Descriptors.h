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

struct ezRHIQueryHeapDesc
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
