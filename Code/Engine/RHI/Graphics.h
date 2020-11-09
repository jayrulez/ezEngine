#pragma once
#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#define arraysize(a) (sizeof(a) / sizeof(a[0]))
#define NOMINMAX
#define ALIGN_16                                             \
  void* operator new(size_t i) { return _mm_malloc(i, 16); } \
  void operator delete(void* p) { _mm_free(p); }

#ifdef _WIN32

#  include <Foundation/Basics/Platform/Win/IncludeWindows.h>
#  include <SDKDDKVer.h>

#  if WINAPI_FAMILY == WINAPI_FAMILY_APP
#    define PLATFORM_UWP
#    include <Windows.UI.Core.h>
#    include <agile.h>
#  endif // UWP

#else

#endif // _WIN32


#ifdef _WIN32
#  ifndef PLATFORM_UWP
using RHIWindowType = HWND;
#  else
using RHIWindowType = Platform::Agile<Windows::UI::Core::CoreWindow>;
#  endif // PLATFORM_UWP
#else
using RHIWindowType = int;
#endif // _WIN32

///////////////////////////////////////////TODO//////////////////////////////////////
//                PR an ezMat3x4 implementation to ez upstream                     //
/////////////////////////////////////////////////////////////////////////////////////
struct Float3X4
{
  union
  {
    struct
    {
      float _11, _12, _13, _14;
      float _21, _22, _23, _24;
      float _31, _32, _33, _34;
    };
    float m[3][4];
    float f[12];
  };

  Float3X4() = default;

  Float3X4(const Float3X4&) = default;
  Float3X4& operator=(const Float3X4&) = default;

  Float3X4(Float3X4&&) = default;
  Float3X4& operator=(Float3X4&&) = default;

  constexpr Float3X4(float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23)
    : _11(m00)
    , _12(m01)
    , _13(m02)
    , _14(m03)
    , _21(m10)
    , _22(m11)
    , _23(m12)
    , _24(m13)
    , _31(m20)
    , _32(m21)
    , _33(m22)
    , _34(m23)
  {
  }
  explicit Float3X4(const float* pArray);

  float operator()(size_t Row, size_t Column) const { return m[Row][Column]; }
  float& operator()(size_t Row, size_t Column) { return m[Row][Column]; }
};

inline Float3X4::Float3X4(
  const float* pArray)
{
  EZ_ASSERT_ALWAYS(pArray != nullptr, "The float pointer passed should not be null.");

  m[0][0] = pArray[0];
  m[0][1] = pArray[1];
  m[0][2] = pArray[2];
  m[0][3] = pArray[3];

  m[1][0] = pArray[4];
  m[1][1] = pArray[5];
  m[1][2] = pArray[6];
  m[1][3] = pArray[7];

  m[2][0] = pArray[8];
  m[2][1] = pArray[9];
  m[2][2] = pArray[10];
  m[2][3] = pArray[11];
}
/////////////////////////////////////////////////////////////////////////////////////









enum INPUT_CLASSIFICATION
{
  INPUT_PER_VERTEX_DATA,
  INPUT_PER_INSTANCE_DATA,
};

enum TEXTURE_ADDRESS_MODE
{
  TEXTURE_ADDRESS_WRAP,
  TEXTURE_ADDRESS_MIRROR,
  TEXTURE_ADDRESS_CLAMP,
  TEXTURE_ADDRESS_BORDER,
};
enum FILTER
{
  FILTER_MIN_MAG_MIP_POINT,
  FILTER_MIN_MAG_POINT_MIP_LINEAR,
  FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
  FILTER_MIN_POINT_MAG_MIP_LINEAR,
  FILTER_MIN_LINEAR_MAG_MIP_POINT,
  FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
  FILTER_MIN_MAG_LINEAR_MIP_POINT,
  FILTER_MIN_MAG_MIP_LINEAR,
  FILTER_ANISOTROPIC,
  FILTER_COMPARISON_MIN_MAG_MIP_POINT,
  FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR,
  FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT,
  FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR,
  FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT,
  FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
  FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
  FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
  FILTER_COMPARISON_ANISOTROPIC,
  FILTER_MINIMUM_MIN_MAG_MIP_POINT,
  FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR,
  FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
  FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR,
  FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT,
  FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
  FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT,
  FILTER_MINIMUM_MIN_MAG_MIP_LINEAR,
  FILTER_MINIMUM_ANISOTROPIC,
  FILTER_MAXIMUM_MIN_MAG_MIP_POINT,
  FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR,
  FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
  FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR,
  FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT,
  FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
  FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT,
  FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR,
  FILTER_MAXIMUM_ANISOTROPIC,
};
enum GPU_QUERY_TYPE
{
  GPU_QUERY_TYPE_INVALID,             // do not use! Indicates if query was not created.
  GPU_QUERY_TYPE_EVENT,               // has the GPU reached this point?
  GPU_QUERY_TYPE_OCCLUSION,           // how many samples passed depthstencil test?
  GPU_QUERY_TYPE_OCCLUSION_PREDICATE, // are there any samples that passed depthstencil test
  GPU_QUERY_TYPE_TIMESTAMP,           // retrieve time point of gpu execution
  GPU_QUERY_TYPE_TIMESTAMP_DISJOINT,  // timestamp frequency information
};

