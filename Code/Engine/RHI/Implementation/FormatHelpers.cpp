#include <RHI/FormatHelpers.h>

namespace FormatHelpers
{
  ezUInt32 GetSize(ezEnum<RHIPixelFormat> format)
  {
    switch (format)
    {
      case RHIPixelFormat::R8_UNorm:
      case RHIPixelFormat::R8_SNorm:
      case RHIPixelFormat::R8_UInt:
      case RHIPixelFormat::R8_SInt:
        return 1;

      case RHIPixelFormat::R16_UNorm:
      case RHIPixelFormat::R16_SNorm:
      case RHIPixelFormat::R16_UInt:
      case RHIPixelFormat::R16_SInt:
      case RHIPixelFormat::R16_Float:
      case RHIPixelFormat::R8_G8_UNorm:
      case RHIPixelFormat::R8_G8_SNorm:
      case RHIPixelFormat::R8_G8_UInt:
      case RHIPixelFormat::R8_G8_SInt:
        return 2;

      case RHIPixelFormat::R32_UInt:
      case RHIPixelFormat::R32_SInt:
      case RHIPixelFormat::R32_Float:
      case RHIPixelFormat::R16_G16_UNorm:
      case RHIPixelFormat::R16_G16_SNorm:
      case RHIPixelFormat::R16_G16_UInt:
      case RHIPixelFormat::R16_G16_SInt:
      case RHIPixelFormat::R16_G16_Float:
      case RHIPixelFormat::R8_G8_B8_A8_UNorm:
      case RHIPixelFormat::R8_G8_B8_A8_UNorm_SRgb:
      case RHIPixelFormat::R8_G8_B8_A8_SNorm:
      case RHIPixelFormat::R8_G8_B8_A8_UInt:
      case RHIPixelFormat::R8_G8_B8_A8_SInt:
      case RHIPixelFormat::B8_G8_R8_A8_UNorm:
      case RHIPixelFormat::B8_G8_R8_A8_UNorm_SRgb:
      case RHIPixelFormat::R10_G10_B10_A2_UNorm:
      case RHIPixelFormat::R10_G10_B10_A2_UInt:
      case RHIPixelFormat::R11_G11_B10_Float:
      case RHIPixelFormat::D24_UNorm_S8_UInt:
        return 4;

      case RHIPixelFormat::D32_Float_S8_UInt:
        return 5;

      case RHIPixelFormat::R16_G16_B16_A16_UNorm:
      case RHIPixelFormat::R16_G16_B16_A16_SNorm:
      case RHIPixelFormat::R16_G16_B16_A16_UInt:
      case RHIPixelFormat::R16_G16_B16_A16_SInt:
      case RHIPixelFormat::R16_G16_B16_A16_Float:
      case RHIPixelFormat::R32_G32_UInt:
      case RHIPixelFormat::R32_G32_SInt:
      case RHIPixelFormat::R32_G32_Float:
        return 8;

      case RHIPixelFormat::R32_G32_B32_A32_Float:
      case RHIPixelFormat::R32_G32_B32_A32_UInt:
      case RHIPixelFormat::R32_G32_B32_A32_SInt:
        return 16;

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
        EZ_REPORT_FAILURE("GetSize should not be used on a compressed format. Specified '{}'.", format);
        break;
      default:
        EZ_REPORT_FAILURE("Unknown RHIPixelFormat value '{}'.", format);
        break;
    }
    return 0;
  }

