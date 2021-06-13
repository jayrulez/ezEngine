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

// Descriptor binding counts:
#define EZ_RHI_GPU_RESOURCE_HEAP_CBV_COUNT 15
#define EZ_RHI_GPU_RESOURCE_HEAP_SRV_COUNT 64
#define EZ_RHI_GPU_RESOURCE_HEAP_UAV_COUNT 16
#define EZ_RHI_GPU_SAMPLER_HEAP_COUNT 16
#define EZ_RHI_MAX_RENDERTARGET_COUNT 8

///////////////////////////////////////////TODO//////////////////////////////////////
//                PR an ezMat3x4 implementation to ez upstream                     //
/////////////////////////////////////////////////////////////////////////////////////
struct EZ_RHI_DLL ezRHIFloat3X4
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

  ezRHIFloat3X4() = default;

  ezRHIFloat3X4(const ezRHIFloat3X4&) = default;
  ezRHIFloat3X4& operator=(const ezRHIFloat3X4&) = default;

  ezRHIFloat3X4(ezRHIFloat3X4&&) = default;
  ezRHIFloat3X4& operator=(ezRHIFloat3X4&&) = default;

  constexpr ezRHIFloat3X4(float m00, float m01, float m02, float m03,
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
  explicit ezRHIFloat3X4(const float* pArray);

  float operator()(size_t Row, size_t Column) const { return m[Row][Column]; }
  float& operator()(size_t Row, size_t Column) { return m[Row][Column]; }
};

inline ezRHIFloat3X4::ezRHIFloat3X4(
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
class ezRHISwapChain;
class ezRHIShader;
class ezRHIResourceBase;
class ezRHITexture;
class ezRHIGPUBuffer;
class ezRHIDepthStencilState;
class ezRHIBlendState;
class ezRHIRasterizerState;
class ezRHIQueryHeap;
class ezRHISamplerState;
class ezRHIStaticSampler;
class ezRHIResourceView;
class ezRHIRenderTargetView;
class ezRHIUnorderedAccessView;
class ezRHIDevice;
class ezRHIRenderPass;
class ezRHICommandList;
class ezRHIPipelineState;

// Basic enums
struct ezRHIPrimitiveTopology
{
  enum Enum
  {
    Undefined,
    TriangleList,
    TriangleStrip,
    PointList,
    LineList,
    LineStrip,
    PatchList,

    ENUM_COUNT
  };
};

struct EZ_RHI_DLL ezRHIIndexType
{
  enum Enum
  {
    UShort,
    UInt,

    ENUM_COUNT
  };
};

struct EZ_RHI_DLL ezRHIShaderStage
{
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
    Lib,

    ENUM_COUNT
  };
};

struct EZ_RHI_DLL ezRHIMSAASampleCount
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    None = 1,
    TwoSamples = 2,
    FourSamples = 4,
    EightSamples = 8,

    ENUM_COUNT = 4,

    Default = None
  };
};

EZ_DECLARE_REFLECTABLE_TYPE(EZ_RHI_DLL, ezRHIMSAASampleCount);

struct ezRHITextureType
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    Texture1D,
    Texture2D,
    Texture3D,

    ENUM_COUNT,

    Default = Texture2D
  };
};

struct EZ_RHI_DLL ezRHIDepthWriteMask
{
  typedef ezUInt32 StorageType;
  enum Enum
  {
    Zero,
    All
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
};

struct ezRHIBlend
{
  enum Enum
  {
    Zero = 0,
    One,
    SrcColor,
    InvSrcColor,
    SrcAlpha,
    InvSrcAlpha,
    DestAlpha,
    InvDestAlpha,
    DestColor,
    InvDestColor,
    SrcAlphaSaturated,
    BlendFactor,
    InvBlendFactor,
    Src1Color,
    InvSrc1Color,
    Src1Alpha,
    InvSrc1Alpha,

    ENUM_COUNT
  };
};

struct ezRHIBlendOp
{
  enum Enum
  {
    Add = 0,
    Subtract,
    RevSubtract,
    Min,
    Max,

    ENUM_COUNT
  };
};

struct ezRHIStencilOp
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    Keep = 0,
    Zero,
    Replace,
    IncrementSaturated,
    DecrementSaturated,
    Invert,
    Increment,
    Decrement,

