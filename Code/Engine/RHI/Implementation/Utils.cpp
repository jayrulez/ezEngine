#include <RHI/Resources/Texture.h>
#include <RHI/Utils.h>

namespace RHIUtils
{
  RHIDeviceBufferRange GetBufferRange(RHIDeviceResource* resource, ezUInt32 additionalOffset)
  {

#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    EZ_VERIFY(resource->GetFlags().IsSet(RHIDeviceResourceFlags::Bindable), "Resource is not bindable.");
#endif

    RHIDeviceBufferRange* range = dynamic_cast<RHIDeviceBufferRange*>(resource);
    if (range)
    {
      return RHIDeviceBufferRange(range->Buffer, range->Offset + additionalOffset, range->Size);
    }
    else
    {
      RHIDeviceBuffer* buffer = dynamic_cast<RHIDeviceBuffer*>(resource);

      return RHIDeviceBufferRange(buffer, additionalOffset, buffer->GetSize());
    }
  }

  void GetMipDimensions(RHITexture* tex, ezUInt32 mipLevel, ezUInt32& width, ezUInt32& height, ezUInt32& depth)
  {
    width = GetDimension(tex->GetWidth(), mipLevel);
    height = GetDimension(tex->GetHeight(), mipLevel);
    depth = GetDimension(tex->GetDepth(), mipLevel);
  }

} // namespace RHIUtils

namespace RHIFormatUtils
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
} // namespace RHIFormatUtils
