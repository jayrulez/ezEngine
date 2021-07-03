
#pragma once

#include <Foundation/Algorithm/HashableStruct.h>
#include <Foundation/Containers/HybridArray.h>
#include <Foundation/Containers/StaticArray.h>
#include <Foundation/Math/Color.h>
#include <Foundation/Types/RefCounted.h>
#include <RHI/Renderer/Basics.h>
#include <RHI/Renderer/Resources/ResourceFormats.h>
#include <RHI/Renderer/Shader/ShaderByteCode.h>
#include <Texture/Image/ImageEnums.h>

#include <RHI/Instance/BaseTypes.h>

class ezWindowBase;

struct ezRHISwapChainCreationDescription : public ezHashableStruct<ezRHISwapChainCreationDescription>
{
  ezWindowBase* m_pWindow = nullptr;

  // Describes the format that should be used for the backbuffer.
  // Note however, that different platforms may enforce restrictions on this.
  ezRHIMSAASampleCount::Enum m_SampleCount = ezRHIMSAASampleCount::None;
  ezRHIResourceFormat::Enum m_BackBufferFormat = ezRHIResourceFormat::RGBAUByteNormalizedsRGB;

  bool m_bDoubleBuffered = true;
  bool m_bAllowScreenshots = false;
};

struct ezRHIRenderDeviceCreationDescription
{
  ezRHISwapChainCreationDescription m_PrimarySwapChainDescription;
  bool m_bDebugDevice = false;
  bool m_bCreatePrimarySwapChain = true;

  ApiType m_eApiType = ApiType::kVulkan;
  bool m_bVsync = true;
  bool m_bRoundFPS = false;
  ezUInt32 m_uiRequiredGpuIndex = 0;
  ezUInt32 m_uiFrameCount = 3;
};

struct ezRHIShaderCreationDescription : public ezHashableStruct<ezRHIShaderCreationDescription>
{
  ezRHIShaderCreationDescription();
  ~ezRHIShaderCreationDescription();

  bool HasByteCodeForStage(ezRHIShaderStage::Enum Stage) const;

  ezScopedRefPointer<ezRHIShaderByteCode> m_ByteCodes[ezRHIShaderStage::ENUM_COUNT];
};

struct ezRHIRenderTargetBlendDescription : public ezHashableStruct<ezRHIRenderTargetBlendDescription>
{
  ezRHIBlend::Enum m_SourceBlend = ezRHIBlend::One;
  ezRHIBlend::Enum m_DestBlend = ezRHIBlend::One;
  ezRHIBlendOp::Enum m_BlendOp = ezRHIBlendOp::Add;

  ezRHIBlend::Enum m_SourceBlendAlpha = ezRHIBlend::One;
  ezRHIBlend::Enum m_DestBlendAlpha = ezRHIBlend::One;
  ezRHIBlendOp::Enum m_BlendOpAlpha = ezRHIBlendOp::Add;

  ezUInt8 m_uiWriteMask = 0xFF;    ///< Enables writes to color channels. Bit1 = Red Channel, Bit2 = Green Channel, Bit3 = Blue Channel, Bit4 = Alpha
                                   ///< Channel, Bit 5-8 are unused
  bool m_bBlendingEnabled = false; ///< If enabled, the color will be blended into the render target. Otherwise it will overwrite the render target.
                                   ///< Set m_uiWriteMask to 0 to disable all writes to the render target.
};

struct ezRHIBlendStateCreationDescription : public ezHashableStruct<ezRHIBlendStateCreationDescription>
{
  ezRHIRenderTargetBlendDescription m_RenderTargetBlendDescriptions[EZ_RHI_MAX_RENDERTARGET_COUNT];

  bool m_bAlphaToCoverage = false;  ///< Alpha-to-coverage can only be used with MSAA render targets. Default is false.
  bool m_bIndependentBlend = false; ///< If disabled, the blend state of the first render target is used for all render targets. Otherwise each
                                    ///< render target uses a different blend state.
};

struct ezRHIStencilOpDescription : public ezHashableStruct<ezRHIStencilOpDescription>
{
  ezEnum<ezRHIStencilOp> m_FailOp = ezRHIStencilOp::Keep;
  ezEnum<ezRHIStencilOp> m_DepthFailOp = ezRHIStencilOp::Keep;
  ezEnum<ezRHIStencilOp> m_PassOp = ezRHIStencilOp::Keep;

  ezEnum<ezRHICompareFunc> m_StencilFunc = ezRHICompareFunc::Always;
};

struct ezRHIDepthStencilStateCreationDescription : public ezHashableStruct<ezRHIDepthStencilStateCreationDescription>
{
  ezRHIStencilOpDescription m_FrontFaceStencilOp;
  ezRHIStencilOpDescription m_BackFaceStencilOp;

  ezEnum<ezRHICompareFunc> m_DepthTestFunc = ezRHICompareFunc::Less;