    ENUM_COUNT,

    Default = Keep
  };
};

struct ezRHICompareFunc
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    Never = 0,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always,

    ENUM_COUNT,

    Default = Never
  };
};

struct ezRHITextureAddressMode
{
  using StorageType = ezUInt8;

  enum Enum
  {
    Wrap,
    Mirror,
    Clamp,
    Border,

    ENUM_COUNT,

    Default = Wrap
  };
};

struct ezRHIFillMode
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    Wireframe,
    Solid,

    ENUM_COUNT,

    Default = Solid
  };
};

struct ezRHICullMode
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    None = 0,
    Front = 1,
    Back = 2,

    ENUM_COUNT,

    Default = Back
  };
};

struct ezRHITextureFilterMode
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    MinMagMipPoint,
    MinMagPointMipLinear,
    MinPointMagLinearMipPoint,
    MinPointMagMipLinear,
    MinLinearMagMipPoint,
    MinLinearMagPointMipLinear,
    MinMagLinearMipPoint,
    MinMagMipLinear,
    Anisotropic,
    ComparisonMinMagMipPoint,
    ComparisonMinMagPointMipLinear,
    ComparisonMinPointMagLinearMipPoint,
    ComparisonMinPointMagMipLinear,
    ComparisonMinLinearMagMipPoint,
    ComparisonMinLinearMagPointMipLinear,
    ComparisonMinMagLinearMipPoint,
    ComparisonMinMagMipLinear,
    ComparisonAnisotropic,
    MinimumMinMagMipPoint,
    MinimumMinMagPointMipLinear,
    MinimumMinPointMagLinearMipPoint,
    MinimumMinPointMagMipLinear,
    MinimumMinLinearMagMipPoint,
    MinimumMinLinearMagPointMipLinear,
    MinimumMinMagLinearMipPoint,
    MinimumMinMagMipLinear,
    MinimumAnisotropic,
    MaximumMinMagMipPoint,
    MaximumMinMagPointMipLinear,
    MaximumMinPointMagLinearMipPoint,
    MaximumMinPointMagMipLinear,
    MaximumMinLinearMagMipPoint,
    MaximumMinLinearMagPointMipLinear,
    MaximumMinMagLinearMipPoint,
    MaximumMinMagMipLinear,
    MaximumAnisotropic,

    Default = MinMagMipLinear
  };
};

/*
struct ezGALUpdateMode
{
  enum Enum
  {
    Discard,
    NoOverwrite,
    CopyToTempStorage
  };
};
*/

struct ezRHIShaderFormat
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    None,
    HLSL5,
    HLSL6,
    SpirV,

    ENUM_COUNT,

    Default = None
  };
};

struct ezRHIShaderModel
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    SM50,
    SM60,
    SM61,
    SM62,
    SM63,
    SM64,
    SM65,

    ENUM_COUNT
  };
};

struct ezRHIInputClassification
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    PerVertexData,
    PerInstanceData,

    ENUM_COUNT
  };
};

struct ezRHIGPUQueryType
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    Timestamp,
    Occlusion,
    OcclusionBinary,

    ENUM_COUNT
  };
};

struct EZ_RHI_DLL ezRHIResourceUsage
{
  typedef ezUInt8 StorageType;
  enum Enum
  {
    Default = 0,
    Immutable,
    Dynamic,
    Staging
  };
};

struct ezRHISubresourceType
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    ConstantBufferView,  // constant buffer view
    ShaderResourceView,  // shader resource view
    UnorderedAccessView, // unordered access view
    RenderTargetView,    // render target view
    DepthStencilView     // depth stencil view
  };
};

struct ezRHIBufferState
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    Undefined,                       // invalid state
    VertexBuffer,                    // vertex buffer, read only
    IndexBuffer,                     // index buffer, read only
    ConstantBuffer,                  // constant buffer, read only
    IndirectArgument,                // argument buffer to DrawIndirect() or DispatchIndirect()
    ShaderResource,                  // shader resource, read only
    ShaderResourceCompute,           // shader resource, read only, non-pixel shader
    UnorderedAccess,                 // shader resource, write enabled
    CopySrc,                         // copy from
    CopyDst,                         // copy to
    RayTracingAccelerationStructure, // shading rate control per tile

    ENUM_COUNT,

    Default = Undefined
  };
};

