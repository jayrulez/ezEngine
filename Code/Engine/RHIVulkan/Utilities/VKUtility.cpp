#include <RHIVulkan/Utilities/VKUtility.h>

namespace VKUtils
{
  vk::Format ToVkFormat(ResourceFormat::Enum value)
  {
    switch (value)
    {
      case ResourceFormat::UNKNOWN:
        return vk::Format::eUndefined;
        break;
      case ResourceFormat::R32G32B32A32_FLOAT:
        return vk::Format::eR32G32B32A32Sfloat;
        break;
      case ResourceFormat::R32G32B32A32_UINT:
        return vk::Format::eR32G32B32A32Uint;
        break;
      case ResourceFormat::R32G32B32A32_SINT:
        return vk::Format::eR32G32B32A32Sint;
        break;
      case ResourceFormat::R32G32B32_FLOAT:
        return vk::Format::eR32G32B32Sfloat;
        break;
      case ResourceFormat::R32G32B32_UINT:
        return vk::Format::eR32G32B32Uint;
        break;
      case ResourceFormat::R32G32B32_SINT:
        return vk::Format::eR32G32B32Sint;
        break;
      case ResourceFormat::R16G16B16A16_FLOAT:
        return vk::Format::eR16G16B16A16Sfloat;
        break;
      case ResourceFormat::R16G16B16A16_UNORM:
        return vk::Format::eR16G16B16A16Unorm;
        break;
      case ResourceFormat::R16G16B16A16_UINT:
        return vk::Format::eR16G16B16A16Uint;
        break;
      case ResourceFormat::R16G16B16A16_SNORM:
        return vk::Format::eR16G16B16A16Snorm;
        break;
      case ResourceFormat::R16G16B16A16_SINT:
        return vk::Format::eR16G16B16A16Sint;
        break;
      case ResourceFormat::R32G32_FLOAT:
        return vk::Format::eR32G32Sfloat;
        break;
      case ResourceFormat::R32G32_UINT:
        return vk::Format::eR32G32Uint;
        break;
      case ResourceFormat::R32G32_SINT:
        return vk::Format::eR32G32Sint;
        break;
      case ResourceFormat::R32G8X24_TYPELESS:
        return vk::Format::eD32SfloatS8Uint;
        break;
      case ResourceFormat::D32_FLOAT_S8X24_UINT:
        return vk::Format::eD32SfloatS8Uint;
        break;
      case ResourceFormat::R10G10B10A2_UNORM:
        return vk::Format::eA2B10G10R10UnormPack32;
        break;
      case ResourceFormat::R10G10B10A2_UINT:
        return vk::Format::eA2B10G10R10UintPack32;
        break;
      case ResourceFormat::R11G11B10_FLOAT:
        return vk::Format::eB10G11R11UfloatPack32;
        break;
      case ResourceFormat::R8G8B8A8_UNORM:
        return vk::Format::eR8G8B8A8Unorm;
        break;
      case ResourceFormat::R8G8B8A8_UNORM_SRGB:
        return vk::Format::eR8G8B8A8Srgb;
        break;
      case ResourceFormat::R8G8B8A8_UINT:
        return vk::Format::eR8G8B8A8Uint;
        break;
      case ResourceFormat::R8G8B8A8_SNORM:
        return vk::Format::eR8G8B8A8Snorm;
        break;
      case ResourceFormat::R8G8B8A8_SINT:
        return vk::Format::eR8G8B8A8Sint;
        break;
      case ResourceFormat::R16G16_FLOAT:
        return vk::Format::eR16G16Sfloat;
        break;
      case ResourceFormat::R16G16_UNORM:
        return vk::Format::eR16G16Unorm;
        break;
      case ResourceFormat::R16G16_UINT:
        return vk::Format::eR16G16Uint;
        break;
      case ResourceFormat::R16G16_SNORM:
        return vk::Format::eR16G16Snorm;
        break;
      case ResourceFormat::R16G16_SINT:
        return vk::Format::eR16G16Sint;
        break;
      case ResourceFormat::R32_TYPELESS:
        return vk::Format::eD32Sfloat;
        break;
      case ResourceFormat::D32_FLOAT:
        return vk::Format::eD32Sfloat;
        break;
      case ResourceFormat::R32_FLOAT:
        return vk::Format::eR32Sfloat;
        break;
      case ResourceFormat::R32_UINT:
        return vk::Format::eR32Uint;
        break;
      case ResourceFormat::R32_SINT:
        return vk::Format::eR32Sint;
        break;
      case ResourceFormat::R24G8_TYPELESS:
        return vk::Format::eD24UnormS8Uint;
        break;
      case ResourceFormat::D24_UNORM_S8_UINT:
        return vk::Format::eD24UnormS8Uint;
        break;
      case ResourceFormat::R8G8_UNORM:
        return vk::Format::eR8G8Unorm;
        break;
      case ResourceFormat::R8G8_UINT:
        return vk::Format::eR8G8Uint;
        break;
      case ResourceFormat::R8G8_SNORM:
        return vk::Format::eR8G8Snorm;
        break;
      case ResourceFormat::R8G8_SINT:
        return vk::Format::eR8G8Sint;
        break;
      case ResourceFormat::R16_TYPELESS:
        return vk::Format::eD16Unorm;
        break;
      case ResourceFormat::R16_FLOAT:
        return vk::Format::eR16Sfloat;
        break;
      case ResourceFormat::D16_UNORM:
        return vk::Format::eD16Unorm;
        break;
      case ResourceFormat::R16_UNORM:
        return vk::Format::eR16Unorm;
        break;
      case ResourceFormat::R16_UINT:
        return vk::Format::eR16Uint;
        break;
      case ResourceFormat::R16_SNORM:
        return vk::Format::eR16Snorm;
        break;
      case ResourceFormat::R16_SINT:
        return vk::Format::eR16Sint;
        break;
      case ResourceFormat::R8_UNORM:
        return vk::Format::eR8Unorm;
        break;
      case ResourceFormat::R8_UINT:
        return vk::Format::eR8Uint;
        break;
      case ResourceFormat::R8_SNORM:
        return vk::Format::eR8Snorm;
        break;
      case ResourceFormat::R8_SINT:
        return vk::Format::eR8Sint;
        break;
      case ResourceFormat::BC1_UNORM:
        return vk::Format::eBc1RgbaUnormBlock;
        break;
      case ResourceFormat::BC1_UNORM_SRGB:
        return vk::Format::eBc1RgbaSrgbBlock;
        break;
      case ResourceFormat::BC2_UNORM:
        return vk::Format::eBc2UnormBlock;
        break;
      case ResourceFormat::BC2_UNORM_SRGB:
        return vk::Format::eBc2SrgbBlock;
        break;
      case ResourceFormat::BC3_UNORM:
        return vk::Format::eBc3UnormBlock;
        break;
      case ResourceFormat::BC3_UNORM_SRGB:
        return vk::Format::eBc3SrgbBlock;
        break;
      case ResourceFormat::BC4_UNORM:
        return vk::Format::eBc4UnormBlock;
        break;
      case ResourceFormat::BC4_SNORM:
        return vk::Format::eBc4SnormBlock;
        break;
      case ResourceFormat::BC5_UNORM:
        return vk::Format::eBc5UnormBlock;
        break;
      case ResourceFormat::BC5_SNORM:
        return vk::Format::eBc5SnormBlock;
        break;
      case ResourceFormat::B8G8R8A8_UNORM:
        return vk::Format::eB8G8R8A8Unorm;
        break;
      case ResourceFormat::B8G8R8A8_UNORM_SRGB:
        return vk::Format::eB8G8R8A8Srgb;
        break;
      case ResourceFormat::BC6H_UF16:
        return vk::Format::eBc6HUfloatBlock;
        break;
      case ResourceFormat::BC6H_SF16:
        return vk::Format::eBc6HSfloatBlock;
        break;
      case ResourceFormat::BC7_UNORM:
        return vk::Format::eBc7UnormBlock;
        break;
      case ResourceFormat::BC7_UNORM_SRGB:
        return vk::Format::eBc7SrgbBlock;
        break;
    }
    return vk::Format::eUndefined;
  }