enum SUBRESOURCE_TYPE
{
  SRV,
  UAV,
  RTV,
  DSV,
};
enum IMAGE_LAYOUT
{
  IMAGE_LAYOUT_UNDEFINED,             // discard contents
  IMAGE_LAYOUT_GENERAL,               // supports everything
  IMAGE_LAYOUT_RENDERTARGET,          // render target, write enabled
  IMAGE_LAYOUT_DEPTHSTENCIL,          // depth stencil, write enabled
  IMAGE_LAYOUT_DEPTHSTENCIL_READONLY, // depth stencil, read only
  IMAGE_LAYOUT_SHADER_RESOURCE,       // shader resource, read only
  IMAGE_LAYOUT_UNORDERED_ACCESS,      // shader resource, write enabled
  IMAGE_LAYOUT_COPY_SRC,              // copy from
  IMAGE_LAYOUT_COPY_DST,              // copy to
  IMAGE_LAYOUT_SHADING_RATE_SOURCE,   // shading rate control per tile
};
enum BUFFER_STATE
{
  BUFFER_STATE_GENERAL,           // supports everything
  BUFFER_STATE_VERTEX_BUFFER,     // vertex buffer, read only
  BUFFER_STATE_INDEX_BUFFER,      // index buffer, read only
  BUFFER_STATE_CONSTANT_BUFFER,   // constant buffer, read only
  BUFFER_STATE_INDIRECT_ARGUMENT, // argument buffer to DrawIndirect() or DispatchIndirect()
  BUFFER_STATE_SHADER_RESOURCE,   // shader resource, read only
  BUFFER_STATE_UNORDERED_ACCESS,  // shader resource, write enabled
  BUFFER_STATE_COPY_SRC,          // copy from
  BUFFER_STATE_COPY_DST,          // copy to
  BUFFER_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
};
enum SHADING_RATE
{
  SHADING_RATE_1X1,
  SHADING_RATE_1X2,
  SHADING_RATE_2X1,
  SHADING_RATE_2X2,
  SHADING_RATE_2X4,
  SHADING_RATE_4X2,
  SHADING_RATE_4X4
};
enum GRAPHICSDEVICE_CAPABILITY
{
  GRAPHICSDEVICE_CAPABILITY_TESSELLATION,
  GRAPHICSDEVICE_CAPABILITY_CONSERVATIVE_RASTERIZATION,
  GRAPHICSDEVICE_CAPABILITY_RASTERIZER_ORDERED_VIEWS,
  GRAPHICSDEVICE_CAPABILITY_UAV_LOAD_FORMAT_COMMON, // eg: R16G16B16A16_FLOAT, R8G8B8A8_UNORM and more common ones
  GRAPHICSDEVICE_CAPABILITY_UAV_LOAD_FORMAT_R11G11B10_FLOAT,
  GRAPHICSDEVICE_CAPABILITY_RENDERTARGET_AND_VIEWPORT_ARRAYINDEX_WITHOUT_GS,
  GRAPHICSDEVICE_CAPABILITY_RAYTRACING,
  GRAPHICSDEVICE_CAPABILITY_RAYTRACING_INLINE,
  GRAPHICSDEVICE_CAPABILITY_DESCRIPTOR_MANAGEMENT,
  GRAPHICSDEVICE_CAPABILITY_VARIABLE_RATE_SHADING,
  GRAPHICSDEVICE_CAPABILITY_VARIABLE_RATE_SHADING_TIER2,
  GRAPHICSDEVICE_CAPABILITY_MESH_SHADER,
  GRAPHICSDEVICE_CAPABILITY_COUNT,
};

