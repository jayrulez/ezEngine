#include <RHIVulkan/Utilities/VKUtility.h>

namespace VKUtils
{
  VkFormat ToVkFormat(ezRHIResourceFormat::Enum value)
  {
    switch (value)
    {
      case ezRHIResourceFormat::UNKNOWN:
        return VkFormat::eUndefined;
        break;
      case ezRHIResourceFormat::R32G32B32A32_FLOAT:
        return VkFormat::eR32G32B32A32Sfloat;
        break;
      case ezRHIResourceFormat::R32G32B32A32_UINT:
        return VkFormat::eR32G32B32A32Uint;
        break;
      case ezRHIResourceFormat::R32G32B32A32_SINT:
        return VkFormat::eR32G32B32A32Sint;
        break;
      case ezRHIResourceFormat::R32G32B32_FLOAT:
        return VkFormat::eR32G32B32Sfloat;
        break;
      case ezRHIResourceFormat::R32G32B32_UINT:
        return VkFormat::eR32G32B32Uint;
        break;
      case ezRHIResourceFormat::R32G32B32_SINT:
        return VkFormat::eR32G32B32Sint;
        break;
      case ezRHIResourceFormat::R16G16B16A16_FLOAT:
        return VkFormat::eR16G16B16A16Sfloat;
        break;
      case ezRHIResourceFormat::R16G16B16A16_UNORM:
        return VkFormat::eR16G16B16A16Unorm;
        break;
      case ezRHIResourceFormat::R16G16B16A16_UINT:
        return VkFormat::eR16G16B16A16Uint;
        break;
      case ezRHIResourceFormat::R16G16B16A16_SNORM:
        return VkFormat::eR16G16B16A16Snorm;
        break;
      case ezRHIResourceFormat::R16G16B16A16_SINT:
        return VkFormat::eR16G16B16A16Sint;
        break;
      case ezRHIResourceFormat::R32G32_FLOAT:
        return VkFormat::eR32G32Sfloat;
        break;
      case ezRHIResourceFormat::R32G32_UINT:
        return VkFormat::eR32G32Uint;
        break;
      case ezRHIResourceFormat::R32G32_SINT:
        return VkFormat::eR32G32Sint;
        break;
      case ezRHIResourceFormat::R32G8X24_TYPELESS:
        return VkFormat::eD32SfloatS8Uint;
        break;
      case ezRHIResourceFormat::D32_FLOAT_S8X24_UINT:
        return VkFormat::eD32SfloatS8Uint;
        break;
      case ezRHIResourceFormat::R10G10B10A2_UNORM:
        return VkFormat::eA2B10G10R10UnormPack32;
        break;
      case ezRHIResourceFormat::R10G10B10A2_UINT:
        return VkFormat::eA2B10G10R10UintPack32;
        break;
      case ezRHIResourceFormat::R11G11B10_FLOAT:
        return VkFormat::eB10G11R11UfloatPack32;
        break;
      case ezRHIResourceFormat::R8G8B8A8_UNORM:
        return VkFormat::eR8G8B8A8Unorm;
        break;
      case ezRHIResourceFormat::R8G8B8A8_UNORM_SRGB:
        return VkFormat::eR8G8B8A8Srgb;
        break;
      case ezRHIResourceFormat::R8G8B8A8_UINT:
        return VkFormat::eR8G8B8A8Uint;
        break;
      case ezRHIResourceFormat::R8G8B8A8_SNORM:
        return VkFormat::eR8G8B8A8Snorm;
        break;
      case ezRHIResourceFormat::R8G8B8A8_SINT:
        return VkFormat::eR8G8B8A8Sint;
        break;
      case ezRHIResourceFormat::R16G16_FLOAT:
        return VkFormat::eR16G16Sfloat;
        break;
      case ezRHIResourceFormat::R16G16_UNORM:
        return VkFormat::eR16G16Unorm;
        break;
      case ezRHIResourceFormat::R16G16_UINT:
        return VkFormat::eR16G16Uint;
        break;
      case ezRHIResourceFormat::R16G16_SNORM:
        return VkFormat::eR16G16Snorm;
        break;
      case ezRHIResourceFormat::R16G16_SINT:
        return VkFormat::eR16G16Sint;
        break;
      case ezRHIResourceFormat::R32_TYPELESS:
        return VkFormat::eD32Sfloat;
        break;
      case ezRHIResourceFormat::D32_FLOAT:
        return VkFormat::eD32Sfloat;
        break;
      case ezRHIResourceFormat::R32_FLOAT:
        return VkFormat::eR32Sfloat;
        break;
      case ezRHIResourceFormat::R32_UINT:
        return VkFormat::eR32Uint;
        break;
      case ezRHIResourceFormat::R32_SINT:
        return VkFormat::eR32Sint;
        break;
      case ezRHIResourceFormat::R24G8_TYPELESS:
        return VkFormat::eD24UnormS8Uint;
        break;
      case ezRHIResourceFormat::D24_UNORM_S8_UINT:
        return VkFormat::eD24UnormS8Uint;
        break;
      case ezRHIResourceFormat::R8G8_UNORM:
        return VkFormat::eR8G8Unorm;
        break;
      case ezRHIResourceFormat::R8G8_UINT:
        return VkFormat::eR8G8Uint;
        break;
      case ezRHIResourceFormat::R8G8_SNORM:
        return VkFormat::eR8G8Snorm;
        break;
      case ezRHIResourceFormat::R8G8_SINT:
        return VkFormat::eR8G8Sint;
        break;
      case ezRHIResourceFormat::R16_TYPELESS:
        return VkFormat::eD16Unorm;
        break;
      case ezRHIResourceFormat::R16_FLOAT:
        return VkFormat::eR16Sfloat;
        break;
      case ezRHIResourceFormat::D16_UNORM:
        return VkFormat::eD16Unorm;
        break;
      case ezRHIResourceFormat::R16_UNORM:
        return VkFormat::eR16Unorm;
        break;
      case ezRHIResourceFormat::R16_UINT:
        return VkFormat::eR16Uint;
        break;
      case ezRHIResourceFormat::R16_SNORM:
        return VkFormat::eR16Snorm;
        break;
      case ezRHIResourceFormat::R16_SINT:
        return VkFormat::eR16Sint;
        break;
      case ezRHIResourceFormat::R8_UNORM:
        return VkFormat::eR8Unorm;
        break;
      case ezRHIResourceFormat::R8_UINT:
        return VkFormat::eR8Uint;
        break;
      case ezRHIResourceFormat::R8_SNORM:
        return VkFormat::eR8Snorm;
        break;
      case ezRHIResourceFormat::R8_SINT:
        return VkFormat::eR8Sint;
        break;
      case ezRHIResourceFormat::BC1_UNORM:
        return VkFormat::eBc1RgbaUnormBlock;
        break;
      case ezRHIResourceFormat::BC1_UNORM_SRGB:
        return VkFormat::eBc1RgbaSrgbBlock;
        break;
      case ezRHIResourceFormat::BC2_UNORM:
        return VkFormat::eBc2UnormBlock;
        break;
      case ezRHIResourceFormat::BC2_UNORM_SRGB:
        return VkFormat::eBc2SrgbBlock;
        break;
      case ezRHIResourceFormat::BC3_UNORM:
        return VkFormat::eBc3UnormBlock;
        break;
      case ezRHIResourceFormat::BC3_UNORM_SRGB:
        return VkFormat::eBc3SrgbBlock;
        break;
      case ezRHIResourceFormat::BC4_UNORM:
        return VkFormat::eBc4UnormBlock;
        break;
      case ezRHIResourceFormat::BC4_SNORM:
        return VkFormat::eBc4SnormBlock;
        break;
      case ezRHIResourceFormat::BC5_UNORM:
        return VkFormat::eBc5UnormBlock;
        break;
      case ezRHIResourceFormat::BC5_SNORM:
        return VkFormat::eBc5SnormBlock;
        break;
      case ezRHIResourceFormat::B8G8R8A8_UNORM:
        return VkFormat::eB8G8R8A8Unorm;
        break;
      case ezRHIResourceFormat::B8G8R8A8_UNORM_SRGB:
        return VkFormat::eB8G8R8A8Srgb;
        break;
      case ezRHIResourceFormat::BC6H_UF16:
        return VkFormat::eBc6HUfloatBlock;
        break;
      case ezRHIResourceFormat::BC6H_SF16:
        return VkFormat::eBc6HSfloatBlock;
        break;
      case ezRHIResourceFormat::BC7_UNORM:
        return VkFormat::eBc7UnormBlock;
        break;
      case ezRHIResourceFormat::BC7_UNORM_SRGB:
        return VkFormat::eBc7SrgbBlock;
        break;
    }
    return VkFormat::eUndefined;
  }

