#include <RHI/DX12/GraphicsDevice_DX12.h>

#ifdef WICKEDENGINE_BUILD_DX12

#  include <Foundation/Basics/Platform/Win/HResultUtils.h>
#  include <Foundation/Logging/Log.h>
#  include <RHI/DX12/D3D12MemAlloc.h>
#  include <RHI/DX12/d3dx12.h>

#  include <d3d12shader.h>
#  include <dxcapi.h>
#  include <pix.h>

#  pragma comment(lib, "d3d12.lib")
#  pragma comment(lib, "Dxgi.lib")
#  pragma comment(lib, "dxguid.lib")

#  ifdef _WIN64
#    ifndef PLATFORM_UWP
#      pragma comment(lib, "dxcompiler.lib")
#    endif // PLATFORM_UWP
#  endif   // _X64

#  ifdef _DEBUG
#    include <d3d12sdklayers.h>
#  endif // _DEBUG

#  include <algorithm>
#  include <sstream>
#  include <tchar.h>
#  include <wincodec.h>

using namespace Microsoft::WRL;

namespace DX12_Internal
{
  // Engine -> Native converters

  inline ezUInt32 _ParseColorWriteMask(ezBitflags<ezRHIColorWriteMask> value)
  {
    ezUInt32 _flag = 0;

    if (value == D3D12_COLOR_WRITE_ENABLE_ALL)
    {
      return D3D12_COLOR_WRITE_ENABLE_ALL;
    }
    else
    {
      if (value.IsSet(ezRHIColorWriteMask::Red))
        _flag |= D3D12_COLOR_WRITE_ENABLE_RED;
      if (value.IsSet(ezRHIColorWriteMask::Green))
        _flag |= D3D12_COLOR_WRITE_ENABLE_GREEN;
      if (value.IsSet(ezRHIColorWriteMask::Blue))
        _flag |= D3D12_COLOR_WRITE_ENABLE_BLUE;
      if (value.IsSet(ezRHIColorWriteMask::Alpha))
        _flag |= D3D12_COLOR_WRITE_ENABLE_ALPHA;
    }

    return _flag;
  }

  inline D3D12_FILTER _ConvertFilter(ezEnum<ezRHIFilter> value)
  {
    switch (value)
    {
      case ezRHIFilter::FILTER_MIN_MAG_MIP_POINT:
        return D3D12_FILTER_MIN_MAG_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_MIN_MAG_POINT_MIP_LINEAR:
        return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT:
        return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_MIN_POINT_MAG_MIP_LINEAR:
        return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_MIN_LINEAR_MAG_MIP_POINT:
        return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
        return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_MIN_MAG_LINEAR_MIP_POINT:
        return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_MIN_MAG_MIP_LINEAR:
        return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_ANISOTROPIC:
        return D3D12_FILTER_ANISOTROPIC;
        break;
      case ezRHIFilter::FILTER_COMPARISON_MIN_MAG_MIP_POINT:
        return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
        return D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
        return D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
        return D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
        return D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
        return D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
        return D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_COMPARISON_MIN_MAG_MIP_LINEAR:
        return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_COMPARISON_ANISOTROPIC:
        return D3D12_FILTER_COMPARISON_ANISOTROPIC;
        break;
      case ezRHIFilter::FILTER_MINIMUM_MIN_MAG_MIP_POINT:
        return D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR:
        return D3D12_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
        return D3D12_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR:
        return D3D12_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT:
        return D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
        return D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT:
        return D3D12_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_MINIMUM_MIN_MAG_MIP_LINEAR:
        return D3D12_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_MINIMUM_ANISOTROPIC:
        return D3D12_FILTER_MINIMUM_ANISOTROPIC;
        break;
      case ezRHIFilter::FILTER_MAXIMUM_MIN_MAG_MIP_POINT:
        return D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR:
        return D3D12_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
        return D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR:
        return D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT:
        return D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
        return D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT:
        return D3D12_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR:
        return D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_MAXIMUM_ANISOTROPIC:
        return D3D12_FILTER_MAXIMUM_ANISOTROPIC;
        break;
      default:
        break;
    }
    return D3D12_FILTER_MIN_MAG_MIP_POINT;
  }
  inline D3D12_TEXTURE_ADDRESS_MODE _ConvertTextureAddressMode(ezEnum<ezRHITextureAddressMode> value)
  {
    switch (value)
    {
      case ezRHITextureAddressMode::Wrap:
        return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        break;
      case ezRHITextureAddressMode::Mirror:
        return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        break;
      case ezRHITextureAddressMode::Clamp:
        return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        break;
      case ezRHITextureAddressMode::Border:
        return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        break;
      default:
        break;
    }
    return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
  }
  inline D3D12_COMPARISON_FUNC _ConvertComparisonFunc(ezEnum<ezRHIComparisonFunc> value)
  {
    switch (value)
    {
      case ezRHIComparisonFunc::Never:
        return D3D12_COMPARISON_FUNC_NEVER;
        break;
      case ezRHIComparisonFunc::Less:
        return D3D12_COMPARISON_FUNC_LESS;
        break;
      case ezRHIComparisonFunc::Equal:
        return D3D12_COMPARISON_FUNC_EQUAL;
        break;
      case ezRHIComparisonFunc::LessEqual:
        return D3D12_COMPARISON_FUNC_LESS_EQUAL;
        break;
      case ezRHIComparisonFunc::Greater:
        return D3D12_COMPARISON_FUNC_GREATER;
        break;
      case ezRHIComparisonFunc::NotEqual:
        return D3D12_COMPARISON_FUNC_NOT_EQUAL;
        break;
      case ezRHIComparisonFunc::GreaterEqual:
        return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
        break;
      case ezRHIComparisonFunc::Always:
        return D3D12_COMPARISON_FUNC_ALWAYS;
        break;
      default:
        break;
    }
    return D3D12_COMPARISON_FUNC_NEVER;
  }
  inline D3D12_FILL_MODE _ConvertFillMode(ezEnum<ezRHIFillMode> value)
  {
    switch (value)
    {
      case ezRHIFillMode::Wireframe:
        return D3D12_FILL_MODE_WIREFRAME;
        break;
      case ezRHIFillMode::Solid:
        return D3D12_FILL_MODE_SOLID;
        break;
      default:
        break;
    }
    return D3D12_FILL_MODE_WIREFRAME;
  }
  inline D3D12_CULL_MODE _ConvertCullMode(ezEnum<ezRHICullMode> value)
  {
    switch (value)
    {
      case ezRHICullMode::None:
        return D3D12_CULL_MODE_NONE;
        break;
      case ezRHICullMode::Front:
        return D3D12_CULL_MODE_FRONT;
        break;
      case ezRHICullMode::Back:
        return D3D12_CULL_MODE_BACK;
        break;
      default:
        break;
    }
    return D3D12_CULL_MODE_NONE;
  }
  inline D3D12_DEPTH_WRITE_MASK _ConvertDepthWriteMask(ezEnum<ezRHIDepthWriteMask> value)
  {
    switch (value)
    {
      case ezRHIDepthWriteMask::Zero:
        return D3D12_DEPTH_WRITE_MASK_ZERO;
        break;
      case ezRHIDepthWriteMask::All:
        return D3D12_DEPTH_WRITE_MASK_ALL;
        break;
      default:
        break;
    }
    return D3D12_DEPTH_WRITE_MASK_ZERO;
  }
  inline D3D12_STENCIL_OP _ConvertStencilOp(ezEnum<ezRHIStencilOp> value)
  {
    switch (value)
    {
      case ezRHIStencilOp::Keep:
        return D3D12_STENCIL_OP_KEEP;
        break;
      case ezRHIStencilOp::Zero:
        return D3D12_STENCIL_OP_ZERO;
        break;
      case ezRHIStencilOp::Replace:
        return D3D12_STENCIL_OP_REPLACE;
        break;
      case ezRHIStencilOp::IncrementSaturated:
        return D3D12_STENCIL_OP_INCR_SAT;
        break;
      case ezRHIStencilOp::DecrementSaturated:
        return D3D12_STENCIL_OP_DECR_SAT;
        break;
      case ezRHIStencilOp::Invert:
        return D3D12_STENCIL_OP_INVERT;
        break;
      case ezRHIStencilOp::Increment:
        return D3D12_STENCIL_OP_INCR;
        break;
      case ezRHIStencilOp::Decrement:
        return D3D12_STENCIL_OP_DECR;
        break;
      default:
        break;
    }
    return D3D12_STENCIL_OP_KEEP;
  }
  inline D3D12_BLEND _ConvertBlend(ezEnum<ezRHIBlendFactor> value)
  {
    switch (value)
    {
      case ezRHIBlendFactor::Zero:
        return D3D12_BLEND_ZERO;
        break;
      case ezRHIBlendFactor::One:
        return D3D12_BLEND_ONE;
        break;
      case ezRHIBlendFactor::SourceColor:
        return D3D12_BLEND_SRC_COLOR;
        break;
      case ezRHIBlendFactor::InverseSourceColor:
        return D3D12_BLEND_INV_SRC_COLOR;
        break;
      case ezRHIBlendFactor::SourceAlpha:
        return D3D12_BLEND_SRC_ALPHA;
        break;
      case ezRHIBlendFactor::InverseSourceAlpha:
        return D3D12_BLEND_INV_SRC_ALPHA;
        break;
      case ezRHIBlendFactor::DestinationAlpha:
        return D3D12_BLEND_DEST_ALPHA;
        break;
      case ezRHIBlendFactor::InverseDestinationAlpha:
        return D3D12_BLEND_INV_DEST_ALPHA;
        break;
      case ezRHIBlendFactor::DestinationColor:
        return D3D12_BLEND_DEST_COLOR;
        break;
      case ezRHIBlendFactor::InverseDestinationColor:
        return D3D12_BLEND_INV_DEST_COLOR;
        break;
      case ezRHIBlendFactor::SourceAlphaSaturated:
        return D3D12_BLEND_SRC_ALPHA_SAT;
        break;
      case ezRHIBlendFactor::BlendFactor:
        return D3D12_BLEND_BLEND_FACTOR;
        break;
      case ezRHIBlendFactor::InverseBlendFactor:
        return D3D12_BLEND_INV_BLEND_FACTOR;
        break;
      case ezRHIBlendFactor::Source1Color:
        return D3D12_BLEND_SRC1_COLOR;
        break;
      case ezRHIBlendFactor::InverseSource1Color:
        return D3D12_BLEND_INV_SRC1_COLOR;
        break;
      case ezRHIBlendFactor::Source1Alpha:
        return D3D12_BLEND_SRC1_ALPHA;
        break;
      case ezRHIBlendFactor::InverseSource1Alpha:
        return D3D12_BLEND_INV_SRC1_ALPHA;
        break;
      default:
        break;
    }
    return D3D12_BLEND_ZERO;
  }
  inline D3D12_BLEND_OP _ConvertBlendOp(ezEnum<ezRHIBlendOp> value)
  {
    switch (value)
    {
      case ezRHIBlendOp::Add:
        return D3D12_BLEND_OP_ADD;
        break;
      case ezRHIBlendOp::Subtract:
        return D3D12_BLEND_OP_SUBTRACT;
        break;
      case ezRHIBlendOp::ReverseSubtract:
        return D3D12_BLEND_OP_REV_SUBTRACT;
        break;
      case ezRHIBlendOp::Min:
        return D3D12_BLEND_OP_MIN;
        break;
      case ezRHIBlendOp::Max:
        return D3D12_BLEND_OP_MAX;
        break;
      default:
        break;
    }
    return D3D12_BLEND_OP_ADD;
  }
  inline D3D12_INPUT_CLASSIFICATION _ConvertInputClassification(ezEnum<ezRHIInputClassification> value)
  {
    switch (value)
    {
      case ezRHIInputClassification::PerVertexData:
        return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        break;
      case ezRHIInputClassification::PerInstanceData:
        return D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
        break;
      default:
        break;
    }
    return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
  }
  inline DXGI_FORMAT _ConvertFormat(ezEnum<ezRHIFormat> value)
  {
    switch (value)
    {
      case ezRHIFormat::Unknown:
        return DXGI_FORMAT_UNKNOWN;
        break;
      case ezRHIFormat::R32G32B32A32_FLOAT:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
        break;
      case ezRHIFormat::R32G32B32A32_UINT:
        return DXGI_FORMAT_R32G32B32A32_UINT;
        break;
      case ezRHIFormat::R32G32B32A32_SINT:
        return DXGI_FORMAT_R32G32B32A32_SINT;
        break;
      case ezRHIFormat::R32G32B32_FLOAT:
        return DXGI_FORMAT_R32G32B32_FLOAT;
        break;
      case ezRHIFormat::R32G32B32_UINT:
        return DXGI_FORMAT_R32G32B32_UINT;
        break;
      case ezRHIFormat::R32G32B32_SINT:
        return DXGI_FORMAT_R32G32B32_SINT;
        break;
      case ezRHIFormat::R16G16B16A16_FLOAT:
        return DXGI_FORMAT_R16G16B16A16_FLOAT;
        break;
      case ezRHIFormat::R16G16B16A16_UNORM:
        return DXGI_FORMAT_R16G16B16A16_UNORM;
        break;
      case ezRHIFormat::R16G16B16A16_UINT:
        return DXGI_FORMAT_R16G16B16A16_UINT;
        break;
      case ezRHIFormat::R16G16B16A16_SNORM:
        return DXGI_FORMAT_R16G16B16A16_SNORM;
        break;
      case ezRHIFormat::R16G16B16A16_SINT:
        return DXGI_FORMAT_R16G16B16A16_SINT;
        break;
      case ezRHIFormat::R32G32_FLOAT:
        return DXGI_FORMAT_R32G32_FLOAT;
        break;
      case ezRHIFormat::R32G32_UINT:
        return DXGI_FORMAT_R32G32_UINT;
        break;
      case ezRHIFormat::R32G32_SINT:
        return DXGI_FORMAT_R32G32_SINT;
        break;
      case ezRHIFormat::R32G8X24_TYPELESS:
        return DXGI_FORMAT_R32G8X24_TYPELESS;
        break;
      case ezRHIFormat::D32_FLOAT_S8X24_UINT:
        return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        break;
      case ezRHIFormat::R10G10B10A2_UNORM:
        return DXGI_FORMAT_R10G10B10A2_UNORM;
        break;
      case ezRHIFormat::R10G10B10A2_UINT:
        return DXGI_FORMAT_R10G10B10A2_UINT;
        break;
      case ezRHIFormat::R11G11B10_FLOAT:
        return DXGI_FORMAT_R11G11B10_FLOAT;
        break;
      case ezRHIFormat::R8G8B8A8_UNORM:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
        break;
      case ezRHIFormat::R8G8B8A8_UNORM_SRGB:
        return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        break;
      case ezRHIFormat::R8G8B8A8_UINT:
        return DXGI_FORMAT_R8G8B8A8_UINT;
        break;
      case ezRHIFormat::R8G8B8A8_SNORM:
        return DXGI_FORMAT_R8G8B8A8_SNORM;
        break;
      case ezRHIFormat::R8G8B8A8_SINT:
        return DXGI_FORMAT_R8G8B8A8_SINT;
        break;
      case ezRHIFormat::R16G16_FLOAT:
        return DXGI_FORMAT_R16G16_FLOAT;
        break;
      case ezRHIFormat::R16G16_UNORM:
        return DXGI_FORMAT_R16G16_UNORM;
        break;
      case ezRHIFormat::R16G16_UINT:
        return DXGI_FORMAT_R16G16_UINT;
        break;
      case ezRHIFormat::R16G16_SNORM:
        return DXGI_FORMAT_R16G16_SNORM;
        break;
      case ezRHIFormat::R16G16_SINT:
        return DXGI_FORMAT_R16G16_SINT;
        break;
      case ezRHIFormat::R32_TYPELESS:
        return DXGI_FORMAT_R32_TYPELESS;
        break;
      case ezRHIFormat::D32_FLOAT:
        return DXGI_FORMAT_D32_FLOAT;
        break;
      case ezRHIFormat::R32_FLOAT:
        return DXGI_FORMAT_R32_FLOAT;
        break;
      case ezRHIFormat::R32_UINT:
        return DXGI_FORMAT_R32_UINT;
        break;
      case ezRHIFormat::R32_SINT:
        return DXGI_FORMAT_R32_SINT;
        break;
      case ezRHIFormat::R8G8_UNORM:
        return DXGI_FORMAT_R8G8_UNORM;
        break;
      case ezRHIFormat::R8G8_UINT:
        return DXGI_FORMAT_R8G8_UINT;
        break;
      case ezRHIFormat::R8G8_SNORM:
        return DXGI_FORMAT_R8G8_SNORM;
        break;
      case ezRHIFormat::R8G8_SINT:
        return DXGI_FORMAT_R8G8_SINT;
        break;
      case ezRHIFormat::R16_TYPELESS:
        return DXGI_FORMAT_R16_TYPELESS;
        break;
      case ezRHIFormat::R16_FLOAT:
        return DXGI_FORMAT_R16_FLOAT;
        break;
      case ezRHIFormat::D16_UNORM:
        return DXGI_FORMAT_D16_UNORM;
        break;
      case ezRHIFormat::R16_UNORM:
        return DXGI_FORMAT_R16_UNORM;
        break;
      case ezRHIFormat::R16_UINT:
        return DXGI_FORMAT_R16_UINT;
        break;
      case ezRHIFormat::R16_SNORM:
        return DXGI_FORMAT_R16_SNORM;
        break;
      case ezRHIFormat::R16_SINT:
        return DXGI_FORMAT_R16_SINT;
        break;
      case ezRHIFormat::R8_UNORM:
        return DXGI_FORMAT_R8_UNORM;
        break;
      case ezRHIFormat::R8_UINT:
        return DXGI_FORMAT_R8_UINT;
        break;
      case ezRHIFormat::R8_SNORM:
        return DXGI_FORMAT_R8_SNORM;
        break;
      case ezRHIFormat::R8_SINT:
        return DXGI_FORMAT_R8_SINT;
        break;
      case ezRHIFormat::BC1_UNORM:
        return DXGI_FORMAT_BC1_UNORM;
        break;
      case ezRHIFormat::BC1_UNORM_SRGB:
        return DXGI_FORMAT_BC1_UNORM_SRGB;
        break;
      case ezRHIFormat::BC2_UNORM:
        return DXGI_FORMAT_BC2_UNORM;
        break;
      case ezRHIFormat::BC2_UNORM_SRGB:
        return DXGI_FORMAT_BC2_UNORM_SRGB;
        break;
      case ezRHIFormat::BC3_UNORM:
        return DXGI_FORMAT_BC3_UNORM;
        break;
      case ezRHIFormat::BC3_UNORM_SRGB:
        return DXGI_FORMAT_BC3_UNORM_SRGB;
        break;
      case ezRHIFormat::BC4_UNORM:
        return DXGI_FORMAT_BC4_UNORM;
        break;
      case ezRHIFormat::BC4_SNORM:
        return DXGI_FORMAT_BC4_SNORM;
        break;
      case ezRHIFormat::BC5_UNORM:
        return DXGI_FORMAT_BC5_UNORM;
        break;
      case ezRHIFormat::BC5_SNORM:
        return DXGI_FORMAT_BC5_SNORM;
        break;
      case ezRHIFormat::B8G8R8A8_UNORM:
        return DXGI_FORMAT_B8G8R8A8_UNORM;
        break;
      case ezRHIFormat::B8G8R8A8_UNORM_SRGB:
        return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        break;
      case ezRHIFormat::BC6H_UF16:
        return DXGI_FORMAT_BC6H_UF16;
        break;
      case ezRHIFormat::BC6H_SF16:
        return DXGI_FORMAT_BC6H_SF16;
        break;
      case ezRHIFormat::BC7_UNORM:
        return DXGI_FORMAT_BC7_UNORM;
        break;
      case ezRHIFormat::BC7_UNORM_SRGB:
        return DXGI_FORMAT_BC7_UNORM_SRGB;
        break;
    }
    return DXGI_FORMAT_UNKNOWN;
  }
  inline D3D12_SUBRESOURCE_DATA _ConvertSubresourceData(const SubresourceData& pInitialData)
  {
    D3D12_SUBRESOURCE_DATA data;
    data.pData = pInitialData.pSysMem;
    data.RowPitch = pInitialData.SysMemPitch;
    data.SlicePitch = pInitialData.SysMemSlicePitch;

    return data;
  }
  constexpr D3D12_RESOURCE_STATES _ConvertImageLayout(IMAGE_LAYOUT value)
  {
    switch (value)
    {
      case IMAGE_LAYOUT_UNDEFINED:
      case IMAGE_LAYOUT_GENERAL:
        return D3D12_RESOURCE_STATE_COMMON;
      case IMAGE_LAYOUT_RENDERTARGET:
        return D3D12_RESOURCE_STATE_RENDER_TARGET;
      case IMAGE_LAYOUT_DEPTHSTENCIL:
        return D3D12_RESOURCE_STATE_DEPTH_WRITE;
      case IMAGE_LAYOUT_DEPTHSTENCIL_READONLY:
        return D3D12_RESOURCE_STATE_DEPTH_READ;
      case IMAGE_LAYOUT_SHADER_RESOURCE:
        return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
      case IMAGE_LAYOUT_UNORDERED_ACCESS:
        return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
      case IMAGE_LAYOUT_COPY_SRC:
        return D3D12_RESOURCE_STATE_COPY_SOURCE;
      case IMAGE_LAYOUT_COPY_DST:
        return D3D12_RESOURCE_STATE_COPY_DEST;
      case IMAGE_LAYOUT_SHADING_RATE_SOURCE:
        return D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE;
    }

    return D3D12_RESOURCE_STATE_COMMON;
  }
  constexpr D3D12_RESOURCE_STATES _ConvertBufferState(BUFFER_STATE value)
  {
    switch (value)
    {
      case BUFFER_STATE_GENERAL:
        return D3D12_RESOURCE_STATE_COMMON;
      case BUFFER_STATE_VERTEX_BUFFER:
        return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
      case BUFFER_STATE_INDEX_BUFFER:
        return D3D12_RESOURCE_STATE_INDEX_BUFFER;
      case BUFFER_STATE_CONSTANT_BUFFER:
        return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
      case BUFFER_STATE_INDIRECT_ARGUMENT:
        return D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
      case BUFFER_STATE_SHADER_RESOURCE:
        return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
      case BUFFER_STATE_UNORDERED_ACCESS:
        return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
      case BUFFER_STATE_COPY_SRC:
        return D3D12_RESOURCE_STATE_COPY_SOURCE;
      case BUFFER_STATE_COPY_DST:
        return D3D12_RESOURCE_STATE_COPY_DEST;
      case BUFFER_STATE_RAYTRACING_ACCELERATION_STRUCTURE:
        return D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
    }

    return D3D12_RESOURCE_STATE_COMMON;
  }

  inline D3D12_SHADER_VISIBILITY _ConvertShaderVisibility(ezEnum<ezRHIShaderStage> value)
  {
    switch (value)
    {
      case ezRHIShaderStage::MeshShader:
        return D3D12_SHADER_VISIBILITY_MESH;
      case ezRHIShaderStage::AmplificationShader:
        return D3D12_SHADER_VISIBILITY_AMPLIFICATION;
      case ezRHIShaderStage::VertexShader:
        return D3D12_SHADER_VISIBILITY_VERTEX;
      case ezRHIShaderStage::HullShader:
        return D3D12_SHADER_VISIBILITY_HULL;
      case ezRHIShaderStage::DomainShader:
        return D3D12_SHADER_VISIBILITY_DOMAIN;
      case ezRHIShaderStage::GeometryShader:
        return D3D12_SHADER_VISIBILITY_GEOMETRY;
      case ezRHIShaderStage::PixelShader:
        return D3D12_SHADER_VISIBILITY_PIXEL;
      default:
        return D3D12_SHADER_VISIBILITY_ALL;
    }
    return D3D12_SHADER_VISIBILITY_ALL;
  }
  constexpr D3D12_SHADING_RATE _ConvertShadingRate(SHADING_RATE value)
  {
    switch (value)
    {
      case SHADING_RATE_1X1:
        return D3D12_SHADING_RATE_1X1;
      case SHADING_RATE_1X2:
        return D3D12_SHADING_RATE_1X2;
      case SHADING_RATE_2X1:
        return D3D12_SHADING_RATE_2X1;
      case SHADING_RATE_2X2:
        return D3D12_SHADING_RATE_2X2;
      case SHADING_RATE_2X4:
        return D3D12_SHADING_RATE_2X4;
      case SHADING_RATE_4X2:
        return D3D12_SHADING_RATE_4X2;
      case SHADING_RATE_4X4:
        return D3D12_SHADING_RATE_4X4;
      default:
        return D3D12_SHADING_RATE_1X1;
    }
    return D3D12_SHADING_RATE_1X1;
  }

  // Native -> Engine converters

