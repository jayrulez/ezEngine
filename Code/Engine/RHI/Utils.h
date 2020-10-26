#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <RHI/Resources/DeviceBufferRange.h>

class RHITexture;

namespace RHIUtils
{
  RHIDeviceBufferRange GetBufferRange(RHIDeviceResource* resource, ezUInt32 additionalOffset);
  void GetMipDimensions(RHITexture* tex, ezUInt32 mipLevel, ezUInt32& width, ezUInt32& height, ezUInt32& depth);

  ezUInt32 GetDimension(ezUInt32 largestLevelDimension, ezUInt32 mipLevel);
} // namespace RHIUtils

namespace RHIFormatUtils
{
  ezUInt32 GetSize(ezEnum<RHIPixelFormat> format);

  ezUInt32 GetSize(ezEnum<RHIVertexElementFormat> format);

  ezInt32 GetElementCount(ezEnum<RHIVertexElementFormat> format);

  /*
  static uint GetSampleCountUInt32(TextureSampleCount sampleCount)
  {
    switch (sampleCount)
    {
      case RHITextureSampleCount::Count1:
        return 1;
      case RHITextureSampleCount::Count2:
        return 2;
      case RHITextureSampleCount::Count4:
        return 4;
      case RHITextureSampleCount::Count8:
        return 8;
      case RHITextureSampleCount::Count16:
        return 16;
      case RHITextureSampleCount::Count32:
        return 32;
      default:
        throw Illegal.Value<TextureSampleCount>();
    }
  }

  static bool IsStencilFormat(PixelFormat format)
  {
    return format == RHIPixelFormat::D24_UNorm_S8_UInt || format == RHIPixelFormat::D32_Float_S8_UInt;
  }

  internal static bool IsDepthStencilFormat(PixelFormat format)
  {
    return format == RHIPixelFormat::D32_Float_S8_UInt || format == RHIPixelFormat::D24_UNorm_S8_UInt || format == RHIPixelFormat::R16_UNorm || format == RHIPixelFormat::R32_Float;
  }
  */

  bool IsCompressedFormat(ezEnum<RHIPixelFormat> format);

  ezUInt32 GetRowPitch(ezUInt32 width, ezEnum<RHIPixelFormat> format);

  ezUInt32 GetBlockSize(ezEnum<RHIPixelFormat> format);

  bool IsFormatViewCompatible(ezEnum<RHIPixelFormat> viewFormat, ezEnum<RHIPixelFormat> realFormat);

  bool IsSrgbCounterpart(ezEnum<RHIPixelFormat> viewFormat, ezEnum<RHIPixelFormat> realFormat);


  ezUInt32 GetNumRows(ezUInt32 height, ezEnum<RHIPixelFormat> format);

  ezUInt32 GetDepthPitch(ezUInt32 rowPitch, ezUInt32 height, ezEnum<RHIPixelFormat> format);

  ezUInt32 GetRegionSize(ezUInt32 width, ezUInt32 height, ezUInt32 depth, ezEnum<RHIPixelFormat> format);

  ezEnum<RHITextureSampleCount> GetSampleCount(ezUInt32 samples);

  ezEnum<RHIPixelFormat> GetViewFamilyFormat(ezEnum<RHIPixelFormat> format);
} // namespace RHIFormatUtils