// Flags ////////////////////////////////////////////
enum BIND_FLAG
{
  BIND_VERTEX_BUFFER = 1 << 0,
  BIND_INDEX_BUFFER = 1 << 1,
  BIND_CONSTANT_BUFFER = 1 << 2,
  BIND_SHADER_RESOURCE = 1 << 3,
  BIND_STREAM_OUTPUT = 1 << 4,
  BIND_RENDER_TARGET = 1 << 5,
  BIND_DEPTH_STENCIL = 1 << 6,
  BIND_UNORDERED_ACCESS = 1 << 7,
};
enum CPU_ACCESS
{
  CPU_ACCESS_WRITE = 1 << 0,
  CPU_ACCESS_READ = 1 << 1,
};
enum RESOURCE_MISC_FLAG
{
  RESOURCE_MISC_SHARED = 1 << 0,
  RESOURCE_MISC_TEXTURECUBE = 1 << 1,
  RESOURCE_MISC_INDIRECT_ARGS = 1 << 2,
  RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS = 1 << 3,
  RESOURCE_MISC_BUFFER_STRUCTURED = 1 << 4,
  RESOURCE_MISC_TILED = 1 << 5,
  RESOURCE_MISC_RAY_TRACING = 1 << 6,
};

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

  std::string SemanticName;
  ezUInt32 SemanticIndex = 0;
  ezRHIFormat::Enum Format = ezRHIFormat::Unknown;
  ezUInt32 InputSlot = 0;
  ezUInt32 AlignedByteOffset = APPEND_ALIGNED_ELEMENT;
  INPUT_CLASSIFICATION InputSlotClass = INPUT_CLASSIFICATION::INPUT_PER_VERTEX_DATA;
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
  FILTER Filter = FILTER_MIN_MAG_MIP_POINT;
  TEXTURE_ADDRESS_MODE AddressU = TEXTURE_ADDRESS_CLAMP;
  TEXTURE_ADDRESS_MODE AddressV = TEXTURE_ADDRESS_CLAMP;
  TEXTURE_ADDRESS_MODE AddressW = TEXTURE_ADDRESS_CLAMP;
  float MipLODBias = 0.0f;
  ezUInt32 MaxAnisotropy = 0;
  ezRHIComparisonFunc::Enum ComparisonFunc = ezRHIComparisonFunc::Never;
  float BorderColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  float MinLOD = 0.0f;
  float MaxLOD = ezMath::MaxValue<float>();
};
struct EZ_RHI_DLL RasterizerStateDesc
{
  ezRHIFillMode::Enum FillMode = ezRHIFillMode::FILL_SOLID;
  ezRHICullMode::Enum CullMode =ezRHICullMode::CULL_NONE;
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
  ezUInt32 StructureByteStride = 0; // needed for typed and structured buffer types!
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


// Resources:

struct EZ_RHI_DLL GraphicsDeviceChild
{
  std::shared_ptr<void> internal_state;
  inline bool IsValid() const { return internal_state.get() != nullptr; }
};

struct EZ_RHI_DLL Shader : public GraphicsDeviceChild
{
  ezEnum<ezRHIShaderStage> stage = ezRHIShaderStage::ENUM_COUNT;
  std::vector<ezUInt8> code;
  const RootSignature* rootSignature = nullptr;
};

struct EZ_RHI_DLL Sampler : public GraphicsDeviceChild
{
  SamplerDesc desc;

  const SamplerDesc& GetDesc() const { return desc; }
};

struct EZ_RHI_DLL GPUResource : public GraphicsDeviceChild
{
  enum class GPU_RESOURCE_TYPE
  {
    BUFFER,
    TEXTURE,
    RAYTRACING_ACCELERATION_STRUCTURE,
    UNKNOWN_TYPE,
  } type = GPU_RESOURCE_TYPE::UNKNOWN_TYPE;
  inline bool IsTexture() const { return type == GPU_RESOURCE_TYPE::TEXTURE; }
  inline bool IsBuffer() const { return type == GPU_RESOURCE_TYPE::BUFFER; }
  inline bool IsAccelerationStructure() const { return type == GPU_RESOURCE_TYPE::RAYTRACING_ACCELERATION_STRUCTURE; }
};

struct EZ_RHI_DLL GPUBuffer : public GPUResource
{
  GPUBufferDesc desc;