  inline ezEnum<ezRHIFormat> _ConvertFormat_Inv(DXGI_FORMAT value)
  {
    switch (value)
    {
      case DXGI_FORMAT_UNKNOWN:
        return ezRHIFormat::Unknown;
        break;
      case DXGI_FORMAT_R32G32B32A32_FLOAT:
        return ezRHIFormat::R32G32B32A32_FLOAT;
        break;
      case DXGI_FORMAT_R32G32B32A32_UINT:
        return ezRHIFormat::R32G32B32A32_UINT;
        break;
      case DXGI_FORMAT_R32G32B32A32_SINT:
        return ezRHIFormat::R32G32B32A32_SINT;
        break;
      case DXGI_FORMAT_R32G32B32_FLOAT:
        return ezRHIFormat::R32G32B32_FLOAT;
        break;
      case DXGI_FORMAT_R32G32B32_UINT:
        return ezRHIFormat::R32G32B32_UINT;
        break;
      case DXGI_FORMAT_R32G32B32_SINT:
        return ezRHIFormat::R32G32B32_SINT;
        break;
      case DXGI_FORMAT_R16G16B16A16_FLOAT:
        return ezRHIFormat::R16G16B16A16_FLOAT;
        break;
      case DXGI_FORMAT_R16G16B16A16_UNORM:
        return ezRHIFormat::R16G16B16A16_UNORM;
        break;
      case DXGI_FORMAT_R16G16B16A16_UINT:
        return ezRHIFormat::R16G16B16A16_UINT;
        break;
      case DXGI_FORMAT_R16G16B16A16_SNORM:
        return ezRHIFormat::R16G16B16A16_SNORM;
        break;
      case DXGI_FORMAT_R16G16B16A16_SINT:
        return ezRHIFormat::R16G16B16A16_SINT;
        break;
      case DXGI_FORMAT_R32G32_FLOAT:
        return ezRHIFormat::R32G32_FLOAT;
        break;
      case DXGI_FORMAT_R32G32_UINT:
        return ezRHIFormat::R32G32_UINT;
        break;
      case DXGI_FORMAT_R32G32_SINT:
        return ezRHIFormat::R32G32_SINT;
        break;
      case DXGI_FORMAT_R32G8X24_TYPELESS:
        return ezRHIFormat::R32G8X24_TYPELESS;
        break;
      case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        return ezRHIFormat::D32_FLOAT_S8X24_UINT;
        break;
      case DXGI_FORMAT_R10G10B10A2_UNORM:
        return ezRHIFormat::R10G10B10A2_UNORM;
        break;
      case DXGI_FORMAT_R10G10B10A2_UINT:
        return ezRHIFormat::R10G10B10A2_UINT;
        break;
      case DXGI_FORMAT_R11G11B10_FLOAT:
        return ezRHIFormat::R11G11B10_FLOAT;
        break;
      case DXGI_FORMAT_R8G8B8A8_UNORM:
        return ezRHIFormat::R8G8B8A8_UNORM;
        break;
      case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        return ezRHIFormat::R8G8B8A8_UNORM_SRGB;
        break;
      case DXGI_FORMAT_R8G8B8A8_UINT:
        return ezRHIFormat::R8G8B8A8_UINT;
        break;
      case DXGI_FORMAT_R8G8B8A8_SNORM:
        return ezRHIFormat::R8G8B8A8_SNORM;
        break;
      case DXGI_FORMAT_R8G8B8A8_SINT:
        return ezRHIFormat::R8G8B8A8_SINT;
        break;
      case DXGI_FORMAT_R16G16_FLOAT:
        return ezRHIFormat::R16G16_FLOAT;
        break;
      case DXGI_FORMAT_R16G16_UNORM:
        return ezRHIFormat::R16G16_UNORM;
        break;
      case DXGI_FORMAT_R16G16_UINT:
        return ezRHIFormat::R16G16_UINT;
        break;
      case DXGI_FORMAT_R16G16_SNORM:
        return ezRHIFormat::R16G16_SNORM;
        break;
      case DXGI_FORMAT_R16G16_SINT:
        return ezRHIFormat::R16G16_SINT;
        break;
      case DXGI_FORMAT_R32_TYPELESS:
        return ezRHIFormat::R32_TYPELESS;
        break;
      case DXGI_FORMAT_D32_FLOAT:
        return ezRHIFormat::D32_FLOAT;
        break;
      case DXGI_FORMAT_R32_FLOAT:
        return ezRHIFormat::R32_FLOAT;
        break;
      case DXGI_FORMAT_R32_UINT:
        return ezRHIFormat::R32_UINT;
        break;
      case DXGI_FORMAT_R32_SINT:
        return ezRHIFormat::R32_SINT;
        break;
      case DXGI_FORMAT_R8G8_UNORM:
        return ezRHIFormat::R8G8_UNORM;
        break;
      case DXGI_FORMAT_R8G8_UINT:
        return ezRHIFormat::R8G8_UINT;
        break;
      case DXGI_FORMAT_R8G8_SNORM:
        return ezRHIFormat::R8G8_SNORM;
        break;
      case DXGI_FORMAT_R8G8_SINT:
        return ezRHIFormat::R8G8_SINT;
        break;
      case DXGI_FORMAT_R16_TYPELESS:
        return ezRHIFormat::R16_TYPELESS;
        break;
      case DXGI_FORMAT_R16_FLOAT:
        return ezRHIFormat::R16_FLOAT;
        break;
      case DXGI_FORMAT_D16_UNORM:
        return ezRHIFormat::D16_UNORM;
        break;
      case DXGI_FORMAT_R16_UNORM:
        return ezRHIFormat::R16_UNORM;
        break;
      case DXGI_FORMAT_R16_UINT:
        return ezRHIFormat::R16_UINT;
        break;
      case DXGI_FORMAT_R16_SNORM:
        return ezRHIFormat::R16_SNORM;
        break;
      case DXGI_FORMAT_R16_SINT:
        return ezRHIFormat::R16_SINT;
        break;
      case DXGI_FORMAT_R8_UNORM:
        return ezRHIFormat::R8_UNORM;
        break;
      case DXGI_FORMAT_R8_UINT:
        return ezRHIFormat::R8_UINT;
        break;
      case DXGI_FORMAT_R8_SNORM:
        return ezRHIFormat::R8_SNORM;
        break;
      case DXGI_FORMAT_R8_SINT:
        return ezRHIFormat::R8_SINT;
        break;
      case DXGI_FORMAT_BC1_UNORM:
        return ezRHIFormat::BC1_UNORM;
        break;
      case DXGI_FORMAT_BC1_UNORM_SRGB:
        return ezRHIFormat::BC1_UNORM_SRGB;
        break;
      case DXGI_FORMAT_BC2_UNORM:
        return ezRHIFormat::BC2_UNORM;
        break;
      case DXGI_FORMAT_BC2_UNORM_SRGB:
        return ezRHIFormat::BC2_UNORM_SRGB;
        break;
      case DXGI_FORMAT_BC3_UNORM:
        return ezRHIFormat::BC3_UNORM;
        break;
      case DXGI_FORMAT_BC3_UNORM_SRGB:
        return ezRHIFormat::BC3_UNORM_SRGB;
        break;
      case DXGI_FORMAT_BC4_UNORM:
        return ezRHIFormat::BC4_UNORM;
        break;
      case DXGI_FORMAT_BC4_SNORM:
        return ezRHIFormat::BC4_SNORM;
        break;
      case DXGI_FORMAT_BC5_UNORM:
        return ezRHIFormat::BC5_UNORM;
        break;
      case DXGI_FORMAT_BC5_SNORM:
        return ezRHIFormat::BC5_SNORM;
        break;
      case DXGI_FORMAT_B8G8R8A8_UNORM:
        return ezRHIFormat::B8G8R8A8_UNORM;
        break;
      case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        return ezRHIFormat::B8G8R8A8_UNORM_SRGB;
        break;
      case DXGI_FORMAT_BC6H_UF16:
        return ezRHIFormat::BC6H_UF16;
        break;
      case DXGI_FORMAT_BC6H_SF16:
        return ezRHIFormat::BC6H_SF16;
        break;
      case DXGI_FORMAT_BC7_UNORM:
        return ezRHIFormat::BC7_UNORM;
        break;
      case DXGI_FORMAT_BC7_UNORM_SRGB:
        return ezRHIFormat::BC7_UNORM_SRGB;
        break;
    }
    return ezRHIFormat::Unknown;
  }

  inline TextureDesc _ConvertTextureDesc_Inv(const D3D12_RESOURCE_DESC& desc)
  {
    TextureDesc retVal;

    switch (desc.Dimension)
    {
      case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
        retVal.type = TextureDesc::TEXTURE_1D;
        retVal.ArraySize = desc.DepthOrArraySize;
        break;
      default:
      case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
        retVal.type = TextureDesc::TEXTURE_2D;
        retVal.ArraySize = desc.DepthOrArraySize;
        break;
      case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
        retVal.type = TextureDesc::TEXTURE_3D;
        retVal.Depth = desc.DepthOrArraySize;
        break;
    }
    retVal.Format = _ConvertFormat_Inv(desc.Format);
    retVal.Width = (ezUInt32)desc.Width;
    retVal.Height = desc.Height;
    retVal.MipLevels = desc.MipLevels;

    return retVal;
  }


  // Local Helpers:

  inline size_t Align(size_t uLocation, size_t uAlign)
  {
    if ((0 == uAlign) || (uAlign & (uAlign - 1)))
    {
      assert(0);
    }

    return ((uLocation + (uAlign - 1)) & ~(uAlign - 1));
  }


  struct Resource_DX12
  {
    std::shared_ptr<GraphicsDevice_DX12::AllocationHandler> allocationhandler;
    D3D12MA::Allocation* allocation = nullptr;
    ComPtr<ID3D12Resource> resource;
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbv = {};
    D3D12_SHADER_RESOURCE_VIEW_DESC srv = {};
    D3D12_UNORDERED_ACCESS_VIEW_DESC uav = {};
    ezDynamicArray<D3D12_SHADER_RESOURCE_VIEW_DESC> subresources_srv;
    ezDynamicArray<D3D12_UNORDERED_ACCESS_VIEW_DESC> subresources_uav;

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;

    GraphicsDevice::GPUAllocation dynamic[COMMANDLIST_COUNT];

    virtual ~Resource_DX12()
    {
      allocationhandler->destroylocker.lock();
      ezUInt64 framecount = allocationhandler->framecount;
      if (allocation)
        allocationhandler->destroyer_allocations.push_back(std::make_pair(allocation, framecount));
      if (resource)
        allocationhandler->destroyer_resources.push_back(std::make_pair(resource, framecount));
      allocationhandler->destroylocker.unlock();
    }
  };
  struct Texture_DX12 : public Resource_DX12
  {
    D3D12_RENDER_TARGET_VIEW_DESC rtv = {};
    D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
    ezDynamicArray<D3D12_RENDER_TARGET_VIEW_DESC> subresources_rtv;
    ezDynamicArray<D3D12_DEPTH_STENCIL_VIEW_DESC> subresources_dsv;

    ~Texture_DX12() override
    {
      allocationhandler->destroylocker.lock();
      ezUInt64 framecount = allocationhandler->framecount;
      allocationhandler->destroylocker.unlock();
    }
  };
  struct Sampler_DX12
  {
    std::shared_ptr<GraphicsDevice_DX12::AllocationHandler> allocationhandler;
    D3D12_SAMPLER_DESC descriptor;

    ~Sampler_DX12()
    {
      allocationhandler->destroylocker.lock();
      ezUInt64 framecount = allocationhandler->framecount;
      allocationhandler->destroylocker.unlock();
    }
  };
  struct Query_DX12
  {
    std::shared_ptr<GraphicsDevice_DX12::AllocationHandler> allocationhandler;
    GPU_QUERY_TYPE query_type = GPU_QUERY_TYPE_INVALID;
    ezUInt32 query_index = ~0;

    ~Query_DX12()
    {
      if (query_index != ~0)
      {
        allocationhandler->destroylocker.lock();
        ezUInt64 framecount = allocationhandler->framecount;
        switch (query_type)
        {
          case GPU_QUERY_TYPE_OCCLUSION:
          case GPU_QUERY_TYPE_OCCLUSION_PREDICATE:
            allocationhandler->destroyer_queries_occlusion.push_back(std::make_pair(query_index, framecount));
            break;
          case GPU_QUERY_TYPE_TIMESTAMP:
            allocationhandler->destroyer_queries_timestamp.push_back(std::make_pair(query_index, framecount));
            break;
        }
        allocationhandler->destroylocker.unlock();
      }
    }
  };
  struct PipelineState_DX12
  {
    std::shared_ptr<GraphicsDevice_DX12::AllocationHandler> allocationhandler;
    ComPtr<ID3D12PipelineState> resource;
    ComPtr<ID3D12RootSignature> rootSignature;

    ezDynamicArray<D3D12_DESCRIPTOR_RANGE> resources;
    ezDynamicArray<D3D12_DESCRIPTOR_RANGE> samplers;

    ~PipelineState_DX12()
    {
      allocationhandler->destroylocker.lock();
      ezUInt64 framecount = allocationhandler->framecount;
      if (resource)
        allocationhandler->destroyer_pipelines.push_back(std::make_pair(resource, framecount));
      if (rootSignature)
        allocationhandler->destroyer_rootSignatures.push_back(std::make_pair(rootSignature, framecount));
      allocationhandler->destroylocker.unlock();
    }
  };
  struct BVH_DX12 : public Resource_DX12
  {
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS desc = {};
    ezDynamicArray<D3D12_RAYTRACING_GEOMETRY_DESC> geometries;
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info = {};
    GPUBuffer scratch;
  };
  struct RTPipelineState_DX12
  {
    std::shared_ptr<GraphicsDevice_DX12::AllocationHandler> allocationhandler;
    ComPtr<ID3D12StateObject> resource;

    ezDynamicArray<ezString> export_strings;
    ezDynamicArray<D3D12_EXPORT_DESC> exports;
    ezDynamicArray<D3D12_DXIL_LIBRARY_DESC> library_descs;
    ezDynamicArray<ezString> group_strings;
    ezDynamicArray<D3D12_HIT_GROUP_DESC> hitgroup_descs;