struct ezRHIImageLayout
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    Undefined,             // invalid state
    RenderTarget,          // render target, write enabled
    DepthStencil,          // depth stencil, write enabled
    DepthStencilReadOnly,  // depth stencil, read only
    ShaderResource,        // shader resource, read only
    ShaderResourceCompute, // shader resource, read only, non-pixel shader
    UnorderedAccess,       // shader resource, write enabled
    CopySrc,               // copy from
    CopyDest,              // copy to
    ShadingRateSrc,        // shading rate control per tile

    ENUM_COUNT,

    Default = Undefined
  };
};

struct ezRHIShadingRate
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    Rate1X1,
    Rate1X2,
    Rate2X1,
    Rate2X2,
    Rate2X4,
    Rate4X2,
    Rate4X4,

    Invalid,

    ENUM_COUNT,

    Default = Invalid
  };
};

// Flags

struct EZ_RHI_DLL ezRHIBindFlags
{
  typedef ezUInt16 StorageType;
  enum Enum
  {
    VertexBuffer = 1 << 0,
    IndexBuffer = 1 << 1,
    ConstantBuffer = 1 << 2,
    ShaderResource = 1 << 3,
    StreamOutput = 1 << 4,
    RenderTarget = 1 << 5,
    DepthStencil = 1 << 6,
    UnorderedAccess = 1 << 7,

    Default = 0
  };
  struct Bits
  {
  };
};
EZ_DECLARE_FLAGS_OPERATORS(ezRHIBindFlags);

struct EZ_RHI_DLL ezRHICPUAccessFlags
{
  typedef ezUInt8 StorageType;
  enum Enum
  {
    Write = 1 << 0,
    Read = 1 << 1,

    Default = 0
  };
  struct Bits
  {
  };
};
EZ_DECLARE_FLAGS_OPERATORS(ezRHICPUAccessFlags);

struct EZ_RHI_DLL ezRHIResourceMiscFlags
{
  typedef ezUInt8 StorageType;
  enum Enum
  {
    Shared = 1 << 0,
    TextureCube = 1 << 1,
    IndirectArgs = 1 << 2,
    BufferAllowRawViews = 1 << 3,
    BufferStructured = 1 << 4,
    Tiled = 1 << 5,
    RayTracing = 1 << 6,

    Default = 0
  };
  struct Bits
  {
  };
};
EZ_DECLARE_FLAGS_OPERATORS(ezRHIResourceMiscFlags);

struct EZ_RHI_DLL ezRHIDeviceCapabilityFlags
{
  typedef ezUInt16 StorageType;
  enum Enum
  {
    Tesselation = 1 << 0,
    Rasterization = 1 << 1,
    RasterizerOrderedViews = 1 << 2,
    UAVLoadFormatCommon = 1 << 3, // eg: R16G16B16A16_FLOAT, R8G8B8A8_UNORM and more common ones
    UAVLoadFormatR11G11B10Float = 1 << 4,
    RenderTargetAndViewportArrayIndexWithoutGeometryShader = 1 << 5,
    RayTracing = 1 << 6,
    RayTracingInline = 1 << 7,
    VariableRateShading = 1 << 9,
    VariableRateShadingTier2 = 1 << 10,
    MeshShader = 1 << 11,
    BindlessDescriptors = 1 << 12,

    Default = 0
  };
  struct Bits
  {
  };
};
EZ_DECLARE_FLAGS_OPERATORS(ezRHIDeviceCapabilityFlags);

// Basic structs
/*
struct ezGALTextureSubresource
{
  ezUInt32 m_uiMipLevel = 0;
  ezUInt32 m_uiArraySlice = 0;
};

struct ezGALSystemMemoryDescription
{
  void* m_pData = nullptr;
  ezUInt32 m_uiRowPitch = 0;
  ezUInt32 m_uiSlicePitch = 0;
};
*/

union EZ_RHI_DLL ezRHIClearValue
{
  ezColor Color;
  struct ClearDepthStencil
  {
    float Depth;
    ezUInt32 Stencil;
  } DepthStencil;
};

class EZ_RHI_DLL ezRHIObject : public ezRefCounted
{
};