  const GPUBufferDesc& GetDesc() const { return desc; }
};

struct EZ_RHI_DLL InputLayout : public GraphicsDeviceChild
{
  std::vector<InputLayoutDesc> desc;
};

struct EZ_RHI_DLL BlendState : public GraphicsDeviceChild
{
  BlendStateDesc desc;

  const BlendStateDesc& GetDesc() const { return desc; }
};

struct EZ_RHI_DLL DepthStencilState : public GraphicsDeviceChild
{
  DepthStencilStateDesc desc;

  const DepthStencilStateDesc& GetDesc() const { return desc; }
};

struct EZ_RHI_DLL RasterizerState : public GraphicsDeviceChild
{
  RasterizerStateDesc desc;

  const RasterizerStateDesc& GetDesc() const { return desc; }
};

struct EZ_RHI_DLL Texture : public GPUResource
{
  TextureDesc desc;

  const TextureDesc& GetDesc() const { return desc; }
};

struct EZ_RHI_DLL GPUQuery : public GraphicsDeviceChild
{
  GPUQueryDesc desc;

  const GPUQueryDesc& GetDesc() const { return desc; }
};

struct EZ_RHI_DLL PipelineState : public GraphicsDeviceChild
{
  size_t hash = 0;
  PipelineStateDesc desc;

  const PipelineStateDesc& GetDesc() const { return desc; }
};

struct EZ_RHI_DLL RenderPass : public GraphicsDeviceChild
{
  size_t hash = 0;
  RenderPassDesc desc;

  const RenderPassDesc& GetDesc() const { return desc; }
};


struct EZ_RHI_DLL RaytracingAccelerationStructureDesc
{
  enum FLAGS
  {
    FLAG_EMPTY = 0,
    FLAG_ALLOW_UPDATE = 1 << 0,
    FLAG_ALLOW_COMPACTION = 1 << 1,
    FLAG_PREFER_FAST_TRACE = 1 << 2,
    FLAG_PREFER_FAST_BUILD = 1 << 3,
    FLAG_MINIMIZE_MEMORY = 1 << 4,
  };
  ezUInt32 _flags = FLAG_EMPTY;

  enum TYPE
  {
    BOTTOMLEVEL,
    TOPLEVEL,
  } type = BOTTOMLEVEL;

  struct BottomLevel
  {
    struct Geometry
    {
      enum FLAGS
      {
        FLAG_EMPTY = 0,
        FLAG_OPAQUE = 1 << 0,
        FLAG_NO_DUPLICATE_ANYHIT_INVOCATION = 1 << 1,
        FLAG_USE_TRANSFORM = 1 << 2,
      };
      ezUInt32 _flags = FLAG_EMPTY;

      enum TYPE
      {
        TRIANGLES,
        PROCEDURAL_AABBS,
      } type = TRIANGLES;

      struct Triangles
      {
        GPUBuffer vertexBuffer;
        GPUBuffer indexBuffer;
        ezUInt32 indexCount = 0;
        ezUInt32 indexOffset = 0;
        ezUInt32 vertexCount = 0;
        ezUInt32 vertexByteOffset = 0;
        ezUInt32 vertexStride = 0;
        ezRHIIndexBufferFormat::Enum indexFormat = ezRHIIndexBufferFormat::UInt32;
        ezRHIFormat::Enum vertexFormat = ezRHIFormat::R32G32B32_FLOAT;
        GPUBuffer transform3x4Buffer;
        ezUInt32 transform3x4BufferOffset = 0;
      } triangles;
      struct Procedural_AABBs
      {
        GPUBuffer aabbBuffer;
        ezUInt32 offset = 0;
        ezUInt32 count = 0;
        ezUInt32 stride = 0;
      } aabbs;
    };
    std::vector<Geometry> geometries;
  } bottomlevel;

  struct TopLevel
  {
    struct Instance
    {
      Float3X4 transform;
      ezUInt32 InstanceID : 24;
      ezUInt32 InstanceMask : 8;
      ezUInt32 InstanceContributionToHitGroupIndex : 24;
      ezUInt32 Flags : 8;
      GPUResource bottomlevel;
    };
    GPUBuffer instanceBuffer;
    ezUInt32 offset = 0;
    ezUInt32 count = 0;
  } toplevel;
};
struct EZ_RHI_DLL RaytracingAccelerationStructure : public GPUResource
{
  RaytracingAccelerationStructureDesc desc;

