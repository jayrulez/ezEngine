#pragma once
#include <RHI/RHIDLL.h>

#include <Foundation/Basics.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Types/Id.h>
#include <Foundation/Types/RefCounted.h>

// Necessary array sizes
#define EZ_GAL_MAX_CONSTANT_BUFFER_COUNT 16
#define EZ_GAL_MAX_SAMPLER_COUNT 16
#define EZ_GAL_MAX_VERTEX_BUFFER_COUNT 16
#define EZ_GAL_MAX_RENDERTARGET_COUNT 8

// Forward declarations

struct ezRHIDeviceCreationDescription;
struct ezRHISwapChainCreationDescription;
struct ezRHIShaderCreationDescription;
struct ezRHITextureCreationDescription;
struct ezRHIBufferCreationDescription;
struct ezRHIDepthStencilStateCreationDescription;
struct ezRHIBlendStateCreationDescription;
struct ezRHIRasterizerStateCreationDescription;
struct ezRHIVertexDeclarationCreationDescription;
struct ezRHIQueryCreationDescription;
struct ezRHISamplerStateCreationDescription;
struct ezRHIResourceViewCreationDescription;
struct ezRHIRenderTargetViewCreationDescription;
struct ezRHIUnorderedAccessViewCreationDescription;

class ezRHISwapChain;
class ezRHIShader;
class ezRHIResourceBase;
class ezRHITexture;
class ezRHIBuffer;
class ezRHIDepthStencilState;
class ezRHIBlendState;
class ezRHIRasterizerState;
class ezRHIRenderTargetSetup;
class ezRHIVertexDeclaration;
class ezRHIFence;
class ezRHIQuery;
class ezRHISamplerState;
class ezRHIResourceView;
class ezRHIRenderTargetView;
class ezRHIUnorderedAccessView;
class ezRHIDevice;
class ezRHIPass;
class ezRHICommandEncoder;
class ezRHIRenderCommandEncoder;
class ezRHIComputeCommandEncoder;

// Basic enums
struct ezRHIPrimitiveTopology
{
  enum Enum
  {
    // keep this order, it is used to allocate the desired number of indices in ezMeshBufferResourceDescriptor::AllocateStreams
    Points,    // 1 index per primitive
    Lines,     // 2 indices per primitive
    Triangles, // 3 indices per primitive

    ENUM_COUNT
  };

  static ezUInt32 VerticesPerPrimitive(ezRHIPrimitiveTopology::Enum e) { return (ezUInt32)e + 1; }
};

struct EZ_RHI_DLL ezRHIIndexType
{
  enum Enum
  {
    UShort,
    UInt,

    ENUM_COUNT
  };


  /// \brief The size in bytes of a single element of the given index format.
  static ezUInt8 GetSize(ezRHIIndexType::Enum format) { return Size[format]; }

private:
  static const ezUInt8 Size[ezRHIIndexType::ENUM_COUNT];
};


struct EZ_RHI_DLL ezRHIShaderStage
{
  enum Enum
  {
    VertexShader,
    HullShader,
    DomainShader,
    GeometryShader,
    PixelShader,

    ComputeShader,

    ENUM_COUNT
  };

  static const char* Names[ENUM_COUNT];
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
    Invalid = -1,
    Texture2D = 0,
    TextureCube,
    Texture3D,
    Texture2DProxy,

    ENUM_COUNT,

    Default = Texture2D
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

/// \brief Defines which sides of a polygon gets culled by the graphics card
struct ezRHICullMode
{
  typedef ezUInt8 StorageType;

  /// \brief Defines which sides of a polygon gets culled by the graphics card
  enum Enum
  {
    None = 0,  ///< Triangles do not get culled
    Front = 1, ///< When the 'front' of a triangle is visible, it gets culled. The rasterizer state defines which side is the 'front'. See
               ///< ezRHIRasterizerStateCreationDescription for details.
    Back = 2,  ///< When the 'back'  of a triangle is visible, it gets culled. The rasterizer state defines which side is the 'front'. See
               ///< ezRHIRasterizerStateCreationDescription for details.

    ENUM_COUNT,

    Default = Back
  };
};

struct ezRHITextureFilterMode
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    Point = 0,
    Linear,
    Anisotropic,

    Default = Linear
  };
};

struct ezRHIUpdateMode
{
  enum Enum
  {
    Discard,
    NoOverwrite,
    CopyToTempStorage
  };
};