  bool m_bSeparateFrontAndBack = false; ///< If false, DX11 will use front face values for both front & back face values, GL will not call
                                        ///< gl*Separate() funcs
  bool m_bDepthTest = true;
  bool m_bDepthWrite = true;
  bool m_bStencilTest = false;
  ezUInt8 m_uiStencilReadMask = 0xFF;
  ezUInt8 m_uiStencilWriteMask = 0xFF;
};

/// \brief Describes the settings for a new rasterizer state. See ezRHIRenderDevice::CreateRasterizerState
struct ezRHIRasterizerStateCreationDescription : public ezHashableStruct<ezRHIRasterizerStateCreationDescription>
{
  ezEnum<ezRHICullMode> m_CullMode = ezRHICullMode::Back; ///< Which sides of a triangle to cull. Default is ezRHICullMode::Back
  ezInt32 m_iDepthBias = 0;                               ///< The pixel depth bias. Default is 0
  float m_fDepthBiasClamp = 0.0f;                         ///< The pixel depth bias clamp. Default is 0
  float m_fSlopeScaledDepthBias = 0.0f;                   ///< The pixel slope scaled depth bias clamp. Default is 0
  bool m_bWireFrame = false;                              ///< Whether triangles are rendered filled or as wireframe. Default is false
  bool m_bFrontCounterClockwise = false; ///< Sets which triangle winding order defines the 'front' of a triangle. If true, the front of a triangle
                                         ///< is the one where the vertices appear in counter clockwise order. Default is false
  bool m_bScissorTest = false;
  bool m_bConservativeRasterization = false; ///< Whether conservative rasterization is enabled
};

struct ezRHISamplerStateCreationDescription : public ezHashableStruct<ezRHISamplerStateCreationDescription>
{
  ezEnum<ezRHITextureFilterMode> m_MinFilter;
  ezEnum<ezRHITextureFilterMode> m_MagFilter;
  ezEnum<ezRHITextureFilterMode> m_MipFilter;

  ezEnum<ezImageAddressMode> m_AddressU;
  ezEnum<ezImageAddressMode> m_AddressV;
  ezEnum<ezImageAddressMode> m_AddressW;

  ezEnum<ezRHICompareFunc> m_SampleCompareFunc;

  ezColor m_BorderColor = ezColor::Black;

  float m_fMipLodBias = 0.0f;
  float m_fMinMip = -1.0f;
  float m_fMaxMip = 42000.0f;

  ezUInt32 m_uiMaxAnisotropy = 4;
};

struct ezRHIVertexAttributeSemantic
{
  enum Enum
  {
    Position,
    Normal,
    Tangent,
    Color0,
    Color1,
    Color2,
    Color3,
    Color4,
    Color5,
    Color6,
    Color7,
    TexCoord0,
    TexCoord1,
    TexCoord2,
    TexCoord3,
    TexCoord4,
    TexCoord5,
    TexCoord6,
    TexCoord7,
    TexCoord8,
    TexCoord9,

    BiTangent,
    BoneIndices0,
    BoneIndices1,
    BoneWeights0,
    BoneWeights1,

    ENUM_COUNT
  };
};

struct ezRHIVertexAttribute
{
  ezRHIVertexAttribute() = default;

  ezRHIVertexAttribute(ezRHIVertexAttributeSemantic::Enum eSemantic, ezRHIResourceFormat::Enum eFormat, ezUInt16 uiOffset, ezUInt8 uiVertexBufferSlot,
    bool bInstanceData);

  ezRHIVertexAttributeSemantic::Enum m_eSemantic = ezRHIVertexAttributeSemantic::Position;
  ezRHIResourceFormat::Enum m_eFormat = ezRHIResourceFormat::XYZFloat;
  ezUInt16 m_uiOffset = 0;
  ezUInt8 m_uiVertexBufferSlot = 0;
  bool m_bInstanceData = false;
};

struct EZ_RHI_DLL ezRHIVertexDeclarationCreationDescription : public ezHashableStruct<ezRHIVertexDeclarationCreationDescription>
{
  ezRHIShaderHandle m_hShader;
  ezStaticArray<ezRHIVertexAttribute, 16> m_VertexAttributes;
};

struct ezRHIResourceAccess
{
  EZ_ALWAYS_INLINE bool IsImmutable() const { return m_bImmutable; }

  bool m_bReadBack = false;
  bool m_bImmutable = true;
};

struct ezRHIBufferType
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    Generic = 0,
    VertexBuffer,
    IndexBuffer,
    ConstantBuffer,

    ENUM_COUNT,

    Default = Generic
  };
};

struct ezRHIBufferCreationDescription : public ezHashableStruct<ezRHIBufferCreationDescription>
{
  ezUInt32 m_uiStructSize = 0;
  ezUInt32 m_uiTotalSize = 0;

  ezEnum<ezRHIBufferType> m_BufferType = ezRHIBufferType::Generic;

  bool m_bUseForIndirectArguments = false;
  bool m_bUseAsStructuredBuffer = false;
  bool m_bAllowRawViews = false;
  bool m_bStreamOutputTarget = false;
  bool m_bAllowShaderResourceView = false;
  bool m_bAllowUAV = false;