    ~RTPipelineState_DX12()
    {
      allocationhandler->destroylocker.lock();
      ezUInt64 framecount = allocationhandler->framecount;
      if (resource)
        allocationhandler->destroyer_stateobjects.push_back(std::make_pair(resource, framecount));
      allocationhandler->destroylocker.unlock();
    }
  };
  struct RenderPass_DX12
  {
    D3D12_RESOURCE_BARRIER barrierdescs_begin[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
    ezUInt32 num_barriers_begin = 0;
    D3D12_RESOURCE_BARRIER barrierdescs_end[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
    ezUInt32 num_barriers_end = 0;
  };
  struct DescriptorTable_DX12
  {
    std::shared_ptr<GraphicsDevice_DX12::AllocationHandler> allocationhandler;

    struct Heap
    {
      ComPtr<ID3D12DescriptorHeap> heap;
      D3D12_DESCRIPTOR_HEAP_DESC desc = {};
      D3D12_CPU_DESCRIPTOR_HANDLE address = {};
      ezDynamicArray<D3D12_DESCRIPTOR_RANGE> ranges;
      ezDynamicArray<size_t> write_remap;
    };
    Heap sampler_heap;
    Heap resource_heap;
    ezDynamicArray<D3D12_STATIC_SAMPLER_DESC> staticsamplers;

    ~DescriptorTable_DX12()
    {
      allocationhandler->destroylocker.lock();
      ezUInt64 framecount = allocationhandler->framecount;
      if (sampler_heap.heap)
        allocationhandler->destroyer_descriptorHeaps.push_back(std::make_pair(sampler_heap.heap, framecount));
      if (resource_heap.heap)
        allocationhandler->destroyer_descriptorHeaps.push_back(std::make_pair(resource_heap.heap, framecount));
      allocationhandler->destroylocker.unlock();
    }
  };
  struct RootSignature_DX12
  {
    std::shared_ptr<GraphicsDevice_DX12::AllocationHandler> allocationhandler;
    ComPtr<ID3D12RootSignature> resource;
    ezDynamicArray<D3D12_ROOT_PARAMETER> params;

    ezDynamicArray<ezUInt32> table_bind_point_remap;
    ezUInt32 root_constant_bind_remap = 0;

    struct RootRemap
    {
      ezUInt32 space = 0;
      ezUInt32 rangeIndex = 0;
    };
    ezDynamicArray<RootRemap> root_remap;

    ~RootSignature_DX12()
    {
      allocationhandler->destroylocker.lock();
      ezUInt64 framecount = allocationhandler->framecount;
      if (resource)
        allocationhandler->destroyer_rootSignatures.push_back(std::make_pair(resource, framecount));
      allocationhandler->destroylocker.unlock();
    }
  };

  Resource_DX12* to_internal(const GPUResource* param)
  {
    return static_cast<Resource_DX12*>(param->internal_state.get());
  }
  Resource_DX12* to_internal(const GPUBuffer* param)
  {
    return static_cast<Resource_DX12*>(param->internal_state.get());
  }
  Texture_DX12* to_internal(const Texture* param)
  {
    return static_cast<Texture_DX12*>(param->internal_state.get());
  }
  Sampler_DX12* to_internal(const Sampler* param)
  {
    return static_cast<Sampler_DX12*>(param->internal_state.get());
  }
  Query_DX12* to_internal(const GPUQuery* param)
  {
    return static_cast<Query_DX12*>(param->internal_state.get());
  }
  PipelineState_DX12* to_internal(const Shader* param)
  {
    return static_cast<PipelineState_DX12*>(param->internal_state.get());
  }
  PipelineState_DX12* to_internal(const PipelineState* param)
  {
    return static_cast<PipelineState_DX12*>(param->internal_state.get());
  }
  BVH_DX12* to_internal(const RaytracingAccelerationStructure* param)
  {
    return static_cast<BVH_DX12*>(param->internal_state.get());
  }
  RTPipelineState_DX12* to_internal(const RaytracingPipelineState* param)
  {
    return static_cast<RTPipelineState_DX12*>(param->internal_state.get());
  }
  RenderPass_DX12* to_internal(const RenderPass* param)
  {
    return static_cast<RenderPass_DX12*>(param->internal_state.get());
  }
  DescriptorTable_DX12* to_internal(const DescriptorTable* param)
  {
    return static_cast<DescriptorTable_DX12*>(param->internal_state.get());
  }
  RootSignature_DX12* to_internal(const RootSignature* param)
  {
    return static_cast<RootSignature_DX12*>(param->internal_state.get());
  }
} // namespace DX12_Internal
using namespace DX12_Internal;

// Allocators:

void GraphicsDevice_DX12::FrameResources::ResourceFrameAllocator::init(GraphicsDevice_DX12* device, size_t size)
{
  this->device = device;
  auto internal_state = std::make_shared<Resource_DX12>();
  internal_state->allocationhandler = device->allocationhandler;
  buffer.internal_state = internal_state;

  HRESULT hr;

  D3D12MA::ALLOCATION_DESC allocationDesc = {};
  allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

  CD3DX12_RESOURCE_DESC resdesc = CD3DX12_RESOURCE_DESC::Buffer(size);

  hr = device->allocationhandler->allocator->CreateResource(
    &allocationDesc,
    &resdesc,
    D3D12_RESOURCE_STATE_GENERIC_READ,
    nullptr,
    &internal_state->allocation,
    IID_PPV_ARGS(&internal_state->resource));
  assert(SUCCEEDED(hr));

  void* pData;
  CD3DX12_RANGE readRange(0, 0);
  internal_state->resource->Map(0, &readRange, &pData);
  dataCur = dataBegin = reinterpret_cast<ezUInt8*>(pData);
  dataEnd = dataBegin + size;

  // Because the "buffer" is created by hand in this, fill the desc to indicate how it can be used:
  buffer.type = GPUResource::GPU_RESOURCE_TYPE::BUFFER;
  buffer.desc.ByteWidth = (ezUInt32)((size_t)dataEnd - (size_t)dataBegin);
  buffer.desc.Usage = ezRHIUsage::Dynamic;
  buffer.desc.BindFlags = BIND_VERTEX_BUFFER | BIND_INDEX_BUFFER | BIND_SHADER_RESOURCE;
  buffer.desc.MiscFlags = RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
  internal_state->srv.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
  internal_state->srv.Format = DXGI_FORMAT_R32_TYPELESS;
  internal_state->srv.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
  internal_state->srv.Buffer.NumElements = buffer.desc.ByteWidth / sizeof(ezUInt32);
  internal_state->srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
}
ezUInt8* GraphicsDevice_DX12::FrameResources::ResourceFrameAllocator::allocate(size_t dataSize, size_t alignment)
{
  dataCur = reinterpret_cast<ezUInt8*>(Align(reinterpret_cast<size_t>(dataCur), alignment));

  if (dataCur + dataSize > dataEnd)
  {
    init(device, ((size_t)dataEnd + dataSize - (size_t)dataBegin) * 2);
  }

  ezUInt8* retVal = dataCur;

  dataCur += dataSize;

  return retVal;
}
void GraphicsDevice_DX12::FrameResources::ResourceFrameAllocator::clear()
{
  dataCur = dataBegin;
}
ezUInt64 GraphicsDevice_DX12::FrameResources::ResourceFrameAllocator::calculateOffset(ezUInt8* address)
{
  assert(address >= dataBegin && address < dataEnd);
  return static_cast<ezUInt64>(address - dataBegin);
}

void GraphicsDevice_DX12::FrameResources::DescriptorTableFrameAllocator::init(GraphicsDevice_DX12* device)
{
  this->device = device;

  // Reset state to empty:
  reset();

  heaps_resource.SetCount(1);
  heaps_sampler.SetCount(1);
}
void GraphicsDevice_DX12::FrameResources::DescriptorTableFrameAllocator::reset()
{
  dirty = true;
  heaps_bound = false;
  for (auto& x : heaps_resource)
  {
    x.ringOffset = 0;
  }
  for (auto& x : heaps_sampler)
  {
    x.ringOffset = 0;
  }
  current_resource_heap = 0;
  current_sampler_heap = 0;

  memset(CBV, 0, sizeof(CBV));
  memset(SRV, 0, sizeof(SRV));
  memset(SRV_index, -1, sizeof(SRV_index));
  memset(UAV, 0, sizeof(UAV));
  memset(UAV_index, -1, sizeof(UAV_index));
  memset(SAM, 0, sizeof(SAM));
}
void GraphicsDevice_DX12::FrameResources::DescriptorTableFrameAllocator::request_heaps(ezUInt32 resources, ezUInt32 samplers, CommandList cmd)
{
  // This function allocatesGPU visible descriptor heaps that can fit the requested table sizes.
  //	First, they grow the heaps until the size fits the dx12 resource limits (tier 1 resource limit = 1 million, sampler limit is 2048)
  //	When the limits are reached, and there is still a need to allocate, then completely new heap blocks are started
  //
  //	The function will automatically bind descriptor heaps when there was a new (growing or block allocation)

  DescriptorHeap& heap_resource = heaps_resource[current_resource_heap];
  ezUInt32 allocation = heap_resource.ringOffset + resources;
  if (heap_resource.heapDesc.NumDescriptors <= allocation)
  {
    if (allocation > 1000000) // tier 1 limit
    {
      // need new block
      allocation -= heap_resource.ringOffset;
      current_resource_heap++;
      if (heaps_resource.GetCount() <= current_resource_heap)
      {
        heaps_resource.SetCount(current_resource_heap + 1);
      }
    }
    DescriptorHeap& heap = heaps_resource[current_resource_heap];

    // Need to re-check if growing is necessary (maybe step into new block is enough):
    if (heap.heapDesc.NumDescriptors <= allocation)
    {
      // grow rate is controlled here:
      allocation = ezMath::Max(512u, allocation);

      allocation = ezMath::PowerOfTwo_Ceil(allocation);
      allocation = ezMath::Min(1000000u, allocation);

      // Issue destruction of the old heap:
      device->allocationhandler->destroylocker.lock();
      ezUInt64 framecount = device->allocationhandler->framecount;
      device->allocationhandler->destroyer_descriptorHeaps.push_back(std::make_pair(heap.heap_GPU, framecount));
      device->allocationhandler->destroylocker.unlock();

      heap.heapDesc.NodeMask = 0;
      heap.heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
      heap.heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
      heap.heapDesc.NumDescriptors = allocation;
      HRESULT hr = device->device->CreateDescriptorHeap(&heap.heapDesc, IID_PPV_ARGS(&heap.heap_GPU));
      assert(SUCCEEDED(hr));

      // Save heap properties:
      heap.start_cpu = heap.heap_GPU->GetCPUDescriptorHandleForHeapStart();
      heap.start_gpu = heap.heap_GPU->GetGPUDescriptorHandleForHeapStart();
    }

    heaps_bound = false;
  }

  DescriptorHeap& heap_sampler = heaps_sampler[current_sampler_heap];
  allocation = heap_sampler.ringOffset + samplers;
  if (heap_sampler.heapDesc.NumDescriptors <= allocation)
  {
    if (allocation > 2048) // sampler limit
    {
      // need new block
      allocation -= heap_sampler.ringOffset;
      current_sampler_heap++;
      if (heaps_sampler.GetCount() <= current_sampler_heap)
      {
        heaps_sampler.SetCount(current_sampler_heap + 1);
      }
    }
    DescriptorHeap& heap = heaps_sampler[current_sampler_heap];

    // Need to re-check if growing is necessary (maybe step into new block is enough):
    if (heap.heapDesc.NumDescriptors <= allocation)
    {
      // grow rate is controlled here:
      allocation = ezMath::Max(512u, allocation);
      allocation = ezMath::PowerOfTwo_Ceil(allocation);
      allocation = ezMath::Min(2048u, allocation);

      // Issue destruction of the old heap:
      device->allocationhandler->destroylocker.lock();
      ezUInt64 framecount = device->allocationhandler->framecount;
      device->allocationhandler->destroyer_descriptorHeaps.push_back(std::make_pair(heap.heap_GPU, framecount));
      device->allocationhandler->destroylocker.unlock();

      heap.heapDesc.NodeMask = 0;
      heap.heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
      heap.heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
      heap.heapDesc.NumDescriptors = allocation;
      HRESULT hr = device->device->CreateDescriptorHeap(&heap.heapDesc, IID_PPV_ARGS(&heap.heap_GPU));
      assert(SUCCEEDED(hr));

      // Save heap properties:
      heap.start_cpu = heap.heap_GPU->GetCPUDescriptorHandleForHeapStart();
      heap.start_gpu = heap.heap_GPU->GetGPUDescriptorHandleForHeapStart();
    }

    heaps_bound = false;
  }

  if (!heaps_bound)
  {
    // definitely re-index the heap blocks!
    ID3D12DescriptorHeap* heaps[] = {
      heaps_resource[current_resource_heap].heap_GPU.Get(),
      heaps_sampler[current_sampler_heap].heap_GPU.Get()};
    device->GetDirectCommandList(cmd)->SetDescriptorHeaps(EZ_ARRAY_SIZE(heaps), heaps);
  }
}
void GraphicsDevice_DX12::FrameResources::DescriptorTableFrameAllocator::validate(bool graphics, CommandList cmd)
{
  if (!dirty)
    return;
  dirty = true;

  auto pso_internal = graphics ? to_internal(device->active_pso[cmd]) : to_internal(device->active_cs[cmd]);

  request_heaps(pso_internal->resources.GetCount(), pso_internal->samplers.GetCount(), cmd);

  UINT root_parameter_index = 0;

  {

    {

      // Resources:
      if (!pso_internal->resources.IsEmpty())
      {
        DescriptorHeap& heap = heaps_resource[current_resource_heap];

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc = {};
        D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
        srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srv_desc.Format = DXGI_FORMAT_R32_UINT;
        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;

        D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
        uav_desc.Format = DXGI_FORMAT_R32_UINT;
        uav_desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;

        ezUInt32 index = 0;
        for (auto& x : pso_internal->resources)
        {
          D3D12_CPU_DESCRIPTOR_HANDLE dst = heap.start_cpu;
          dst.ptr += (heap.ringOffset + index) * device->resource_descriptor_size;

          switch (x.RangeType)
          {
            default:
            case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
            {
              const GPUResource* resource = SRV[x.BaseShaderRegister];
              const int subresource = SRV_index[x.BaseShaderRegister];
              if (resource == nullptr || !resource->IsValid())
              {
                device->device->CreateShaderResourceView(nullptr, &srv_desc, dst);
              }
              else
              {
                auto internal_state = to_internal(resource);

                if (resource->IsAccelerationStructure())
                {
                  device->device->CreateShaderResourceView(nullptr, &internal_state->srv, dst);
                }
                else
                {
                  if (subresource < 0)
                  {
                    device->device->CreateShaderResourceView(internal_state->resource.Get(), &internal_state->srv, dst);
                  }
                  else
                  {
                    device->device->CreateShaderResourceView(internal_state->resource.Get(), &internal_state->subresources_srv[subresource], dst);
                  }
                }
              }
            }
            break;

            case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
            {
              const GPUResource* resource = UAV[x.BaseShaderRegister];
              const int subresource = UAV_index[x.BaseShaderRegister];
              if (resource == nullptr || !resource->IsValid())
              {
                device->device->CreateUnorderedAccessView(nullptr, nullptr, &uav_desc, dst);
              }
              else
              {
                auto internal_state = to_internal(resource);

                D3D12_CPU_DESCRIPTOR_HANDLE src = {};
                if (subresource < 0)
                {
                  device->device->CreateUnorderedAccessView(internal_state->resource.Get(), nullptr, &internal_state->uav, dst);
                }
                else
                {
                  device->device->CreateUnorderedAccessView(internal_state->resource.Get(), nullptr, &internal_state->subresources_uav[subresource], dst);
                }

                if (src.ptr != 0)
                {
                  device->device->CopyDescriptorsSimple(1, dst, src, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                }
              }
            }
            break;

            case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
            {
              const GPUBuffer* buffer = CBV[x.BaseShaderRegister];
              if (buffer == nullptr || !buffer->IsValid())
              {
                device->device->CreateConstantBufferView(&cbv_desc, dst);
              }
              else
              {
                auto internal_state = to_internal(buffer);

                if (buffer->desc.Usage == ezRHIUsage::Dynamic)
                {
                  GraphicsDevice::GPUAllocation allocation = internal_state->dynamic[cmd];
                  D3D12_CONSTANT_BUFFER_VIEW_DESC cbv;
                  cbv.BufferLocation = to_internal(allocation.buffer)->resource->GetGPUVirtualAddress();
                  cbv.BufferLocation += (D3D12_GPU_VIRTUAL_ADDRESS)allocation.offset;
                  cbv.SizeInBytes = (ezUInt32)Align((size_t)buffer->desc.ByteWidth, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

                  device->device->CreateConstantBufferView(&cbv, dst);
                }
                else
                {
                  device->device->CreateConstantBufferView(&internal_state->cbv, dst);
                }
              }
            }
            break;
          }

          index++;
        }

        D3D12_GPU_DESCRIPTOR_HANDLE binding_table = heap.start_gpu;
        binding_table.ptr += (UINT64)heap.ringOffset * (UINT64)device->resource_descriptor_size;

        if (graphics)
        {
          device->GetDirectCommandList(cmd)->SetGraphicsRootDescriptorTable(root_parameter_index, binding_table);
        }
        else
        {
          device->GetDirectCommandList(cmd)->SetComputeRootDescriptorTable(root_parameter_index, binding_table);
        }

        heap.ringOffset += pso_internal->resources.GetCount();
        root_parameter_index++;
      }

      // Samplers:
      if (!pso_internal->samplers.IsEmpty())
      {
        DescriptorHeap& heap = heaps_sampler[current_sampler_heap];

        D3D12_SAMPLER_DESC sampler_desc = {};
        sampler_desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        sampler_desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        sampler_desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        sampler_desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        sampler_desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;

        size_t index = 0;
        for (auto& x : pso_internal->samplers)
        {
          D3D12_CPU_DESCRIPTOR_HANDLE dst = heap.start_cpu;
          dst.ptr += (heap.ringOffset + index) * device->sampler_descriptor_size;

          const Sampler* sampler = SAM[x.BaseShaderRegister];
          if (sampler == nullptr || !sampler->IsValid())
          {
            device->device->CreateSampler(&sampler_desc, dst);
          }
          else
          {
            auto internal_state = to_internal(sampler);
            device->device->CreateSampler(&internal_state->descriptor, dst);
          }

          index++;
        }

        D3D12_GPU_DESCRIPTOR_HANDLE binding_table = heap.start_gpu;
        binding_table.ptr += (UINT64)heap.ringOffset * (UINT64)device->sampler_descriptor_size;

        if (graphics)
        {
          device->GetDirectCommandList(cmd)->SetGraphicsRootDescriptorTable(root_parameter_index, binding_table);
        }
        else
        {
          device->GetDirectCommandList(cmd)->SetComputeRootDescriptorTable(root_parameter_index, binding_table);
        }

        heap.ringOffset += pso_internal->samplers.GetCount();
        root_parameter_index++;
      }
    }
  }
}
GraphicsDevice_DX12::FrameResources::DescriptorTableFrameAllocator::DescriptorHandles
GraphicsDevice_DX12::FrameResources::DescriptorTableFrameAllocator::commit(const DescriptorTable* table, CommandList cmd)
{
  auto internal_state = to_internal(table);

  request_heaps(internal_state->resource_heap.desc.NumDescriptors, internal_state->sampler_heap.desc.NumDescriptors, cmd);

  DescriptorHandles handles;

  if (!internal_state->sampler_heap.ranges.IsEmpty())
  {
    DescriptorHeap& heap = heaps_sampler[current_sampler_heap];
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = heap.start_cpu;
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = heap.start_gpu;
    cpu_handle.ptr += heap.ringOffset * device->sampler_descriptor_size;
    gpu_handle.ptr += heap.ringOffset * device->sampler_descriptor_size;
    heap.ringOffset += internal_state->sampler_heap.desc.NumDescriptors;
    device->device->CopyDescriptorsSimple(
      internal_state->sampler_heap.desc.NumDescriptors,
      cpu_handle,
      internal_state->sampler_heap.address,
      internal_state->sampler_heap.desc.Type);
    handles.sampler_handle = gpu_handle;
  }

  if (!internal_state->resource_heap.ranges.IsEmpty())
  {
    DescriptorHeap& heap = heaps_resource[current_resource_heap];
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = heap.start_cpu;
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = heap.start_gpu;
    cpu_handle.ptr += heap.ringOffset * device->resource_descriptor_size;
    gpu_handle.ptr += heap.ringOffset * device->resource_descriptor_size;
    heap.ringOffset += internal_state->resource_heap.desc.NumDescriptors;
    device->device->CopyDescriptorsSimple(
      internal_state->resource_heap.desc.NumDescriptors,
      cpu_handle,
      internal_state->resource_heap.address,
      internal_state->resource_heap.desc.Type);
    handles.resource_handle = gpu_handle;
  }

  return handles;
}


void GraphicsDevice_DX12::pso_validate(CommandList cmd)
{
  if (!dirty_pso[cmd])
    return;

  const PipelineState* pso = active_pso[cmd];
  size_t pipeline_hash = prev_pipeline_hash[cmd];

  auto internal_state = to_internal(pso);

  ID3D12PipelineState* pipeline = nullptr;
  auto it = pipelines_global.find(pipeline_hash);
  if (it == pipelines_global.end())
  {
    for (auto& x : pipelines_worker[cmd])
    {
      if (pipeline_hash == x.first)
      {
        pipeline = x.second.Get();
        break;
      }
    }

    if (pipeline == nullptr)
    {
      struct PSO_STREAM
      {
        CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
        CD3DX12_PIPELINE_STATE_STREAM_VS VS;
        CD3DX12_PIPELINE_STATE_STREAM_HS HS;
        CD3DX12_PIPELINE_STATE_STREAM_DS DS;
        CD3DX12_PIPELINE_STATE_STREAM_GS GS;
        CD3DX12_PIPELINE_STATE_STREAM_PS PS;
        CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER RS;
        CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DSS;
        CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC BD;
        CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PT;
        CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT IL;
        CD3DX12_PIPELINE_STATE_STREAM_IB_STRIP_CUT_VALUE STRIP;
        CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSFormat;
        CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS Formats;
        CD3DX12_PIPELINE_STATE_STREAM_SAMPLE_DESC SampleDesc;
        CD3DX12_PIPELINE_STATE_STREAM_SAMPLE_MASK SampleMask;

        CD3DX12_PIPELINE_STATE_STREAM_MS MS;
        CD3DX12_PIPELINE_STATE_STREAM_AS AS;
      } stream = {};

      if (pso->desc.vs != nullptr)
      {
        stream.VS = {pso->desc.vs->code.GetData(), pso->desc.vs->code.GetCount()};
      }
      if (pso->desc.hs != nullptr)
      {
        stream.HS = {pso->desc.hs->code.GetData(), pso->desc.hs->code.GetCount()};
      }
      if (pso->desc.ds != nullptr)
      {
        stream.DS = {pso->desc.ds->code.GetData(), pso->desc.ds->code.GetCount()};
      }
      if (pso->desc.gs != nullptr)
      {
        stream.GS = {pso->desc.gs->code.GetData(), pso->desc.gs->code.GetCount()};
      }
      if (pso->desc.ps != nullptr)
      {
        stream.PS = {pso->desc.ps->code.GetData(), pso->desc.ps->code.GetCount()};
      }

      if (pso->desc.ms != nullptr)
      {
        stream.MS = {pso->desc.ms->code.GetData(), pso->desc.ms->code.GetCount()};
      }
      if (pso->desc.as != nullptr)
      {
        stream.AS = {pso->desc.as->code.GetData(), pso->desc.as->code.GetCount()};
      }

      RasterizerStateDesc pRasterizerStateDesc = pso->desc.rs != nullptr ? pso->desc.rs->GetDesc() : RasterizerStateDesc();
      CD3DX12_RASTERIZER_DESC rs = {};
      rs.FillMode = _ConvertFillMode(pRasterizerStateDesc.FillMode);
      rs.CullMode = _ConvertCullMode(pRasterizerStateDesc.CullMode);
      rs.FrontCounterClockwise = pRasterizerStateDesc.FrontCounterClockwise;
      rs.DepthBias = pRasterizerStateDesc.DepthBias;
      rs.DepthBiasClamp = pRasterizerStateDesc.DepthBiasClamp;
      rs.SlopeScaledDepthBias = pRasterizerStateDesc.SlopeScaledDepthBias;
      rs.DepthClipEnable = pRasterizerStateDesc.DepthClipEnable;
      rs.MultisampleEnable = pRasterizerStateDesc.MultisampleEnable;
      rs.AntialiasedLineEnable = pRasterizerStateDesc.AntialiasedLineEnable;
      rs.ConservativeRaster = ((CONSERVATIVE_RASTERIZATION && pRasterizerStateDesc.ConservativeRasterizationEnable) ? D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON : D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);
      rs.ForcedSampleCount = pRasterizerStateDesc.ForcedSampleCount;
      stream.RS = rs;

      DepthStencilStateDesc pDepthStencilStateDesc = pso->desc.dss != nullptr ? pso->desc.dss->GetDesc() : DepthStencilStateDesc();
      CD3DX12_DEPTH_STENCIL_DESC dss = {};
      dss.DepthEnable = pDepthStencilStateDesc.DepthEnable;
      dss.DepthWriteMask = _ConvertDepthWriteMask(pDepthStencilStateDesc.DepthWriteMask);
      dss.DepthFunc = _ConvertComparisonFunc(pDepthStencilStateDesc.DepthFunc);
      dss.StencilEnable = pDepthStencilStateDesc.StencilEnable;
      dss.StencilReadMask = pDepthStencilStateDesc.StencilReadMask;
      dss.StencilWriteMask = pDepthStencilStateDesc.StencilWriteMask;
      dss.FrontFace.StencilDepthFailOp = _ConvertStencilOp(pDepthStencilStateDesc.FrontFace.StencilDepthFailOp);
      dss.FrontFace.StencilFailOp = _ConvertStencilOp(pDepthStencilStateDesc.FrontFace.StencilFailOp);
      dss.FrontFace.StencilFunc = _ConvertComparisonFunc(pDepthStencilStateDesc.FrontFace.StencilFunc);
      dss.FrontFace.StencilPassOp = _ConvertStencilOp(pDepthStencilStateDesc.FrontFace.StencilPassOp);
      dss.BackFace.StencilDepthFailOp = _ConvertStencilOp(pDepthStencilStateDesc.BackFace.StencilDepthFailOp);
      dss.BackFace.StencilFailOp = _ConvertStencilOp(pDepthStencilStateDesc.BackFace.StencilFailOp);
      dss.BackFace.StencilFunc = _ConvertComparisonFunc(pDepthStencilStateDesc.BackFace.StencilFunc);
      dss.BackFace.StencilPassOp = _ConvertStencilOp(pDepthStencilStateDesc.BackFace.StencilPassOp);
      stream.DSS = dss;

      BlendStateDesc pBlendStateDesc = pso->desc.bs != nullptr ? pso->desc.bs->GetDesc() : BlendStateDesc();
      CD3DX12_BLEND_DESC bd = {};
      bd.AlphaToCoverageEnable = pBlendStateDesc.AlphaToCoverageEnable;
      bd.IndependentBlendEnable = pBlendStateDesc.IndependentBlendEnable;
      for (int i = 0; i < 8; ++i)
      {
        bd.RenderTarget[i].BlendEnable = pBlendStateDesc.RenderTarget[i].BlendEnable;
        bd.RenderTarget[i].SrcBlend = _ConvertBlend(pBlendStateDesc.RenderTarget[i].SrcBlend);
        bd.RenderTarget[i].DestBlend = _ConvertBlend(pBlendStateDesc.RenderTarget[i].DestBlend);
        bd.RenderTarget[i].BlendOp = _ConvertBlendOp(pBlendStateDesc.RenderTarget[i].BlendOp);
        bd.RenderTarget[i].SrcBlendAlpha = _ConvertBlend(pBlendStateDesc.RenderTarget[i].SrcBlendAlpha);
        bd.RenderTarget[i].DestBlendAlpha = _ConvertBlend(pBlendStateDesc.RenderTarget[i].DestBlendAlpha);
        bd.RenderTarget[i].BlendOpAlpha = _ConvertBlendOp(pBlendStateDesc.RenderTarget[i].BlendOpAlpha);
        bd.RenderTarget[i].RenderTargetWriteMask = _ParseColorWriteMask(pBlendStateDesc.RenderTarget[i].RenderTargetWriteMask);
      }
      stream.BD = bd;

      ezDynamicArray<D3D12_INPUT_ELEMENT_DESC> elements;
      D3D12_INPUT_LAYOUT_DESC il = {};
      if (pso->desc.il != nullptr)
      {
        il.NumElements = (ezUInt32)pso->desc.il->desc.GetCount();
        elements.SetCount(il.NumElements);
        for (ezUInt32 i = 0; i < il.NumElements; ++i)
        {
          elements[i].SemanticName = pso->desc.il->desc[i].SemanticName.GetData();
          elements[i].SemanticIndex = pso->desc.il->desc[i].SemanticIndex;
          elements[i].Format = _ConvertFormat(pso->desc.il->desc[i].Format);
          elements[i].InputSlot = pso->desc.il->desc[i].InputSlot;
          elements[i].AlignedByteOffset = pso->desc.il->desc[i].AlignedByteOffset;
          if (elements[i].AlignedByteOffset == InputLayoutDesc::APPEND_ALIGNED_ELEMENT)
            elements[i].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
          elements[i].InputSlotClass = _ConvertInputClassification(pso->desc.il->desc[i].InputSlotClass);
          elements[i].InstanceDataStepRate = pso->desc.il->desc[i].InstanceDataStepRate;
        }
      }
      il.pInputElementDescs = elements.GetData();
      stream.IL = il;

      DXGI_FORMAT DSFormat = DXGI_FORMAT_UNKNOWN;
      D3D12_RT_FORMAT_ARRAY formats = {};
      formats.NumRenderTargets = 0;
      DXGI_SAMPLE_DESC sampleDesc = {};
      sampleDesc.Count = 1;
      sampleDesc.Quality = 0;
      if (active_renderpass[cmd] == nullptr)
      {
        formats.NumRenderTargets = 1;
        formats.RTFormats[0] = _ConvertFormat(BACKBUFFER_FORMAT);
      }
      else
      {
        for (auto& attachment : active_renderpass[cmd]->desc.attachments)
        {
          if (attachment.type == RenderPassAttachment::RESOLVE || attachment.texture == nullptr)
            continue;

          switch (attachment.type)
          {
            case RenderPassAttachment::RENDERTARGET:
              switch (attachment.texture->desc.Format)
              {
                case ezRHIFormat::R16_TYPELESS:
                  formats.RTFormats[formats.NumRenderTargets] = DXGI_FORMAT_R16_UNORM;
                  break;
                case ezRHIFormat::R32_TYPELESS:
                  formats.RTFormats[formats.NumRenderTargets] = DXGI_FORMAT_R32_FLOAT;
                  break;
                case ezRHIFormat::R24G8_TYPELESS:
                  formats.RTFormats[formats.NumRenderTargets] = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
                  break;
                case ezRHIFormat::R32G8X24_TYPELESS:
                  formats.RTFormats[formats.NumRenderTargets] = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
                  break;
                default:
                  formats.RTFormats[formats.NumRenderTargets] = _ConvertFormat(attachment.texture->desc.Format);
                  break;
              }
              formats.NumRenderTargets++;
              break;
            case RenderPassAttachment::DEPTH_STENCIL:
              switch (attachment.texture->desc.Format)
              {
                case ezRHIFormat::R16_TYPELESS:
                  DSFormat = DXGI_FORMAT_D16_UNORM;
                  break;
                case ezRHIFormat::R32_TYPELESS:
                  DSFormat = DXGI_FORMAT_D32_FLOAT;
                  break;
                case ezRHIFormat::R24G8_TYPELESS:
                  DSFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
                  break;
                case ezRHIFormat::R32G8X24_TYPELESS:
                  DSFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
                  break;
                default:
                  DSFormat = _ConvertFormat(attachment.texture->desc.Format);
                  break;
              }
              break;
            default:
              assert(0);
              break;
          }

          sampleDesc.Count = attachment.texture->desc.SampleCount;
          sampleDesc.Quality = 0;
        }
      }
      stream.DSFormat = DSFormat;
      stream.Formats = formats;
      stream.SampleDesc = sampleDesc;
      stream.SampleMask = pso->desc.sampleMask;

      switch (pso->desc.pt)
      {
        case ezRHIPrimitiveTopology::PointList:
          stream.PT = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
          break;
        case ezRHIPrimitiveTopology::LineList:
        case ezRHIPrimitiveTopology::LineStrip:
          stream.PT = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
          break;
        case ezRHIPrimitiveTopology::TriangleList:
        case ezRHIPrimitiveTopology::TriangleStrip:
          stream.PT = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
          break;
        case ezRHIPrimitiveTopology::PatchList:
          stream.PT = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
          break;
        default:
          stream.PT = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
          break;
      }

      stream.STRIP = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

      if (pso->desc.rootSignature == nullptr)
      {
        stream.pRootSignature = internal_state->rootSignature.Get();
      }
      else
      {
        stream.pRootSignature = to_internal(pso->desc.rootSignature)->resource.Get();
      }

      D3D12_PIPELINE_STATE_STREAM_DESC streamDesc = {};
      streamDesc.pPipelineStateSubobjectStream = &stream;
      streamDesc.SizeInBytes = sizeof(stream);

      ComPtr<ID3D12PipelineState> newpso;
      HRESULT hr = device->CreatePipelineState(&streamDesc, IID_PPV_ARGS(&newpso));
      assert(SUCCEEDED(hr));

      pipelines_worker[cmd].PushBack(std::make_pair(pipeline_hash, newpso));
      pipeline = newpso.Get();
    }
  }
  else
  {
    pipeline = it->second.Get();
  }
  assert(pipeline != nullptr);

  GetDirectCommandList(cmd)->SetPipelineState(pipeline);

  if (prev_pt[cmd] != pso->desc.pt)
  {
    prev_pt[cmd] = pso->desc.pt;

    D3D12_PRIMITIVE_TOPOLOGY d3dType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    switch (pso->desc.pt)
    {
      case ezRHIPrimitiveTopology::TriangleList:
        d3dType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        break;
      case ezRHIPrimitiveTopology::TriangleStrip:
        d3dType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        break;
      case ezRHIPrimitiveTopology::PointList:
        d3dType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
        break;
      case ezRHIPrimitiveTopology::LineList:
        d3dType = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
        break;
      case ezRHIPrimitiveTopology::LineStrip:
        d3dType = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
        break;
      case ezRHIPrimitiveTopology::PatchList:
        d3dType = D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
        break;
      default:
        break;
    };
    GetDirectCommandList(cmd)->IASetPrimitiveTopology(d3dType);
  }
}

void GraphicsDevice_DX12::predraw(CommandList cmd)
{
  pso_validate(cmd);

  if (active_pso[cmd]->desc.rootSignature == nullptr)
  {
    GetFrameResources().descriptors[cmd].validate(true, cmd);
  }
}
void GraphicsDevice_DX12::predispatch(CommandList cmd)
{
  if (active_cs[cmd]->rootSignature == nullptr)
  {
    GetFrameResources().descriptors[cmd].validate(false, cmd);
  }
}
void GraphicsDevice_DX12::preraytrace(CommandList cmd)
{
  if (active_rt[cmd]->desc.rootSignature == nullptr)
  {
    GetFrameResources().descriptors[cmd].validate(false, cmd);
  }
}

// Engine functions
GraphicsDevice_DX12::GraphicsDevice_DX12(RHIWindowType window, bool fullscreen, bool debuglayer)
{
  DESCRIPTOR_MANAGEMENT = true;
  SHADER_IDENTIFIER_SIZE = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
  TOPLEVEL_ACCELERATION_STRUCTURE_INSTANCE_SIZE = sizeof(D3D12_RAYTRACING_INSTANCE_DESC);

  DEBUGDEVICE = debuglayer;
  FULLSCREEN = fullscreen;

#  ifndef PLATFORM_UWP
  RECT rect = RECT();
  GetClientRect(window, &rect);
  RESOLUTIONWIDTH = rect.right - rect.left;
  RESOLUTIONHEIGHT = rect.bottom - rect.top;
#  else PLATFORM_UWP
  float dpiscale = wiPlatform::GetDPIScaling();
  RESOLUTIONWIDTH = int(window->Bounds.Width * dpiscale);
  RESOLUTIONHEIGHT = int(window->Bounds.Height * dpiscale);
#  endif

  HRESULT hr = E_FAIL;

#  if !defined(PLATFORM_UWP)
  if (debuglayer)
  {
    // Enable the debug layer.
    HMODULE dx12 = LoadLibraryEx(_T("d3d12.dll"),
      nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    auto pD3D12GetDebugInterface =
      reinterpret_cast<PFN_D3D12_GET_DEBUG_INTERFACE>(
        GetProcAddress(dx12, "D3D12GetDebugInterface"));
    if (pD3D12GetDebugInterface)
    {
      ID3D12Debug* debugController;
      if (SUCCEEDED(pD3D12GetDebugInterface(
            IID_PPV_ARGS(&debugController))))
      {
        debugController->EnableDebugLayer();
      }
    }
  }
#  endif

  hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&device));
  if (FAILED(hr))
  {
    EZ_REPORT_FAILURE("Failed to create the graphics device! ERROR: {}", ezHRESULTtoString(hr) /*todo: std::hex*/);
  }

  D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
  allocatorDesc.pDevice = device.Get();

  allocationhandler = std::make_shared<AllocationHandler>();
  allocationhandler->device = device;

  hr = D3D12MA::CreateAllocator(&allocatorDesc, &allocationhandler->allocator);
  assert(SUCCEEDED(hr));

  // Create command queue
  D3D12_COMMAND_QUEUE_DESC directQueueDesc = {};
  directQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
  directQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
  directQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
  directQueueDesc.NodeMask = 0;
  hr = device->CreateCommandQueue(&directQueueDesc, IID_PPV_ARGS(&directQueue));
  assert(SUCCEEDED(hr));

  // Create fences for command queue:
  hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&frameFence));
  assert(SUCCEEDED(hr));
  frameFenceEvent = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);



  // Create swapchain

  ComPtr<IDXGIFactory4> pIDXGIFactory;
  hr = CreateDXGIFactory1(IID_PPV_ARGS(&pIDXGIFactory));
  assert(SUCCEEDED(hr));

  ComPtr<IDXGISwapChain1> _swapChain;

  DXGI_SWAP_CHAIN_DESC1 sd = {};
  sd.Width = RESOLUTIONWIDTH;
  sd.Height = RESOLUTIONHEIGHT;
  sd.Format = _ConvertFormat(GetBackBufferFormat());
  sd.Stereo = false;
  sd.SampleDesc.Count = 1; // Don't use multi-sampling.
  sd.SampleDesc.Quality = 0;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.BufferCount = BACKBUFFER_COUNT;
  sd.Flags = 0;
  sd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

#  ifndef PLATFORM_UWP
  sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
  sd.Scaling = DXGI_SCALING_STRETCH;

  DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc;
  fullscreenDesc.RefreshRate.Numerator = 60;
  fullscreenDesc.RefreshRate.Denominator = 1;
  fullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // needs to be unspecified for correct fullscreen scaling!
  fullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
  fullscreenDesc.Windowed = !fullscreen;
  hr = pIDXGIFactory->CreateSwapChainForHwnd(directQueue.Get(), window, &sd, &fullscreenDesc, nullptr, &_swapChain);
#  else
  sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // All Windows Store apps must use this SwapEffect.
  sd.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH;

  hr = pIDXGIFactory->CreateSwapChainForCoreWindow(directQueue.Get(), reinterpret_cast<IUnknown*>(window.Get()), &sd, nullptr, &_swapChain);
#  endif

  if (FAILED(hr))
  {
    EZ_REPORT_FAILURE("Failed to create a swapchain for the graphics device!");
  }

  hr = _swapChain.As(&swapChain);
  assert(SUCCEEDED(hr));



  // Create common descriptor heaps

