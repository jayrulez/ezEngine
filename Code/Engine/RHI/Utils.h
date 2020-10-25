#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/DeviceBuffer.h>
#include <RHI/Resources/DeviceBufferRange.h>
#include <RHI/Resources/DeviceResource.h>

class RHITexture;

namespace RHIUtils
{
  static RHIDeviceBufferRange GetBufferRange(RHIDeviceResource* resource, ezUInt32 additionalOffset);
  static void GetMipDimensions(RHITexture* tex, ezUInt32 mipLevel, ezUInt32& width, ezUInt32& height, ezUInt32& depth);

  static ezUInt32 GetDimension(ezUInt32 largestLevelDimension, ezUInt32 mipLevel)
  {
    ezUInt32 ret = largestLevelDimension;
    for (ezUInt32 i = 0; i < mipLevel; i++)
    {
      ret /= 2;
    }

    return ezMath::Max((ezUInt32)1, ret);
  }
} // namespace RHIUtils

namespace RHIFormatUtils
{
  static ezUInt32 GetSize(ezEnum<RHIPixelFormat> format);

  /*
  uint GetSizeInBytes(VertexElementFormat format)
  {
    switch (format)
    {
      case VertexElementFormat.Byte2_Norm:
      case VertexElementFormat.Byte2:
      case VertexElementFormat.SByte2_Norm:
      case VertexElementFormat.SByte2:
      case VertexElementFormat.Half1:
        return 2;
      case VertexElementFormat.Float1:
      case VertexElementFormat.UInt1:
      case VertexElementFormat.Int1:
      case VertexElementFormat.Byte4_Norm:
      case VertexElementFormat.Byte4:
      case VertexElementFormat.SByte4_Norm:
      case VertexElementFormat.SByte4:
      case VertexElementFormat.UShort2_Norm:
      case VertexElementFormat.UShort2:
      case VertexElementFormat.Short2_Norm:
      case VertexElementFormat.Short2:
      case VertexElementFormat.Half2:
        return 4;
      case VertexElementFormat.Float2:
      case VertexElementFormat.UInt2:
      case VertexElementFormat.Int2:
      case VertexElementFormat.UShort4_Norm:
      case VertexElementFormat.UShort4:
      case VertexElementFormat.Short4_Norm:
      case VertexElementFormat.Short4:
      case VertexElementFormat.Half4:
        return 8;
      case VertexElementFormat.Float3:
      case VertexElementFormat.UInt3:
      case VertexElementFormat.Int3:
        return 12;
      case VertexElementFormat.Float4:
      case VertexElementFormat.UInt4:
      case VertexElementFormat.Int4:
        return 16;
      default:
        throw Illegal.Value<VertexElementFormat>();
    }
  }

  static int GetElementCount(VertexElementFormat format)
  {
    switch (format)
    {
      case VertexElementFormat.Float1:
      case VertexElementFormat.UInt1:
      case VertexElementFormat.Int1:
      case VertexElementFormat.Half1:
        return 1;
      case VertexElementFormat.Float2:
      case VertexElementFormat.Byte2_Norm:
      case VertexElementFormat.Byte2:
      case VertexElementFormat.SByte2_Norm:
      case VertexElementFormat.SByte2:
      case VertexElementFormat.UShort2_Norm:
      case VertexElementFormat.UShort2:
      case VertexElementFormat.Short2_Norm:
      case VertexElementFormat.Short2:
      case VertexElementFormat.UInt2:
      case VertexElementFormat.Int2:
      case VertexElementFormat.Half2:
        return 2;
      case VertexElementFormat.Float3:
      case VertexElementFormat.UInt3:
      case VertexElementFormat.Int3:
        return 3;
      case VertexElementFormat.Float4:
      case VertexElementFormat.Byte4_Norm:
      case VertexElementFormat.Byte4:
      case VertexElementFormat.SByte4_Norm:
      case VertexElementFormat.SByte4:
      case VertexElementFormat.UShort4_Norm:
      case VertexElementFormat.UShort4:
      case VertexElementFormat.Short4_Norm:
      case VertexElementFormat.Short4:
      case VertexElementFormat.UInt4:
      case VertexElementFormat.Int4:
      case VertexElementFormat.Half4:
        return 4;
      default:
        throw Illegal.Value<VertexElementFormat>();
    }
  }

  static uint GetSampleCountUInt32(TextureSampleCount sampleCount)
  {
    switch (sampleCount)
    {
      case TextureSampleCount.Count1:
        return 1;
      case TextureSampleCount.Count2:
        return 2;
      case TextureSampleCount.Count4:
        return 4;
      case TextureSampleCount.Count8:
        return 8;
      case TextureSampleCount.Count16:
        return 16;
      case TextureSampleCount.Count32:
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

  static bool IsCompressedFormat(ezEnum<RHIPixelFormat> format);

  /*
  internal static uint GetRowPitch(uint width, PixelFormat format)
  {
    switch (format)
    {
      case RHIPixelFormat::BC1_Rgb_UNorm:
      case RHIPixelFormat::BC1_Rgb_UNorm_SRgb:
      case RHIPixelFormat::BC1_Rgba_UNorm:
      case RHIPixelFormat::BC1_Rgba_UNorm_SRgb:
      case RHIPixelFormat::BC2_UNorm:
      case RHIPixelFormat::BC2_UNorm_SRgb:
      case RHIPixelFormat::BC3_UNorm:
      case RHIPixelFormat::BC3_UNorm_SRgb:
      case RHIPixelFormat::BC4_UNorm:
      case RHIPixelFormat::BC4_SNorm:
      case RHIPixelFormat::BC5_UNorm:
      case RHIPixelFormat::BC5_SNorm:
      case RHIPixelFormat::BC7_UNorm:
      case RHIPixelFormat::BC7_UNorm_SRgb:
      case RHIPixelFormat::ETC2_R8_G8_B8_UNorm:
      case RHIPixelFormat::ETC2_R8_G8_B8_A1_UNorm:
      case RHIPixelFormat::ETC2_R8_G8_B8_A8_UNorm:
        var blocksPerRow = (width + 3) / 4;
        var blockSizeInBytes = GetBlockSizeInBytes(format);
        return blocksPerRow * blockSizeInBytes;

      default:
        return width * GetSizeInBytes(format);
    }
  }

  static uint GetBlockSizeInBytes(PixelFormat format)
  {
    switch (format)
    {
      case RHIPixelFormat::BC1_Rgb_UNorm:
      case RHIPixelFormat::BC1_Rgb_UNorm_SRgb:
      case RHIPixelFormat::BC1_Rgba_UNorm:
      case RHIPixelFormat::BC1_Rgba_UNorm_SRgb:
      case RHIPixelFormat::BC4_UNorm:
      case RHIPixelFormat::BC4_SNorm:
      case RHIPixelFormat::ETC2_R8_G8_B8_UNorm:
      case RHIPixelFormat::ETC2_R8_G8_B8_A1_UNorm:
        return 8;
      case RHIPixelFormat::BC2_UNorm:
      case RHIPixelFormat::BC2_UNorm_SRgb:
      case RHIPixelFormat::BC3_UNorm:
      case RHIPixelFormat::BC3_UNorm_SRgb:
      case RHIPixelFormat::BC5_UNorm:
      case RHIPixelFormat::BC5_SNorm:
      case RHIPixelFormat::BC7_UNorm:
      case RHIPixelFormat::BC7_UNorm_SRgb:
      case RHIPixelFormat::ETC2_R8_G8_B8_A8_UNorm:
        return 16;
      default:
        throw Illegal.Value<PixelFormat>();
    }
  }

  static bool IsFormatViewCompatible(PixelFormat viewFormat, PixelFormat realFormat)
  {
    if (IsCompressedFormat(realFormat))
    {
      return IsSrgbCounterpart(viewFormat, realFormat);
    }
    else
    {
      return GetViewFamilyFormat(viewFormat) == GetViewFamilyFormat(realFormat);
    }
  }

  static bool IsSrgbCounterpart(PixelFormat viewFormat, PixelFormat realFormat)
  {
    throw new NotImplementedException();
  }

  static uint GetNumRows(uint height, PixelFormat format)
  {
    switch (format)
    {
      case RHIPixelFormat::BC1_Rgb_UNorm:
      case RHIPixelFormat::BC1_Rgb_UNorm_SRgb:
      case RHIPixelFormat::BC1_Rgba_UNorm:
      case RHIPixelFormat::BC1_Rgba_UNorm_SRgb:
      case RHIPixelFormat::BC2_UNorm:
      case RHIPixelFormat::BC2_UNorm_SRgb:
      case RHIPixelFormat::BC3_UNorm:
      case RHIPixelFormat::BC3_UNorm_SRgb:
      case RHIPixelFormat::BC4_UNorm:
      case RHIPixelFormat::BC4_SNorm:
      case RHIPixelFormat::BC5_UNorm:
      case RHIPixelFormat::BC5_SNorm:
      case RHIPixelFormat::BC7_UNorm:
      case RHIPixelFormat::BC7_UNorm_SRgb:
      case RHIPixelFormat::ETC2_R8_G8_B8_UNorm:
      case RHIPixelFormat::ETC2_R8_G8_B8_A1_UNorm:
      case RHIPixelFormat::ETC2_R8_G8_B8_A8_UNorm:
        return (height + 3) / 4;

      default:
        return height;
    }
  }

  static uint GetDepthPitch(uint rowPitch, uint height, PixelFormat format)
  {
    return rowPitch * GetNumRows(height, format);
  }

  static uint GetRegionSize(uint width, uint height, uint depth, PixelFormat format)
  {
    uint blockSizeInBytes;
    if (IsCompressedFormat(format))
    {
      Debug.Assert((width % 4 == 0 || width < 4) && (height % 4 == 0 || height < 4));
      blockSizeInBytes = GetBlockSizeInBytes(format);
      width /= 4;
      height /= 4;
    }
    else
    {
      blockSizeInBytes = GetSizeInBytes(format);
    }

    return width * height * depth * blockSizeInBytes;
  }

  static TextureSampleCount GetSampleCount(uint samples)
  {
    switch (samples)
    {
      case 1:
        return TextureSampleCount.Count1;
      case 2:
        return TextureSampleCount.Count2;
      case 4:
        return TextureSampleCount.Count4;
      case 8:
        return TextureSampleCount.Count8;
      case 16:
        return TextureSampleCount.Count16;
      case 32:
        return TextureSampleCount.Count32;
      default:
        throw new VeldridException("Unsupported multisample count: " + samples);
    }
  }

  static PixelFormat GetViewFamilyFormat(PixelFormat format)
  {
    switch (format)
    {
      case RHIPixelFormat::R32_G32_B32_A32_Float:
      case RHIPixelFormat::R32_G32_B32_A32_UInt:
      case RHIPixelFormat::R32_G32_B32_A32_SInt:
        return RHIPixelFormat::R32_G32_B32_A32_Float;
      case RHIPixelFormat::R16_G16_B16_A16_Float:
      case RHIPixelFormat::R16_G16_B16_A16_UNorm:
      case RHIPixelFormat::R16_G16_B16_A16_UInt:
      case RHIPixelFormat::R16_G16_B16_A16_SNorm:
      case RHIPixelFormat::R16_G16_B16_A16_SInt:
        return RHIPixelFormat::R16_G16_B16_A16_Float;
      case RHIPixelFormat::R32_G32_Float:
      case RHIPixelFormat::R32_G32_UInt:
      case RHIPixelFormat::R32_G32_SInt:
        return RHIPixelFormat::R32_G32_Float;
      case RHIPixelFormat::R10_G10_B10_A2_UNorm:
      case RHIPixelFormat::R10_G10_B10_A2_UInt:
        return RHIPixelFormat::R10_G10_B10_A2_UNorm;
      case RHIPixelFormat::R8_G8_B8_A8_UNorm:
      case RHIPixelFormat::R8_G8_B8_A8_UNorm_SRgb:
      case RHIPixelFormat::R8_G8_B8_A8_UInt:
      case RHIPixelFormat::R8_G8_B8_A8_SNorm:
      case RHIPixelFormat::R8_G8_B8_A8_SInt:
        return RHIPixelFormat::R8_G8_B8_A8_UNorm;
      case RHIPixelFormat::R16_G16_Float:
      case RHIPixelFormat::R16_G16_UNorm:
      case RHIPixelFormat::R16_G16_UInt:
      case RHIPixelFormat::R16_G16_SNorm:
      case RHIPixelFormat::R16_G16_SInt:
        return RHIPixelFormat::R16_G16_Float;
      case RHIPixelFormat::R32_Float:
      case RHIPixelFormat::R32_UInt:
      case RHIPixelFormat::R32_SInt:
        return RHIPixelFormat::R32_Float;
      case RHIPixelFormat::R8_G8_UNorm:
      case RHIPixelFormat::R8_G8_UInt:
      case RHIPixelFormat::R8_G8_SNorm:
      case RHIPixelFormat::R8_G8_SInt:
        return RHIPixelFormat::R8_G8_UNorm;
      case RHIPixelFormat::R16_Float:
      case RHIPixelFormat::R16_UNorm:
      case RHIPixelFormat::R16_UInt:
      case RHIPixelFormat::R16_SNorm:
      case RHIPixelFormat::R16_SInt:
        return RHIPixelFormat::R16_Float;
      case RHIPixelFormat::R8_UNorm:
      case RHIPixelFormat::R8_UInt:
      case RHIPixelFormat::R8_SNorm:
      case RHIPixelFormat::R8_SInt:
        return RHIPixelFormat::R8_UNorm;
      case RHIPixelFormat::BC1_Rgba_UNorm:
      case RHIPixelFormat::BC1_Rgba_UNorm_SRgb:
      case RHIPixelFormat::BC1_Rgb_UNorm:
      case RHIPixelFormat::BC1_Rgb_UNorm_SRgb:
        return RHIPixelFormat::BC1_Rgba_UNorm;
      case RHIPixelFormat::BC2_UNorm:
      case RHIPixelFormat::BC2_UNorm_SRgb:
        return RHIPixelFormat::BC2_UNorm;
      case RHIPixelFormat::BC3_UNorm:
      case RHIPixelFormat::BC3_UNorm_SRgb:
        return RHIPixelFormat::BC3_UNorm;
      case RHIPixelFormat::BC4_UNorm:
      case RHIPixelFormat::BC4_SNorm:
        return RHIPixelFormat::BC4_UNorm;
      case RHIPixelFormat::BC5_UNorm:
      case RHIPixelFormat::BC5_SNorm:
        return RHIPixelFormat::BC5_UNorm;
      case RHIPixelFormat::B8_G8_R8_A8_UNorm:
      case RHIPixelFormat::B8_G8_R8_A8_UNorm_SRgb:
        return RHIPixelFormat::B8_G8_R8_A8_UNorm;
      case RHIPixelFormat::BC7_UNorm:
      case RHIPixelFormat::BC7_UNorm_SRgb:
        return RHIPixelFormat::BC7_UNorm;
      default:
        return format;
    }
    */
} // namespace RHIFormatUtils