  ResourceFormat::Enum ToEngineFormat(vk::Format value)
  {
    switch (value)
    {
      case vk::Format::eUndefined:
        return ResourceFormat::UNKNOWN;
        break;
      case vk::Format::eR32G32B32A32Sfloat:
        return ResourceFormat::R32G32B32A32_FLOAT;
        break;
      case vk::Format::eR32G32B32A32Uint:
        return ResourceFormat::R32G32B32A32_UINT;
        break;
      case vk::Format::eR32G32B32A32Sint:
        return ResourceFormat::R32G32B32A32_SINT;
        break;
      case vk::Format::eR32G32B32Sfloat:
        return ResourceFormat::R32G32B32_FLOAT;
        break;
      case vk::Format::eR32G32B32Uint:
        return ResourceFormat::R32G32B32_UINT;
        break;
      case vk::Format::eR32G32B32Sint:
        return ResourceFormat::R32G32B32_SINT;
        break;
      case vk::Format::eR16G16B16A16Sfloat:
        return ResourceFormat::R16G16B16A16_FLOAT;
        break;
      case vk::Format::eR16G16B16A16Unorm:
        return ResourceFormat::R16G16B16A16_UNORM;
        break;
      case vk::Format::eR16G16B16A16Uint:
        return ResourceFormat::R16G16B16A16_UINT;
        break;
      case vk::Format::eR16G16B16A16Snorm:
        return ResourceFormat::R16G16B16A16_SNORM;
        break;
      case vk::Format::eR16G16B16A16Sint:
        return ResourceFormat::R16G16B16A16_SINT;
        break;
      case vk::Format::eR32G32Sfloat:
        return ResourceFormat::R32G32_FLOAT;
        break;
      case vk::Format::eR32G32Uint:
        return ResourceFormat::R32G32_UINT;
        break;
      case vk::Format::eR32G32Sint:
        return ResourceFormat::R32G32_SINT;
        break;
      case vk::Format::eD32SfloatS8Uint:
        return ResourceFormat::D32_FLOAT_S8X24_UINT;
        break;
      case vk::Format::eA2B10G10R10UnormPack32:
        return ResourceFormat::R10G10B10A2_UNORM;
        break;
      case vk::Format::eA2B10G10R10UintPack32:
        return ResourceFormat::R10G10B10A2_UINT;
        break;
      case vk::Format::eB10G11R11UfloatPack32:
        return ResourceFormat::R11G11B10_FLOAT;
        break;
      case vk::Format::eR8G8B8A8Unorm:
        return ResourceFormat::R8G8B8A8_UNORM;
        break;
      case vk::Format::eR8G8B8A8Srgb:
        return ResourceFormat::R8G8B8A8_UNORM_SRGB;
        break;
      case vk::Format::eR8G8B8A8Uint:
        return ResourceFormat::R8G8B8A8_UINT;
        break;
      case vk::Format::eR8G8B8A8Snorm:
        return ResourceFormat::R8G8B8A8_SNORM;
        break;
      case vk::Format::eR8G8B8A8Sint:
        return ResourceFormat::R8G8B8A8_SINT;
        break;
      case vk::Format::eR16G16Sfloat:
        return ResourceFormat::R16G16_FLOAT;
        break;
      case vk::Format::eR16G16Unorm:
        return ResourceFormat::R16G16_UNORM;
        break;
      case vk::Format::eR16G16Uint:
        return ResourceFormat::R16G16_UINT;
        break;
      case vk::Format::eR16G16Snorm:
        return ResourceFormat::R16G16_SNORM;
        break;
      case vk::Format::eR16G16Sint:
        return ResourceFormat::R16G16_SINT;
        break;
      case vk::Format::eD32Sfloat:
        return ResourceFormat::D32_FLOAT;
        break;
      case vk::Format::eR32Sfloat:
        return ResourceFormat::R32_FLOAT;
        break;
      case vk::Format::eR32Uint:
        return ResourceFormat::R32_UINT;
        break;
      case vk::Format::eR32Sint:
        return ResourceFormat::R32_SINT;
        break;
      case vk::Format::eD24UnormS8Uint:
        return ResourceFormat::D24_UNORM_S8_UINT;
        break;
      case vk::Format::eR8G8Unorm:
        return ResourceFormat::R8G8_UNORM;
        break;
      case vk::Format::eR8G8Uint:
        return ResourceFormat::R8G8_UINT;
        break;
      case vk::Format::eR8G8Snorm:
        return ResourceFormat::R8G8_SNORM;
        break;
      case vk::Format::eR8G8Sint:
        return ResourceFormat::R8G8_SINT;
        break;
      case vk::Format::eR16Sfloat:
        return ResourceFormat::R16_FLOAT;
        break;
      case vk::Format::eD16Unorm:
        return ResourceFormat::D16_UNORM;
        break;
      case vk::Format::eR16Unorm:
        return ResourceFormat::R16_UNORM;
        break;
      case vk::Format::eR16Uint:
        return ResourceFormat::R16_UINT;
        break;
      case vk::Format::eR16Snorm:
        return ResourceFormat::R16_SNORM;
        break;
      case vk::Format::eR16Sint:
        return ResourceFormat::R16_SINT;
        break;
      case vk::Format::eR8Unorm:
        return ResourceFormat::R8_UNORM;
        break;
      case vk::Format::eR8Uint:
        return ResourceFormat::R8_UINT;
        break;
      case vk::Format::eR8Snorm:
        return ResourceFormat::R8_SNORM;
        break;
      case vk::Format::eR8Sint:
        return ResourceFormat::R8_SINT;
        break;
      case vk::Format::eBc1RgbaUnormBlock:
        return ResourceFormat::BC1_UNORM;
        break;
      case vk::Format::eBc1RgbaSrgbBlock:
        return ResourceFormat::BC1_UNORM_SRGB;
        break;
      case vk::Format::eBc2UnormBlock:
        return ResourceFormat::BC2_UNORM;
        break;
      case vk::Format::eBc2SrgbBlock:
        return ResourceFormat::BC2_UNORM_SRGB;
        break;
      case vk::Format::eBc3UnormBlock:
        return ResourceFormat::BC3_UNORM;
        break;
      case vk::Format::eBc3SrgbBlock:
        return ResourceFormat::BC3_UNORM_SRGB;
        break;
      case vk::Format::eBc4UnormBlock:
        return ResourceFormat::BC4_UNORM;
        break;
      case vk::Format::eBc4SnormBlock:
        return ResourceFormat::BC4_SNORM;
        break;
      case vk::Format::eBc5UnormBlock:
        return ResourceFormat::BC5_UNORM;
        break;
      case vk::Format::eBc5SnormBlock:
        return ResourceFormat::BC5_SNORM;
        break;
      case vk::Format::eB8G8R8A8Unorm:
        return ResourceFormat::B8G8R8A8_UNORM;
        break;
      case vk::Format::eB8G8R8A8Srgb:
        return ResourceFormat::B8G8R8A8_UNORM_SRGB;
        break;
      case vk::Format::eBc6HUfloatBlock:
        return ResourceFormat::BC6H_UF16;
        break;
      case vk::Format::eBc6HSfloatBlock:
        return ResourceFormat::BC6H_SF16;
        break;
      case vk::Format::eBc7UnormBlock:
        return ResourceFormat::BC7_UNORM;
        break;
      case vk::Format::eBc7SrgbBlock:
        return ResourceFormat::BC7_UNORM_SRGB;
        break;
    }
    return ResourceFormat::UNKNOWN;
  }
} // namespace VKUtils