  {
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NodeMask = 0;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    heapDesc.NumDescriptors = D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT * COMMANDLIST_COUNT;
    HRESULT hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&descriptorheap_RTV));
    assert(SUCCEEDED(hr));
    rtv_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    rtv_descriptor_heap_start = descriptorheap_RTV->GetCPUDescriptorHandleForHeapStart();
  }
  {
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NodeMask = 0;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    heapDesc.NumDescriptors = COMMANDLIST_COUNT;
    HRESULT hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&descriptorheap_DSV));
    assert(SUCCEEDED(hr));
    dsv_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    dsv_descriptor_heap_start = descriptorheap_DSV->GetCPUDescriptorHandleForHeapStart();
  }

  resource_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
  sampler_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

  // Create frame-resident resources:
  for (ezUInt32 fr = 0; fr < BACKBUFFER_COUNT; ++fr)
  {
    hr = swapChain->GetBuffer(fr, IID_PPV_ARGS(&backBuffers[fr]));
    assert(SUCCEEDED(hr));

    // Create copy queue:
    {
      D3D12_COMMAND_QUEUE_DESC copyQueueDesc = {};
      copyQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
      copyQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
      copyQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
      copyQueueDesc.NodeMask = 0;
      hr = device->CreateCommandQueue(&copyQueueDesc, IID_PPV_ARGS(&frames[fr].copyQueue));
      assert(SUCCEEDED(hr));
      hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&frames[fr].copyAllocator));
      assert(SUCCEEDED(hr));
      hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, frames[fr].copyAllocator.Get(), nullptr, IID_PPV_ARGS(&frames[fr].copyCommandList));
      assert(SUCCEEDED(hr));

      hr = static_cast<ID3D12GraphicsCommandList*>(frames[fr].copyCommandList.Get())->Close();
      assert(SUCCEEDED(hr));
    }
  }

  hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&copyFence));
  assert(SUCCEEDED(hr));

  // Query features:

  TESSELLATION = true;

  hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &features_0, sizeof(features_0));
  CONSERVATIVE_RASTERIZATION = features_0.ConservativeRasterizationTier >= D3D12_CONSERVATIVE_RASTERIZATION_TIER_1;
  RASTERIZER_ORDERED_VIEWS = features_0.ROVsSupported == TRUE;
  RENDERTARGET_AND_VIEWPORT_ARRAYINDEX_WITHOUT_GS = features_0.VPAndRTArrayIndexFromAnyShaderFeedingRasterizerSupportedWithoutGSEmulation == TRUE;

  if (features_0.TypedUAVLoadAdditionalFormats)
  {
    // More info about UAV format load support: https://docs.microsoft.com/en-us/windows/win32/direct3d12/typed-unordered-access-view-loads
    UAV_LOAD_FORMAT_COMMON = true;

    D3D12_FEATURE_DATA_FORMAT_SUPPORT FormatSupport = {DXGI_FORMAT_R11G11B10_FLOAT, D3D12_FORMAT_SUPPORT1_NONE, D3D12_FORMAT_SUPPORT2_NONE};
    hr = device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &FormatSupport, sizeof(FormatSupport));
    if (SUCCEEDED(hr) && (FormatSupport.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
    {
      UAV_LOAD_FORMAT_R11G11B10_FLOAT = true;
    }
  }

  hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &features_5, sizeof(features_5));
  RAYTRACING = features_5.RaytracingTier >= D3D12_RAYTRACING_TIER_1_0;
  RAYTRACING_INLINE = features_5.RaytracingTier >= D3D12_RAYTRACING_TIER_1_1;

  hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS6, &features_6, sizeof(features_6));
  VARIABLE_RATE_SHADING = features_6.VariableShadingRateTier >= D3D12_VARIABLE_SHADING_RATE_TIER_1;
  VARIABLE_RATE_SHADING_TIER2 = features_6.VariableShadingRateTier >= D3D12_VARIABLE_SHADING_RATE_TIER_2;
  VARIABLE_RATE_SHADING_TILE_SIZE = features_6.ShadingRateImageTileSize;

  hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &features_7, sizeof(features_7));
  MESH_SHADER = features_7.MeshShaderTier >= D3D12_MESH_SHADER_TIER_1;

  // Create common indirect command signatures:

  D3D12_COMMAND_SIGNATURE_DESC cmd_desc = {};

  D3D12_INDIRECT_ARGUMENT_DESC dispatchArgs[1];
  dispatchArgs[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;

  D3D12_INDIRECT_ARGUMENT_DESC drawInstancedArgs[1];
  drawInstancedArgs[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;

  D3D12_INDIRECT_ARGUMENT_DESC drawIndexedInstancedArgs[1];
  drawIndexedInstancedArgs[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

  cmd_desc.ByteStride = sizeof(IndirectDispatchArgs);
  cmd_desc.NumArgumentDescs = 1;
  cmd_desc.pArgumentDescs = dispatchArgs;
  hr = device->CreateCommandSignature(&cmd_desc, nullptr, IID_PPV_ARGS(&dispatchIndirectCommandSignature));
  assert(SUCCEEDED(hr));

  cmd_desc.ByteStride = sizeof(IndirectDrawArgsInstanced);
  cmd_desc.NumArgumentDescs = 1;
  cmd_desc.pArgumentDescs = drawInstancedArgs;
  hr = device->CreateCommandSignature(&cmd_desc, nullptr, IID_PPV_ARGS(&drawInstancedIndirectCommandSignature));
  assert(SUCCEEDED(hr));

  cmd_desc.ByteStride = sizeof(IndirectDrawArgsIndexedInstanced);
  cmd_desc.NumArgumentDescs = 1;
  cmd_desc.pArgumentDescs = drawIndexedInstancedArgs;
  hr = device->CreateCommandSignature(&cmd_desc, nullptr, IID_PPV_ARGS(&drawIndexedInstancedIndirectCommandSignature));
  assert(SUCCEEDED(hr));

  if (MESH_SHADER)
  {
    D3D12_INDIRECT_ARGUMENT_DESC dispatchMeshArgs[1];
    dispatchMeshArgs[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH_MESH;

    cmd_desc.ByteStride = sizeof(IndirectDispatchArgs);
    cmd_desc.NumArgumentDescs = 1;
    cmd_desc.pArgumentDescs = dispatchMeshArgs;
    hr = device->CreateCommandSignature(&cmd_desc, nullptr, IID_PPV_ARGS(&dispatchMeshIndirectCommandSignature));
    assert(SUCCEEDED(hr));
  }

  // GPU Queries:
  {
    D3D12_QUERY_HEAP_DESC queryheapdesc = {};
    queryheapdesc.NodeMask = 0;

    for (ezUInt32 i = 0; i < timestamp_query_count; ++i)
    {
      allocationhandler->free_timestampqueries.push_back(i);
    }
    queryheapdesc.Count = timestamp_query_count;
    queryheapdesc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
    hr = device->CreateQueryHeap(&queryheapdesc, IID_PPV_ARGS(&querypool_timestamp));
    assert(SUCCEEDED(hr));

    for (ezUInt32 i = 0; i < occlusion_query_count; ++i)
    {
      allocationhandler->free_occlusionqueries.push_back(i);
    }
    queryheapdesc.Count = occlusion_query_count;
    queryheapdesc.Type = D3D12_QUERY_HEAP_TYPE_OCCLUSION;
    hr = device->CreateQueryHeap(&queryheapdesc, IID_PPV_ARGS(&querypool_occlusion));
    assert(SUCCEEDED(hr));


    D3D12MA::ALLOCATION_DESC allocationDesc = {};
    allocationDesc.HeapType = D3D12_HEAP_TYPE_READBACK;

    CD3DX12_RESOURCE_DESC resdesc = CD3DX12_RESOURCE_DESC::Buffer(timestamp_query_count * sizeof(ezUInt64));

    hr = allocationhandler->allocator->CreateResource(
      &allocationDesc,
      &resdesc,
      D3D12_RESOURCE_STATE_COPY_DEST,
      nullptr,
      &allocation_querypool_timestamp_readback,
      IID_PPV_ARGS(&querypool_timestamp_readback));
    assert(SUCCEEDED(hr));

    resdesc = CD3DX12_RESOURCE_DESC::Buffer(occlusion_query_count * sizeof(ezUInt64));

    hr = allocationhandler->allocator->CreateResource(
      &allocationDesc,
      &resdesc,
      D3D12_RESOURCE_STATE_COPY_DEST,
      nullptr,
      &allocation_querypool_occlusion_readback,
      IID_PPV_ARGS(&querypool_occlusion_readback));
    assert(SUCCEEDED(hr));
  }

  ezLog::Info("Created GraphicsDevice_DX12");
}
GraphicsDevice_DX12::~GraphicsDevice_DX12()
{
  WaitForGPU();

  CloseHandle(frameFenceEvent);

  allocation_querypool_timestamp_readback->Release();
  allocation_querypool_occlusion_readback->Release();
}

void GraphicsDevice_DX12::SetResolution(int width, int height)
{
  if ((width != RESOLUTIONWIDTH || height != RESOLUTIONHEIGHT) && width > 0 && height > 0)
  {
    RESOLUTIONWIDTH = width;
    RESOLUTIONHEIGHT = height;

    for (ezUInt32 fr = 0; fr < BACKBUFFER_COUNT; ++fr)
    {
      backBuffers[fr].Reset();
    }

    HRESULT hr = swapChain->ResizeBuffers(GetBackBufferCount(), width, height, _ConvertFormat(GetBackBufferFormat()), 0);
    assert(SUCCEEDED(hr));

    for (ezUInt32 i = 0; i < BACKBUFFER_COUNT; ++i)
    {
      ezUInt32 fr = (GetFrameCount() + i) % BACKBUFFER_COUNT;
      hr = swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[fr]));
      assert(SUCCEEDED(hr));
    }
  }
}

Texture GraphicsDevice_DX12::GetBackBuffer()
{
  auto internal_state = std::make_shared<Texture_DX12>();
  internal_state->allocationhandler = allocationhandler;
  internal_state->resource = backBuffers[backbuffer_index];
  internal_state->rtv = {};
  internal_state->rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

  D3D12_RESOURCE_DESC desc = internal_state->resource->GetDesc();
  device->GetCopyableFootprints(&desc, 0, 1, 0, &internal_state->footprint, nullptr, nullptr, nullptr);

  Texture result;
  result.type = GPUResource::GPU_RESOURCE_TYPE::TEXTURE;
  result.internal_state = internal_state;
  result.desc = _ConvertTextureDesc_Inv(desc);
  return result;
}

bool GraphicsDevice_DX12::CreateBuffer(const GPUBufferDesc* pDesc, const SubresourceData* pInitialData, GPUBuffer* pBuffer)
{
  auto internal_state = std::make_shared<Resource_DX12>();
  internal_state->allocationhandler = allocationhandler;
  pBuffer->internal_state = internal_state;
  pBuffer->type = GPUResource::GPU_RESOURCE_TYPE::BUFFER;

  pBuffer->desc = *pDesc;

  if (pDesc->Usage == ezRHIUsage::Dynamic && pDesc->BindFlags & BIND_CONSTANT_BUFFER)
  {
    // this special case will use frame allocator
    return true;
  }

  HRESULT hr = E_FAIL;

  ezUInt32 alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
  if (pDesc->BindFlags & BIND_CONSTANT_BUFFER)
  {
    alignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
  }
  size_t alignedSize = Align(pDesc->ByteWidth, alignment);

  D3D12_RESOURCE_DESC desc;
  desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
  desc.Format = DXGI_FORMAT_UNKNOWN;
  desc.Width = (UINT64)alignedSize;
  desc.Height = 1;
  desc.MipLevels = 1;
  desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
  desc.DepthOrArraySize = 1;
  desc.Alignment = 0;
  desc.Flags = D3D12_RESOURCE_FLAG_NONE;
  if (pDesc->BindFlags & BIND_UNORDERED_ACCESS)
  {
    desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
  }
  desc.SampleDesc.Count = 1;
  desc.SampleDesc.Quality = 0;

  D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON;

  D3D12MA::ALLOCATION_DESC allocationDesc = {};
  allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
  if (pDesc->Usage == ezRHIUsage::Staging)
  {
    if (pDesc->CPUAccessFlags & CPU_ACCESS_READ)
    {
      allocationDesc.HeapType = D3D12_HEAP_TYPE_READBACK;
      resourceState = D3D12_RESOURCE_STATE_COPY_DEST;
    }
    else
    {
      allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
      resourceState = D3D12_RESOURCE_STATE_GENERIC_READ;
    }
  }

  device->GetCopyableFootprints(&desc, 0, 1, 0, &internal_state->footprint, nullptr, nullptr, nullptr);

  hr = allocationhandler->allocator->CreateResource(
    &allocationDesc,
    &desc,
    resourceState,
    nullptr,
    &internal_state->allocation,
    IID_PPV_ARGS(&internal_state->resource));
  assert(SUCCEEDED(hr));


  // Issue data copy on request:
  if (pInitialData != nullptr)
  {
    GPUBufferDesc uploaddesc;
    uploaddesc.ByteWidth = pDesc->ByteWidth;
    uploaddesc.Usage = ezRHIUsage::Staging;
    GPUBuffer uploadbuffer;
    bool upload_success = CreateBuffer(&uploaddesc, nullptr, &uploadbuffer);
    assert(upload_success);
    ID3D12Resource* upload_resource = to_internal(&uploadbuffer)->resource.Get();

    void* pData;
    CD3DX12_RANGE readRange(0, 0);
    hr = upload_resource->Map(0, &readRange, &pData);
    assert(SUCCEEDED(hr));
    memcpy(pData, pInitialData->pSysMem, pDesc->ByteWidth);

    copyQueueMutex.lock();
    {
      auto& frame = GetFrameResources();
      if (!copyQueueUse)
      {
        copyQueueUse = true;

        HRESULT hr = frame.copyAllocator->Reset();
        assert(SUCCEEDED(hr));
        hr = static_cast<ID3D12GraphicsCommandList*>(frame.copyCommandList.Get())->Reset(frame.copyAllocator.Get(), nullptr);
        assert(SUCCEEDED(hr));
      }
      static_cast<ID3D12GraphicsCommandList*>(frame.copyCommandList.Get())->CopyBufferRegion(internal_state->resource.Get(), 0, upload_resource, 0, pDesc->ByteWidth);
    }
    copyQueueMutex.unlock();
  }


  // Create resource views if needed
  if (pDesc->BindFlags & BIND_CONSTANT_BUFFER)
  {
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc = {};
    cbv_desc.SizeInBytes = (ezUInt32)alignedSize;
    cbv_desc.BufferLocation = internal_state->resource->GetGPUVirtualAddress();

    internal_state->cbv = cbv_desc;
  }

  if (pDesc->BindFlags & BIND_SHADER_RESOURCE)
  {
    CreateSubresource(pBuffer, SRV, 0);
  }

  if (pDesc->BindFlags & BIND_UNORDERED_ACCESS)
  {
    CreateSubresource(pBuffer, UAV, 0);
  }

  return SUCCEEDED(hr);
}
bool GraphicsDevice_DX12::CreateTexture(const TextureDesc* pDesc, const SubresourceData* pInitialData, Texture* pTexture)
{
  auto internal_state = std::make_shared<Texture_DX12>();
  internal_state->allocationhandler = allocationhandler;
  pTexture->internal_state = internal_state;
  pTexture->type = GPUResource::GPU_RESOURCE_TYPE::TEXTURE;

  pTexture->desc = *pDesc;


  HRESULT hr = E_FAIL;

  D3D12MA::ALLOCATION_DESC allocationDesc = {};
  allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

  D3D12_RESOURCE_DESC desc;
  desc.Format = _ConvertFormat(pDesc->Format);
  desc.Width = pDesc->Width;
  desc.Height = pDesc->Height;
  desc.MipLevels = pDesc->MipLevels;
  desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
  desc.DepthOrArraySize = (UINT16)pDesc->ArraySize;
  desc.SampleDesc.Count = pDesc->SampleCount;
  desc.SampleDesc.Quality = 0;
  desc.Alignment = 0;
  desc.Flags = D3D12_RESOURCE_FLAG_NONE;
  if (pDesc->BindFlags & BIND_DEPTH_STENCIL)
  {
    desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    allocationDesc.Flags |= D3D12MA::ALLOCATION_FLAG_COMMITTED;
    if (!(pDesc->BindFlags & BIND_SHADER_RESOURCE))
    {
      desc.Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
    }
  }
  else if (desc.SampleDesc.Count == 1)
  {
    desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;
  }
  if (pDesc->BindFlags & BIND_RENDER_TARGET)
  {
    desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    allocationDesc.Flags |= D3D12MA::ALLOCATION_FLAG_COMMITTED;
  }
  if (pDesc->BindFlags & BIND_UNORDERED_ACCESS)
  {
    desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
  }

  switch (pTexture->desc.type)
  {
    case TextureDesc::TEXTURE_1D:
      desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
      break;
    case TextureDesc::TEXTURE_2D:
      desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
      break;
    case TextureDesc::TEXTURE_3D:
      desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
      desc.DepthOrArraySize = (UINT16)pDesc->Depth;
      break;
    default:
      assert(0);
      break;
  }

  D3D12_CLEAR_VALUE optimizedClearValue = {};
  optimizedClearValue.Color[0] = pTexture->desc.clear.color[0];
  optimizedClearValue.Color[1] = pTexture->desc.clear.color[1];
  optimizedClearValue.Color[2] = pTexture->desc.clear.color[2];
  optimizedClearValue.Color[3] = pTexture->desc.clear.color[3];
  optimizedClearValue.DepthStencil.Depth = pTexture->desc.clear.depthstencil.depth;
  optimizedClearValue.DepthStencil.Stencil = pTexture->desc.clear.depthstencil.stencil;
  optimizedClearValue.Format = desc.Format;
  if (optimizedClearValue.Format == DXGI_FORMAT_R16_TYPELESS)
  {
    optimizedClearValue.Format = DXGI_FORMAT_D16_UNORM;
  }
  else if (optimizedClearValue.Format == DXGI_FORMAT_R32_TYPELESS)
  {
    optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
  }
  else if (optimizedClearValue.Format == DXGI_FORMAT_R32G8X24_TYPELESS)
  {
    optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
  }
  bool useClearValue = pDesc->BindFlags & BIND_RENDER_TARGET || pDesc->BindFlags & BIND_DEPTH_STENCIL;

  D3D12_RESOURCE_STATES resourceState = _ConvertImageLayout(pTexture->desc.layout);

  if (pTexture->desc.Usage == ezRHIUsage::Staging)
  {
    UINT64 RequiredSize = 0;
    device->GetCopyableFootprints(&desc, 0, 1, 0, &internal_state->footprint, nullptr, nullptr, &RequiredSize);
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Width = RequiredSize;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    if (pTexture->desc.CPUAccessFlags & CPU_ACCESS_READ)
    {
      allocationDesc.HeapType = D3D12_HEAP_TYPE_READBACK;
      resourceState = D3D12_RESOURCE_STATE_COPY_DEST;
    }
    else
    {
      allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
      resourceState = D3D12_RESOURCE_STATE_GENERIC_READ;
    }
  }

  hr = allocationhandler->allocator->CreateResource(
    &allocationDesc,
    &desc,
    resourceState,
    useClearValue ? &optimizedClearValue : nullptr,
    &internal_state->allocation,
    IID_PPV_ARGS(&internal_state->resource));
  assert(SUCCEEDED(hr));

  if (pTexture->desc.MipLevels == 0)
  {
    pTexture->desc.MipLevels = (ezUInt32)log2(ezMath::Max(pTexture->desc.Width, pTexture->desc.Height)) + 1;
  }


  // Issue data copy on request:
  if (pInitialData != nullptr)
  {
    ezUInt32 dataCount = pDesc->ArraySize * ezMath::Max(1u, pDesc->MipLevels);
    ezDynamicArray<D3D12_SUBRESOURCE_DATA> data;
    data.SetCount(dataCount);
    for (ezUInt32 slice = 0; slice < dataCount; ++slice)
    {
      data[slice] = _ConvertSubresourceData(pInitialData[slice]);
    }

    UINT64 RequiredSize = 0;
    ezDynamicArray<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> layouts;
    layouts.SetCount(dataCount);

    ezDynamicArray<UINT64> rowSizesInBytes;
    rowSizesInBytes.SetCount(dataCount);

    ezDynamicArray<UINT> numRows;
    numRows.SetCount(dataCount);

    device->GetCopyableFootprints(&desc, 0, dataCount, 0, layouts.GetData(), numRows.GetData(), rowSizesInBytes.GetData(), &RequiredSize);

    GPUBufferDesc uploaddesc;
    uploaddesc.ByteWidth = (ezUInt32)RequiredSize;
    uploaddesc.Usage = ezRHIUsage::Staging;
    GPUBuffer uploadbuffer;
    bool upload_success = CreateBuffer(&uploaddesc, nullptr, &uploadbuffer);
    assert(upload_success);
    ID3D12Resource* upload_resource = to_internal(&uploadbuffer)->resource.Get();

    ezUInt8* pData;
    CD3DX12_RANGE readRange(0, 0);
    hr = upload_resource->Map(0, &readRange, (void**)&pData);
    assert(SUCCEEDED(hr));

    for (ezUInt32 i = 0; i < dataCount; ++i)
    {
      if (rowSizesInBytes[i] > (SIZE_T)-1)
        return 0;
      D3D12_MEMCPY_DEST DestData = {pData + layouts[i].Offset, layouts[i].Footprint.RowPitch, layouts[i].Footprint.RowPitch * numRows[i]};
      MemcpySubresource(&DestData, &data[i], (SIZE_T)rowSizesInBytes[i], numRows[i], layouts[i].Footprint.Depth);
    }

    copyQueueMutex.lock();
    {
      auto& frame = GetFrameResources();
      if (!copyQueueUse)
      {
        copyQueueUse = true;

        HRESULT hr = frame.copyAllocator->Reset();
        assert(SUCCEEDED(hr));
        hr = static_cast<ID3D12GraphicsCommandList*>(frame.copyCommandList.Get())->Reset(frame.copyAllocator.Get(), nullptr);
        assert(SUCCEEDED(hr));
      }

      for (UINT i = 0; i < dataCount; ++i)
      {
        CD3DX12_TEXTURE_COPY_LOCATION Dst(internal_state->resource.Get(), i);
        CD3DX12_TEXTURE_COPY_LOCATION Src(upload_resource, layouts[i]);
        static_cast<ID3D12GraphicsCommandList*>(frame.copyCommandList.Get())->CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);
      }
    }
    copyQueueMutex.unlock();
  }

  if (pTexture->desc.BindFlags & BIND_RENDER_TARGET)
  {
    CreateSubresource(pTexture, RTV, 0, -1, 0, -1);
  }
  if (pTexture->desc.BindFlags & BIND_DEPTH_STENCIL)
  {
    CreateSubresource(pTexture, DSV, 0, -1, 0, -1);
  }
  if (pTexture->desc.BindFlags & BIND_SHADER_RESOURCE)
  {
    CreateSubresource(pTexture, SRV, 0, -1, 0, -1);
  }
  if (pTexture->desc.BindFlags & BIND_UNORDERED_ACCESS)
  {
    CreateSubresource(pTexture, UAV, 0, -1, 0, -1);
  }

  return SUCCEEDED(hr);
}
bool GraphicsDevice_DX12::CreateInputLayout(const InputLayoutDesc* pInputElementDescs, ezUInt32 NumElements, const Shader* shader, InputLayout* pInputLayout)
{
  pInputLayout->internal_state = allocationhandler;

  pInputLayout->desc.Clear();
  pInputLayout->desc.Reserve((size_t)NumElements);
  for (ezUInt32 i = 0; i < NumElements; ++i)
  {
    pInputLayout->desc.PushBack(pInputElementDescs[i]);
  }

  return true;
}
bool GraphicsDevice_DX12::CreateShader(ezEnum<ezRHIShaderStage> stage, const void* pShaderBytecode, size_t BytecodeLength, Shader* pShader)
{
  auto internal_state = std::make_shared<PipelineState_DX12>();
  internal_state->allocationhandler = allocationhandler;
  pShader->internal_state = internal_state;

  pShader->code.SetCount(BytecodeLength);
  std::memcpy(pShader->code.GetData(), pShaderBytecode, BytecodeLength);
  pShader->stage = stage;

  HRESULT hr = (pShader->code.IsEmpty() ? E_FAIL : S_OK);
  assert(SUCCEEDED(hr));


  if (pShader->rootSignature == nullptr)
  {
#  ifdef _WIN64          // TODO: Can't use dxcompiler.dll in 32-bit, so can't use shader reflection
#    ifndef PLATFORM_UWP // TODO: Can't use dxcompiler.dll in UWP, so can't use shader reflection
    struct ShaderBlob : public IDxcBlob
    {
      LPVOID address;
      SIZE_T size;
      LPVOID STDMETHODCALLTYPE GetBufferPointer() override { return address; }
      SIZE_T STDMETHODCALLTYPE GetBufferSize() override { return size; }
      HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) { return E_FAIL; }
      ULONG STDMETHODCALLTYPE AddRef(void) { return 0; }
      ULONG STDMETHODCALLTYPE Release(void) { return 0; }
    };
    ShaderBlob blob;
    blob.address = (LPVOID)pShaderBytecode;
    blob.size = BytecodeLength;

    ComPtr<IDxcContainerReflection> container_reflection;
    hr = DxcCreateInstance(CLSID_DxcContainerReflection, __uuidof(IDxcContainerReflection), (void**)&container_reflection);
    assert(SUCCEEDED(hr));
    hr = container_reflection->Load(&blob);
    assert(SUCCEEDED(hr));

    UINT32 shaderIdx;
    hr = container_reflection->FindFirstPartKind('LIXD', &shaderIdx); // Say 'DXIL' in Little-Endian
    assert(SUCCEEDED(hr));

    auto insert_descriptor = [&](const D3D12_SHADER_INPUT_BIND_DESC& desc) {
      if (desc.Type == D3D_SIT_SAMPLER)
      {
        D3D12_DESCRIPTOR_RANGE& descriptor = internal_state->samplers.ExpandAndGetRef();

        descriptor.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
        descriptor.BaseShaderRegister = desc.BindPoint;
        descriptor.NumDescriptors = desc.BindCount;
        descriptor.RegisterSpace = desc.Space;
        descriptor.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
      }
      else
      {
        D3D12_DESCRIPTOR_RANGE& descriptor = internal_state->resources.ExpandAndGetRef();

        switch (desc.Type)
        {
          default:
          case D3D_SIT_CBUFFER:
            descriptor.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
            break;
          case D3D_SIT_TBUFFER:
          case D3D_SIT_TEXTURE:
          case D3D_SIT_STRUCTURED:
          case D3D_SIT_BYTEADDRESS:
          case D3D_SIT_RTACCELERATIONSTRUCTURE:
            descriptor.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            break;
          case D3D_SIT_UAV_RWTYPED:
          case D3D_SIT_UAV_RWSTRUCTURED:
          case D3D_SIT_UAV_RWBYTEADDRESS:
          case D3D_SIT_UAV_APPEND_STRUCTURED:
          case D3D_SIT_UAV_CONSUME_STRUCTURED:
          case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
          case D3D_SIT_UAV_FEEDBACKTEXTURE:
            descriptor.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
            break;
        }
        descriptor.BaseShaderRegister = desc.BindPoint;
        descriptor.NumDescriptors = desc.BindCount;
        descriptor.RegisterSpace = desc.Space;
        descriptor.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
      }
    };

    if (stage == ezRHIShaderStage::ENUM_COUNT) // Library reflection
    {
      ComPtr<ID3D12LibraryReflection> reflection;
      hr = container_reflection->GetPartReflection(shaderIdx, IID_PPV_ARGS(&reflection));
      assert(SUCCEEDED(hr));

      D3D12_LIBRARY_DESC library_desc;
      hr = reflection->GetDesc(&library_desc);
      assert(SUCCEEDED(hr));

      for (UINT i = 0; i < library_desc.FunctionCount; ++i)
      {
        ID3D12FunctionReflection* function_reflection = reflection->GetFunctionByIndex((INT)i);
        assert(function_reflection != nullptr);
        D3D12_FUNCTION_DESC function_desc;
        hr = function_reflection->GetDesc(&function_desc);
        assert(SUCCEEDED(hr));

        for (UINT i = 0; i < function_desc.BoundResources; ++i)
        {
          D3D12_SHADER_INPUT_BIND_DESC desc;
          hr = function_reflection->GetResourceBindingDesc(i, &desc);
          assert(SUCCEEDED(hr));
          insert_descriptor(desc);
        }
      }
    }
    else // Shader reflection
    {
      ComPtr<ID3D12ShaderReflection> reflection;
      hr = container_reflection->GetPartReflection(shaderIdx, IID_PPV_ARGS(&reflection));
      assert(SUCCEEDED(hr));

      D3D12_SHADER_DESC shader_desc;
      hr = reflection->GetDesc(&shader_desc);
      assert(SUCCEEDED(hr));

      for (UINT i = 0; i < shader_desc.BoundResources; ++i)
      {
        D3D12_SHADER_INPUT_BIND_DESC desc;
        hr = reflection->GetResourceBindingDesc(i, &desc);
        assert(SUCCEEDED(hr));
        insert_descriptor(desc);
      }
    }
#    endif // PLATFORM_UWP
#  endif   // _X64


    if (stage == ezRHIShaderStage::ComputeShader || stage == ezRHIShaderStage::ENUM_COUNT)
    {
      ezDynamicArray<D3D12_ROOT_PARAMETER> params;

      if (!internal_state->resources.IsEmpty())
      {
        D3D12_ROOT_PARAMETER& param = params.ExpandAndGetRef();
        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        param.DescriptorTable.NumDescriptorRanges = (UINT)internal_state->resources.GetCount();
        param.DescriptorTable.pDescriptorRanges = internal_state->resources.GetData();
      }

      if (!internal_state->samplers.IsEmpty())
      {
        D3D12_ROOT_PARAMETER& param = params.ExpandAndGetRef();
        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        param.DescriptorTable.NumDescriptorRanges = internal_state->samplers.GetCount();
        param.DescriptorTable.pDescriptorRanges = internal_state->samplers.GetData();
      }

      D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
      rootSigDesc.NumStaticSamplers = 0;
      rootSigDesc.NumParameters = (UINT)params.GetCount();
      rootSigDesc.pParameters = params.GetData();
      rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

      ID3DBlob* rootSigBlob;
      ID3DBlob* rootSigError;
      hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &rootSigError);
      if (FAILED(hr))
      {
        OutputDebugStringA((char*)rootSigError->GetBufferPointer());
        assert(0);
      }
      hr = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&internal_state->rootSignature));
      assert(SUCCEEDED(hr));

      if (stage == ezRHIShaderStage::ComputeShader)
      {
        struct PSO_STREAM
        {
          CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
          CD3DX12_PIPELINE_STATE_STREAM_CS CS;
        } stream;

        stream.pRootSignature = internal_state->rootSignature.Get();
        stream.CS = {pShader->code.GetData(), pShader->code.GetCount()};

        D3D12_PIPELINE_STATE_STREAM_DESC streamDesc = {};
        streamDesc.pPipelineStateSubobjectStream = &stream;
        streamDesc.SizeInBytes = sizeof(stream);

        hr = device->CreatePipelineState(&streamDesc, IID_PPV_ARGS(&internal_state->resource));
        assert(SUCCEEDED(hr));
      }
    }
  }

  return SUCCEEDED(hr);
}
bool GraphicsDevice_DX12::CreateBlendState(const BlendStateDesc* pBlendStateDesc, BlendState* pBlendState)
{
  pBlendState->internal_state = allocationhandler;

  pBlendState->desc = *pBlendStateDesc;
  return true;
}
bool GraphicsDevice_DX12::CreateDepthStencilState(const DepthStencilStateDesc* pDepthStencilStateDesc, DepthStencilState* pDepthStencilState)
{
  pDepthStencilState->internal_state = allocationhandler;

  pDepthStencilState->desc = *pDepthStencilStateDesc;
  return true;
}
bool GraphicsDevice_DX12::CreateRasterizerState(const RasterizerStateDesc* pRasterizerStateDesc, RasterizerState* pRasterizerState)
{
  pRasterizerState->internal_state = allocationhandler;

  pRasterizerState->desc = *pRasterizerStateDesc;
  return true;
}
bool GraphicsDevice_DX12::CreateSampler(const SamplerDesc* pSamplerDesc, Sampler* pSamplerState)
{
  auto internal_state = std::make_shared<Sampler_DX12>();
  internal_state->allocationhandler = allocationhandler;
  pSamplerState->internal_state = internal_state;

  D3D12_SAMPLER_DESC desc;
  desc.Filter = _ConvertFilter(pSamplerDesc->Filter);
  desc.AddressU = _ConvertTextureAddressMode(pSamplerDesc->AddressU);
  desc.AddressV = _ConvertTextureAddressMode(pSamplerDesc->AddressV);
  desc.AddressW = _ConvertTextureAddressMode(pSamplerDesc->AddressW);
  desc.MipLODBias = pSamplerDesc->MipLODBias;
  desc.MaxAnisotropy = pSamplerDesc->MaxAnisotropy;
  desc.ComparisonFunc = _ConvertComparisonFunc(pSamplerDesc->ComparisonFunc);
  desc.BorderColor[0] = pSamplerDesc->BorderColor[0];
  desc.BorderColor[1] = pSamplerDesc->BorderColor[1];
  desc.BorderColor[2] = pSamplerDesc->BorderColor[2];
  desc.BorderColor[3] = pSamplerDesc->BorderColor[3];
  desc.MinLOD = pSamplerDesc->MinLOD;
  desc.MaxLOD = pSamplerDesc->MaxLOD;

  pSamplerState->desc = *pSamplerDesc;

  internal_state->descriptor = desc;

  return true;
}
bool GraphicsDevice_DX12::CreateQuery(const GPUQueryDesc* pDesc, GPUQuery* pQuery)
{
  auto internal_state = std::make_shared<Query_DX12>();
  internal_state->allocationhandler = allocationhandler;
  pQuery->internal_state = internal_state;

  HRESULT hr = E_FAIL;

  pQuery->desc = *pDesc;
  internal_state->query_type = pQuery->desc.Type;

  switch (pDesc->Type)
  {
    case GPU_QUERY_TYPE_TIMESTAMP:
      if (allocationhandler->free_timestampqueries.pop_front(internal_state->query_index))
      {
        hr = S_OK;
      }
      else
      {
        internal_state->query_type = GPU_QUERY_TYPE_INVALID;
        assert(0);
      }
      break;
    case GPU_QUERY_TYPE_TIMESTAMP_DISJOINT:
      hr = S_OK;
      break;
    case GPU_QUERY_TYPE_OCCLUSION:
    case GPU_QUERY_TYPE_OCCLUSION_PREDICATE:
      if (allocationhandler->free_occlusionqueries.pop_front(internal_state->query_index))
      {
        hr = S_OK;
      }
      else
      {
        internal_state->query_type = GPU_QUERY_TYPE_INVALID;
        assert(0);
      }
      break;
  }

  assert(SUCCEEDED(hr));

  return SUCCEEDED(hr);
}

