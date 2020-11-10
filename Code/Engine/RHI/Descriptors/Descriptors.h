#pragma once
#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

// Descriptor structs:

struct EZ_RHI_DLL Viewport
{
  float TopLeftX = 0.0f;
  float TopLeftY = 0.0f;
  float Width = 0.0f;
  float Height = 0.0f;
  float MinDepth = 0.0f;
  float MaxDepth = 1.0f;
};
struct EZ_RHI_DLL InputLayoutDesc
{
  static const ezUInt32 APPEND_ALIGNED_ELEMENT = 0xffffffff; // automatically figure out AlignedByteOffset depending on Format

  ezString SemanticName;
  ezUInt32 SemanticIndex = 0;
  ezRHIFormat::Enum Format = ezRHIFormat::Unknown;
  ezUInt32 InputSlot = 0;
  ezUInt32 AlignedByteOffset = APPEND_ALIGNED_ELEMENT;
  ezRHIInputClassification::Enum InputSlotClass = ezRHIInputClassification::PerVertexData;
  ezUInt32 InstanceDataStepRate = 0;
};
union EZ_RHI_DLL ClearValue
{
  float color[4];
  struct ClearDepthStencil
  {
    float depth;
    ezUInt32 stencil;
  } depthstencil;
};
struct EZ_RHI_DLL TextureDesc
{
  enum TEXTURE_TYPE
  {
    TEXTURE_1D,
    TEXTURE_2D,
    TEXTURE_3D,
  } type = TEXTURE_2D;
  ezUInt32 Width = 0;
  ezUInt32 Height = 0;
  ezUInt32 Depth = 0;
  ezUInt32 ArraySize = 1;
  ezUInt32 MipLevels = 1;
  ezRHIFormat::Enum Format = ezRHIFormat::Unknown;
  ezUInt32 SampleCount = 1;
  ezBitflags<ezRHIUsage> Usage = ezRHIUsage::Default;
  ezUInt32 BindFlags = 0;
  ezUInt32 CPUAccessFlags = 0;
  ezUInt32 MiscFlags = 0;
  ClearValue clear = {};
  IMAGE_LAYOUT layout = IMAGE_LAYOUT_GENERAL;
};
struct EZ_RHI_DLL SamplerDesc
{
  ezRHIFilter::Enum Filter = ezRHIFilter::FILTER_MIN_MAG_MIP_POINT;
  ezRHITextureAddressMode::Enum AddressU = ezRHITextureAddressMode::Clamp;
  ezRHITextureAddressMode::Enum AddressV = ezRHITextureAddressMode::Clamp;
  ezRHITextureAddressMode::Enum AddressW = ezRHITextureAddressMode::Clamp;
  float MipLODBias = 0.0f;
  ezUInt32 MaxAnisotropy = 0;
  ezRHIComparisonFunc::Enum ComparisonFunc = ezRHIComparisonFunc::Never;
  float BorderColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  float MinLOD = 0.0f;
  float MaxLOD = ezMath::MaxValue<float>();
};
struct EZ_RHI_DLL RasterizerStateDesc
{
  ezRHIFillMode::Enum FillMode = ezRHIFillMode::Solid;
  ezRHICullMode::Enum CullMode = ezRHICullMode::None;
  bool FrontCounterClockwise = false;
  int32_t DepthBias = 0;
  float DepthBiasClamp = 0.0f;
  float SlopeScaledDepthBias = 0.0f;
  bool DepthClipEnable = false;
  bool MultisampleEnable = false;
  bool AntialiasedLineEnable = false;
  bool ConservativeRasterizationEnable = false;
  ezUInt32 ForcedSampleCount = 0;
};
struct EZ_RHI_DLL DepthStencilOpDesc
{
  ezRHIStencilOp::Enum StencilFailOp = ezRHIStencilOp::Keep;
  ezRHIStencilOp::Enum StencilDepthFailOp = ezRHIStencilOp::Keep;
  ezRHIStencilOp::Enum StencilPassOp = ezRHIStencilOp::Keep;
  ezRHIComparisonFunc::Enum StencilFunc = ezRHIComparisonFunc::Never;
};
struct EZ_RHI_DLL DepthStencilStateDesc
{
  bool DepthEnable = false;
  ezRHIDepthWriteMask::Enum DepthWriteMask = ezRHIDepthWriteMask::Zero;
  ezRHIComparisonFunc::Enum DepthFunc = ezRHIComparisonFunc::Never;
  bool StencilEnable = false;
  ezUInt8 StencilReadMask = 0xff;
  ezUInt8 StencilWriteMask = 0xff;
  DepthStencilOpDesc FrontFace;
  DepthStencilOpDesc BackFace;
};
struct EZ_RHI_DLL RenderTargetBlendStateDesc
{
  bool BlendEnable = false;
  ezRHIBlendFactor::Enum SrcBlend = ezRHIBlendFactor::SourceAlpha;
  ezRHIBlendFactor::Enum DestBlend = ezRHIBlendFactor::InverseSourceAlpha;
  ezRHIBlendOp::Enum BlendOp = ezRHIBlendOp::Add;
  ezRHIBlendFactor::Enum SrcBlendAlpha = ezRHIBlendFactor::One;
  ezRHIBlendFactor::Enum DestBlendAlpha = ezRHIBlendFactor::One;
  ezRHIBlendOp::Enum BlendOpAlpha = ezRHIBlendOp::Add;
  ezBitflags<ezRHIColorWriteMask> RenderTargetWriteMask = ezRHIColorWriteMask::All;
};
struct EZ_RHI_DLL BlendStateDesc
{
  bool AlphaToCoverageEnable = false;
  bool IndependentBlendEnable = false;
  RenderTargetBlendStateDesc RenderTarget[8];
};
struct EZ_RHI_DLL GPUBufferDesc
{
  ezUInt32 ByteWidth = 0;
  ezBitflags<ezRHIUsage> Usage = ezRHIUsage::Default;
  ezUInt32 BindFlags = 0;
  ezUInt32 CPUAccessFlags = 0;
  ezUInt32 MiscFlags = 0;
  ezUInt32 StructureByteStride = 0;                // needed for typed and structured buffer types!
  ezRHIFormat::Enum Format = ezRHIFormat::Unknown; // only needed for typed buffer!
};
struct EZ_RHI_DLL GPUQueryDesc
{
  GPU_QUERY_TYPE Type = GPU_QUERY_TYPE_INVALID;
};
struct EZ_RHI_DLL GPUQueryResult
{
  ezUInt64 result_passed_sample_count = 0;
  ezUInt64 result_timestamp = 0;
  ezUInt64 result_timestamp_frequency = 0;
};
struct EZ_RHI_DLL PipelineStateDesc
{
  const RootSignature* rootSignature = nullptr;
  const Shader* vs = nullptr;
  const Shader* ps = nullptr;
  const Shader* hs = nullptr;
  const Shader* ds = nullptr;
  const Shader* gs = nullptr;
  const Shader* ms = nullptr;
  const Shader* as = nullptr;
  const BlendState* bs = nullptr;
  const RasterizerState* rs = nullptr;
  const DepthStencilState* dss = nullptr;
  const InputLayout* il = nullptr;
  ezRHIPrimitiveTopology::Enum pt = ezRHIPrimitiveTopology::TriangleList;
  ezUInt32 sampleMask = 0xFFFFFFFF;
};
struct EZ_RHI_DLL GPUBarrier
{
  enum TYPE
  {
    MEMORY_BARRIER, // UAV accesses
    IMAGE_BARRIER,  // image layout transition
    BUFFER_BARRIER, // buffer state transition
  } type = MEMORY_BARRIER;

