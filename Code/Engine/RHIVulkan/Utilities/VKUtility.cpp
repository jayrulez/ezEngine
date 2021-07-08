#include <RHIVulkan/Utilities/VKUtility.h>

namespace VKUtils
{
  VkFormat ToVkFormat(ezRHIResourceFormat::Enum value)
  {
    switch (value)
    {
      case ezRHIResourceFormat::UNKNOWN:
        return VK_FORMAT_UNDEFINED;
        break;
      case ezRHIResourceFormat::R32G32B32A32_FLOAT:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
        break;
      case ezRHIResourceFormat::R32G32B32A32_UINT:
        return VK_FORMAT_R32G32B32A32_UINT;
        break;
      case ezRHIResourceFormat::R32G32B32A32_SINT:
        return VK_FORMAT_R32G32B32A32_SINT;
        break;
      case ezRHIResourceFormat::R32G32B32_FLOAT:
        return VK_FORMAT_R32G32B32_SFLOAT;
        break;
      case ezRHIResourceFormat::R32G32B32_UINT:
        return VK_FORMAT_R32G32B32_UINT;
        break;
      case ezRHIResourceFormat::R32G32B32_SINT:
        return VK_FORMAT_R32G32B32_SINT;
        break;
      case ezRHIResourceFormat::R16G16B16A16_FLOAT:
        return VK_FORMAT_R16G16B16A16_SFLOAT;
        break;
      case ezRHIResourceFormat::R16G16B16A16_UNORM:
        return VK_FORMAT_R16G16B16A16_UNORM;
        break;
      case ezRHIResourceFormat::R16G16B16A16_UINT:
        return VK_FORMAT_R16G16B16A16_UINT;
        break;
      case ezRHIResourceFormat::R16G16B16A16_SNORM:
        return VK_FORMAT_R16G16B16A16_SNORM;
        break;
      case ezRHIResourceFormat::R16G16B16A16_SINT:
        return VK_FORMAT_R16G16B16A16_SINT;
        break;
      case ezRHIResourceFormat::R32G32_FLOAT:
        return VK_FORMAT_R32G32_SFLOAT;
        break;
      case ezRHIResourceFormat::R32G32_UINT:
        return VK_FORMAT_R32G32_UINT;
        break;
      case ezRHIResourceFormat::R32G32_SINT:
        return VK_FORMAT_R32G32_SINT;
        break;
      case ezRHIResourceFormat::R32G8X24_TYPELESS:
        return VK_FORMAT_D32_SFLOAT_S8_UINT;
        break;
      case ezRHIResourceFormat::D32_FLOAT_S8X24_UINT:
        return VK_FORMAT_D32_SFLOAT_S8_UINT;
        break;
      case ezRHIResourceFormat::R10G10B10A2_UNORM:
        return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
        break;
      case ezRHIResourceFormat::R10G10B10A2_UINT:
        return VK_FORMAT_A2B10G10R10_UINT_PACK32;
        break;
      case ezRHIResourceFormat::R11G11B10_FLOAT:
        return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
        break;
      case ezRHIResourceFormat::R8G8B8A8_UNORM:
        return VK_FORMAT_R8G8B8A8_UNORM;
        break;
      case ezRHIResourceFormat::R8G8B8A8_UNORM_SRGB:
        return VK_FORMAT_R8G8B8A8_SRGB;
        break;
      case ezRHIResourceFormat::R8G8B8A8_UINT:
        return VK_FORMAT_R8G8B8A8_UINT;
        break;
      case ezRHIResourceFormat::R8G8B8A8_SNORM:
        return VK_FORMAT_R8G8B8A8_SNORM;
        break;
      case ezRHIResourceFormat::R8G8B8A8_SINT:
        return VK_FORMAT_R8G8B8A8_SINT;
        break;
      case ezRHIResourceFormat::R16G16_FLOAT:
        return VK_FORMAT_R16G16_SFLOAT;
        break;
      case ezRHIResourceFormat::R16G16_UNORM:
        return VK_FORMAT_R16G16_UNORM;
        break;
      case ezRHIResourceFormat::R16G16_UINT:
        return VK_FORMAT_R16G16_UINT;
        break;
      case ezRHIResourceFormat::R16G16_SNORM:
        return VK_FORMAT_R16G16_SNORM;
        break;
      case ezRHIResourceFormat::R16G16_SINT:
        return VK_FORMAT_R16G16_SINT;
        break;
      case ezRHIResourceFormat::R32_TYPELESS:
        return VK_FORMAT_D32_SFLOAT;
        break;
      case ezRHIResourceFormat::D32_FLOAT:
        return VK_FORMAT_D32_SFLOAT;
        break;
      case ezRHIResourceFormat::R32_FLOAT:
        return VK_FORMAT_R32_SFLOAT;
        break;
      case ezRHIResourceFormat::R32_UINT:
        return VK_FORMAT_R32_UINT;
        break;
      case ezRHIResourceFormat::R32_SINT:
        return VK_FORMAT_R32_SINT;
        break;
      case ezRHIResourceFormat::R24G8_TYPELESS:
        return VK_FORMAT_D24_UNORM_S8_UINT;
        break;
      case ezRHIResourceFormat::D24_UNORM_S8_UINT:
        return VK_FORMAT_D24_UNORM_S8_UINT;
        break;
      case ezRHIResourceFormat::R8G8_UNORM:
        return VK_FORMAT_R8G8_UNORM;
        break;
      case ezRHIResourceFormat::R8G8_UINT:
        return VK_FORMAT_R8G8_UINT;
        break;
      case ezRHIResourceFormat::R8G8_SNORM:
        return VK_FORMAT_R8G8_SNORM;
        break;
      case ezRHIResourceFormat::R8G8_SINT:
        return VK_FORMAT_R8G8_SINT;
        break;
      case ezRHIResourceFormat::R16_TYPELESS:
        return VK_FORMAT_D16_UNORM;
        break;
      case ezRHIResourceFormat::R16_FLOAT:
        return VK_FORMAT_R16_SFLOAT;
        break;
      case ezRHIResourceFormat::D16_UNORM:
        return VK_FORMAT_D16_UNORM;
        break;
      case ezRHIResourceFormat::R16_UNORM:
        return VK_FORMAT_R16_UNORM;
        break;
      case ezRHIResourceFormat::R16_UINT:
        return VK_FORMAT_R16_UINT;
        break;
      case ezRHIResourceFormat::R16_SNORM:
        return VK_FORMAT_R16_SNORM;
        break;
      case ezRHIResourceFormat::R16_SINT:
        return VK_FORMAT_R16_SINT;
        break;
      case ezRHIResourceFormat::R8_UNORM:
        return VK_FORMAT_R8_UNORM;
        break;
      case ezRHIResourceFormat::R8_UINT:
        return VK_FORMAT_R8_UINT;
        break;
      case ezRHIResourceFormat::R8_SNORM:
        return VK_FORMAT_R8_SNORM;
        break;
      case ezRHIResourceFormat::R8_SINT:
        return VK_FORMAT_R8_SINT;
        break;
      case ezRHIResourceFormat::BC1_UNORM:
        return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
        break;
      case ezRHIResourceFormat::BC1_UNORM_SRGB:
        return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
        break;
      case ezRHIResourceFormat::BC2_UNORM:
        return VK_FORMAT_BC2_UNORM_BLOCK;
        break;
      case ezRHIResourceFormat::BC2_UNORM_SRGB:
        return VK_FORMAT_BC2_SRGB_BLOCK;
        break;
      case ezRHIResourceFormat::BC3_UNORM:
        return VK_FORMAT_BC3_UNORM_BLOCK;
        break;
      case ezRHIResourceFormat::BC3_UNORM_SRGB:
        return VK_FORMAT_BC3_SRGB_BLOCK;
        break;
      case ezRHIResourceFormat::BC4_UNORM:
        return VK_FORMAT_BC4_UNORM_BLOCK;
        break;
      case ezRHIResourceFormat::BC4_SNORM:
        return VK_FORMAT_BC4_SNORM_BLOCK;
        break;
      case ezRHIResourceFormat::BC5_UNORM:
        return VK_FORMAT_BC5_UNORM_BLOCK;
        break;
      case ezRHIResourceFormat::BC5_SNORM:
        return VK_FORMAT_BC5_SNORM_BLOCK;
        break;
      case ezRHIResourceFormat::B8G8R8A8_UNORM:
        return VK_FORMAT_B8G8R8A8_UNORM;
        break;
      case ezRHIResourceFormat::B8G8R8A8_UNORM_SRGB:
        return VK_FORMAT_B8G8R8A8_SRGB;
        break;
      case ezRHIResourceFormat::BC6H_UF16:
        return VK_FORMAT_BC6H_UFLOAT_BLOCK;
        break;
      case ezRHIResourceFormat::BC6H_SF16:
        return VK_FORMAT_BC6H_SFLOAT_BLOCK;
        break;
      case ezRHIResourceFormat::BC7_UNORM:
        return VK_FORMAT_BC7_UNORM_BLOCK;
        break;
      case ezRHIResourceFormat::BC7_UNORM_SRGB:
        return VK_FORMAT_BC7_SRGB_BLOCK;
        break;
    }
    return VkFormat::VK_FORMAT_UNDEFINED;
  }