bool GraphicsDevice_DX12::CreatePipelineState(const PipelineStateDesc* pDesc, PipelineState* pso)
{
  auto internal_state = std::make_shared<PipelineState_DX12>();
  internal_state->allocationhandler = allocationhandler;
  pso->internal_state = internal_state;

  pso->desc = *pDesc;

  pso->hash = ezHashHelper<PipelineStateDesc>::Hash(pso->desc);

  if (pDesc->rootSignature == nullptr)
  {
    // Root signature comes from reflection data when there is no root signature specified:

    auto insert_shader = [&](const Shader* shader) {
      if (shader == nullptr)
        return;

      auto shader_internal = to_internal(shader);

      if (shader_internal->resources.IsEmpty() && shader_internal->samplers.IsEmpty())
        return;

      size_t check_max = internal_state->resources.GetCount(); // dont't check for duplicates within self table
      for (auto& x : shader_internal->resources)
      {
        bool found = false;
        size_t i = 0;
        for (auto& y : internal_state->resources)
        {
          if (x.BaseShaderRegister == y.BaseShaderRegister && x.RangeType == y.RangeType)
          {
            found = true;
            break;
          }
          if (i++ >= check_max)
            break;
        }

        if (!found)
        {
          internal_state->resources.PushBack(x);
        }
      }

      check_max = internal_state->samplers.GetCount(); // dont't check for duplicates within self table
      for (auto& x : shader_internal->samplers)
      {
        bool found = false;
        size_t i = 0;
        for (auto& y : internal_state->samplers)
        {
          if (x.BaseShaderRegister == y.BaseShaderRegister && x.RangeType == y.RangeType)
          {
            found = true;
            break;
          }
          if (i++ >= check_max)
            break;
        }

        if (!found)
        {
          internal_state->samplers.PushBack(x);
        }
      }
    };

    insert_shader(pDesc->ms);
    insert_shader(pDesc->as);
    insert_shader(pDesc->vs);
    insert_shader(pDesc->hs);
    insert_shader(pDesc->ds);
    insert_shader(pDesc->gs);
    insert_shader(pDesc->ps);

    ezDynamicArray<D3D12_ROOT_PARAMETER> params;

    if (!internal_state->resources.IsEmpty())
    {
      D3D12_ROOT_PARAMETER& param = params.ExpandAndGetRef();
      param = {};
      param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
      param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
      param.DescriptorTable.NumDescriptorRanges = (UINT)internal_state->resources.GetCount();
      param.DescriptorTable.pDescriptorRanges = internal_state->resources.GetData();
    }

    if (!internal_state->samplers.IsEmpty())
    {
      D3D12_ROOT_PARAMETER& param = params.ExpandAndGetRef();
      param = {};
      param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
      param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
      param.DescriptorTable.NumDescriptorRanges = (UINT)internal_state->samplers.GetCount();
      param.DescriptorTable.pDescriptorRanges = internal_state->samplers.GetData();
    }

    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.NumStaticSamplers = 0;
    rootSigDesc.NumParameters = params.GetCount();
    rootSigDesc.pParameters = params.GetData();
    rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ID3DBlob* rootSigBlob;
    ID3DBlob* rootSigError;
    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &rootSigError);
    if (FAILED(hr))
    {
      assert(0);
      OutputDebugStringA((char*)rootSigError->GetBufferPointer());
    }
    hr = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&internal_state->rootSignature));
    assert(SUCCEEDED(hr));

    return SUCCEEDED(hr);
  }

  return true;
}
bool GraphicsDevice_DX12::CreateRenderPass(const RenderPassDesc* pDesc, RenderPass* renderpass)
{
  auto internal_state = std::make_shared<RenderPass_DX12>();
  renderpass->internal_state = internal_state;

  renderpass->desc = *pDesc;

  renderpass->hash = ezHashHelper<RenderPassDesc>::Hash(renderpass->desc);

  // Beginning barriers:
  for (auto& attachment : renderpass->desc.attachments)
  {
    if (attachment.texture == nullptr)
      continue;

    auto texture_internal = to_internal(attachment.texture);

    D3D12_RESOURCE_BARRIER& barrierdesc = internal_state->barrierdescs_begin[internal_state->num_barriers_begin++];

    barrierdesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrierdesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrierdesc.Transition.pResource = texture_internal->resource.Get();
    barrierdesc.Transition.StateBefore = _ConvertImageLayout(attachment.initial_layout);
    if (attachment.type == RenderPassAttachment::RESOLVE)
    {
      barrierdesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_DEST;
    }
    else
    {
      barrierdesc.Transition.StateAfter = _ConvertImageLayout(attachment.subpass_layout);
    }
    barrierdesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    if (barrierdesc.Transition.StateBefore == barrierdesc.Transition.StateAfter)
    {
      internal_state->num_barriers_begin--;
      continue;
    }
  }

  // Ending barriers:
  for (auto& attachment : renderpass->desc.attachments)
  {
    if (attachment.texture == nullptr)
      continue;

    auto texture_internal = to_internal(attachment.texture);

    D3D12_RESOURCE_BARRIER& barrierdesc = internal_state->barrierdescs_end[internal_state->num_barriers_end++];

    barrierdesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrierdesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrierdesc.Transition.pResource = texture_internal->resource.Get();
    if (attachment.type == RenderPassAttachment::RESOLVE)
    {
      barrierdesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RESOLVE_DEST;
    }
    else
    {
      barrierdesc.Transition.StateBefore = _ConvertImageLayout(attachment.subpass_layout);
    }
    barrierdesc.Transition.StateAfter = _ConvertImageLayout(attachment.final_layout);
    barrierdesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    if (barrierdesc.Transition.StateBefore == barrierdesc.Transition.StateAfter)
    {
      internal_state->num_barriers_end--;
      continue;
    }
  }

  return true;
}
bool GraphicsDevice_DX12::CreateRaytracingAccelerationStructure(const RaytracingAccelerationStructureDesc* pDesc, RaytracingAccelerationStructure* bvh)
{
  auto internal_state = std::make_shared<BVH_DX12>();
  internal_state->allocationhandler = allocationhandler;
  bvh->internal_state = internal_state;
  bvh->type = GPUResource::GPU_RESOURCE_TYPE::RAYTRACING_ACCELERATION_STRUCTURE;

  bvh->desc = *pDesc;

  if (pDesc->_flags & RaytracingAccelerationStructureDesc::FLAG_ALLOW_UPDATE)
  {
    internal_state->desc.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
  }
  if (pDesc->_flags & RaytracingAccelerationStructureDesc::FLAG_ALLOW_COMPACTION)
  {
    internal_state->desc.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_COMPACTION;
  }
  if (pDesc->_flags & RaytracingAccelerationStructureDesc::FLAG_PREFER_FAST_TRACE)
  {
    internal_state->desc.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
  }
  if (pDesc->_flags & RaytracingAccelerationStructureDesc::FLAG_PREFER_FAST_BUILD)
  {
    internal_state->desc.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_BUILD;
  }
  if (pDesc->_flags & RaytracingAccelerationStructureDesc::FLAG_MINIMIZE_MEMORY)
  {
    internal_state->desc.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_MINIMIZE_MEMORY;
  }


  switch (pDesc->type)
  {
    case RaytracingAccelerationStructureDesc::BOTTOMLEVEL:
    {
      internal_state->desc.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
      internal_state->desc.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;

      for (auto& x : pDesc->bottomlevel.geometries)
      {
        auto& geometry = internal_state->geometries.ExpandAndGetRef();
        geometry = {};

        if (x.type == RaytracingAccelerationStructureDesc::BottomLevel::Geometry::TRIANGLES)
        {
          geometry.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
          geometry.Triangles.VertexBuffer.StartAddress = to_internal(&x.triangles.vertexBuffer)->resource->GetGPUVirtualAddress() + (D3D12_GPU_VIRTUAL_ADDRESS)x.triangles.vertexByteOffset;
          geometry.Triangles.VertexBuffer.StrideInBytes = (UINT64)x.triangles.vertexStride;
          geometry.Triangles.VertexCount = x.triangles.vertexCount;
          geometry.Triangles.VertexFormat = _ConvertFormat(x.triangles.vertexFormat);
          geometry.Triangles.IndexFormat = (x.triangles.indexFormat == ezRHIIndexBufferFormat::UInt16 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT);
          geometry.Triangles.IndexBuffer = to_internal(&x.triangles.indexBuffer)->resource->GetGPUVirtualAddress() +
                                           (D3D12_GPU_VIRTUAL_ADDRESS)x.triangles.indexOffset * (x.triangles.indexFormat == ezRHIIndexBufferFormat::UInt16 ? sizeof(ezUInt16) : sizeof(ezUInt32));
          geometry.Triangles.IndexCount = x.triangles.indexCount;

          if (x._flags & RaytracingAccelerationStructureDesc::BottomLevel::Geometry::FLAG_USE_TRANSFORM)
          {
            geometry.Triangles.Transform3x4 = to_internal(&x.triangles.transform3x4Buffer)->resource->GetGPUVirtualAddress() +
                                              (D3D12_GPU_VIRTUAL_ADDRESS)x.triangles.transform3x4BufferOffset;
          }
        }
        else if (x.type == RaytracingAccelerationStructureDesc::BottomLevel::Geometry::PROCEDURAL_AABBS)
        {
          geometry.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_PROCEDURAL_PRIMITIVE_AABBS;
          geometry.AABBs.AABBs.StartAddress = to_internal(&x.aabbs.aabbBuffer)->resource->GetGPUVirtualAddress() +
                                              (D3D12_GPU_VIRTUAL_ADDRESS)x.aabbs.offset;
          geometry.AABBs.AABBs.StrideInBytes = (UINT64)x.aabbs.stride;
          geometry.AABBs.AABBCount = x.aabbs.count;
        }
      }

      internal_state->desc.pGeometryDescs = internal_state->geometries.GetData();
      internal_state->desc.NumDescs = (UINT)internal_state->geometries.GetCount();
    }
    break;
    case RaytracingAccelerationStructureDesc::TOPLEVEL:
    {
      internal_state->desc.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
      internal_state->desc.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;

      internal_state->desc.InstanceDescs = to_internal(&pDesc->toplevel.instanceBuffer)->resource->GetGPUVirtualAddress() +
                                           (D3D12_GPU_VIRTUAL_ADDRESS)pDesc->toplevel.offset;
      internal_state->desc.NumDescs = (UINT)pDesc->toplevel.count;
    }
    break;
  }

  device->GetRaytracingAccelerationStructurePrebuildInfo(&internal_state->desc, &internal_state->info);


  size_t alignment = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT;
  size_t alignedSize = Align((size_t)internal_state->info.ResultDataMaxSizeInBytes, alignment);

  D3D12_RESOURCE_DESC desc;
  desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
  desc.Format = DXGI_FORMAT_UNKNOWN;
  desc.Width = (UINT64)alignedSize;
  desc.Height = 1;
  desc.MipLevels = 1;
  desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
  desc.DepthOrArraySize = 1;
  desc.Alignment = 0;
  desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
  desc.SampleDesc.Count = 1;
  desc.SampleDesc.Quality = 0;

  D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;

  D3D12MA::ALLOCATION_DESC allocationDesc = {};
  allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
  allocationDesc.Flags = D3D12MA::ALLOCATION_FLAG_COMMITTED;

  HRESULT hr = allocationhandler->allocator->CreateResource(
    &allocationDesc,
    &desc,
    resourceState,
    nullptr,
    &internal_state->allocation,
    IID_PPV_ARGS(&internal_state->resource));
  assert(SUCCEEDED(hr));

  D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
  srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
  srv_desc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
  srv_desc.RaytracingAccelerationStructure.Location = internal_state->resource->GetGPUVirtualAddress();

  internal_state->srv = srv_desc;

  GPUBufferDesc scratch_desc;
  scratch_desc.ByteWidth = (ezUInt32)ezMath::Max(internal_state->info.ScratchDataSizeInBytes, internal_state->info.UpdateScratchDataSizeInBytes);

  return CreateBuffer(&scratch_desc, nullptr, &internal_state->scratch);
}
bool GraphicsDevice_DX12::CreateRaytracingPipelineState(const RaytracingPipelineStateDesc* pDesc, RaytracingPipelineState* rtpso)
{
  auto internal_state = std::make_shared<RTPipelineState_DX12>();
  internal_state->allocationhandler = allocationhandler;
  rtpso->internal_state = internal_state;

  rtpso->desc = *pDesc;

  D3D12_STATE_OBJECT_DESC desc = {};
  desc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;

  ezDynamicArray<D3D12_STATE_SUBOBJECT> subobjects;

  D3D12_RAYTRACING_PIPELINE_CONFIG pipeline_config = {};
  {
    auto& subobject = subobjects.ExpandAndGetRef();
    subobject = {};
    subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
    pipeline_config.MaxTraceRecursionDepth = pDesc->max_trace_recursion_depth;
    subobject.pDesc = &pipeline_config;
  }

  D3D12_RAYTRACING_SHADER_CONFIG shader_config = {};
  {
    auto& subobject = subobjects.ExpandAndGetRef();
    subobject = {};
    subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
    shader_config.MaxAttributeSizeInBytes = pDesc->max_attribute_size_in_bytes;
    shader_config.MaxPayloadSizeInBytes = pDesc->max_payload_size_in_bytes;
    subobject.pDesc = &shader_config;
  }

  D3D12_GLOBAL_ROOT_SIGNATURE global_rootsig = {};
  {
    auto& subobject = subobjects.ExpandAndGetRef();
    subobject = {};
    subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
    if (pDesc->rootSignature == nullptr)
    {
      //auto shader_internal = to_internal(pDesc->shaderlibraries.front().shader); // think better way
      auto shader_internal = to_internal(pDesc->shaderlibraries[0].shader);  // ezTODO: do much better
      global_rootsig.pGlobalRootSignature = shader_internal->rootSignature.Get();
    }
    else
    {
      global_rootsig.pGlobalRootSignature = to_internal(pDesc->rootSignature)->resource.Get();
    }
    subobject.pDesc = &global_rootsig;
  }

  internal_state->exports.Reserve(pDesc->shaderlibraries.GetCount());
  internal_state->library_descs.Reserve(pDesc->shaderlibraries.GetCount());
  for (auto& x : pDesc->shaderlibraries)
  {
    auto& subobject = subobjects.ExpandAndGetRef();
    subobject = {};
    subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
    auto& library_desc = internal_state->library_descs.ExpandAndGetRef();
    library_desc = {};
    library_desc.DXILLibrary.pShaderBytecode = x.shader->code.GetData();
    library_desc.DXILLibrary.BytecodeLength = x.shader->code.GetCount();
    library_desc.NumExports = 1;

    D3D12_EXPORT_DESC& export_desc = internal_state->exports.ExpandAndGetRef();
    internal_state->export_strings.PushBack(x.function_name);

    export_desc.Name = ezStringWChar(internal_state->export_strings.PeekBack()).GetData();
    library_desc.pExports = &export_desc;

    subobject.pDesc = &library_desc;
  }

  internal_state->hitgroup_descs.Reserve(pDesc->hitgroups.GetCount());
  for (auto& x : pDesc->hitgroups)
  {
    internal_state->group_strings.PushBack(x.name);

    if (x.type == ShaderHitGroup::GENERAL)
      continue;
    auto& subobject = subobjects.ExpandAndGetRef();
    subobject = {};
    subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
    auto& hitgroup_desc = internal_state->hitgroup_descs.ExpandAndGetRef();
    hitgroup_desc = {};
    switch (x.type)
    {
      default:
      case ShaderHitGroup::TRIANGLES:
        hitgroup_desc.Type = D3D12_HIT_GROUP_TYPE_TRIANGLES;
        break;
      case ShaderHitGroup::PROCEDURAL:
        hitgroup_desc.Type = D3D12_HIT_GROUP_TYPE_PROCEDURAL_PRIMITIVE;
        break;
    }
    if (!x.name.IsEmpty())
    {
      hitgroup_desc.HitGroupExport = ezStringWChar(internal_state->group_strings.PeekBack()).GetData();
    }
    if (x.closesthit_shader != ~0)
    {
      hitgroup_desc.ClosestHitShaderImport = internal_state->exports[x.closesthit_shader].Name;
    }
    if (x.anyhit_shader != ~0)
    {
      hitgroup_desc.AnyHitShaderImport = internal_state->exports[x.anyhit_shader].Name;
    }
    if (x.intersection_shader != ~0)
    {
      hitgroup_desc.IntersectionShaderImport = internal_state->exports[x.intersection_shader].Name;
    }
    subobject.pDesc = &hitgroup_desc;
  }

  desc.NumSubobjects = (UINT)subobjects.GetCount();
  desc.pSubobjects = subobjects.GetData();

  HRESULT hr = device->CreateStateObject(&desc, IID_PPV_ARGS(&internal_state->resource));
  assert(SUCCEEDED(hr));

  return SUCCEEDED(hr);
}
bool GraphicsDevice_DX12::CreateDescriptorTable(DescriptorTable* table)
{
  auto internal_state = std::make_shared<DescriptorTable_DX12>();
  internal_state->allocationhandler = allocationhandler;
  table->internal_state = internal_state;

  internal_state->resource_heap.desc.NodeMask = 0;
  internal_state->resource_heap.desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  internal_state->resource_heap.desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

  size_t prefix_sum = 0;
  for (auto& x : table->resources)
  {
    if (x.binding < CONSTANTBUFFER)
    {
      internal_state->resource_heap.write_remap.PushBack(prefix_sum);
      continue;
    }

    auto& range = internal_state->resource_heap.ranges.ExpandAndGetRef();
    range = {};
    range.BaseShaderRegister = x.slot;
    range.NumDescriptors = x.count;
    range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    range.RegisterSpace = 0; // this will be filled by root signature depending on the table position (to mirror Vulkan behaviour)
    internal_state->resource_heap.desc.NumDescriptors += range.NumDescriptors;

    switch (x.binding)
    {
      case CONSTANTBUFFER:
        range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
        break;
      case RAWBUFFER:
      case STRUCTUREDBUFFER:
      case TYPEDBUFFER:
      case TEXTURE1D:
      case TEXTURE1DARRAY:
      case TEXTURE2D:
      case TEXTURE2DARRAY:
      case TEXTURECUBE:
      case TEXTURECUBEARRAY:
      case TEXTURE3D:
      case ACCELERATIONSTRUCTURE:
        range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        break;
      case RWRAWBUFFER:
      case RWSTRUCTUREDBUFFER:
      case RWTYPEDBUFFER:
      case RWTEXTURE1D:
      case RWTEXTURE1DARRAY:
      case RWTEXTURE2D:
      case RWTEXTURE2DARRAY:
      case RWTEXTURE3D:
        range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
        break;
      default:
        assert(0);
        break;
    }

    internal_state->resource_heap.write_remap.PushBack(prefix_sum);
    prefix_sum += (size_t)range.NumDescriptors;
  }

  internal_state->sampler_heap.desc.NodeMask = 0;
  internal_state->sampler_heap.desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  internal_state->sampler_heap.desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;

  prefix_sum = 0;
  for (auto& x : table->samplers)
  {
    auto& range = internal_state->sampler_heap.ranges.ExpandAndGetRef();
    range = {};
    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
    range.BaseShaderRegister = x.slot;
    range.NumDescriptors = x.count;
    range.RegisterSpace = 0; // this will be filled by root signature depending on the table position (to mirror Vulkan behaviour)
    range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    internal_state->sampler_heap.desc.NumDescriptors += range.NumDescriptors;

    internal_state->sampler_heap.write_remap.PushBack(prefix_sum);
    prefix_sum += (size_t)range.NumDescriptors;
  }

  for (auto& x : table->staticsamplers)
  {
    auto& desc = internal_state->staticsamplers.ExpandAndGetRef();
    desc = {};
    desc.ShaderRegister = x.slot;
    desc.Filter = _ConvertFilter(x.sampler.desc.Filter);
    desc.AddressU = _ConvertTextureAddressMode(x.sampler.desc.AddressU);
    desc.AddressV = _ConvertTextureAddressMode(x.sampler.desc.AddressV);
    desc.AddressW = _ConvertTextureAddressMode(x.sampler.desc.AddressW);
    desc.MipLODBias = x.sampler.desc.MipLODBias;
    desc.MaxAnisotropy = x.sampler.desc.MaxAnisotropy;
    desc.ComparisonFunc = _ConvertComparisonFunc(x.sampler.desc.ComparisonFunc);
    desc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    desc.MinLOD = x.sampler.desc.MinLOD;
    desc.MaxLOD = x.sampler.desc.MaxLOD;
  }

  HRESULT hr = S_OK;

  if (internal_state->resource_heap.desc.NumDescriptors > 0)
  {
    hr = device->CreateDescriptorHeap(&internal_state->resource_heap.desc, IID_PPV_ARGS(&internal_state->resource_heap.heap));
    assert(SUCCEEDED(hr));
    internal_state->resource_heap.address = internal_state->resource_heap.heap->GetCPUDescriptorHandleForHeapStart();

    ezUInt32 slot = 0;
    for (auto& x : table->resources)
    {
      for (ezUInt32 i = 0; i < x.count; ++i)
      {
        WriteDescriptor(table, slot, i, (const GPUResource*)nullptr);
      }
      slot++;
    }
  }
  if (internal_state->sampler_heap.desc.NumDescriptors > 0)
  {
    hr = device->CreateDescriptorHeap(&internal_state->sampler_heap.desc, IID_PPV_ARGS(&internal_state->sampler_heap.heap));
    assert(SUCCEEDED(hr));
    internal_state->sampler_heap.address = internal_state->sampler_heap.heap->GetCPUDescriptorHandleForHeapStart();

    ezUInt32 slot = 0;
    for (auto& x : table->samplers)
    {
      for (ezUInt32 i = 0; i < x.count; ++i)
      {
        WriteDescriptor(table, slot, i, (const Sampler*)nullptr);
      }
      slot++;
    }
  }

  return SUCCEEDED(hr);
}
bool GraphicsDevice_DX12::CreateRootSignature(RootSignature* rootsig)
{
  auto internal_state = std::make_shared<RootSignature_DX12>();
  internal_state->allocationhandler = allocationhandler;
  rootsig->internal_state = internal_state;

  internal_state->params.Reserve(rootsig->tables.GetCount());

  ezDynamicArray<D3D12_STATIC_SAMPLER_DESC> staticsamplers;

  ezDynamicArray<ezDynamicArray<D3D12_DESCRIPTOR_RANGE>> table_ranges_resource;
  ezDynamicArray<ezDynamicArray<D3D12_DESCRIPTOR_RANGE>> table_ranges_sampler;

  table_ranges_resource.Reserve(rootsig->tables.GetCount());

  table_ranges_sampler.Reserve(rootsig->tables.GetCount());

  ezUInt32 space = 0;
  for (auto& x : rootsig->tables)
  {
    table_ranges_resource.ExpandAndGetRef();
    table_ranges_sampler.ExpandAndGetRef();

    ezUInt32 rangeIndex = 0;
    for (auto& binding : x.resources)
    {
      if (binding.binding < CONSTANTBUFFER)
      {
        assert(binding.count == 1); // descriptor array not allowed in the root
        internal_state->root_remap.ExpandAndGetRef();
        internal_state->root_remap.PeekBack().space = space; // Space assignment for Root Signature
        internal_state->root_remap.PeekBack().rangeIndex = rangeIndex;

        auto& param = internal_state->params.ExpandAndGetRef();
        switch (binding.binding)
        {
          case ROOT_CONSTANTBUFFER:
            param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
            break;
          case ROOT_RAWBUFFER:
          case ROOT_STRUCTUREDBUFFER:
            param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
            break;
          case ROOT_RWRAWBUFFER:
          case ROOT_RWSTRUCTUREDBUFFER:
            param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
            break;
          default:
            break;
        }
        param.ShaderVisibility = _ConvertShaderVisibility(x.stage);
        param.Descriptor.RegisterSpace = space;
        param.Descriptor.ShaderRegister = binding.slot;
      }
      else
      {
        // Space assignment for Root Signature:
        D3D12_DESCRIPTOR_RANGE& range = table_ranges_resource.PeekBack().ExpandAndGetRef();
        auto table_internal = to_internal(&x);
        range = table_internal->resource_heap.ranges[rangeIndex];
        range.RegisterSpace = space;
      }
      rangeIndex++;
    }
    for (auto& binding : x.samplers)
    {
      // Space assignment for Root Signature:
      D3D12_DESCRIPTOR_RANGE& range = table_ranges_sampler.PeekBack().ExpandAndGetRef();
      auto table_internal = to_internal(&x);
      range = table_internal->sampler_heap.ranges[rangeIndex];
      range.RegisterSpace = space;
    }
    space++;
  }

  space = 0;
  ezUInt32 bind_point = (ezUInt32)internal_state->params.GetCount();
  for (auto& x : rootsig->tables)
  {
    auto table_internal = to_internal(&x);

    if (table_internal->resource_heap.desc.NumDescriptors == 0 &&
        table_internal->sampler_heap.desc.NumDescriptors == 0)
    {
      // No real bind point
      internal_state->table_bind_point_remap.PushBack(-1);
    }
    else
    {
      internal_state->table_bind_point_remap.PushBack((int)bind_point);
    }

    if (table_internal->resource_heap.desc.NumDescriptors > 0)
    {
      auto& param = internal_state->params.ExpandAndGetRef();
      param = {};
      param.ShaderVisibility = _ConvertShaderVisibility(x.stage);
      param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
      param.DescriptorTable.pDescriptorRanges = table_ranges_resource[space].GetData();
      param.DescriptorTable.NumDescriptorRanges = (UINT)table_ranges_resource[space].GetCount();
      bind_point++;
    }
    if (table_internal->sampler_heap.desc.NumDescriptors > 0)
    {
      auto& param = internal_state->params.ExpandAndGetRef();
      param = {};
      param.ShaderVisibility = _ConvertShaderVisibility(x.stage);
      param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
      param.DescriptorTable.pDescriptorRanges = table_ranges_sampler[space].GetData();
      param.DescriptorTable.NumDescriptorRanges = (UINT)table_ranges_sampler[space].GetCount();
      bind_point++;
    }

    ezDynamicArray<D3D12_STATIC_SAMPLER_DESC> tmp_staticsamplers(table_internal->staticsamplers);
    for (auto& sam : tmp_staticsamplers)
    {
      // Space assignment for Root Signature:
      sam.RegisterSpace = space;
    }
    staticsamplers.PushBackRange(tmp_staticsamplers);

    space++;
  }

  internal_state->root_constant_bind_remap = bind_point;
  for (auto& x : rootsig->rootconstants)
  {
    auto& param = internal_state->params.ExpandAndGetRef();
    param = {};
    param.ShaderVisibility = _ConvertShaderVisibility(x.stage);
    param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    param.Constants.ShaderRegister = x.slot;
    param.Constants.RegisterSpace = 0;
    param.Constants.Num32BitValues = x.size / sizeof(ezUInt32);
  }

  D3D12_ROOT_SIGNATURE_DESC desc = {};
  if (rootsig->_flags & RootSignature::FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)
  {
    desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
  }
  desc.NumParameters = (UINT)internal_state->params.GetCount();
  desc.pParameters = internal_state->params.GetData();
  desc.NumStaticSamplers = (UINT)staticsamplers.GetCount();
  desc.pStaticSamplers = staticsamplers.GetData();

  ID3DBlob* rootSigBlob;
  ID3DBlob* rootSigError;
  HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &rootSigError);
  if (FAILED(hr))
  {
    OutputDebugStringA((char*)rootSigError->GetBufferPointer());
    assert(0);
  }
  hr = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&internal_state->resource));
  assert(SUCCEEDED(hr));

  return SUCCEEDED(hr);
}