  struct Memory
  {
    const GPUResource* resource;
  };
  struct Image
  {
    const Texture* texture;
    IMAGE_LAYOUT layout_before;
    IMAGE_LAYOUT layout_after;
  };
  struct Buffer
  {
    const GPUBuffer* buffer;
    BUFFER_STATE state_before;
    BUFFER_STATE state_after;
  };
  union
  {
    Memory memory;
    Image image;
    Buffer buffer;
  };

  static GPUBarrier Memory(const GPUResource* resource = nullptr)
  {
    GPUBarrier barrier;
    barrier.type = MEMORY_BARRIER;
    barrier.memory.resource = resource;
    return barrier;
  }
  static GPUBarrier Image(const Texture* texture, IMAGE_LAYOUT before, IMAGE_LAYOUT after)
  {
    GPUBarrier barrier;
    barrier.type = IMAGE_BARRIER;
    barrier.image.texture = texture;
    barrier.image.layout_before = before;
    barrier.image.layout_after = after;
    return barrier;
  }
  static GPUBarrier Buffer(const GPUBuffer* buffer, BUFFER_STATE before, BUFFER_STATE after)
  {
    GPUBarrier barrier;
    barrier.type = BUFFER_BARRIER;
    barrier.buffer.buffer = buffer;
    barrier.buffer.state_before = before;
    barrier.buffer.state_after = after;
    return barrier;
  }
};
struct EZ_RHI_DLL RenderPassAttachment
{
  enum TYPE
  {
    RENDERTARGET,
    DEPTH_STENCIL,
    RESOLVE,
  } type = RENDERTARGET;
  enum LOAD_OPERATION
  {
    LOADOP_LOAD,
    LOADOP_CLEAR,
    LOADOP_DONTCARE,
  } loadop = LOADOP_LOAD;
  const Texture* texture = nullptr;
  int subresource = -1;
  enum STORE_OPERATION
  {
    STOREOP_STORE,
    STOREOP_DONTCARE,
  } storeop = STOREOP_STORE;
  IMAGE_LAYOUT initial_layout = IMAGE_LAYOUT_GENERAL;      // layout before the render pass
  IMAGE_LAYOUT final_layout = IMAGE_LAYOUT_GENERAL;        // layout after the render pass
  IMAGE_LAYOUT subpass_layout = IMAGE_LAYOUT_RENDERTARGET; // layout within the render pass

