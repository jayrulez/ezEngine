#pragma once

#include <Foundation/Reflection/Reflection.h>
#include <RHI/RHIDLL.h>

struct EZ_RHI_DLL ezRHIResourceFormat
{
  typedef ezUInt32 StorageType;

  enum Enum
  {
    UNKNOWN,
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
    R32G8X24_TYPELESS,    // depth (32-bit) + stencil (8-bit) + shader resource (32-bit)
    D32_FLOAT_S8X24_UINT, // depth (32-bit) + stencil (8-bit)

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
    R32_TYPELESS, // depth (32-bit) + shader resource (32-bit)
    D32_FLOAT,    // depth (32-bit)
    R32_FLOAT,
    R32_UINT,
    R32_SINT,
    R24G8_TYPELESS,    // depth (24-bit) + stencil (8-bit) + shader resource (24-bit)
    D24_UNORM_S8_UINT, // depth (24-bit) + stencil (8-bit)

    R8G8_UNORM,
    R8G8_UINT,
    R8G8_SNORM,
    R8G8_SINT,
    R16_TYPELESS, // depth (16-bit) + shader resource (16-bit)
    R16_FLOAT,
    D16_UNORM, // depth (16-bit)
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
    BC7_UNORM_SRGB,

    ENUM_COUNT,

    Default = R8G8B8A8_UNORM_SRGB //RGBAUByteNormalizedsRGB
  };


  // General format Meta-Informations:

  static ezUInt32 GetFormatStride(ezRHIResourceFormat::Enum value);
  static bool IsFormatUnorm(ezRHIResourceFormat::Enum value);
  static bool IsFormatBlockCompressed(ezRHIResourceFormat::Enum value);
  static bool IsFormatStencilSupport(ezRHIResourceFormat::Enum value);
};

EZ_DECLARE_REFLECTABLE_TYPE(EZ_RHI_DLL, ezRHIResourceFormat);