  ezRHIResourceAccess m_ResourceAccess;
};

struct ezRHITextureCreationDescription : public ezHashableStruct<ezRHITextureCreationDescription>
{
  void SetAsRenderTarget(
    ezUInt32 uiWidth, ezUInt32 uiHeight, ezRHIResourceFormat::Enum format, ezRHIMSAASampleCount::Enum sampleCount = ezRHIMSAASampleCount::None);

  ezUInt32 m_uiWidth = 0;
  ezUInt32 m_uiHeight = 0;
  ezUInt32 m_uiDepth = 1;

  ezUInt32 m_uiMipLevelCount = 1;

  ezUInt32 m_uiArraySize = 1;

  ezEnum<ezRHIResourceFormat> m_Format = ezRHIResourceFormat::Invalid;

  ezEnum<ezRHIMSAASampleCount> m_SampleCount = ezRHIMSAASampleCount::None;

  ezEnum<ezRHITextureType> m_Type = ezRHITextureType::Texture2D;

  bool m_bAllowShaderResourceView = true;
  bool m_bAllowUAV = false;
  bool m_bCreateRenderTarget = false;
  bool m_bAllowDynamicMipGeneration = false;

  ezRHIResourceAccess m_ResourceAccess;

  void* m_pExisitingNativeObject = nullptr; ///< Can be used to encapsulate existing native textures in objects usable by the GAL
};

struct ezRHIResourceViewCreationDescription : public ezHashableStruct<ezRHIResourceViewCreationDescription>
{
  ezRHITextureHandle m_hTexture;

  ezRHIBufferHandle m_hBuffer;

  ezEnum<ezRHIResourceFormat> m_OverrideViewFormat = ezRHIResourceFormat::Invalid;

  ezUInt32 m_uiMostDetailedMipLevel = 0;
  ezUInt32 m_uiMipLevelsToUse = 0xFFFFFFFFu;

  ezUInt32 m_uiFirstArraySlice = 0; // For cubemap array: index of first 2d slice to start with
  ezUInt32 m_uiArraySize = 1;       // For cubemap array: number of cubemaps

  // Buffer only
  ezUInt32 m_uiFirstElement = 0;
  ezUInt32 m_uiNumElements = 0;
  bool m_bRawView = false;
};

struct ezRHIRenderTargetViewCreationDescription : public ezHashableStruct<ezRHIRenderTargetViewCreationDescription>
{
  ezRHITextureHandle m_hTexture;

  ezEnum<ezRHIResourceFormat> m_OverrideViewFormat = ezRHIResourceFormat::Invalid;

  ezUInt32 m_uiMipLevel = 0;

  ezUInt32 m_uiFirstSlice = 0;
  ezUInt32 m_uiSliceCount = 1;

  bool m_bReadOnly = false; ///< Can be used for depth stencil views to create read only views (e.g. for soft particles using the native depth buffer)
};

struct ezRHIUnorderedAccessViewCreationDescription : public ezHashableStruct<ezRHIUnorderedAccessViewCreationDescription>
{
  ezRHITextureHandle m_hTexture;

  ezRHIBufferHandle m_hBuffer;

  ezEnum<ezRHIResourceFormat> m_OverrideViewFormat = ezRHIResourceFormat::Invalid;

  // Texture only
  ezUInt32 m_uiMipLevelToUse = 0;   ///< Which MipLevel is accessed with this UAV
  ezUInt32 m_uiFirstArraySlice = 0; ///< First depth slice for 3D Textures.
  ezUInt32 m_uiArraySize = 1;       ///< Number of depth slices for 3D textures.

  // Buffer only
  ezUInt32 m_uiFirstElement = 0;
  ezUInt32 m_uiNumElements = 0;
  bool m_bRawView = false;
  bool m_bAppend = false; // Allows appending data to the end of the buffer.
};

struct ezRHIQueryType
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    /// Number of samples that passed the depth and stencil test between begin and end (on a context).
    NumSamplesPassed,
    /// Boolean version of NumSamplesPassed.
    AnySamplesPassed,

    Default = NumSamplesPassed

    // Note:
    // GALFence provides an implementation of "event queries".
  };
};

struct ezRHIQueryCreationDescription : public ezHashableStruct<ezRHIQueryCreationDescription>
{
  ezEnum<ezRHIQueryType> m_type = ezRHIQueryType::NumSamplesPassed;

  /// In case this query is used for occlusion culling (type AnySamplesPassed), this determines whether drawing should be done if the query
  /// status is still unknown.
  bool m_bDrawIfUnknown = true;
};

/// \brief Type for important GAL events.
struct ezRHIRenderDeviceEvent
{
  enum Type
  {
    AfterInit,
    BeforeShutdown,
    BeforeBeginFrame,
    AfterBeginFrame,
    BeforeEndFrame,
    AfterEndFrame,
    // could add resource creation/destruction events, if this would be useful
  };

  Type m_Type;
  class ezRHIRenderDevice* m_pDevice;
};

#include <RHI/Renderer/Descriptors/Implementation/Descriptors_inl.h>
