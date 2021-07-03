
#pragma once

#include <Foundation/Reflection/Reflection.h>
#include <RHI/Renderer/Basics.h>

struct EZ_RHI_DLL ezRHIResourceFormat
{
  typedef ezUInt32 StorageType;

  enum Enum
  {
    Invalid = 0,

    RGBAFloat,
    XYZWFloat = RGBAFloat,
    RGBAUInt,
    RGBAInt,

    RGBFloat,
    XYZFloat = RGBFloat,
    UVWFloat = RGBFloat,
    RGBUInt,
    RGBInt,

    B5G6R5UNormalized,
    BGRAUByteNormalized,
    BGRAUByteNormalizedsRGB,

    RGBAHalf,
    XYZWHalf = RGBAHalf,
    RGBAUShort,
    RGBAUShortNormalized,
    RGBAShort,
    RGBAShortNormalized,

    RGFloat,
    XYFloat = RGFloat,
    UVFloat = RGFloat,
    RGUInt,
    RGInt,

    RGB10A2UInt,
    RGB10A2UIntNormalized,
    RG11B10Float,

    RGBAUByteNormalized,
    RGBAUByteNormalizedsRGB,
    RGBAUByte,
    RGBAByteNormalized,
    RGBAByte,

    RGHalf,
    XYHalf = RGHalf,
    UVHalf = RGHalf,
    RGUShort,
    RGUShortNormalized,
    RGShort,
    RGShortNormalized,
    RGUByte,
    RGUByteNormalized,
    RGByte,
    RGByteNormalized,

    DFloat,

    RFloat,
    RUInt,
    RInt,
    RHalf,
    RUShort,
    RUShortNormalized,
    RShort,
    RShortNormalized,
    RUByte,
    RUByteNormalized,
    RByte,
    RByteNormalized,

    AUByteNormalized,

    D16,
    D24S8,

    BC1,
    BC1sRGB,
    BC2,
    BC2sRGB,
    BC3,
    BC3sRGB,
    BC4UNormalized,
    BC4Normalized,
    BC5UNormalized,
    BC5Normalized,
    BC6UFloat,
    BC6Float,
    BC7UNormalized,
    BC7UNormalizedsRGB,

    ENUM_COUNT,

    Default = RGBAUByteNormalizedsRGB
  };


  // General format Meta-Informations:

  /// \brief The size in bits per element (usually pixels, except for mesh stream elements) of a single element of the given resource format.
  static ezUInt32 GetBitsPerElement(ezRHIResourceFormat::Enum format);

  /// \brief The number of color channels this format contains.
  static ezUInt8 GetChannelCount(ezRHIResourceFormat::Enum format);

  /// \todo A combination of propertyflags, something like srgb, normalized, ...
  // Would be very useful for some GL stuff and Testing.

  /// \brief Returns whether the given resource format is a depth format
  static bool IsDepthFormat(ezRHIResourceFormat::Enum format);

  static bool IsSrgb(ezRHIResourceFormat::Enum format);

private:
  static const ezUInt8 s_BitsPerElement[ezRHIResourceFormat::ENUM_COUNT];

  static const ezUInt8 s_ChannelCount[ezRHIResourceFormat::ENUM_COUNT];
};

EZ_DECLARE_REFLECTABLE_TYPE(EZ_RHI_DLL, ezRHIResourceFormat);


template <typename NativeFormatType, NativeFormatType InvalidFormat>
class ezRHIFormatLookupEntry
{
public:
  inline ezRHIFormatLookupEntry();

  inline ezRHIFormatLookupEntry(NativeFormatType Storage);

  inline ezRHIFormatLookupEntry<NativeFormatType, InvalidFormat>& RT(NativeFormatType RenderTargetType);

  inline ezRHIFormatLookupEntry<NativeFormatType, InvalidFormat>& D(NativeFormatType DepthOnlyType);

  inline ezRHIFormatLookupEntry<NativeFormatType, InvalidFormat>& S(NativeFormatType StencilOnlyType);

  inline ezRHIFormatLookupEntry<NativeFormatType, InvalidFormat>& DS(NativeFormatType DepthStencilType);

  inline ezRHIFormatLookupEntry<NativeFormatType, InvalidFormat>& VA(NativeFormatType VertexAttributeType);

  inline ezRHIFormatLookupEntry<NativeFormatType, InvalidFormat>& RV(NativeFormatType ResourceViewType);

  NativeFormatType m_eStorage;
  NativeFormatType m_eRenderTarget;
  NativeFormatType m_eDepthOnlyType;
  NativeFormatType m_eStencilOnlyType;
  NativeFormatType m_eDepthStencilType;
  NativeFormatType m_eVertexAttributeType;
  NativeFormatType m_eResourceViewType;
};

// Reusable table class to store lookup information (from ezRHIResourceFormat to the various formats for texture/buffer storage, views)
template <typename FormatClass>
class ezRHIFormatLookupTable
{
public:
  ezRHIFormatLookupTable();

  EZ_ALWAYS_INLINE const FormatClass& GetFormatInfo(ezRHIResourceFormat::Enum eFormat) const;

  EZ_ALWAYS_INLINE void SetFormatInfo(ezRHIResourceFormat::Enum eFormat, const FormatClass& NewFormatInfo);

private:
  FormatClass m_Formats[ezRHIResourceFormat::ENUM_COUNT];
};

#include <RHI/Renderer/Resources/Implementation/ResourceFormats_inl.h>