  ezRHIResourceFormat::Enum ToEngineFormat(VkFormat value)
  {
    switch (value)
    {
      case VK_FORMAT_UNDEFINED:
        return ezRHIResourceFormat::UNKNOWN;
        break;

      case VK_FORMAT_R32G32B32A32_SFLOAT:
        return ezRHIResourceFormat::R32G32B32A32_FLOAT;
        break;

      case VK_FORMAT_R32G32B32A32_UINT:
        return ezRHIResourceFormat::R32G32B32A32_UINT;
        break;

      case VK_FORMAT_R32G32B32A32_SINT:
        return ezRHIResourceFormat::R32G32B32A32_SINT;
        break;

      case VK_FORMAT_R32G32B32_SFLOAT:
        return ezRHIResourceFormat::R32G32B32_FLOAT;
        break;

      case VK_FORMAT_R32G32B32_UINT:
        return ezRHIResourceFormat::R32G32B32_UINT;
        break;

      case VK_FORMAT_R32G32B32_SINT:
        return ezRHIResourceFormat::R32G32B32_SINT;
        break;

      case VK_FORMAT_R16G16B16A16_SFLOAT:
        return ezRHIResourceFormat::R16G16B16A16_FLOAT;
        break;

      case VK_FORMAT_R16G16B16A16_UNORM:
        return ezRHIResourceFormat::R16G16B16A16_UNORM;
        break;

      case VK_FORMAT_R16G16B16A16_UINT:
        return ezRHIResourceFormat::R16G16B16A16_UINT;
        break;

      case VK_FORMAT_R16G16B16A16_SNORM:
        return ezRHIResourceFormat::R16G16B16A16_SNORM;
        break;

      case VK_FORMAT_R16G16B16A16_SINT:
        return ezRHIResourceFormat::R16G16B16A16_SINT;
        break;

      case VK_FORMAT_R32G32_SFLOAT:
        return ezRHIResourceFormat::R32G32_FLOAT;
        break;

      case VK_FORMAT_R32G32_UINT:
        return ezRHIResourceFormat::R32G32_UINT;
        break;

      case VK_FORMAT_R32G32_SINT:
        return ezRHIResourceFormat::R32G32_SINT;
        break;

      //case VK_FORMAT_D32_SFLOAT_S8_UINT:
      //  return ezRHIResourceFormat::R32G8X24_TYPELESS;
      //  break;

      case VK_FORMAT_D32_SFLOAT_S8_UINT:
        return ezRHIResourceFormat::D32_FLOAT_S8X24_UINT;
        break;

      case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
        return ezRHIResourceFormat::R10G10B10A2_UNORM;
        break;

      case VK_FORMAT_A2B10G10R10_UINT_PACK32:
        return ezRHIResourceFormat::R10G10B10A2_UINT;
        break;

      case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
        return ezRHIResourceFormat::R11G11B10_FLOAT;
        break;

      case VK_FORMAT_R8G8B8A8_UNORM:
        return ezRHIResourceFormat::R8G8B8A8_UNORM;
        break;

      case VK_FORMAT_R8G8B8A8_SRGB:
        return ezRHIResourceFormat::R8G8B8A8_UNORM_SRGB;
        break;

      case VK_FORMAT_R8G8B8A8_UINT:
        return ezRHIResourceFormat::R8G8B8A8_UINT;
        break;

      case VK_FORMAT_R8G8B8A8_SNORM:
        return ezRHIResourceFormat::R8G8B8A8_SNORM;
        break;

      case VK_FORMAT_R8G8B8A8_SINT:
        return ezRHIResourceFormat::R8G8B8A8_SINT;
        break;

      case VK_FORMAT_R16G16_SFLOAT:
        return ezRHIResourceFormat::R16G16_FLOAT;
        break;

      case VK_FORMAT_R16G16_UNORM:
        return ezRHIResourceFormat::R16G16_UNORM;
        break;

      case VK_FORMAT_R16G16_UINT:
        return ezRHIResourceFormat::R16G16_UINT;
        break;

      case VK_FORMAT_R16G16_SNORM:
        return ezRHIResourceFormat::R16G16_SNORM;
        break;

      case VK_FORMAT_R16G16_SINT:
        return ezRHIResourceFormat::R16G16_SINT;
        break;

      //case VK_FORMAT_D32_SFLOAT:
      //  return ezRHIResourceFormat::R32_TYPELESS;
      //  break;

      case VK_FORMAT_D32_SFLOAT:
        return ezRHIResourceFormat::D32_FLOAT;
        break;

      case VK_FORMAT_R32_SFLOAT:
        return ezRHIResourceFormat::R32_FLOAT;
        break;

      case VK_FORMAT_R32_UINT:
        return ezRHIResourceFormat::R32_UINT;
        break;

      case VK_FORMAT_R32_SINT:
        return ezRHIResourceFormat::R32_SINT;
        break;

      //case VK_FORMAT_D24_UNORM_S8_UINT:
      //  return ezRHIResourceFormat::R24G8_TYPELESS;
      //  break;

      case VK_FORMAT_D24_UNORM_S8_UINT:
        return ezRHIResourceFormat::D24_UNORM_S8_UINT;
        break;

      case VK_FORMAT_R8G8_UNORM:
        return ezRHIResourceFormat::R8G8_UNORM;
        break;

      case VK_FORMAT_R8G8_UINT:
        return ezRHIResourceFormat::R8G8_UINT;
        break;

      case VK_FORMAT_R8G8_SNORM:
        return ezRHIResourceFormat::R8G8_SNORM;
        break;

      case VK_FORMAT_R8G8_SINT:
        return ezRHIResourceFormat::R8G8_SINT;
        break;

      //case VK_FORMAT_D16_UNORM:
      //  return ezRHIResourceFormat::R16_TYPELESS;
      //  break;

      case VK_FORMAT_R16_SFLOAT:
        return ezRHIResourceFormat::R16_FLOAT;
        break;

      case VK_FORMAT_D16_UNORM:
        return ezRHIResourceFormat::D16_UNORM;
        break;

      case VK_FORMAT_R16_UNORM:
        return ezRHIResourceFormat::R16_UNORM;
        break;

      case VK_FORMAT_R16_UINT:
        return ezRHIResourceFormat::R16_UINT;
        break;

      case VK_FORMAT_R16_SNORM:
        return ezRHIResourceFormat::R16_SNORM;
        break;

      case VK_FORMAT_R16_SINT:
        return ezRHIResourceFormat::R16_SINT;
        break;

      case VK_FORMAT_R8_UNORM:
        return ezRHIResourceFormat::R8_UNORM;
        break;

      case VK_FORMAT_R8_UINT:
        return ezRHIResourceFormat::R8_UINT;
        break;

      case VK_FORMAT_R8_SNORM:
        return ezRHIResourceFormat::R8_SNORM;
        break;

      case VK_FORMAT_R8_SINT:
        return ezRHIResourceFormat::R8_SINT;
        break;

      case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
        return ezRHIResourceFormat::BC1_UNORM;
        break;

      case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
        return ezRHIResourceFormat::BC1_UNORM_SRGB;
        break;

      case VK_FORMAT_BC2_UNORM_BLOCK:
        return ezRHIResourceFormat::BC2_UNORM;
        break;

      case VK_FORMAT_BC2_SRGB_BLOCK:
        return ezRHIResourceFormat::BC2_UNORM_SRGB;
        break;

      case VK_FORMAT_BC3_UNORM_BLOCK:
        return ezRHIResourceFormat::BC3_UNORM;
        break;

      case VK_FORMAT_BC3_SRGB_BLOCK:
        return ezRHIResourceFormat::BC3_UNORM_SRGB;
        break;

      case VK_FORMAT_BC4_UNORM_BLOCK:
        return ezRHIResourceFormat::BC4_UNORM;
        break;

      case VK_FORMAT_BC4_SNORM_BLOCK:
        return ezRHIResourceFormat::BC4_SNORM;
        break;

      case VK_FORMAT_BC5_UNORM_BLOCK:
        return ezRHIResourceFormat::BC5_UNORM;
        break;

      case VK_FORMAT_BC5_SNORM_BLOCK:
        return ezRHIResourceFormat::BC5_SNORM;
        break;

      case VK_FORMAT_B8G8R8A8_UNORM:
        return ezRHIResourceFormat::B8G8R8A8_UNORM;
        break;

      case VK_FORMAT_B8G8R8A8_SRGB:
        return ezRHIResourceFormat::B8G8R8A8_UNORM_SRGB;
        break;

      case VK_FORMAT_BC6H_UFLOAT_BLOCK:
        return ezRHIResourceFormat::BC6H_UF16;
        break;

      case VK_FORMAT_BC6H_SFLOAT_BLOCK:
        return ezRHIResourceFormat::BC6H_SF16;
        break;

      case VK_FORMAT_BC7_UNORM_BLOCK:
        return ezRHIResourceFormat::BC7_UNORM;
        break;

      case VK_FORMAT_BC7_SRGB_BLOCK:
        return ezRHIResourceFormat::BC7_UNORM_SRGB;
        break;
    }
    return ezRHIResourceFormat::UNKNOWN;
  }

    VkIndexType GetVkIndexType(ezRHIResourceFormat::Enum format)
  {
    VkFormat vk_format = VKUtils::ToVkFormat(format);
    switch (vk_format)
    {
      case VkFormat::VK_FORMAT_R16_UINT:
        return VkIndexType::VK_INDEX_TYPE_UINT16;
      case VkFormat::VK_FORMAT_R32_UINT:
        return VkIndexType::VK_INDEX_TYPE_UINT32;
      default:
        EZ_ASSERT_NOT_IMPLEMENTED;
        return {};
    }
  }
} // namespace VKUtils