  ezUInt32 GetSize(ezEnum<RHIVertexElementFormat> format)
  {
    switch (format)
    {
      case RHIVertexElementFormat::Byte2_Norm:
      case RHIVertexElementFormat::Byte2:
      case RHIVertexElementFormat::SByte2_Norm:
      case RHIVertexElementFormat::SByte2:
      case RHIVertexElementFormat::Half1:
        return 2;
      case RHIVertexElementFormat::Float1:
      case RHIVertexElementFormat::UInt1:
      case RHIVertexElementFormat::Int1:
      case RHIVertexElementFormat::Byte4_Norm:
      case RHIVertexElementFormat::Byte4:
      case RHIVertexElementFormat::SByte4_Norm:
      case RHIVertexElementFormat::SByte4:
      case RHIVertexElementFormat::UShort2_Norm:
      case RHIVertexElementFormat::UShort2:
      case RHIVertexElementFormat::Short2_Norm:
      case RHIVertexElementFormat::Short2:
      case RHIVertexElementFormat::Half2:
        return 4;
      case RHIVertexElementFormat::Float2:
      case RHIVertexElementFormat::UInt2:
      case RHIVertexElementFormat::Int2:
      case RHIVertexElementFormat::UShort4_Norm:
      case RHIVertexElementFormat::UShort4:
      case RHIVertexElementFormat::Short4_Norm:
      case RHIVertexElementFormat::Short4:
      case RHIVertexElementFormat::Half4:
        return 8;
      case RHIVertexElementFormat::Float3:
      case RHIVertexElementFormat::UInt3:
      case RHIVertexElementFormat::Int3:
        return 12;
      case RHIVertexElementFormat::Float4:
      case RHIVertexElementFormat::UInt4:
      case RHIVertexElementFormat::Int4:
        return 16;
      default:
        EZ_REPORT_FAILURE("Invalid format specified for vertex element format.");
    }
    return 0;
  }

  int GetElementCount(ezEnum<RHIVertexElementFormat> format)
  {
    switch (format)
    {
      case RHIVertexElementFormat::Float1:
      case RHIVertexElementFormat::UInt1:
      case RHIVertexElementFormat::Int1:
      case RHIVertexElementFormat::Half1:
        return 1;
      case RHIVertexElementFormat::Float2:
      case RHIVertexElementFormat::Byte2_Norm:
      case RHIVertexElementFormat::Byte2:
      case RHIVertexElementFormat::SByte2_Norm:
      case RHIVertexElementFormat::SByte2:
      case RHIVertexElementFormat::UShort2_Norm:
      case RHIVertexElementFormat::UShort2:
      case RHIVertexElementFormat::Short2_Norm:
      case RHIVertexElementFormat::Short2:
      case RHIVertexElementFormat::UInt2:
      case RHIVertexElementFormat::Int2:
      case RHIVertexElementFormat::Half2:
        return 2;
      case RHIVertexElementFormat::Float3:
      case RHIVertexElementFormat::UInt3:
      case RHIVertexElementFormat::Int3:
        return 3;
      case RHIVertexElementFormat::Float4:
      case RHIVertexElementFormat::Byte4_Norm:
      case RHIVertexElementFormat::Byte4:
      case RHIVertexElementFormat::SByte4_Norm:
      case RHIVertexElementFormat::SByte4:
      case RHIVertexElementFormat::UShort4_Norm:
      case RHIVertexElementFormat::UShort4:
      case RHIVertexElementFormat::Short4_Norm:
      case RHIVertexElementFormat::Short4:
      case RHIVertexElementFormat::UInt4:
      case RHIVertexElementFormat::Int4:
      case RHIVertexElementFormat::Half4:
        return 4;
      default:
        EZ_REPORT_FAILURE("Invalid format specified for vertex element format.");
    }
    return 0;
  }

