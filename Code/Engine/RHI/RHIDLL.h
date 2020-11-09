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