  static RenderPassAttachment RenderTarget(
    const Texture* resource = nullptr,
    LOAD_OPERATION load_op = LOADOP_LOAD,
    STORE_OPERATION store_op = STOREOP_STORE,
    IMAGE_LAYOUT initial_layout = IMAGE_LAYOUT_GENERAL,
    IMAGE_LAYOUT subpass_layout = IMAGE_LAYOUT_RENDERTARGET,
    IMAGE_LAYOUT final_layout = IMAGE_LAYOUT_GENERAL)
  {
    RenderPassAttachment attachment;
    attachment.type = RENDERTARGET;
    attachment.texture = resource;
    attachment.loadop = load_op;
    attachment.storeop = store_op;
    attachment.initial_layout = initial_layout;
    attachment.subpass_layout = subpass_layout;
    attachment.final_layout = final_layout;
    return attachment;
  }

  static RenderPassAttachment DepthStencil(
    const Texture* resource = nullptr,
    LOAD_OPERATION load_op = LOADOP_LOAD,
    STORE_OPERATION store_op = STOREOP_STORE,
    IMAGE_LAYOUT initial_layout = IMAGE_LAYOUT_DEPTHSTENCIL,
    IMAGE_LAYOUT subpass_layout = IMAGE_LAYOUT_DEPTHSTENCIL,
    IMAGE_LAYOUT final_layout = IMAGE_LAYOUT_DEPTHSTENCIL)
  {
    RenderPassAttachment attachment;
    attachment.type = DEPTH_STENCIL;
    attachment.texture = resource;
    attachment.loadop = load_op;
    attachment.storeop = store_op;
    attachment.initial_layout = initial_layout;
    attachment.subpass_layout = subpass_layout;
    attachment.final_layout = final_layout;
    return attachment;
  }

  static RenderPassAttachment Resolve(
    const Texture* resource = nullptr,
    IMAGE_LAYOUT initial_layout = IMAGE_LAYOUT_GENERAL,
    IMAGE_LAYOUT final_layout = IMAGE_LAYOUT_GENERAL)
  {
    RenderPassAttachment attachment;
    attachment.type = RESOLVE;
    attachment.texture = resource;
    attachment.initial_layout = initial_layout;
    attachment.final_layout = final_layout;
    return attachment;
  }
};
struct EZ_RHI_DLL RenderPassDesc
{
  enum FLAGS
  {
    FLAG_EMPTY = 0,
    FLAG_ALLOW_UAV_WRITES = 1 << 0,
  };
  ezUInt32 _flags = FLAG_EMPTY;
  std::vector<RenderPassAttachment> attachments;
};
struct EZ_RHI_DLL IndirectDrawArgsInstanced
{
  ezUInt32 VertexCountPerInstance = 0;
  ezUInt32 InstanceCount = 0;
  ezUInt32 StartVertexLocation = 0;
  ezUInt32 StartInstanceLocation = 0;
};
struct EZ_RHI_DLL IndirectDrawArgsIndexedInstanced
{
  ezUInt32 IndexCountPerInstance = 0;
  ezUInt32 InstanceCount = 0;
  ezUInt32 StartIndexLocation = 0;
  int32_t BaseVertexLocation = 0;
  ezUInt32 StartInstanceLocation = 0;
};
struct EZ_RHI_DLL IndirectDispatchArgs
{
  ezUInt32 ThreadGroupCountX = 0;
  ezUInt32 ThreadGroupCountY = 0;
  ezUInt32 ThreadGroupCountZ = 0;
};
struct EZ_RHI_DLL SubresourceData
{
  const void* pSysMem = nullptr;
  ezUInt32 SysMemPitch = 0;
  ezUInt32 SysMemSlicePitch = 0;
};
struct EZ_RHI_DLL Rect
{
  int32_t left = 0;
  int32_t top = 0;
  int32_t right = 0;
  int32_t bottom = 0;
};
struct EZ_RHI_DLL Mapping
{
  enum FLAGS
  {
    FLAG_EMPTY = 0,
    FLAG_READ = 1 << 0,
    FLAG_WRITE = 1 << 1,
  };
  ezUInt32 _flags = FLAG_EMPTY;
  size_t offset = 0;
  size_t size = 0;
  ezUInt32 rowpitch = 0; // output
  void* data = nullptr;  // output
};
