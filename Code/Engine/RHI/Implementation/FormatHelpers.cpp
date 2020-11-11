#include <RHI/FormatHelpers.h>

ezUInt32 FormatHelpers::GetFormatStride(ezEnum<ezRHIFormat> value)
{
  switch (value)
  {

    case ezRHIFormat::R32G32B32A32_FLOAT:
    case ezRHIFormat::R32G32B32A32_UINT:
    case ezRHIFormat::R32G32B32A32_SINT:
    case ezRHIFormat::BC1_UNORM:
    case ezRHIFormat::BC1_UNORM_SRGB:
    case ezRHIFormat::BC2_UNORM:
    case ezRHIFormat::BC2_UNORM_SRGB:
    case ezRHIFormat::BC3_UNORM:
    case ezRHIFormat::BC3_UNORM_SRGB:
    case ezRHIFormat::BC4_SNORM:
    case ezRHIFormat::BC4_UNORM:
    case ezRHIFormat::BC5_SNORM:
    case ezRHIFormat::BC5_UNORM:
    case ezRHIFormat::BC6H_UF16:
    case ezRHIFormat::BC6H_SF16:
    case ezRHIFormat::BC7_UNORM:
    case ezRHIFormat::BC7_UNORM_SRGB:
      return 16;

    case ezRHIFormat::R32G32B32_FLOAT:
    case ezRHIFormat::R32G32B32_UINT:
    case ezRHIFormat::R32G32B32_SINT:
      return 12;

    case ezRHIFormat::R16G16B16A16_FLOAT:
    case ezRHIFormat::R16G16B16A16_UNORM:
    case ezRHIFormat::R16G16B16A16_UINT:
    case ezRHIFormat::R16G16B16A16_SNORM:
    case ezRHIFormat::R16G16B16A16_SINT:
      return 8;

    case ezRHIFormat::R32G32_FLOAT:
    case ezRHIFormat::R32G32_UINT:
    case ezRHIFormat::R32G32_SINT:
    case ezRHIFormat::R32G8X24_TYPELESS:
    case ezRHIFormat::D32_FLOAT_S8X24_UINT:
      return 8;

    case ezRHIFormat::R10G10B10A2_UNORM:
    case ezRHIFormat::R10G10B10A2_UINT:
    case ezRHIFormat::R11G11B10_FLOAT:
    case ezRHIFormat::R8G8B8A8_UNORM:
    case ezRHIFormat::R8G8B8A8_UNORM_SRGB:
    case ezRHIFormat::R8G8B8A8_UINT:
    case ezRHIFormat::R8G8B8A8_SNORM:
    case ezRHIFormat::R8G8B8A8_SINT:
    case ezRHIFormat::B8G8R8A8_UNORM:
    case ezRHIFormat::B8G8R8A8_UNORM_SRGB:
    case ezRHIFormat::R16G16_FLOAT:
    case ezRHIFormat::R16G16_UNORM:
    case ezRHIFormat::R16G16_UINT:
    case ezRHIFormat::R16G16_SNORM:
    case ezRHIFormat::R16G16_SINT:
    case ezRHIFormat::R32_TYPELESS:
    case ezRHIFormat::D32_FLOAT:
    case ezRHIFormat::R32_FLOAT:
    case ezRHIFormat::R32_UINT:
    case ezRHIFormat::R32_SINT:
    case ezRHIFormat::R24G8_TYPELESS:
    case ezRHIFormat::D24_UNORM_S8_UINT:
      return 4;

    case ezRHIFormat::R8G8_UNORM:
    case ezRHIFormat::R8G8_UINT:
    case ezRHIFormat::R8G8_SNORM:
    case ezRHIFormat::R8G8_SINT:
    case ezRHIFormat::R16_TYPELESS:
    case ezRHIFormat::R16_FLOAT:
    case ezRHIFormat::D16_UNORM:
    case ezRHIFormat::R16_UNORM:
    case ezRHIFormat::R16_UINT:
    case ezRHIFormat::R16_SNORM:
    case ezRHIFormat::R16_SINT:
      return 2;

    case ezRHIFormat::R8_UNORM:
    case ezRHIFormat::R8_UINT:
    case ezRHIFormat::R8_SNORM:
    case ezRHIFormat::R8_SINT:
      return 1;


    default:
      EZ_REPORT_FAILURE("Unsupported format."); // didn't catch format!
      break;
  }

  return 16;
}

bool FormatHelpers::IsFormatUnorm(ezEnum<ezRHIFormat> value)
{
  switch (value)
  {
    case ezRHIFormat::R16G16B16A16_UNORM:
    case ezRHIFormat::R10G10B10A2_UNORM:
    case ezRHIFormat::R8G8B8A8_UNORM:
    case ezRHIFormat::R8G8B8A8_UNORM_SRGB:
    case ezRHIFormat::B8G8R8A8_UNORM:
    case ezRHIFormat::B8G8R8A8_UNORM_SRGB:
    case ezRHIFormat::R16G16_UNORM:
    case ezRHIFormat::D24_UNORM_S8_UINT:
    case ezRHIFormat::R8G8_UNORM:
    case ezRHIFormat::D16_UNORM:
    case ezRHIFormat::R16_UNORM:
    case ezRHIFormat::R8_UNORM:
      return true;
  }

  return false;
}

bool FormatHelpers::IsFormatBlockCompressed(ezEnum<ezRHIFormat> value)
{
  switch (value)
  {
    case ezRHIFormat::BC1_UNORM:
    case ezRHIFormat::BC1_UNORM_SRGB:
    case ezRHIFormat::BC2_UNORM:
    case ezRHIFormat::BC2_UNORM_SRGB:
    case ezRHIFormat::BC3_UNORM:
    case ezRHIFormat::BC3_UNORM_SRGB:
    case ezRHIFormat::BC4_UNORM:
    case ezRHIFormat::BC4_SNORM:
    case ezRHIFormat::BC5_UNORM:
    case ezRHIFormat::BC5_SNORM:
    case ezRHIFormat::BC6H_UF16:
    case ezRHIFormat::BC6H_SF16:
    case ezRHIFormat::BC7_UNORM:
    case ezRHIFormat::BC7_UNORM_SRGB:
      return true;
  }

  return false;
}

bool FormatHelpers::IsFormatStencilSupport(ezEnum<ezRHIFormat> value)
{
  switch (value)
  {
    case ezRHIFormat::R32G8X24_TYPELESS:
    case ezRHIFormat::D32_FLOAT_S8X24_UINT:
    case ezRHIFormat::R24G8_TYPELESS:
    case ezRHIFormat::D24_UNORM_S8_UINT:
      return true;
  }

  return false;
}