  const RaytracingAccelerationStructureDesc& GetDesc() const { return desc; }
};

struct EZ_RHI_DLL ShaderLibrary
{
  enum TYPE
  {
    RAYGENERATION,
    MISS,
    CLOSESTHIT,
    ANYHIT,
    INTERSECTION,
  } type = RAYGENERATION;
  const Shader* shader = nullptr;
  std::string function_name;
};
struct EZ_RHI_DLL ShaderHitGroup
{
  enum TYPE
  {
    GENERAL, // raygen or miss
    TRIANGLES,
    PROCEDURAL,
  } type = TRIANGLES;
  std::string name;
  ezUInt32 general_shader = ~0;
  ezUInt32 closesthit_shader = ~0;
  ezUInt32 anyhit_shader = ~0;
  ezUInt32 intersection_shader = ~0;
};
struct EZ_RHI_DLL RaytracingPipelineStateDesc
{
  const RootSignature* rootSignature = nullptr;
  std::vector<ShaderLibrary> shaderlibraries;
  std::vector<ShaderHitGroup> hitgroups;
  ezUInt32 max_trace_recursion_depth = 1;
  ezUInt32 max_attribute_size_in_bytes = 0;
  ezUInt32 max_payload_size_in_bytes = 0;
};
struct EZ_RHI_DLL RaytracingPipelineState : public GraphicsDeviceChild
{
  RaytracingPipelineStateDesc desc;

  const RaytracingPipelineStateDesc& GetDesc() const { return desc; }
};

struct EZ_RHI_DLL ShaderTable
{
  const GPUBuffer* buffer = nullptr;
  ezUInt64 offset = 0;
  ezUInt64 size = 0;
  ezUInt64 stride = 0;
};
struct EZ_RHI_DLL DispatchRaysDesc
{
  ShaderTable raygeneration;
  ShaderTable miss;
  ShaderTable hitgroup;
  ShaderTable callable;
  ezUInt32 Width = 1;
  ezUInt32 Height = 1;
  ezUInt32 Depth = 1;
};

enum BINDPOINT
{
  GRAPHICS,
  COMPUTE,
  RAYTRACING,
};
enum RESOURCEBINDING
{
  ROOT_CONSTANTBUFFER,
  ROOT_RAWBUFFER,
  ROOT_STRUCTUREDBUFFER,
  ROOT_RWRAWBUFFER,
  ROOT_RWSTRUCTUREDBUFFER,

  CONSTANTBUFFER,
  RAWBUFFER,
  STRUCTUREDBUFFER,
  TYPEDBUFFER,
  TEXTURE1D,
  TEXTURE1DARRAY,
  TEXTURE2D,
  TEXTURE2DARRAY,
  TEXTURECUBE,
  TEXTURECUBEARRAY,
  TEXTURE3D,
  ACCELERATIONSTRUCTURE,
  RWRAWBUFFER,
  RWSTRUCTUREDBUFFER,
  RWTYPEDBUFFER,
  RWTEXTURE1D,
  RWTEXTURE1DARRAY,
  RWTEXTURE2D,
  RWTEXTURE2DARRAY,
  RWTEXTURE3D,

  RESOURCEBINDING_COUNT
};
struct EZ_RHI_DLL ResourceRange
{
  RESOURCEBINDING binding = CONSTANTBUFFER;
  ezUInt32 slot = 0;
  ezUInt32 count = 1;
};
struct EZ_RHI_DLL SamplerRange
{
  ezUInt32 slot = 0;
  ezUInt32 count = 1;
};
struct EZ_RHI_DLL StaticSampler
{
  Sampler sampler;
  ezUInt32 slot = 0;
};
struct EZ_RHI_DLL DescriptorTable : public GraphicsDeviceChild
{
  ezEnum<ezRHIShaderStage> stage = ezRHIShaderStage::ENUM_COUNT;
  std::vector<ResourceRange> resources;
  std::vector<SamplerRange> samplers;
  std::vector<StaticSampler> staticsamplers;
};
struct EZ_RHI_DLL RootConstantRange
{
  ezEnum<ezRHIShaderStage> stage = ezRHIShaderStage::ENUM_COUNT;
  ezUInt32 slot = 0;
  ezUInt32 size = 0;
  ezUInt32 offset = 0;
};
struct EZ_RHI_DLL RootSignature : public GraphicsDeviceChild
{
  enum FLAGS
  {
    FLAG_EMPTY = 0,
    FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT = 1 << 0,
  };
  ezUInt32 _flags = FLAG_EMPTY;
  std::vector<DescriptorTable> tables;
  std::vector<RootConstantRange> rootconstants;
};