// Basic structs
struct ezRHITextureSubresource
{
  ezUInt32 m_uiMipLevel = 0;
  ezUInt32 m_uiArraySlice = 0;
};

struct ezRHISystemMemoryDescription
{
  void* m_pData = nullptr;
  ezUInt32 m_uiRowPitch = 0;
  ezUInt32 m_uiSlicePitch = 0;
};

/// \brief Base class for GAL objects, stores a creation description of the object and also allows for reference counting.
template <typename CreationDescription>
class ezRHIObject : public ezRefCounted
{
public:
  ezRHIObject(const CreationDescription& Description)
    : m_Description(Description)
  {
  }

  EZ_ALWAYS_INLINE const CreationDescription& GetDescription() const { return m_Description; }

protected:
  const CreationDescription m_Description;
};

// Handles
namespace ezRHI
{
  typedef ezGenericId<16, 16> ez16_16Id;
  typedef ezGenericId<18, 14> ez18_14Id;
  typedef ezGenericId<20, 12> ez20_12Id;
} // namespace ezRHI

class ezRHISwapChainHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHISwapChainHandle, ezRHI::ez16_16Id);

  friend class ezRHIDevice;
};

class ezRHIShaderHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHIShaderHandle, ezRHI::ez18_14Id);

  friend class ezRHIDevice;
};

class ezRHITextureHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHITextureHandle, ezRHI::ez18_14Id);

  friend class ezRHIDevice;
};

class ezRHIBufferHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHIBufferHandle, ezRHI::ez18_14Id);

  friend class ezRHIDevice;
};

class ezRHIResourceViewHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHIResourceViewHandle, ezRHI::ez18_14Id);

  friend class ezRHIDevice;
};

class ezRHIUnorderedAccessViewHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHIUnorderedAccessViewHandle, ezRHI::ez18_14Id);

  friend class ezRHIDevice;
};

class ezRHIRenderTargetViewHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHIRenderTargetViewHandle, ezRHI::ez18_14Id);

  friend class ezRHIDevice;
};

class ezRHIDepthStencilStateHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHIDepthStencilStateHandle, ezRHI::ez16_16Id);

  friend class ezRHIDevice;
};

class ezRHIBlendStateHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHIBlendStateHandle, ezRHI::ez16_16Id);

  friend class ezRHIDevice;
};

class ezRHIRasterizerStateHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHIRasterizerStateHandle, ezRHI::ez16_16Id);

  friend class ezRHIDevice;
};

class ezRHISamplerStateHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHISamplerStateHandle, ezRHI::ez16_16Id);

  friend class ezRHIDevice;
};

class ezRHIVertexDeclarationHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHIVertexDeclarationHandle, ezRHI::ez18_14Id);

  friend class ezRHIDevice;
};

class ezRHIFenceHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHIFenceHandle, ezRHI::ez20_12Id);

  friend class ezRHIDevice;
};

class ezRHIQueryHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHIQueryHandle, ezRHI::ez20_12Id);

  friend class ezRHIDevice;
};

struct ezRHITimestampHandle
{
  EZ_DECLARE_POD_TYPE();

  ezUInt64 m_uiIndex;
  ezUInt64 m_uiFrameCounter;
};

namespace ezRHI
{
  struct ModifiedRange
  {
    EZ_ALWAYS_INLINE void Reset()
    {
      m_uiMin = ezInvalidIndex;
      m_uiMax = 0;
    }

    EZ_FORCE_INLINE void SetToIncludeValue(ezUInt32 value)
    {
      m_uiMin = ezMath::Min(m_uiMin, value);
      m_uiMax = ezMath::Max(m_uiMax, value);
    }

    EZ_FORCE_INLINE void SetToIncludeRange(ezUInt32 uiMin, ezUInt32 uiMax)
    {
      m_uiMin = ezMath::Min(m_uiMin, uiMin);
      m_uiMax = ezMath::Max(m_uiMax, uiMax);
    }

    EZ_ALWAYS_INLINE bool IsValid() const { return m_uiMin <= m_uiMax; }

    EZ_ALWAYS_INLINE ezUInt32 GetCount() const { return m_uiMax - m_uiMin + 1; }

    ezUInt32 m_uiMin = ezInvalidIndex;
    ezUInt32 m_uiMax = 0;
  };
} // namespace ezRHI