  ezRHIResourceFormat::Enum ToEngineFormat(VkFormat value)
  {
    switch (value)
    {
      case VkFormat::eUndefined:
        return ezRHIResourceFormat::UNKNOWN;
        break;
      case VkFormat::eR32G32B32A32Sfloat:
        return ezRHIResourceFormat::R32G32B32A32_FLOAT;
        break;
      case VkFormat::eR32G32B32A32Uint:
        return ezRHIResourceFormat::R32G32B32A32_UINT;
        break;
      case VkFormat::eR32G32B32A32Sint:
        return ezRHIResourceFormat::R32G32B32A32_SINT;
        break;
      case VkFormat::eR32G32B32Sfloat:
        return ezRHIResourceFormat::R32G32B32_FLOAT;
        break;
      case VkFormat::eR32G32B32Uint:
        return ezRHIResourceFormat::R32G32B32_UINT;
        break;
      case VkFormat::eR32G32B32Sint:
        return ezRHIResourceFormat::R32G32B32_SINT;
        break;
      case VkFormat::eR16G16B16A16Sfloat:
        return ezRHIResourceFormat::R16G16B16A16_FLOAT;
        break;
      case VkFormat::eR16G16B16A16Unorm:
        return ezRHIResourceFormat::R16G16B16A16_UNORM;
        break;
      case VkFormat::eR16G16B16A16Uint:
        return ezRHIResourceFormat::R16G16B16A16_UINT;
        break;
      case VkFormat::eR16G16B16A16Snorm:
        return ezRHIResourceFormat::R16G16B16A16_SNORM;
        break;
      case VkFormat::eR16G16B16A16Sint:
        return ezRHIResourceFormat::R16G16B16A16_SINT;
        break;
      case VkFormat::eR32G32Sfloat:
        return ezRHIResourceFormat::R32G32_FLOAT;
        break;
      case VkFormat::eR32G32Uint:
        return ezRHIResourceFormat::R32G32_UINT;
        break;
      case VkFormat::eR32G32Sint:
        return ezRHIResourceFormat::R32G32_SINT;
        break;
      case VkFormat::eD32SfloatS8Uint:
        return ezRHIResourceFormat::D32_FLOAT_S8X24_UINT;
        break;
      case VkFormat::eA2B10G10R10UnormPack32:
        return ezRHIResourceFormat::R10G10B10A2_UNORM;
        break;
      case VkFormat::eA2B10G10R10UintPack32:
        return ezRHIResourceFormat::R10G10B10A2_UINT;
        break;
      case VkFormat::eB10G11R11UfloatPack32:
        return ezRHIResourceFormat::R11G11B10_FLOAT;
        break;
      case VkFormat::eR8G8B8A8Unorm:
        return ezRHIResourceFormat::R8G8B8A8_UNORM;
        break;
      case VkFormat::eR8G8B8A8Srgb:
        return ezRHIResourceFormat::R8G8B8A8_UNORM_SRGB;
        break;
      case VkFormat::eR8G8B8A8Uint:
        return ezRHIResourceFormat::R8G8B8A8_UINT;
        break;
      case VkFormat::eR8G8B8A8Snorm:
        return ezRHIResourceFormat::R8G8B8A8_SNORM;
        break;
      case VkFormat::eR8G8B8A8Sint:
        return ezRHIResourceFormat::R8G8B8A8_SINT;
        break;
      case VkFormat::eR16G16Sfloat:
        return ezRHIResourceFormat::R16G16_FLOAT;
        break;
      case VkFormat::eR16G16Unorm:
        return ezRHIResourceFormat::R16G16_UNORM;
        break;
      case VkFormat::eR16G16Uint:
        return ezRHIResourceFormat::R16G16_UINT;
        break;
      case VkFormat::eR16G16Snorm:
        return ezRHIResourceFormat::R16G16_SNORM;
        break;
      case VkFormat::eR16G16Sint:
        return ezRHIResourceFormat::R16G16_SINT;
        break;
      case VkFormat::eD32Sfloat:
        return ezRHIResourceFormat::D32_FLOAT;
        break;
      case VkFormat::eR32Sfloat:
        return ezRHIResourceFormat::R32_FLOAT;
        break;
      case VkFormat::eR32Uint:
        return ezRHIResourceFormat::R32_UINT;
        break;
      case VkFormat::eR32Sint:
        return ezRHIResourceFormat::R32_SINT;
        break;
      case VkFormat::eD24UnormS8Uint:
        return ezRHIResourceFormat::D24_UNORM_S8_UINT;
        break;
      case VkFormat::eR8G8Unorm:
        return ezRHIResourceFormat::R8G8_UNORM;
        break;
      case VkFormat::eR8G8Uint:
        return ezRHIResourceFormat::R8G8_UINT;
        break;
      case VkFormat::eR8G8Snorm:
        return ezRHIResourceFormat::R8G8_SNORM;
        break;
      case VkFormat::eR8G8Sint:
        return ezRHIResourceFormat::R8G8_SINT;
        break;
      case VkFormat::eR16Sfloat:
        return ezRHIResourceFormat::R16_FLOAT;
        break;
      case VkFormat::eD16Unorm:
        return ezRHIResourceFormat::D16_UNORM;
        break;
      case VkFormat::eR16Unorm:
        return ezRHIResourceFormat::R16_UNORM;
        break;
      case VkFormat::eR16Uint:
        return ezRHIResourceFormat::R16_UINT;
        break;
      case VkFormat::eR16Snorm:
        return ezRHIResourceFormat::R16_SNORM;
        break;
      case VkFormat::eR16Sint:
        return ezRHIResourceFormat::R16_SINT;
        break;
      case VkFormat::eR8Unorm:
        return ezRHIResourceFormat::R8_UNORM;
        break;
      case VkFormat::eR8Uint:
        return ezRHIResourceFormat::R8_UINT;
        break;
      case VkFormat::eR8Snorm:
        return ezRHIResourceFormat::R8_SNORM;
        break;
      case VkFormat::eR8Sint:
        return ezRHIResourceFormat::R8_SINT;
        break;
      case VkFormat::eBc1RgbaUnormBlock:
        return ezRHIResourceFormat::BC1_UNORM;
        break;
      case VkFormat::eBc1RgbaSrgbBlock:
        return ezRHIResourceFormat::BC1_UNORM_SRGB;
        break;
      case VkFormat::eBc2UnormBlock:
        return ezRHIResourceFormat::BC2_UNORM;
        break;
      case VkFormat::eBc2SrgbBlock:
        return ezRHIResourceFormat::BC2_UNORM_SRGB;
        break;
      case VkFormat::eBc3UnormBlock:
        return ezRHIResourceFormat::BC3_UNORM;
        break;
      case VkFormat::eBc3SrgbBlock:
        return ezRHIResourceFormat::BC3_UNORM_SRGB;
        break;
      case VkFormat::eBc4UnormBlock:
        return ezRHIResourceFormat::BC4_UNORM;
        break;
      case VkFormat::eBc4SnormBlock:
        return ezRHIResourceFormat::BC4_SNORM;
        break;
      case VkFormat::eBc5UnormBlock:
        return ezRHIResourceFormat::BC5_UNORM;
        break;
      case VkFormat::eBc5SnormBlock:
        return ezRHIResourceFormat::BC5_SNORM;
        break;
      case VkFormat::eB8G8R8A8Unorm:
        return ezRHIResourceFormat::B8G8R8A8_UNORM;
        break;
      case VkFormat::eB8G8R8A8Srgb:
        return ezRHIResourceFormat::B8G8R8A8_UNORM_SRGB;
        break;
      case VkFormat::eBc6HUfloatBlock:
        return ezRHIResourceFormat::BC6H_UF16;
        break;
      case VkFormat::eBc6HSfloatBlock:
        return ezRHIResourceFormat::BC6H_SF16;
        break;
      case VkFormat::eBc7UnormBlock:
        return ezRHIResourceFormat::BC7_UNORM;
        break;
      case VkFormat::eBc7SrgbBlock:
        return ezRHIResourceFormat::BC7_UNORM_SRGB;
        break;
    }
    return ezRHIResourceFormat::UNKNOWN;
  }
} // namespace VKUtils
