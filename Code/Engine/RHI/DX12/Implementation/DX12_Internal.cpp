#include <RHI/DX12/DX12_Internal.h>

ezUInt32 DX12_Internal::_ParseColorWriteMask(ezBitflags<ezRHIColorWriteMask> value)
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

D3D12_FILTER DX12_Internal::_ConvertFilter(ezEnum<ezRHIFilter> value)
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

D3D12_TEXTURE_ADDRESS_MODE DX12_Internal::_ConvertTextureAddressMode(ezEnum<ezRHITextureAddressMode> value)
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

D3D12_COMPARISON_FUNC DX12_Internal::_ConvertComparisonFunc(ezEnum<ezRHIComparisonFunc> value)
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

D3D12_FILL_MODE DX12_Internal::_ConvertFillMode(ezEnum<ezRHIFillMode> value)
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

D3D12_CULL_MODE DX12_Internal::_ConvertCullMode(ezEnum<ezRHICullMode> value)
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

D3D12_DEPTH_WRITE_MASK DX12_Internal::_ConvertDepthWriteMask(ezEnum<ezRHIDepthWriteMask> value)
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

D3D12_STENCIL_OP DX12_Internal::_ConvertStencilOp(ezEnum<ezRHIStencilOp> value)
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

D3D12_BLEND DX12_Internal::_ConvertBlend(ezEnum<ezRHIBlendFactor> value)
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

D3D12_BLEND_OP DX12_Internal::_ConvertBlendOp(ezEnum<ezRHIBlendOp> value)
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

D3D12_INPUT_CLASSIFICATION DX12_Internal::_ConvertInputClassification(ezEnum<ezRHIInputClassification> value)
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

DXGI_FORMAT DX12_Internal::_ConvertFormat(ezEnum<ezRHIFormat> value)
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

D3D12_SUBRESOURCE_DATA DX12_Internal::_ConvertSubresourceData(const SubresourceData& pInitialData)
{
  D3D12_SUBRESOURCE_DATA data;
  data.pData = pInitialData.pSysMem;
  data.RowPitch = pInitialData.SysMemPitch;
  data.SlicePitch = pInitialData.SysMemSlicePitch;

  return data;
}

constexpr D3D12_RESOURCE_STATES DX12_Internal::_ConvertImageLayout(IMAGE_LAYOUT value)
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

constexpr D3D12_RESOURCE_STATES DX12_Internal::_ConvertBufferState(BUFFER_STATE value)
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

D3D12_SHADER_VISIBILITY DX12_Internal::_ConvertShaderVisibility(ezEnum<ezRHIShaderStage> value)
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

constexpr D3D12_SHADING_RATE DX12_Internal::_ConvertShadingRate(SHADING_RATE value)
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

ezEnum<ezRHIFormat> DX12_Internal::_ConvertFormat_Inv(DXGI_FORMAT value)
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

TextureDesc DX12_Internal::_ConvertTextureDesc_Inv(const D3D12_RESOURCE_DESC& desc)
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
