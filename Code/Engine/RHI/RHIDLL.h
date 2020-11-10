#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Types/Id.h>
#include <Foundation/Types/RefCounted.h>


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

// Configure the DLL Import/Export Define
#if EZ_ENABLED(EZ_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_RHI_LIB
#    define EZ_RHI_DLL __declspec(dllexport)
#  else
#    define EZ_RHI_DLL __declspec(dllimport)
#  endif
#else
#  define EZ_RHI_DLL
#endif

// Descriptor layout counts per shader stage:
//	Rebuilding shaders and graphics devices are necessary after changing these values
#define GPU_RESOURCE_HEAP_CBV_COUNT 12
#define GPU_RESOURCE_HEAP_SRV_COUNT 48
#define GPU_RESOURCE_HEAP_UAV_COUNT 8
#define GPU_SAMPLER_HEAP_COUNT 16


///////////////////////////////////////////TODO//////////////////////////////////////
//                PR an ezMat3x4 implementation to ez upstream                     //
/////////////////////////////////////////////////////////////////////////////////////
struct EZ_RHI_DLL Float3X4
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


// Forward declarations
struct Shader;
struct BlendState;
struct RasterizerState;
struct DepthStencilState;
struct InputLayout;
struct GPUResource;
struct GPUBuffer;
struct Texture;
struct RootSignature;


struct EZ_RHI_DLL ezRHIPrimitiveTopology
{
  typedef ezUInt32 StorageType;
  enum Enum
  {
    Undefined,
    TriangleList,
    TriangleStrip,
    PointList,
    LineList,
    LineStrip,
    PatchList,
  };
};

struct EZ_RHI_DLL ezRHIIndexBufferFormat
{
  typedef ezUInt32 StorageType;
  enum Enum
  {
    UInt16,
    UInt32
  };
};

struct EZ_RHI_DLL ezRHIDepthWriteMask
{
  typedef ezUInt32 StorageType;
  enum Enum
  {
    Zero,
    All,
  };
};

struct EZ_RHI_DLL ezRHIColorWriteMask
{
  typedef ezUInt8 StorageType;
  enum Enum
  {
    None = 0,
    Red = 1,
    Green = 2,
    Blue = 4,
    Alpha = 8,
    All = (((Red | Green) | Blue) | Alpha)
  };
  struct Bits
  {
  };
};
EZ_DECLARE_FLAGS_OPERATORS(ezRHIColorWriteMask);

struct EZ_RHI_DLL ezRHIBlendFactor
{
  typedef ezUInt32 StorageType;
  enum Enum
  {
    Zero,
    One,
    SourceColor,
    InverseSourceColor,
    SourceAlpha,
    InverseSourceAlpha,
    DestinationAlpha,
    InverseDestinationAlpha,
    DestinationColor,
    InverseDestinationColor,
    SourceAlphaSaturated,
    BlendFactor,
    InverseBlendFactor,
    Source1Color,
    InverseSource1Color,
    Source1Alpha,
    InverseSource1Alpha,
  };
};

struct EZ_RHI_DLL ezRHIBlendOp
{
  typedef ezUInt32 StorageType;
  enum Enum
  {
    Add,
    Subtract,
    ReverseSubtract,
    Min,
    Max,
  };
};



struct EZ_RHI_DLL ezRHIStencilOp
{
  typedef ezUInt8 StorageType;
  enum Enum
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
};

struct EZ_RHI_DLL ezRHIComparisonFunc
{
  typedef ezUInt32 StorageType;
  enum Enum
  {
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always,
  };
};

struct EZ_RHI_DLL ezRHIShaderStage
{
  typedef ezUInt32 StorageType;
  enum Enum
  {
    MeshShader,
    AmplificationShader,
    VertexShader,
    HullShader,
    DomainShader,
    GeometryShader,
    PixelShader,
    ComputeShader,
    ENUM_COUNT,
  };
};

struct EZ_RHI_DLL ezRHIUsage
{
  typedef ezUInt8 StorageType;
  enum Enum
  {
    Default,
    Immutable,
    Dynamic,
    Staging,
  };
  struct Bits
  {
  };
};
EZ_DECLARE_FLAGS_OPERATORS(ezRHIUsage);

struct EZ_RHI_DLL ezRHIFormat
{
  typedef ezUInt8 StorageType;
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
    R32G8X24_TYPELESS,    // depth + stencil (alias)
    D32_FLOAT_S8X24_UINT, // depth + stencil

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
    R32_TYPELESS, // depth (alias)
    D32_FLOAT,    // depth
    R32_FLOAT,
    R32_UINT,
    R32_SINT,
    R24G8_TYPELESS,    // depth + stencil (alias)
    D24_UNORM_S8_UINT, // depth + stencil

    R8G8_UNORM,
    R8G8_UINT,
    R8G8_SNORM,
    R8G8_SINT,
    R16_TYPELESS, // depth (alias)
    R16_FLOAT,
    D16_UNORM, // depth
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
    BC7_UNORM_SRGB
  };
};

struct EZ_RHI_DLL ezRHIFillMode
{
  typedef ezUInt8 StorageType;
  enum Enum
  {
    Wireframe,
    Solid,
  };
};


struct EZ_RHI_DLL ezRHICullMode
{
  typedef ezUInt8 StorageType;
  enum Enum
  {
    None,
    Front,
    Back,
  };
};

struct EZ_RHI_DLL ezRHIInputClassification
{
  typedef ezUInt32 StorageType;
  enum Enum
  {
    PerVertexData,
    PerInstanceData,
  };
};

struct EZ_RHI_DLL ezRHITextureAddressMode
{
  typedef ezUInt32 StorageType;
  enum Enum
  {
    Wrap,
    Mirror,
    Clamp,
    Border,
  };
};

struct EZ_RHI_DLL ezRHIFilter
{
  typedef ezUInt8 StorageType;
  enum Enum
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



/*
struct EZ_RHI_DLL ezRHIStruct
{
  typedef ezUInt8 StorageType;
  enum Enum
  {
    ENUM_COUNT
  };
  struct Bits { };
};
EZ_DECLARE_FLAGS_OPERATORS(_bitFlagName);
*/

/////////////////////// Not used ////////////////////////////
/*




*/
