#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Types/Id.h>
#include <Foundation/Types/RefCounted.h>

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
    FILL_WIREFRAME,
    FILL_SOLID,
  };
};


struct EZ_RHI_DLL ezRHICullMode
{
  typedef ezUInt8 StorageType;
  enum Enum
  {
    CULL_NONE,
    CULL_FRONT,
    CULL_BACK,
  };
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