int GraphicsDevice_DX12::CreateSubresource(Texture* texture, SUBRESOURCE_TYPE type, ezUInt32 firstSlice, ezUInt32 sliceCount, ezUInt32 firstMip, ezUInt32 mipCount)
{
  auto internal_state = to_internal(texture);

  switch (type)
  {
    case SRV:
    {
      D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
      srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

      // Try to resolve resource format:
      switch (texture->desc.Format)
      {
        case ezRHIFormat::R16_TYPELESS:
          srv_desc.Format = DXGI_FORMAT_R16_UNORM;
          break;
        case ezRHIFormat::R32_TYPELESS:
          srv_desc.Format = DXGI_FORMAT_R32_FLOAT;
          break;
        case ezRHIFormat::R24G8_TYPELESS:
          srv_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
          break;
        case ezRHIFormat::R32G8X24_TYPELESS:
          srv_desc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
          break;
        default:
          srv_desc.Format = _ConvertFormat(texture->desc.Format);
          break;
      }

      if (texture->desc.type == TextureDesc::TEXTURE_1D)
      {
        if (texture->desc.ArraySize > 1)
        {
          srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
          srv_desc.Texture1DArray.FirstArraySlice = firstSlice;
          srv_desc.Texture1DArray.ArraySize = sliceCount;
          srv_desc.Texture1DArray.MostDetailedMip = firstMip;
          srv_desc.Texture1DArray.MipLevels = mipCount;
        }
        else
        {
          srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
          srv_desc.Texture1D.MostDetailedMip = firstMip;
          srv_desc.Texture1D.MipLevels = mipCount;
        }
      }
      else if (texture->desc.type == TextureDesc::TEXTURE_2D)
      {
        if (texture->desc.ArraySize > 1)
        {
          if (texture->desc.MiscFlags & RESOURCE_MISC_TEXTURECUBE)
          {
            if (texture->desc.ArraySize > 6)
            {
              srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
              srv_desc.TextureCubeArray.First2DArrayFace = firstSlice;
              srv_desc.TextureCubeArray.NumCubes = ezMath::Min(texture->desc.ArraySize, sliceCount) / 6;
              srv_desc.TextureCubeArray.MostDetailedMip = firstMip;
              srv_desc.TextureCubeArray.MipLevels = mipCount;
            }
            else
            {
              srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
              srv_desc.TextureCube.MostDetailedMip = firstMip;
              srv_desc.TextureCube.MipLevels = mipCount;
            }
          }
          else
          {
            if (texture->desc.SampleCount > 1)
            {
              srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
              srv_desc.Texture2DMSArray.FirstArraySlice = firstSlice;
              srv_desc.Texture2DMSArray.ArraySize = sliceCount;
            }
            else
            {
              srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
              srv_desc.Texture2DArray.FirstArraySlice = firstSlice;
              srv_desc.Texture2DArray.ArraySize = sliceCount;
              srv_desc.Texture2DArray.MostDetailedMip = firstMip;
              srv_desc.Texture2DArray.MipLevels = mipCount;
            }
          }
        }
        else
        {
          if (texture->desc.SampleCount > 1)
          {
            srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
          }
          else
          {
            srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srv_desc.Texture2D.MostDetailedMip = firstMip;
            srv_desc.Texture2D.MipLevels = mipCount;
          }
        }
      }
      else if (texture->desc.type == TextureDesc::TEXTURE_3D)
      {
        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
        srv_desc.Texture3D.MostDetailedMip = firstMip;
        srv_desc.Texture3D.MipLevels = mipCount;
      }

      if (internal_state->srv.ViewDimension == D3D12_SRV_DIMENSION_UNKNOWN)
      {
        internal_state->srv = srv_desc;
        return -1;
      }
      internal_state->subresources_srv.PushBack(srv_desc);
      return int(internal_state->subresources_srv.GetCount() - 1);
    }
    break;
    case UAV:
    {
      D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};

      // Try to resolve resource format:
      switch (texture->desc.Format)
      {
        case ezRHIFormat::R16_TYPELESS:
          uav_desc.Format = DXGI_FORMAT_R16_UNORM;
          break;
        case ezRHIFormat::R32_TYPELESS:
          uav_desc.Format = DXGI_FORMAT_R32_FLOAT;
          break;
        case ezRHIFormat::R24G8_TYPELESS:
          uav_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
          break;
        case ezRHIFormat::R32G8X24_TYPELESS:
          uav_desc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
          break;
        default:
          uav_desc.Format = _ConvertFormat(texture->desc.Format);
          break;
      }

      if (texture->desc.type == TextureDesc::TEXTURE_1D)
      {
        if (texture->desc.ArraySize > 1)
        {
          uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
          uav_desc.Texture1DArray.FirstArraySlice = firstSlice;
          uav_desc.Texture1DArray.ArraySize = sliceCount;
          uav_desc.Texture1DArray.MipSlice = firstMip;
        }
        else
        {
          uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
          uav_desc.Texture1D.MipSlice = firstMip;
        }
      }
      else if (texture->desc.type == TextureDesc::TEXTURE_2D)
      {
        if (texture->desc.ArraySize > 1)
        {
          uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
          uav_desc.Texture2DArray.FirstArraySlice = firstSlice;
          uav_desc.Texture2DArray.ArraySize = sliceCount;
          uav_desc.Texture2DArray.MipSlice = firstMip;
        }
        else
        {
          uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
          uav_desc.Texture2D.MipSlice = firstMip;
        }
      }
      else if (texture->desc.type == TextureDesc::TEXTURE_3D)
      {
        uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
        uav_desc.Texture3D.MipSlice = firstMip;
        uav_desc.Texture3D.FirstWSlice = 0;
        uav_desc.Texture3D.WSize = -1;
      }

      if (internal_state->uav.ViewDimension == D3D12_UAV_DIMENSION_UNKNOWN)
      {
        internal_state->uav = uav_desc;
        return -1;
      }
      internal_state->subresources_uav.PushBack(uav_desc);
      return int(internal_state->subresources_uav.GetCount() - 1);
    }
    break;
    case RTV:
    {
      D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};

      // Try to resolve resource format:
      switch (texture->desc.Format)
      {
        case ezRHIFormat::R16_TYPELESS:
          rtv_desc.Format = DXGI_FORMAT_R16_UNORM;
          break;
        case ezRHIFormat::R32_TYPELESS:
          rtv_desc.Format = DXGI_FORMAT_R32_FLOAT;
          break;
        case ezRHIFormat::R24G8_TYPELESS:
          rtv_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
          break;
        case ezRHIFormat::R32G8X24_TYPELESS:
          rtv_desc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
          break;
        default:
          rtv_desc.Format = _ConvertFormat(texture->desc.Format);
          break;
      }

      if (texture->desc.type == TextureDesc::TEXTURE_1D)
      {
        if (texture->desc.ArraySize > 1)
        {
          rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
          rtv_desc.Texture1DArray.FirstArraySlice = firstSlice;
          rtv_desc.Texture1DArray.ArraySize = sliceCount;
          rtv_desc.Texture1DArray.MipSlice = firstMip;
        }
        else
        {
          rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
          rtv_desc.Texture1D.MipSlice = firstMip;
        }
      }
      else if (texture->desc.type == TextureDesc::TEXTURE_2D)
      {
        if (texture->desc.ArraySize > 1)
        {
          if (texture->desc.SampleCount > 1)
          {
            rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
            rtv_desc.Texture2DMSArray.FirstArraySlice = firstSlice;
            rtv_desc.Texture2DMSArray.ArraySize = sliceCount;
          }
          else
          {
            rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
            rtv_desc.Texture2DArray.FirstArraySlice = firstSlice;
            rtv_desc.Texture2DArray.ArraySize = sliceCount;
            rtv_desc.Texture2DArray.MipSlice = firstMip;
          }
        }
        else
        {
          if (texture->desc.SampleCount > 1)
          {
            rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
          }
          else
          {
            rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
            rtv_desc.Texture2D.MipSlice = firstMip;
          }
        }
      }
      else if (texture->desc.type == TextureDesc::TEXTURE_3D)
      {
        rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
        rtv_desc.Texture3D.MipSlice = firstMip;
        rtv_desc.Texture3D.FirstWSlice = 0;
        rtv_desc.Texture3D.WSize = -1;
      }

      if (internal_state->rtv.ViewDimension == D3D12_RTV_DIMENSION_UNKNOWN)
      {
        internal_state->rtv = rtv_desc;
        return -1;
      }
      internal_state->subresources_rtv.PushBack(rtv_desc);
      return int(internal_state->subresources_rtv.GetCount() - 1);
    }
    break;
    case DSV:
    {
      D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};

      // Try to resolve resource format:
      switch (texture->desc.Format)
      {
        case ezRHIFormat::R16_TYPELESS:
          dsv_desc.Format = DXGI_FORMAT_D16_UNORM;
          break;
        case ezRHIFormat::R32_TYPELESS:
          dsv_desc.Format = DXGI_FORMAT_D32_FLOAT;
          break;
        case ezRHIFormat::R24G8_TYPELESS:
          dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
          break;
        case ezRHIFormat::R32G8X24_TYPELESS:
          dsv_desc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
          break;
        default:
          dsv_desc.Format = _ConvertFormat(texture->desc.Format);
          break;
      }

      if (texture->desc.type == TextureDesc::TEXTURE_1D)
      {
        if (texture->desc.ArraySize > 1)
        {
          dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
          dsv_desc.Texture1DArray.FirstArraySlice = firstSlice;
          dsv_desc.Texture1DArray.ArraySize = sliceCount;
          dsv_desc.Texture1DArray.MipSlice = firstMip;
        }
        else
        {
          dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
          dsv_desc.Texture1D.MipSlice = firstMip;
        }
      }
      else if (texture->desc.type == TextureDesc::TEXTURE_2D)
      {
        if (texture->desc.ArraySize > 1)
        {
          if (texture->desc.SampleCount > 1)
          {
            dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
            dsv_desc.Texture2DMSArray.FirstArraySlice = firstSlice;
            dsv_desc.Texture2DMSArray.ArraySize = sliceCount;
          }
          else
          {
            dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
            dsv_desc.Texture2DArray.FirstArraySlice = firstSlice;
            dsv_desc.Texture2DArray.ArraySize = sliceCount;
            dsv_desc.Texture2DArray.MipSlice = firstMip;
          }
        }
        else
        {
          if (texture->desc.SampleCount > 1)
          {
            dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
          }
          else
          {
            dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            dsv_desc.Texture2D.MipSlice = firstMip;
          }
        }
      }

      if (internal_state->dsv.ViewDimension == D3D12_DSV_DIMENSION_UNKNOWN)
      {
        internal_state->dsv = dsv_desc;
        return -1;
      }
      internal_state->subresources_dsv.PushBack(dsv_desc);
      return int(internal_state->subresources_dsv.GetCount() - 1);
    }
    break;
    default:
      break;
  }
  return -1;
}
int GraphicsDevice_DX12::CreateSubresource(GPUBuffer* buffer, SUBRESOURCE_TYPE type, ezUInt64 offset, ezUInt64 size)
{
  auto internal_state = to_internal(buffer);
  const GPUBufferDesc& desc = buffer->GetDesc();

  switch (type)
  {
    case SRV:
    {
      D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
      srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

      if (desc.MiscFlags & RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
      {
        // This is a Raw Buffer
        srv_desc.Format = DXGI_FORMAT_R32_TYPELESS;
        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srv_desc.Buffer.FirstElement = (UINT)offset / sizeof(ezUInt32);
        srv_desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
        srv_desc.Buffer.NumElements = ezMath::Min((UINT)size, desc.ByteWidth - (UINT)offset) / sizeof(ezUInt32);
      }
      else if (desc.MiscFlags & RESOURCE_MISC_BUFFER_STRUCTURED)
      {
        // This is a Structured Buffer
        srv_desc.Format = DXGI_FORMAT_UNKNOWN;
        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srv_desc.Buffer.FirstElement = (UINT)offset / desc.StructureByteStride;
        srv_desc.Buffer.NumElements = ezMath::Min((UINT)size, desc.ByteWidth - (UINT)offset) / desc.StructureByteStride;
        srv_desc.Buffer.StructureByteStride = desc.StructureByteStride;
      }
      else
      {
        // This is a Typed Buffer
        ezUInt32 stride = GetFormatStride(desc.Format);
        srv_desc.Format = _ConvertFormat(desc.Format);
        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srv_desc.Buffer.FirstElement = offset / stride;
        srv_desc.Buffer.NumElements = ezMath::Min((UINT)size, desc.ByteWidth - (UINT)offset) / stride;
      }

      if (internal_state->srv.ViewDimension == D3D12_SRV_DIMENSION_UNKNOWN)
      {
        internal_state->srv = srv_desc;
        return -1;
      }
      internal_state->subresources_srv.PushBack(srv_desc);
      return int(internal_state->subresources_srv.GetCount() - 1);
    }
    break;
    case UAV:
    {

      D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
      uav_desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
      uav_desc.Buffer.FirstElement = 0;

      if (desc.MiscFlags & RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
      {
        // This is a Raw Buffer
        uav_desc.Format = DXGI_FORMAT_R32_TYPELESS;
        uav_desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
        uav_desc.Buffer.FirstElement = (UINT)offset / sizeof(ezUInt32);
        uav_desc.Buffer.NumElements = ezMath::Min((UINT)size, desc.ByteWidth - (UINT)offset) / sizeof(ezUInt32);
      }
      else if (desc.MiscFlags & RESOURCE_MISC_BUFFER_STRUCTURED)
      {
        // This is a Structured Buffer
        uav_desc.Format = DXGI_FORMAT_UNKNOWN;
        uav_desc.Buffer.FirstElement = (UINT)offset / desc.StructureByteStride;
        uav_desc.Buffer.NumElements = ezMath::Min((UINT)size, desc.ByteWidth - (UINT)offset) / desc.StructureByteStride;
        uav_desc.Buffer.StructureByteStride = desc.StructureByteStride;
      }
      else
      {
        // This is a Typed Buffer
        ezUInt32 stride = GetFormatStride(desc.Format);
        uav_desc.Format = _ConvertFormat(desc.Format);
        uav_desc.Buffer.FirstElement = (UINT)offset / stride;
        uav_desc.Buffer.NumElements = ezMath::Min((UINT)size, desc.ByteWidth - (UINT)offset) / stride;
      }

      if (internal_state->uav.ViewDimension == D3D12_UAV_DIMENSION_UNKNOWN)
      {
        internal_state->uav = uav_desc;
        return -1;
      }
      internal_state->subresources_uav.PushBack(uav_desc);
      return int(internal_state->subresources_uav.GetCount() - 1);
    }
    break;
    default:
      assert(0);
      break;
  }

  return -1;
}

void GraphicsDevice_DX12::WriteShadingRateValue(SHADING_RATE rate, void* dest)
{
  D3D12_SHADING_RATE _rate = _ConvertShadingRate(rate);
  if (!features_6.AdditionalShadingRatesSupported)
  {
    _rate = ezMath::Min(_rate, D3D12_SHADING_RATE_2X2);
  }
  *(ezUInt8*)dest = _rate;
}
void GraphicsDevice_DX12::WriteTopLevelAccelerationStructureInstance(const RaytracingAccelerationStructureDesc::TopLevel::Instance* instance, void* dest)
{
  D3D12_RAYTRACING_INSTANCE_DESC* desc = (D3D12_RAYTRACING_INSTANCE_DESC*)dest;
  desc->AccelerationStructure = to_internal(&instance->bottomlevel)->resource->GetGPUVirtualAddress();
  memcpy(desc->Transform, &instance->transform, sizeof(desc->Transform));
  desc->InstanceID = instance->InstanceID;
  desc->InstanceMask = instance->InstanceMask;
  desc->InstanceContributionToHitGroupIndex = instance->InstanceContributionToHitGroupIndex;
  desc->Flags = instance->Flags;
}
void GraphicsDevice_DX12::WriteShaderIdentifier(const RaytracingPipelineState* rtpso, ezUInt32 group_index, void* dest)
{
  auto internal_state = to_internal(rtpso);

  ComPtr<ID3D12StateObjectProperties> stateObjectProperties;
  HRESULT hr = internal_state->resource.As(&stateObjectProperties);
  assert(SUCCEEDED(hr));

  void* identifier = stateObjectProperties->GetShaderIdentifier(ezStringWChar(internal_state->group_strings[group_index]).GetData());
  memcpy(dest, identifier, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
}
void GraphicsDevice_DX12::WriteDescriptor(const DescriptorTable* table, ezUInt32 rangeIndex, ezUInt32 arrayIndex, const GPUResource* resource, int subresource, ezUInt64 offset)
{
  auto table_internal = to_internal(table);
  D3D12_CPU_DESCRIPTOR_HANDLE dst = table_internal->resource_heap.address;
  size_t remap = table_internal->resource_heap.write_remap[rangeIndex];
  dst.ptr += (remap + arrayIndex) * (size_t)resource_descriptor_size;

  RESOURCEBINDING binding = table->resources[rangeIndex].binding;
  switch (binding)
  {
    case CONSTANTBUFFER:
      if (resource == nullptr || !resource->IsValid())
      {
        D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc = {};
        device->CreateConstantBufferView(&cbv_desc, dst);
      }
      else if (resource->IsBuffer())
      {
        const GPUBuffer* buffer = (const GPUBuffer*)resource;
        auto internal_state = to_internal(buffer);
        if (buffer->desc.BindFlags & BIND_CONSTANT_BUFFER)
        {
          D3D12_CONSTANT_BUFFER_VIEW_DESC cbv = internal_state->cbv;
          cbv.BufferLocation += offset;
          device->CreateConstantBufferView(&cbv, dst);
        }
      }
      break;
    case RAWBUFFER:
    case STRUCTUREDBUFFER:
    case TYPEDBUFFER:
    case TEXTURE1D:
    case TEXTURE1DARRAY:
    case TEXTURE2D:
    case TEXTURE2DARRAY:
    case TEXTURECUBE:
    case TEXTURECUBEARRAY:
    case TEXTURE3D:
    case ACCELERATIONSTRUCTURE:
      if (resource == nullptr || !resource->IsValid())
      {
        D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
        srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        switch (binding)
        {
          case RAWBUFFER:
          case STRUCTUREDBUFFER:
          case TYPEDBUFFER:
            srv_desc.Format = DXGI_FORMAT_R32_UINT;
            srv_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
            break;
          case TEXTURE1D:
            srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
            break;
          case TEXTURE1DARRAY:
            srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
            break;
          case TEXTURE2D:
            srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            break;
          case TEXTURE2DARRAY:
            srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
            break;
          case TEXTURECUBE:
            srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
            break;
          case TEXTURECUBEARRAY:
            srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
            break;
          case TEXTURE3D:
            srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
            break;
          case ACCELERATIONSTRUCTURE:
            srv_desc.Format = DXGI_FORMAT_UNKNOWN;
            srv_desc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
            break;
          default:
            assert(0);
            break;
        }
        device->CreateShaderResourceView(nullptr, &srv_desc, dst);
      }
      else if (resource->IsTexture())
      {
        auto internal_state = to_internal((const Texture*)resource);
        if (subresource < 0)
        {
          device->CreateShaderResourceView(internal_state->resource.Get(), &internal_state->srv, dst);
        }
        else
        {
          device->CreateShaderResourceView(internal_state->resource.Get(), &internal_state->subresources_srv[subresource], dst);
        }
      }
      else if (resource->IsBuffer())
      {
        const GPUBuffer* buffer = (const GPUBuffer*)resource;
        auto internal_state = to_internal(buffer);
        D3D12_SHADER_RESOURCE_VIEW_DESC srv = subresource < 0 ? internal_state->srv : internal_state->subresources_srv[subresource];
        switch (binding)
        {
          default:
          case RAWBUFFER:
            srv.Buffer.FirstElement += offset / sizeof(ezUInt32);
            break;
          case STRUCTUREDBUFFER:
            srv.Buffer.FirstElement += offset / srv.Buffer.StructureByteStride;
            break;
          case TYPEDBUFFER:
            srv.Buffer.FirstElement += offset / GetFormatStride(buffer->desc.Format);
            break;
        }
        device->CreateShaderResourceView(internal_state->resource.Get(), &srv, dst);
      }
      else if (resource->IsAccelerationStructure())
      {
        auto internal_state = to_internal((const RaytracingAccelerationStructure*)resource);
        device->CreateShaderResourceView(nullptr, &internal_state->srv, dst);
      }
      break;
    case RWRAWBUFFER:
    case RWSTRUCTUREDBUFFER:
    case RWTYPEDBUFFER:
    case RWTEXTURE1D:
    case RWTEXTURE1DARRAY:
    case RWTEXTURE2D:
    case RWTEXTURE2DARRAY:
    case RWTEXTURE3D:
      if (resource == nullptr || !resource->IsValid())
      {
        D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
        uav_desc.Format = DXGI_FORMAT_R32_UINT;
        switch (binding)
        {
          case RWRAWBUFFER:
          case RWSTRUCTUREDBUFFER:
          case RWTYPEDBUFFER:
            uav_desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
            break;
          case RWTEXTURE1D:
            uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
            break;
          case RWTEXTURE1DARRAY:
            uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
            break;
          case RWTEXTURE2D:
            uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
            break;
          case RWTEXTURE2DARRAY:
            uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
            break;
          case RWTEXTURE3D:
            uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
            break;
          default:
            assert(0);
            break;
        }
        device->CreateUnorderedAccessView(nullptr, nullptr, &uav_desc, dst);
      }
      else if (resource->IsTexture())
      {
        auto internal_state = to_internal((const Texture*)resource);
        if (subresource < 0)
        {
          device->CreateUnorderedAccessView(internal_state->resource.Get(), nullptr, &internal_state->uav, dst);
        }
        else
        {
          device->CreateUnorderedAccessView(internal_state->resource.Get(), nullptr, &internal_state->subresources_uav[subresource], dst);
        }
      }
      else if (resource->IsBuffer())
      {
        const GPUBuffer* buffer = (const GPUBuffer*)resource;
        auto internal_state = to_internal(buffer);
        D3D12_UNORDERED_ACCESS_VIEW_DESC uav = subresource < 0 ? internal_state->uav : internal_state->subresources_uav[subresource];
        switch (binding)
        {
          default:
          case RWRAWBUFFER:
            uav.Buffer.FirstElement += offset / sizeof(ezUInt32);
            break;
          case RWSTRUCTUREDBUFFER:
            uav.Buffer.FirstElement += offset / uav.Buffer.StructureByteStride;
            break;
          case RWTYPEDBUFFER:
            uav.Buffer.FirstElement += offset / GetFormatStride(buffer->desc.Format);
            break;
        }
        device->CreateUnorderedAccessView(internal_state->resource.Get(), nullptr, &uav, dst);
      }
      break;
    default:
      break;
  }
}
void GraphicsDevice_DX12::WriteDescriptor(const DescriptorTable* table, ezUInt32 rangeIndex, ezUInt32 arrayIndex, const Sampler* sampler)
{
  auto table_internal = to_internal(table);
  D3D12_CPU_DESCRIPTOR_HANDLE dst = table_internal->sampler_heap.address;
  size_t remap = table_internal->sampler_heap.write_remap[rangeIndex];
  dst.ptr += (remap + arrayIndex) * (size_t)sampler_descriptor_size;

  if (sampler == nullptr || !sampler->IsValid())
  {
    D3D12_SAMPLER_DESC sam = {};
    sam.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
    sam.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    sam.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    sam.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    sam.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    device->CreateSampler(&sam, dst);
  }
  else
  {
    auto internal_state = to_internal(sampler);
    device->CreateSampler(&internal_state->descriptor, dst);
  }
}

void GraphicsDevice_DX12::Map(const GPUResource* resource, Mapping* mapping)
{
  auto internal_state = to_internal(resource);
  D3D12_RANGE read_range = {};
  if (mapping->_flags & Mapping::FLAG_READ)
  {
    read_range.Begin = mapping->offset;
    read_range.End = mapping->size;
  }
  HRESULT hr = internal_state->resource->Map(0, &read_range, &mapping->data);
  if (SUCCEEDED(hr))
  {
    mapping->rowpitch = internal_state->footprint.Footprint.RowPitch;
  }
  else
  {
    assert(0);
    mapping->data = nullptr;
    mapping->rowpitch = 0;
  }
}
void GraphicsDevice_DX12::Unmap(const GPUResource* resource)
{
  auto internal_state = to_internal(resource);
  internal_state->resource->Unmap(0, nullptr);
}
bool GraphicsDevice_DX12::QueryRead(const GPUQuery* query, GPUQueryResult* result)
{
  auto internal_state = to_internal(query);

  D3D12_RANGE range;
  range.Begin = (size_t)internal_state->query_index * sizeof(size_t);
  range.End = range.Begin + sizeof(ezUInt64);
  D3D12_RANGE nullrange = {};
  void* data = nullptr;

  switch (query->desc.Type)
  {
    case GPU_QUERY_TYPE_EVENT:
      assert(0); // not implemented yet
      break;
    case GPU_QUERY_TYPE_TIMESTAMP:
      querypool_timestamp_readback->Map(0, &range, &data);
      result->result_timestamp = *(ezUInt64*)((size_t)data + range.Begin);
      querypool_timestamp_readback->Unmap(0, &nullrange);
      break;
    case GPU_QUERY_TYPE_TIMESTAMP_DISJOINT:
      directQueue->GetTimestampFrequency(&result->result_timestamp_frequency);
      break;
    case GPU_QUERY_TYPE_OCCLUSION_PREDICATE:
    {
      BOOL passed = FALSE;
      querypool_occlusion_readback->Map(0, &range, &data);
      passed = *(BOOL*)((size_t)data + range.Begin);
      querypool_occlusion_readback->Unmap(0, &nullrange);
      result->result_passed_sample_count = (ezUInt64)passed;
      break;
    }
    case GPU_QUERY_TYPE_OCCLUSION:
      querypool_occlusion_readback->Map(0, &range, &data);
      result->result_passed_sample_count = *(ezUInt64*)((size_t)data + range.Begin);
      querypool_occlusion_readback->Unmap(0, &nullrange);
      break;
  }

  return true;
}

void GraphicsDevice_DX12::SetName(GPUResource* pResource, const char* name)
{
  ezStringWChar text(name);
  if (text.GetElementCount() > 0)
  {
    auto internal_state = to_internal(pResource);
    if (internal_state->resource != nullptr)
    {
      internal_state->resource->SetName(text);
    }
  }
}

void GraphicsDevice_DX12::PresentBegin(CommandList cmd)
{
  D3D12_RESOURCE_BARRIER barrier = {};
  barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
  barrier.Transition.pResource = backBuffers[backbuffer_index].Get();
  barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
  barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
  barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
  barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
  GetDirectCommandList(cmd)->ResourceBarrier(1, &barrier);

  const float clearcolor[] = {0, 0, 0, 1};
  device->CreateRenderTargetView(backBuffers[backbuffer_index].Get(), nullptr, rtv_descriptor_heap_start);

  D3D12_RENDER_PASS_RENDER_TARGET_DESC RTV = {};
  RTV.cpuDescriptor = rtv_descriptor_heap_start;
  RTV.BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
  RTV.BeginningAccess.Clear.ClearValue.Color[0] = clearcolor[0];
  RTV.BeginningAccess.Clear.ClearValue.Color[1] = clearcolor[1];
  RTV.BeginningAccess.Clear.ClearValue.Color[2] = clearcolor[2];
  RTV.BeginningAccess.Clear.ClearValue.Color[3] = clearcolor[3];
  RTV.EndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
  GetDirectCommandList(cmd)->BeginRenderPass(1, &RTV, nullptr, D3D12_RENDER_PASS_FLAG_ALLOW_UAV_WRITES);
}
void GraphicsDevice_DX12::PresentEnd(CommandList cmd)
{
  GetDirectCommandList(cmd)->EndRenderPass();

  // Indicate that the back buffer will now be used to present.
  D3D12_RESOURCE_BARRIER barrier = {};
  barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
  barrier.Transition.pResource = backBuffers[backbuffer_index].Get();
  barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
  barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
  barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
  barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
  GetDirectCommandList(cmd)->ResourceBarrier(1, &barrier);

  SubmitCommandLists();

  HRESULT hr = swapChain->Present(VSYNC, 0);
  assert(SUCCEEDED(hr));
  backbuffer_index = (backbuffer_index + 1) % BACKBUFFER_COUNT;
}

CommandList GraphicsDevice_DX12::BeginCommandList()
{
  CommandList cmd = cmd_count.fetch_add(1);
  if (GetDirectCommandList(cmd) == nullptr)
  {
    // need to create one more command list:
    assert(cmd < COMMANDLIST_COUNT);

    HRESULT hr;
    for (ezUInt32 fr = 0; fr < BACKBUFFER_COUNT; ++fr)
    {
      hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frames[fr].commandAllocators[cmd]));
      hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, frames[fr].commandAllocators[cmd].Get(), nullptr, IID_PPV_ARGS(&frames[fr].commandLists[cmd]));
      hr = static_cast<ID3D12GraphicsCommandList5*>(frames[fr].commandLists[cmd].Get())->Close();

      frames[fr].descriptors[cmd].init(this);
      frames[fr].resourceBuffer[cmd].init(this, 1024 * 1024); // 1 MB starting size

      ezStringBuilder sb("");
      sb.AppendFormat("cmd{}", cmd);
      ezStringWChar name(sb.GetData());
      
      frames[fr].commandLists[cmd].Get()->SetName(name.GetData());
    }
  }


  // Start the command list in a default state:

  HRESULT hr = GetFrameResources().commandAllocators[cmd]->Reset();
  assert(SUCCEEDED(hr));
  hr = GetDirectCommandList(cmd)->Reset(GetFrameResources().commandAllocators[cmd].Get(), nullptr);
  assert(SUCCEEDED(hr));

  GetFrameResources().descriptors[cmd].reset();
  GetFrameResources().resourceBuffer[cmd].clear();

  D3D12_VIEWPORT vp = {};
  vp.Width = (float)RESOLUTIONWIDTH;
  vp.Height = (float)RESOLUTIONHEIGHT;
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  GetDirectCommandList(cmd)->RSSetViewports(1, &vp);

  D3D12_RECT pRects[8];
  for (ezUInt32 i = 0; i < 8; ++i)
  {
    pRects[i].bottom = INT32_MAX;
    pRects[i].left = INT32_MIN;
    pRects[i].right = INT32_MAX;
    pRects[i].top = INT32_MIN;
  }
  GetDirectCommandList(cmd)->RSSetScissorRects(8, pRects);

  prev_pt[cmd] = ezRHIPrimitiveTopology::Undefined;
  prev_pipeline_hash[cmd] = 0;
  active_pso[cmd] = nullptr;
  active_cs[cmd] = nullptr;
  active_rt[cmd] = nullptr;
  active_rootsig_graphics[cmd] = nullptr;
  active_rootsig_compute[cmd] = nullptr;
  active_renderpass[cmd] = nullptr;
  prev_shadingrate[cmd] = D3D12_SHADING_RATE_1X1;
  dirty_pso[cmd] = false;

  if (VARIABLE_RATE_SHADING)
  {
    D3D12_SHADING_RATE_COMBINER combiners[] =
      {
        D3D12_SHADING_RATE_COMBINER_MAX,
        D3D12_SHADING_RATE_COMBINER_MAX,
      };
    GetDirectCommandList(cmd)->RSSetShadingRate(D3D12_SHADING_RATE_1X1, combiners);
  }

  return cmd;
}
void GraphicsDevice_DX12::SubmitCommandLists()
{
  // Sync up copy queue:
  copyQueueMutex.lock();
  if (copyQueueUse)
  {
    copyQueueUse = false;
    auto& frame = GetFrameResources();
    static_cast<ID3D12GraphicsCommandList*>(frame.copyCommandList.Get())->Close();
    ID3D12CommandList* commandlists[] = {
      frame.copyCommandList.Get()};
    frame.copyQueue->ExecuteCommandLists(1, commandlists);

    // Signal and increment the fence value.
    HRESULT hr = frame.copyQueue->Signal(copyFence.Get(), FRAMECOUNT);
    assert(SUCCEEDED(hr));
    hr = frame.copyQueue->Wait(copyFence.Get(), FRAMECOUNT);
    assert(SUCCEEDED(hr));
  }

  // Execute deferred command lists:
  {
    ID3D12CommandList* cmdLists[COMMANDLIST_COUNT];
    CommandList cmds[COMMANDLIST_COUNT];
    ezUInt32 counter = 0;

    CommandList cmd_last = cmd_count.load();
    cmd_count.store(0);
    for (CommandList cmd = 0; cmd < cmd_last; ++cmd)
    {
      // Perform query resolves (must be outside of render pass):
      for (auto& x : query_resolves[cmd])
      {
        switch (x.type)
        {
          case GPU_QUERY_TYPE_TIMESTAMP:
            GetDirectCommandList(cmd)->ResolveQueryData(querypool_timestamp.Get(), D3D12_QUERY_TYPE_TIMESTAMP, x.index, 1, querypool_timestamp_readback.Get(), (ezUInt64)x.index * sizeof(ezUInt64));
            break;
          case GPU_QUERY_TYPE_OCCLUSION_PREDICATE:
            GetDirectCommandList(cmd)->ResolveQueryData(querypool_occlusion.Get(), D3D12_QUERY_TYPE_BINARY_OCCLUSION, x.index, 1, querypool_occlusion_readback.Get(), (ezUInt64)x.index * sizeof(ezUInt64));
            break;
          case GPU_QUERY_TYPE_OCCLUSION:
            GetDirectCommandList(cmd)->ResolveQueryData(querypool_occlusion.Get(), D3D12_QUERY_TYPE_OCCLUSION, x.index, 1, querypool_occlusion_readback.Get(), (ezUInt64)x.index * sizeof(ezUInt64));
            break;
        }
      }
      query_resolves[cmd].Clear();

      HRESULT hr = GetDirectCommandList(cmd)->Close();
      assert(SUCCEEDED(hr));

      cmdLists[counter] = GetDirectCommandList(cmd);
      cmds[counter] = cmd;
      counter++;

      for (auto& x : pipelines_worker[cmd])
      {
        if (pipelines_global.count(x.first) == 0)
        {
          pipelines_global[x.first] = x.second;
        }
        else
        {
          allocationhandler->destroylocker.lock();
          allocationhandler->destroyer_pipelines.push_back(std::make_pair(x.second, FRAMECOUNT));
          allocationhandler->destroylocker.unlock();
        }
      }
      pipelines_worker[cmd].Clear();
    }

    directQueue->ExecuteCommandLists(counter, cmdLists);
  }

  // This acts as a barrier, following this we will be using the next frame's resources when calling GetFrameResources()!
  FRAMECOUNT++;
  HRESULT hr = directQueue->Signal(frameFence.Get(), FRAMECOUNT);

  // Determine the last frame that we should not wait on:
  const ezUInt64 lastFrameToAllowLatency = ezMath::Max(ezUInt64(BACKBUFFER_COUNT - 1u), FRAMECOUNT) - (BACKBUFFER_COUNT - 1);

  // Wait if too many frames are being incomplete:
  if (frameFence->GetCompletedValue() < lastFrameToAllowLatency)
  {
    hr = frameFence->SetEventOnCompletion(lastFrameToAllowLatency, frameFenceEvent);
    WaitForSingleObject(frameFenceEvent, INFINITE);
  }

  allocationhandler->Update(FRAMECOUNT, BACKBUFFER_COUNT);

  copyQueueMutex.unlock();
}