  ezUInt32 GetSampleCount(ezEnum<RHITextureSampleCount> sampleCount)
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
        EZ_REPORT_FAILURE("Invalid SampleCount");
        return 0;
    }
  }

  bool IsStencilFormat(ezEnum<RHIPixelFormat> format)
  {
    return format == RHIPixelFormat::D24_UNorm_S8_UInt || format == RHIPixelFormat::D32_Float_S8_UInt;
  }

  bool IsDepthStencilFormat(ezEnum<RHIPixelFormat> format)
  {
    return format == RHIPixelFormat::D32_Float_S8_UInt || format == RHIPixelFormat::D24_UNorm_S8_UInt || format == RHIPixelFormat::R16_UNorm || format == RHIPixelFormat::R32_Float;
  }

  bool IsCompressedFormat(ezEnum<RHIPixelFormat> format)
  {
    return format == RHIPixelFormat::BC1_Rgb_UNorm ||
           format == RHIPixelFormat::BC1_Rgb_UNorm_SRgb ||
           format == RHIPixelFormat::BC1_Rgba_UNorm ||
           format == RHIPixelFormat::BC1_Rgba_UNorm_SRgb ||
           format == RHIPixelFormat::BC2_UNorm ||
           format == RHIPixelFormat::BC2_UNorm_SRgb ||
           format == RHIPixelFormat::BC3_UNorm ||
           format == RHIPixelFormat::BC3_UNorm_SRgb ||
           format == RHIPixelFormat::BC4_UNorm ||
           format == RHIPixelFormat::BC4_SNorm ||
           format == RHIPixelFormat::BC5_UNorm ||
           format == RHIPixelFormat::BC5_SNorm ||
           format == RHIPixelFormat::BC7_UNorm ||
           format == RHIPixelFormat::BC7_UNorm_SRgb ||
           format == RHIPixelFormat::ETC2_R8_G8_B8_UNorm ||
           format == RHIPixelFormat::ETC2_R8_G8_B8_A1_UNorm ||
           format == RHIPixelFormat::ETC2_R8_G8_B8_A8_UNorm;
  }
  ezUInt32 GetRowPitch(ezUInt32 width, ezEnum<RHIPixelFormat> format)
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
      {
        ezUInt32 blocksPerRow = (width + 3) / 4;
        ezUInt32 blockSizeInBytes = GetBlockSize(format);
        return blocksPerRow * blockSizeInBytes;
      }

      default:
      {
        return width * GetSize(format);
      }
    }
  }
  ezUInt32 GetBlockSize(ezEnum<RHIPixelFormat> format)
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
        EZ_REPORT_FAILURE("Invalid value for RHIPixelFormat.");
    }

    return 0;
  }
  bool IsFormatViewCompatible(ezEnum<RHIPixelFormat> viewFormat, ezEnum<RHIPixelFormat> realFormat)
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
  bool IsSrgbCounterpart(ezEnum<RHIPixelFormat> viewFormat, ezEnum<RHIPixelFormat> realFormat)
  {
    EZ_ASSERT_NOT_IMPLEMENTED;
    return false;
  }
  ezUInt32 GetNumRows(ezUInt32 height, ezEnum<RHIPixelFormat> format)
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
  ezUInt32 GetDepthPitch(ezUInt32 rowPitch, ezUInt32 height, ezEnum<RHIPixelFormat> format)
  {
    return rowPitch * GetNumRows(height, format);
  }

  ezUInt32 GetRegionSize(ezUInt32 width, ezUInt32 height, ezUInt32 depth, ezEnum<RHIPixelFormat> format)
  {
    ezUInt32 blockSizeInBytes;
    if (IsCompressedFormat(format))
    {
      EZ_VERIFY((width % 4 == 0 || width < 4) && (height % 4 == 0 || height < 4), "");
      blockSizeInBytes = GetBlockSize(format);
      width /= 4;
      height /= 4;
    }
    else
    {
      blockSizeInBytes = GetSize(format);
    }

    return width * height * depth * blockSizeInBytes;
  }

  ezEnum<RHITextureSampleCount> GetSampleCount(ezUInt32 samples)
  {
    switch (samples)
    {
      case 1:
        return RHITextureSampleCount::Count1;
      case 2:
        return RHITextureSampleCount::Count2;
      case 4:
        return RHITextureSampleCount::Count4;
      case 8:
        return RHITextureSampleCount::Count8;
      case 16:
        return RHITextureSampleCount::Count16;
      case 32:
        return RHITextureSampleCount::Count32;
      default:
        EZ_REPORT_FAILURE("Unsupported multisample count: {}", samples);
        return RHITextureSampleCount::Default;
    }
  }
  ezEnum<RHIPixelFormat> GetViewFamilyFormat(ezEnum<RHIPixelFormat> format)
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
  }
} // namespace FormatHelpers