void GraphicsDevice_DX12::WaitForGPU()
{
  if (frameFence->GetCompletedValue() < FRAMECOUNT)
  {
    HRESULT result = frameFence->SetEventOnCompletion(FRAMECOUNT, frameFenceEvent);
    WaitForSingleObject(frameFenceEvent, INFINITE);
  }
}
void GraphicsDevice_DX12::ClearPipelineStateCache()
{
  allocationhandler->destroylocker.lock();
  for (auto& x : pipelines_global)
  {
    allocationhandler->destroyer_pipelines.push_back(std::make_pair(x.second, FRAMECOUNT));
  }
  pipelines_global.clear();

  for (int i = 0; i < EZ_ARRAY_SIZE(pipelines_worker); ++i)
  {
    for (auto& x : pipelines_worker[i])
    {
      allocationhandler->destroyer_pipelines.push_back(std::make_pair(x.second, FRAMECOUNT));
    }
    pipelines_worker[i].Clear();
  }
  allocationhandler->destroylocker.unlock();
}

void GraphicsDevice_DX12::RenderPassBegin(const RenderPass* renderpass, CommandList cmd)
{
  active_renderpass[cmd] = renderpass;

  auto internal_state = to_internal(active_renderpass[cmd]);
  if (internal_state->num_barriers_begin > 0)
  {
    GetDirectCommandList(cmd)->ResourceBarrier(internal_state->num_barriers_begin, internal_state->barrierdescs_begin);
  }

  const RenderPassDesc& desc = renderpass->GetDesc();

  D3D12_CPU_DESCRIPTOR_HANDLE descriptors_RTV = rtv_descriptor_heap_start;
  descriptors_RTV.ptr += rtv_descriptor_size * D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT * cmd;

  D3D12_CPU_DESCRIPTOR_HANDLE descriptors_DSV = dsv_descriptor_heap_start;
  descriptors_DSV.ptr += dsv_descriptor_size * cmd;

  ezUInt32 rt_count = 0;
  D3D12_RENDER_PASS_RENDER_TARGET_DESC RTVs[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
  bool dsv = false;
  D3D12_RENDER_PASS_DEPTH_STENCIL_DESC DSV = {};
  int resolve_dst_counter = 0;
  for (auto& attachment : desc.attachments)
  {
    const Texture* texture = attachment.texture;
    int subresource = attachment.subresource;
    auto texture_internal = to_internal(texture);

    D3D12_CLEAR_VALUE clear_value;
    clear_value.Format = _ConvertFormat(texture->desc.Format);

    if (attachment.type == RenderPassAttachment::RENDERTARGET)
    {
      RTVs[rt_count].cpuDescriptor = descriptors_RTV;
      RTVs[rt_count].cpuDescriptor.ptr += rtv_descriptor_size * rt_count;

      if (subresource < 0 || texture_internal->subresources_rtv.IsEmpty())
      {
        device->CreateRenderTargetView(texture_internal->resource.Get(), &texture_internal->rtv, RTVs[rt_count].cpuDescriptor);
      }
      else
      {
        assert(texture_internal->subresources_rtv.GetCount() > size_t(subresource) && "Invalid RTV subresource!");
        device->CreateRenderTargetView(texture_internal->resource.Get(), &texture_internal->subresources_rtv[subresource], RTVs[rt_count].cpuDescriptor);
      }

      switch (attachment.loadop)
      {
        default:
        case RenderPassAttachment::LOADOP_LOAD:
          RTVs[rt_count].BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
          break;
        case RenderPassAttachment::LOADOP_CLEAR:
          RTVs[rt_count].BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
          clear_value.Color[0] = texture->desc.clear.color[0];
          clear_value.Color[1] = texture->desc.clear.color[1];
          clear_value.Color[2] = texture->desc.clear.color[2];
          clear_value.Color[3] = texture->desc.clear.color[3];
          RTVs[rt_count].BeginningAccess.Clear.ClearValue = clear_value;
          break;
        case RenderPassAttachment::LOADOP_DONTCARE:
          RTVs[rt_count].BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
          break;
      }

      switch (attachment.storeop)
      {
        default:
        case RenderPassAttachment::STOREOP_STORE:
          RTVs[rt_count].EndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
          break;
        case RenderPassAttachment::STOREOP_DONTCARE:
          RTVs[rt_count].EndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
          break;
      }

      rt_count++;
    }
    else if (attachment.type == RenderPassAttachment::DEPTH_STENCIL)
    {
      dsv = true;

      DSV.cpuDescriptor = descriptors_DSV;

      if (subresource < 0 || texture_internal->subresources_dsv.IsEmpty())
      {
        device->CreateDepthStencilView(texture_internal->resource.Get(), &texture_internal->dsv, DSV.cpuDescriptor);
      }
      else
      {
        assert(texture_internal->subresources_dsv.GetCount() > size_t(subresource) && "Invalid DSV subresource!");
        device->CreateDepthStencilView(texture_internal->resource.Get(), &texture_internal->subresources_dsv[subresource], DSV.cpuDescriptor);
      }

      switch (attachment.loadop)
      {
        default:
        case RenderPassAttachment::LOADOP_LOAD:
          DSV.DepthBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
          DSV.StencilBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
          break;
        case RenderPassAttachment::LOADOP_CLEAR:
          DSV.DepthBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
          DSV.StencilBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
          clear_value.DepthStencil.Depth = texture->desc.clear.depthstencil.depth;
          clear_value.DepthStencil.Stencil = texture->desc.clear.depthstencil.stencil;
          DSV.DepthBeginningAccess.Clear.ClearValue = clear_value;
          DSV.StencilBeginningAccess.Clear.ClearValue = clear_value;
          break;
        case RenderPassAttachment::LOADOP_DONTCARE:
          DSV.DepthBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
          DSV.StencilBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
          break;
      }

      switch (attachment.storeop)
      {
        default:
        case RenderPassAttachment::STOREOP_STORE:
          DSV.DepthEndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
          DSV.StencilEndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
          break;
        case RenderPassAttachment::STOREOP_DONTCARE:
          DSV.DepthEndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
          DSV.StencilEndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
          break;
      }
    }
    else if (attachment.type == RenderPassAttachment::RESOLVE)
    {
      if (texture != nullptr)
      {
        int resolve_src_counter = 0;
        for (auto& src : active_renderpass[cmd]->desc.attachments)
        {
          if (src.type == RenderPassAttachment::RENDERTARGET && src.texture != nullptr)
          {
            if (resolve_src_counter == resolve_dst_counter)
            {
              auto src_internal = to_internal(src.texture);

              D3D12_RENDER_PASS_RENDER_TARGET_DESC& src_RTV = RTVs[resolve_src_counter];
              src_RTV.EndingAccess.Resolve.PreserveResolveSource = src_RTV.EndingAccess.Type == D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
              src_RTV.EndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_RESOLVE;
              src_RTV.EndingAccess.Resolve.Format = clear_value.Format;
              src_RTV.EndingAccess.Resolve.ResolveMode = D3D12_RESOLVE_MODE_AVERAGE;
              src_RTV.EndingAccess.Resolve.SubresourceCount = 1;
              src_RTV.EndingAccess.Resolve.pDstResource = texture_internal->resource.Get();
              src_RTV.EndingAccess.Resolve.pSrcResource = src_internal->resource.Get();

              // Due to a API bug, this resolve_subresources array must be kept alive between BeginRenderpass() and EndRenderpass()!
              src_RTV.EndingAccess.Resolve.pSubresourceParameters = &resolve_subresources[cmd][resolve_src_counter];
              resolve_subresources[cmd][resolve_src_counter].SrcRect.left = 0;
              resolve_subresources[cmd][resolve_src_counter].SrcRect.right = (LONG)texture->desc.Width;
              resolve_subresources[cmd][resolve_src_counter].SrcRect.bottom = (LONG)texture->desc.Height;
              resolve_subresources[cmd][resolve_src_counter].SrcRect.top = 0;

              break;
            }
            resolve_src_counter++;
          }
        }
      }
      resolve_dst_counter++;
    }
  }

  D3D12_RENDER_PASS_FLAGS flags = D3D12_RENDER_PASS_FLAG_NONE;
  if (desc._flags & RenderPassDesc::FLAG_ALLOW_UAV_WRITES)
  {
    flags &= D3D12_RENDER_PASS_FLAG_ALLOW_UAV_WRITES;
  }
  GetDirectCommandList(cmd)->BeginRenderPass(rt_count, RTVs, dsv ? &DSV : nullptr, flags);
}
void GraphicsDevice_DX12::RenderPassEnd(CommandList cmd)
{
  GetDirectCommandList(cmd)->EndRenderPass();

  auto internal_state = to_internal(active_renderpass[cmd]);
  if (internal_state->num_barriers_end > 0)
  {
    GetDirectCommandList(cmd)->ResourceBarrier(internal_state->num_barriers_end, internal_state->barrierdescs_end);
  }

  active_renderpass[cmd] = nullptr;
}
void GraphicsDevice_DX12::BindScissorRects(ezUInt32 numRects, const Rect* rects, CommandList cmd)
{
  assert(rects != nullptr);
  assert(numRects <= 8);
  D3D12_RECT pRects[8];
  for (ezUInt32 i = 0; i < numRects; ++i)
  {
    pRects[i].bottom = (LONG)rects[i].bottom;
    pRects[i].left = (LONG)rects[i].left;
    pRects[i].right = (LONG)rects[i].right;
    pRects[i].top = (LONG)rects[i].top;
  }
  GetDirectCommandList(cmd)->RSSetScissorRects(numRects, pRects);
}
void GraphicsDevice_DX12::BindViewports(ezUInt32 NumViewports, const Viewport* pViewports, CommandList cmd)
{
  assert(NumViewports <= 6);
  D3D12_VIEWPORT d3dViewPorts[6];
  for (ezUInt32 i = 0; i < NumViewports; ++i)
  {
    d3dViewPorts[i].TopLeftX = pViewports[i].TopLeftX;
    d3dViewPorts[i].TopLeftY = pViewports[i].TopLeftY;
    d3dViewPorts[i].Width = pViewports[i].Width;
    d3dViewPorts[i].Height = pViewports[i].Height;
    d3dViewPorts[i].MinDepth = pViewports[i].MinDepth;
    d3dViewPorts[i].MaxDepth = pViewports[i].MaxDepth;
  }
  GetDirectCommandList(cmd)->RSSetViewports(NumViewports, d3dViewPorts);
}
void GraphicsDevice_DX12::BindResource(ezEnum<ezRHIShaderStage> stage, const GPUResource* resource, ezUInt32 slot, CommandList cmd, int subresource)
{
  assert(slot < GPU_RESOURCE_HEAP_SRV_COUNT);
  auto& descriptors = GetFrameResources().descriptors[cmd];
  if (descriptors.SRV[slot] != resource || descriptors.SRV_index[slot] != subresource)
  {
    descriptors.SRV[slot] = resource;
    descriptors.SRV_index[slot] = subresource;
    descriptors.dirty = true;
  }
}
void GraphicsDevice_DX12::BindResources(ezEnum<ezRHIShaderStage> stage, const GPUResource* const* resources, ezUInt32 slot, ezUInt32 count, CommandList cmd)
{
  if (resources != nullptr)
  {
    for (ezUInt32 i = 0; i < count; ++i)
    {
      BindResource(stage, resources[i], slot + i, cmd, -1);
    }
  }
}
void GraphicsDevice_DX12::BindUAV(ezEnum<ezRHIShaderStage> stage, const GPUResource* resource, ezUInt32 slot, CommandList cmd, int subresource)
{
  assert(slot < GPU_RESOURCE_HEAP_UAV_COUNT);
  auto& descriptors = GetFrameResources().descriptors[cmd];
  if (descriptors.UAV[slot] != resource || descriptors.UAV_index[slot] != subresource)
  {
    descriptors.UAV[slot] = resource;
    descriptors.UAV_index[slot] = subresource;
    descriptors.dirty = true;
  }
}
void GraphicsDevice_DX12::BindUAVs(ezEnum<ezRHIShaderStage> stage, const GPUResource* const* resources, ezUInt32 slot, ezUInt32 count, CommandList cmd)
{
  if (resources != nullptr)
  {
    for (ezUInt32 i = 0; i < count; ++i)
    {
      BindUAV(stage, resources[i], slot + i, cmd, -1);
    }
  }
}
void GraphicsDevice_DX12::UnbindResources(ezUInt32 slot, ezUInt32 num, CommandList cmd)
{
}
void GraphicsDevice_DX12::UnbindUAVs(ezUInt32 slot, ezUInt32 num, CommandList cmd)
{
}
void GraphicsDevice_DX12::BindSampler(ezEnum<ezRHIShaderStage> stage, const Sampler* sampler, ezUInt32 slot, CommandList cmd)
{
  assert(slot < GPU_SAMPLER_HEAP_COUNT);
  auto& descriptors = GetFrameResources().descriptors[cmd];
  if (descriptors.SAM[slot] != sampler)
  {
    descriptors.SAM[slot] = sampler;
    descriptors.dirty = true;
  }
}
void GraphicsDevice_DX12::BindConstantBuffer(ezEnum<ezRHIShaderStage> stage, const GPUBuffer* buffer, ezUInt32 slot, CommandList cmd)
{
  assert(slot < GPU_RESOURCE_HEAP_CBV_COUNT);
  auto& descriptors = GetFrameResources().descriptors[cmd];
  if (buffer->desc.Usage == ezRHIUsage::Dynamic || descriptors.CBV[slot] != buffer)
  {
    descriptors.CBV[slot] = buffer;
    descriptors.dirty = true;
  }
}
void GraphicsDevice_DX12::BindVertexBuffers(const GPUBuffer* const* vertexBuffers, ezUInt32 slot, ezUInt32 count, const ezUInt32* strides, const ezUInt32* offsets, CommandList cmd)
{
  assert(count <= 8);
  D3D12_VERTEX_BUFFER_VIEW res[8] = {0};
  for (ezUInt32 i = 0; i < count; ++i)
  {
    if (vertexBuffers[i] != nullptr)
    {
      res[i].BufferLocation = vertexBuffers[i]->IsValid() ? to_internal(vertexBuffers[i])->resource->GetGPUVirtualAddress() : 0;
      res[i].SizeInBytes = vertexBuffers[i]->desc.ByteWidth;
      if (offsets != nullptr)
      {
        res[i].BufferLocation += (D3D12_GPU_VIRTUAL_ADDRESS)offsets[i];
        res[i].SizeInBytes -= offsets[i];
      }
      res[i].StrideInBytes = strides[i];
    }
  }
  GetDirectCommandList(cmd)->IASetVertexBuffers(static_cast<ezUInt32>(slot), static_cast<ezUInt32>(count), res);
}
void GraphicsDevice_DX12::BindIndexBuffer(const GPUBuffer* indexBuffer, const ezEnum<ezRHIIndexBufferFormat> format, ezUInt32 offset, CommandList cmd)
{
  D3D12_INDEX_BUFFER_VIEW res = {};
  if (indexBuffer != nullptr)
  {
    auto internal_state = to_internal(indexBuffer);

    res.BufferLocation = internal_state->resource->GetGPUVirtualAddress() + (D3D12_GPU_VIRTUAL_ADDRESS)offset;
    res.Format = (format == ezRHIIndexBufferFormat::UInt16 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT);
    res.SizeInBytes = indexBuffer->desc.ByteWidth;
  }
  GetDirectCommandList(cmd)->IASetIndexBuffer(&res);
}
void GraphicsDevice_DX12::BindStencilRef(ezUInt32 value, CommandList cmd)
{
  GetDirectCommandList(cmd)->OMSetStencilRef(value);
}
void GraphicsDevice_DX12::BindBlendFactor(float r, float g, float b, float a, CommandList cmd)
{
  const float blendFactor[4] = {r, g, b, a};
  GetDirectCommandList(cmd)->OMSetBlendFactor(blendFactor);
}
void GraphicsDevice_DX12::BindShadingRate(SHADING_RATE rate, CommandList cmd)
{
  D3D12_SHADING_RATE _rate = D3D12_SHADING_RATE_1X1;
  WriteShadingRateValue(rate, &_rate);

  if (VARIABLE_RATE_SHADING && prev_shadingrate[cmd] != _rate)
  {
    prev_shadingrate[cmd] = _rate;
    // Combiners are set to MAX by default in BeginCommandList
    GetDirectCommandList(cmd)->RSSetShadingRate(_rate, nullptr);
  }
}
void GraphicsDevice_DX12::BindShadingRateImage(const Texture* texture, CommandList cmd)
{
  if (VARIABLE_RATE_SHADING_TIER2)
  {
    if (texture == nullptr)
    {
      GetDirectCommandList(cmd)->RSSetShadingRateImage(nullptr);
    }
    else
    {
      assert(texture->desc.Format == ezRHIFormat::R8_UINT);
      GetDirectCommandList(cmd)->RSSetShadingRateImage(to_internal(texture)->resource.Get());
    }
  }
}
void GraphicsDevice_DX12::BindPipelineState(const PipelineState* pso, CommandList cmd)
{
  size_t pipeline_hash = pso->hash;

  if (active_renderpass[cmd] != nullptr)
  {
    pipeline_hash = ezHashingUtils::xxHash32(&active_renderpass[cmd]->hash, sizeof(ezUInt32), pipeline_hash);
  }

  if (prev_pipeline_hash[cmd] == pipeline_hash)
  {
    return;
  }

  prev_pipeline_hash[cmd] = pipeline_hash;

  if (pso->desc.rootSignature == nullptr)
  {
    active_rootsig_graphics[cmd] = nullptr;
    GetDirectCommandList(cmd)->SetGraphicsRootSignature(to_internal(pso)->rootSignature.Get());
  }
  else if (active_pso[cmd] != pso && active_rootsig_graphics[cmd] != pso->desc.rootSignature)
  {
    active_rootsig_graphics[cmd] = pso->desc.rootSignature;
    GetDirectCommandList(cmd)->SetGraphicsRootSignature(to_internal(pso->desc.rootSignature)->resource.Get());
  }

  GetFrameResources().descriptors[cmd].dirty = true;
  active_pso[cmd] = pso;
  dirty_pso[cmd] = true;
}
void GraphicsDevice_DX12::BindComputeShader(const Shader* cs, CommandList cmd)
{
  assert(cs->stage == ezRHIShaderStage::ComputeShader);
  if (active_cs[cmd] != cs)
  {
    prev_pipeline_hash[cmd] = 0;
    GetFrameResources().descriptors[cmd].dirty = true;
    active_cs[cmd] = cs;

    auto internal_state = to_internal(cs);
    GetDirectCommandList(cmd)->SetPipelineState(internal_state->resource.Get());

    if (cs->rootSignature == nullptr)
    {
      active_rootsig_compute[cmd] = nullptr;
      GetDirectCommandList(cmd)->SetComputeRootSignature(internal_state->rootSignature.Get());
    }
    else if (active_rootsig_compute[cmd] != cs->rootSignature)
    {
      active_rootsig_compute[cmd] = cs->rootSignature;
      GetDirectCommandList(cmd)->SetComputeRootSignature(to_internal(cs->rootSignature)->resource.Get());
    }
  }
}
void GraphicsDevice_DX12::Draw(ezUInt32 vertexCount, ezUInt32 startVertexLocation, CommandList cmd)
{
  predraw(cmd);
  GetDirectCommandList(cmd)->DrawInstanced(vertexCount, 1, startVertexLocation, 0);
}
void GraphicsDevice_DX12::DrawIndexed(ezUInt32 indexCount, ezUInt32 startIndexLocation, ezUInt32 baseVertexLocation, CommandList cmd)
{
  predraw(cmd);
  GetDirectCommandList(cmd)->DrawIndexedInstanced(indexCount, 1, startIndexLocation, baseVertexLocation, 0);
}
void GraphicsDevice_DX12::DrawInstanced(ezUInt32 vertexCount, ezUInt32 instanceCount, ezUInt32 startVertexLocation, ezUInt32 startInstanceLocation, CommandList cmd)
{
  predraw(cmd);
  GetDirectCommandList(cmd)->DrawInstanced(vertexCount, instanceCount, startVertexLocation, startInstanceLocation);
}
void GraphicsDevice_DX12::DrawIndexedInstanced(ezUInt32 indexCount, ezUInt32 instanceCount, ezUInt32 startIndexLocation, ezUInt32 baseVertexLocation, ezUInt32 startInstanceLocation, CommandList cmd)
{
  predraw(cmd);
  GetDirectCommandList(cmd)->DrawIndexedInstanced(indexCount, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}
void GraphicsDevice_DX12::DrawInstancedIndirect(const GPUBuffer* args, ezUInt32 args_offset, CommandList cmd)
{
  predraw(cmd);
  auto internal_state = to_internal(args);
  GetDirectCommandList(cmd)->ExecuteIndirect(drawInstancedIndirectCommandSignature.Get(), 1, internal_state->resource.Get(), args_offset, nullptr, 0);
}
void GraphicsDevice_DX12::DrawIndexedInstancedIndirect(const GPUBuffer* args, ezUInt32 args_offset, CommandList cmd)
{
  predraw(cmd);
  auto internal_state = to_internal(args);
  GetDirectCommandList(cmd)->ExecuteIndirect(drawIndexedInstancedIndirectCommandSignature.Get(), 1, internal_state->resource.Get(), args_offset, nullptr, 0);
}
void GraphicsDevice_DX12::Dispatch(ezUInt32 threadGroupCountX, ezUInt32 threadGroupCountY, ezUInt32 threadGroupCountZ, CommandList cmd)
{
  predispatch(cmd);
  GetDirectCommandList(cmd)->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}
void GraphicsDevice_DX12::DispatchIndirect(const GPUBuffer* args, ezUInt32 args_offset, CommandList cmd)
{
  predispatch(cmd);
  auto internal_state = to_internal(args);
  GetDirectCommandList(cmd)->ExecuteIndirect(dispatchIndirectCommandSignature.Get(), 1, internal_state->resource.Get(), args_offset, nullptr, 0);
}
void GraphicsDevice_DX12::DispatchMesh(ezUInt32 threadGroupCountX, ezUInt32 threadGroupCountY, ezUInt32 threadGroupCountZ, CommandList cmd)
{
  predraw(cmd);
  GetDirectCommandList(cmd)->DispatchMesh(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}
void GraphicsDevice_DX12::DispatchMeshIndirect(const GPUBuffer* args, ezUInt32 args_offset, CommandList cmd)
{
  predraw(cmd);
  auto internal_state = to_internal(args);
  GetDirectCommandList(cmd)->ExecuteIndirect(dispatchMeshIndirectCommandSignature.Get(), 1, internal_state->resource.Get(), args_offset, nullptr, 0);
}
void GraphicsDevice_DX12::CopyResource(const GPUResource* pDst, const GPUResource* pSrc, CommandList cmd)
{
  auto internal_state_src = to_internal(pSrc);
  auto internal_state_dst = to_internal(pDst);
  D3D12_RESOURCE_DESC desc_src = internal_state_src->resource->GetDesc();
  D3D12_RESOURCE_DESC desc_dst = internal_state_dst->resource->GetDesc();
  if (desc_dst.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER && desc_src.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER)
  {
    CD3DX12_TEXTURE_COPY_LOCATION Src(internal_state_src->resource.Get(), 0);
    CD3DX12_TEXTURE_COPY_LOCATION Dst(internal_state_dst->resource.Get(), internal_state_src->footprint);
    GetDirectCommandList(cmd)->CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);
  }
  else if (desc_src.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER && desc_dst.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER)
  {
    CD3DX12_TEXTURE_COPY_LOCATION Src(internal_state_src->resource.Get(), internal_state_dst->footprint);
    CD3DX12_TEXTURE_COPY_LOCATION Dst(internal_state_dst->resource.Get(), 0);
    GetDirectCommandList(cmd)->CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);
  }
  else
  {
    GetDirectCommandList(cmd)->CopyResource(internal_state_dst->resource.Get(), internal_state_src->resource.Get());
  }
}
void GraphicsDevice_DX12::UpdateBuffer(const GPUBuffer* buffer, const void* data, CommandList cmd, int dataSize)
{
  assert(buffer->desc.Usage != ezRHIUsage::Immutable && "Cannot update IMMUTABLE GPUBuffer!");
  assert((int)buffer->desc.ByteWidth >= dataSize || dataSize < 0 && "Data size is too big!");

  if (dataSize == 0)
  {
    return;
  }

  dataSize = ezMath::Min((int)buffer->desc.ByteWidth, dataSize);
  dataSize = (dataSize >= 0 ? dataSize : buffer->desc.ByteWidth);

  if (buffer->desc.Usage == ezRHIUsage::Dynamic && buffer->desc.BindFlags & BIND_CONSTANT_BUFFER)
  {
    // Dynamic buffer will be used from host memory directly:
    auto internal_state = to_internal(buffer);
    GPUAllocation allocation = AllocateGPU(dataSize, cmd);
    memcpy(allocation.data, data, dataSize);
    internal_state->dynamic[cmd] = allocation;

    GetFrameResources().descriptors[cmd].dirty = true;
  }
  else
  {
    assert(active_renderpass[cmd] == nullptr);

    // Contents will be transferred to device memory:
    auto internal_state_src = std::static_pointer_cast<Resource_DX12>(GetFrameResources().resourceBuffer[cmd].buffer.internal_state);
    auto internal_state_dst = to_internal(buffer);

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = internal_state_dst->resource.Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
    if (buffer->desc.BindFlags & BIND_CONSTANT_BUFFER || buffer->desc.BindFlags & BIND_VERTEX_BUFFER)
    {
      barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
    }
    else if (buffer->desc.BindFlags & BIND_INDEX_BUFFER)
    {
      barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_INDEX_BUFFER;
    }
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    GetDirectCommandList(cmd)->ResourceBarrier(1, &barrier);

    ezUInt8* dest = GetFrameResources().resourceBuffer[cmd].allocate(dataSize, 1);
    memcpy(dest, data, dataSize);
    GetDirectCommandList(cmd)->CopyBufferRegion(
      internal_state_dst->resource.Get(), 0,
      internal_state_src->resource.Get(), GetFrameResources().resourceBuffer[cmd].calculateOffset(dest),
      dataSize);

    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
    GetDirectCommandList(cmd)->ResourceBarrier(1, &barrier);
  }
}
void GraphicsDevice_DX12::QueryBegin(const GPUQuery* query, CommandList cmd)
{
  auto internal_state = to_internal(query);

  switch (query->desc.Type)
  {
    case GPU_QUERY_TYPE_TIMESTAMP:
      GetDirectCommandList(cmd)->BeginQuery(querypool_timestamp.Get(), D3D12_QUERY_TYPE_TIMESTAMP, internal_state->query_index);
      break;
    case GPU_QUERY_TYPE_OCCLUSION_PREDICATE:
      GetDirectCommandList(cmd)->BeginQuery(querypool_occlusion.Get(), D3D12_QUERY_TYPE_BINARY_OCCLUSION, internal_state->query_index);
      break;
    case GPU_QUERY_TYPE_OCCLUSION:
      GetDirectCommandList(cmd)->BeginQuery(querypool_occlusion.Get(), D3D12_QUERY_TYPE_OCCLUSION, internal_state->query_index);
      break;
  }
}
void GraphicsDevice_DX12::QueryEnd(const GPUQuery* query, CommandList cmd)
{
  auto internal_state = to_internal(query);

  switch (query->desc.Type)
  {
    case GPU_QUERY_TYPE_TIMESTAMP:
      GetDirectCommandList(cmd)->EndQuery(querypool_timestamp.Get(), D3D12_QUERY_TYPE_TIMESTAMP, internal_state->query_index);
      break;
    case GPU_QUERY_TYPE_OCCLUSION_PREDICATE:
      GetDirectCommandList(cmd)->EndQuery(querypool_occlusion.Get(), D3D12_QUERY_TYPE_BINARY_OCCLUSION, internal_state->query_index);
      break;
    case GPU_QUERY_TYPE_OCCLUSION:
      GetDirectCommandList(cmd)->EndQuery(querypool_occlusion.Get(), D3D12_QUERY_TYPE_OCCLUSION, internal_state->query_index);
      break;
  }

  Query_Resolve& resolver = query_resolves[cmd].ExpandAndGetRef();
  resolver.type = query->desc.Type;
  resolver.index = internal_state->query_index;
}
void GraphicsDevice_DX12::Barrier(const GPUBarrier* barriers, ezUInt32 numBarriers, CommandList cmd)
{
  D3D12_RESOURCE_BARRIER barrierdescs[8];

  for (ezUInt32 i = 0; i < numBarriers; ++i)
  {
    const GPUBarrier& barrier = barriers[i];
    D3D12_RESOURCE_BARRIER& barrierdesc = barrierdescs[i];

    switch (barrier.type)
    {
      default:
      case GPUBarrier::MEMORY_BARRIER:
      {
        barrierdesc.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
        barrierdesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrierdesc.UAV.pResource = barrier.memory.resource == nullptr ? nullptr : to_internal(barrier.memory.resource)->resource.Get();
      }
      break;
      case GPUBarrier::IMAGE_BARRIER:
      {
        barrierdesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrierdesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrierdesc.Transition.pResource = to_internal(barrier.image.texture)->resource.Get();
        barrierdesc.Transition.StateBefore = _ConvertImageLayout(barrier.image.layout_before);
        barrierdesc.Transition.StateAfter = _ConvertImageLayout(barrier.image.layout_after);
        barrierdesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
      }
      break;
      case GPUBarrier::BUFFER_BARRIER:
      {
        barrierdesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrierdesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrierdesc.Transition.pResource = to_internal(barrier.buffer.buffer)->resource.Get();
        barrierdesc.Transition.StateBefore = _ConvertBufferState(barrier.buffer.state_before);
        barrierdesc.Transition.StateAfter = _ConvertBufferState(barrier.buffer.state_after);
        barrierdesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
      }
      break;
    }
  }

  GetDirectCommandList(cmd)->ResourceBarrier(numBarriers, barrierdescs);
}
void GraphicsDevice_DX12::BuildRaytracingAccelerationStructure(const RaytracingAccelerationStructure* dst, CommandList cmd, const RaytracingAccelerationStructure* src)
{
  auto dst_internal = to_internal(dst);

  D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc = {};
  desc.Inputs = dst_internal->desc;

  // Make a copy of geometries, don't overwrite internal_state (thread safety)
  ezDynamicArray<D3D12_RAYTRACING_GEOMETRY_DESC> geometries;
  geometries = dst_internal->geometries;
  desc.Inputs.pGeometryDescs = geometries.GetData();

  // The real GPU addresses get filled here:
  switch (dst->desc.type)
  {
    case RaytracingAccelerationStructureDesc::BOTTOMLEVEL:
    {
      size_t i = 0;
      for (auto& x : dst->desc.bottomlevel.geometries)
      {
        auto& geometry = geometries[i++];
        if (x._flags & RaytracingAccelerationStructureDesc::BottomLevel::Geometry::FLAG_OPAQUE)
        {
          geometry.Flags |= D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
        }
        if (x._flags & RaytracingAccelerationStructureDesc::BottomLevel::Geometry::FLAG_NO_DUPLICATE_ANYHIT_INVOCATION)
        {
          geometry.Flags |= D3D12_RAYTRACING_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT_INVOCATION;
        }

        if (x.type == RaytracingAccelerationStructureDesc::BottomLevel::Geometry::TRIANGLES)
        {
          geometry.Triangles.VertexBuffer.StartAddress = to_internal(&x.triangles.vertexBuffer)->resource->GetGPUVirtualAddress() +
                                                         (D3D12_GPU_VIRTUAL_ADDRESS)x.triangles.vertexByteOffset;
          geometry.Triangles.IndexBuffer = to_internal(&x.triangles.indexBuffer)->resource->GetGPUVirtualAddress() +
                                           (D3D12_GPU_VIRTUAL_ADDRESS)x.triangles.indexOffset * (x.triangles.indexFormat == ezRHIIndexBufferFormat::UInt16 ? sizeof(ezUInt16) : sizeof(ezUInt32));

          if (x._flags & RaytracingAccelerationStructureDesc::BottomLevel::Geometry::FLAG_USE_TRANSFORM)
          {
            geometry.Triangles.Transform3x4 = to_internal(&x.triangles.transform3x4Buffer)->resource->GetGPUVirtualAddress() +
                                              (D3D12_GPU_VIRTUAL_ADDRESS)x.triangles.transform3x4BufferOffset;
          }
        }
        else if (x.type == RaytracingAccelerationStructureDesc::BottomLevel::Geometry::PROCEDURAL_AABBS)
        {
          geometry.AABBs.AABBs.StartAddress = to_internal(&x.aabbs.aabbBuffer)->resource->GetGPUVirtualAddress() +
                                              (D3D12_GPU_VIRTUAL_ADDRESS)x.aabbs.offset;
        }
      }
    }
    break;
    case RaytracingAccelerationStructureDesc::TOPLEVEL:
    {
      desc.Inputs.InstanceDescs = to_internal(&dst->desc.toplevel.instanceBuffer)->resource->GetGPUVirtualAddress() +
                                  (D3D12_GPU_VIRTUAL_ADDRESS)dst->desc.toplevel.offset;
    }
    break;
  }

  if (src != nullptr)
  {
    desc.Inputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;

    auto src_internal = to_internal(src);
    desc.SourceAccelerationStructureData = src_internal->resource->GetGPUVirtualAddress();
  }
  desc.DestAccelerationStructureData = dst_internal->resource->GetGPUVirtualAddress();
  desc.ScratchAccelerationStructureData = to_internal(&dst_internal->scratch)->resource->GetGPUVirtualAddress();
  GetDirectCommandList(cmd)->BuildRaytracingAccelerationStructure(&desc, 0, nullptr);
}
void GraphicsDevice_DX12::BindRaytracingPipelineState(const RaytracingPipelineState* rtpso, CommandList cmd)
{
  if (rtpso != active_rt[cmd])
  {
    prev_pipeline_hash[cmd] = 0;
    active_rt[cmd] = rtpso;
    GetFrameResources().descriptors[cmd].dirty = true;

    auto internal_state = to_internal(rtpso);
    GetDirectCommandList(cmd)->SetPipelineState1(internal_state->resource.Get());

    if (rtpso->desc.rootSignature == nullptr)
    {
      // we just take the first shader (todo: better)
      //active_cs[cmd] = rtpso->desc.shaderlibraries.front().shader;
      active_cs[cmd] = rtpso->desc.shaderlibraries[0].shader; // ezTODO: do much better
      active_rootsig_compute[cmd] = nullptr;
      GetDirectCommandList(cmd)->SetComputeRootSignature(to_internal(active_cs[cmd])->rootSignature.Get());
    }
    else if (active_rootsig_compute[cmd] != rtpso->desc.rootSignature)
    {
      active_rootsig_compute[cmd] = rtpso->desc.rootSignature;
      GetDirectCommandList(cmd)->SetComputeRootSignature(to_internal(rtpso->desc.rootSignature)->resource.Get());
    }
  }
}
void GraphicsDevice_DX12::DispatchRays(const DispatchRaysDesc* desc, CommandList cmd)
{
  preraytrace(cmd);

  D3D12_DISPATCH_RAYS_DESC dispatchrays_desc = {};

  dispatchrays_desc.Width = desc->Width;
  dispatchrays_desc.Height = desc->Height;
  dispatchrays_desc.Depth = desc->Depth;

  if (desc->raygeneration.buffer != nullptr)
  {
    dispatchrays_desc.RayGenerationShaderRecord.StartAddress =
      to_internal(desc->raygeneration.buffer)->resource->GetGPUVirtualAddress() +
      (D3D12_GPU_VIRTUAL_ADDRESS)desc->raygeneration.offset;
    dispatchrays_desc.RayGenerationShaderRecord.SizeInBytes =
      desc->raygeneration.size;
  }

  if (desc->miss.buffer != nullptr)
  {
    dispatchrays_desc.MissShaderTable.StartAddress =
      to_internal(desc->miss.buffer)->resource->GetGPUVirtualAddress() +
      (D3D12_GPU_VIRTUAL_ADDRESS)desc->miss.offset;
    dispatchrays_desc.MissShaderTable.SizeInBytes =
      desc->miss.size;
    dispatchrays_desc.MissShaderTable.StrideInBytes =
      desc->miss.stride;
  }

  if (desc->hitgroup.buffer != nullptr)
  {
    dispatchrays_desc.HitGroupTable.StartAddress =
      to_internal(desc->hitgroup.buffer)->resource->GetGPUVirtualAddress() +
      (D3D12_GPU_VIRTUAL_ADDRESS)desc->hitgroup.offset;
    dispatchrays_desc.HitGroupTable.SizeInBytes =
      desc->hitgroup.size;
    dispatchrays_desc.HitGroupTable.StrideInBytes =
      desc->hitgroup.stride;
  }

  if (desc->callable.buffer != nullptr)
  {
    dispatchrays_desc.CallableShaderTable.StartAddress =
      to_internal(desc->callable.buffer)->resource->GetGPUVirtualAddress() +
      (D3D12_GPU_VIRTUAL_ADDRESS)desc->callable.offset;
    dispatchrays_desc.CallableShaderTable.SizeInBytes =
      desc->callable.size;
    dispatchrays_desc.CallableShaderTable.StrideInBytes =
      desc->callable.stride;
  }

  GetDirectCommandList(cmd)->DispatchRays(&dispatchrays_desc);
}

void GraphicsDevice_DX12::BindDescriptorTable(BINDPOINT bindpoint, ezUInt32 space, const DescriptorTable* table, CommandList cmd)
{
  const RootSignature* rootsig = nullptr;
  switch (bindpoint)
  {
    default:
    case GRAPHICS:
      rootsig = active_pso[cmd]->desc.rootSignature;
      break;
    case COMPUTE:
      rootsig = active_cs[cmd]->rootSignature;
      break;
    case ::RAYTRACING:
      rootsig = active_rt[cmd]->desc.rootSignature;
      break;
  }
  auto rootsig_internal = to_internal(rootsig);
  ezUInt32 bind_point_remap = rootsig_internal->table_bind_point_remap[space];
  auto& descriptors = GetFrameResources().descriptors[cmd];
  auto handles = descriptors.commit(table, cmd);
  if (handles.resource_handle.ptr != 0)
  {
    switch (bindpoint)
    {
      default:
      case GRAPHICS:
        GetDirectCommandList(cmd)->SetGraphicsRootDescriptorTable(bind_point_remap, handles.resource_handle);
        break;
      case COMPUTE:
      case ::RAYTRACING:
        GetDirectCommandList(cmd)->SetComputeRootDescriptorTable(bind_point_remap, handles.resource_handle);
        break;
    }
    bind_point_remap++;
  }
  if (handles.sampler_handle.ptr != 0)
  {
    switch (bindpoint)
    {
      default:
      case GRAPHICS:
        GetDirectCommandList(cmd)->SetGraphicsRootDescriptorTable(bind_point_remap, handles.sampler_handle);
        break;
      case COMPUTE:
      case ::RAYTRACING:
        GetDirectCommandList(cmd)->SetComputeRootDescriptorTable(bind_point_remap, handles.sampler_handle);
        break;
    }
  }
}
void GraphicsDevice_DX12::BindRootDescriptor(BINDPOINT bindpoint, ezUInt32 index, const GPUBuffer* buffer, ezUInt32 offset, CommandList cmd)
{
  const RootSignature* rootsig = nullptr;
  switch (bindpoint)
  {
    default:
    case GRAPHICS:
      rootsig = active_pso[cmd]->desc.rootSignature;
      break;
    case COMPUTE:
      rootsig = active_cs[cmd]->rootSignature;
      break;
    case ::RAYTRACING:
      rootsig = active_rt[cmd]->desc.rootSignature;
      break;
  }
  auto rootsig_internal = to_internal(rootsig);
  auto internal_state = to_internal(buffer);
  D3D12_GPU_VIRTUAL_ADDRESS address = internal_state->resource.Get()->GetGPUVirtualAddress() + (UINT64)offset;

  auto remap = rootsig_internal->root_remap[index];
  auto binding = rootsig->tables[remap.space].resources[remap.rangeIndex].binding;
  switch (binding)
  {
    case ROOT_CONSTANTBUFFER:
      switch (bindpoint)
      {
        default:
        case GRAPHICS:
          GetDirectCommandList(cmd)->SetGraphicsRootConstantBufferView(index, address);
          break;
        case COMPUTE:
        case ::RAYTRACING:
          GetDirectCommandList(cmd)->SetComputeRootConstantBufferView(index, address);
          break;
      }
      break;
    case ROOT_RAWBUFFER:
    case ROOT_STRUCTUREDBUFFER:
      switch (bindpoint)
      {
        default:
        case GRAPHICS:
          GetDirectCommandList(cmd)->SetGraphicsRootShaderResourceView(index, address);
          break;
        case COMPUTE:
        case ::RAYTRACING:
          GetDirectCommandList(cmd)->SetComputeRootShaderResourceView(index, address);
          break;
      }
      break;
    case ROOT_RWRAWBUFFER:
    case ROOT_RWSTRUCTUREDBUFFER:
      switch (bindpoint)
      {
        default:
        case GRAPHICS:
          GetDirectCommandList(cmd)->SetGraphicsRootUnorderedAccessView(index, address);
          break;
        case COMPUTE:
        case ::RAYTRACING:
          GetDirectCommandList(cmd)->SetComputeRootUnorderedAccessView(index, address);
          break;
      }
      break;
    default:
      break;
  }
}
void GraphicsDevice_DX12::BindRootConstants(BINDPOINT bindpoint, ezUInt32 index, const void* srcdata, CommandList cmd)
{
  const RootSignature* rootsig = nullptr;
  switch (bindpoint)
  {
    default:
    case GRAPHICS:
      rootsig = active_pso[cmd]->desc.rootSignature;
      break;
    case COMPUTE:
      rootsig = active_cs[cmd]->rootSignature;
      break;
    case ::RAYTRACING:
      rootsig = active_rt[cmd]->desc.rootSignature;
      break;
  }
  auto rootsig_internal = to_internal(rootsig);
  const RootConstantRange& range = rootsig->rootconstants[index];

  switch (bindpoint)
  {
    default:
    case GRAPHICS:
      GetDirectCommandList(cmd)->SetGraphicsRoot32BitConstants(
        rootsig_internal->root_constant_bind_remap + index,
        range.size / sizeof(ezUInt32),
        srcdata,
        range.offset / sizeof(ezUInt32));
      break;
    case COMPUTE:
    case ::RAYTRACING:
      GetDirectCommandList(cmd)->SetComputeRoot32BitConstants(
        rootsig_internal->root_constant_bind_remap + index,
        range.size / sizeof(ezUInt32),
        srcdata,
        range.offset / sizeof(ezUInt32));
      break;
  }
}

GraphicsDevice::GPUAllocation GraphicsDevice_DX12::AllocateGPU(size_t dataSize, CommandList cmd)
{
  GPUAllocation result;
  if (dataSize == 0)
  {
    return result;
  }

  FrameResources::ResourceFrameAllocator& allocator = GetFrameResources().resourceBuffer[cmd];
  ezUInt8* dest = allocator.allocate(dataSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
  assert(dest != nullptr);

  result.buffer = &allocator.buffer;
  result.offset = (ezUInt32)allocator.calculateOffset(dest);
  result.data = (void*)dest;
  return result;
}

void GraphicsDevice_DX12::EventBegin(const char* name, CommandList cmd)
{
  ezStringWChar text(name);
  if (text.GetElementCount() > 0)
  {
    PIXBeginEvent(GetDirectCommandList(cmd), 0xFF000000, text);
  }
}
void GraphicsDevice_DX12::EventEnd(CommandList cmd)
{
  PIXEndEvent(GetDirectCommandList(cmd));
}
void GraphicsDevice_DX12::SetMarker(const char* name, CommandList cmd)
{
  ezStringWChar text;
  if (text.GetElementCount() > 0)
  {
    PIXSetMarker(GetDirectCommandList(cmd), 0xFFFF0000, text);
  }
}

#endif // WICKEDENGINE_BUILD_DX12
