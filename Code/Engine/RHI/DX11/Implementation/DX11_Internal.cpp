#include <RHI/DX11/DX11_Internal.h>

namespace DX11_Internal
{
  constexpr ezUInt32 _ParseBindFlags(ezUInt32 value)
  {
    ezUInt32 _flag = 0;

    if (value & BIND_VERTEX_BUFFER)
      _flag |= D3D11_BIND_VERTEX_BUFFER;
    if (value & BIND_INDEX_BUFFER)
      _flag |= D3D11_BIND_INDEX_BUFFER;
    if (value & BIND_CONSTANT_BUFFER)
      _flag |= D3D11_BIND_CONSTANT_BUFFER;
    if (value & BIND_SHADER_RESOURCE)
      _flag |= D3D11_BIND_SHADER_RESOURCE;
    if (value & BIND_STREAM_OUTPUT)
      _flag |= D3D11_BIND_STREAM_OUTPUT;
    if (value & BIND_RENDER_TARGET)
      _flag |= D3D11_BIND_RENDER_TARGET;
    if (value & BIND_DEPTH_STENCIL)
      _flag |= D3D11_BIND_DEPTH_STENCIL;
    if (value & BIND_UNORDERED_ACCESS)
      _flag |= D3D11_BIND_UNORDERED_ACCESS;

    return _flag;
  }
  constexpr ezUInt32 _ParseCPUAccessFlags(ezUInt32 value)
  {
    ezUInt32 _flag = 0;

    if (value & CPU_ACCESS_WRITE)
      _flag |= D3D11_CPU_ACCESS_WRITE;
    if (value & CPU_ACCESS_READ)
      _flag |= D3D11_CPU_ACCESS_READ;

    return _flag;
  }
  constexpr ezUInt32 _ParseResourceMiscFlags(ezUInt32 value)
  {
    ezUInt32 _flag = 0;

    if (value & RESOURCE_MISC_SHARED)
      _flag |= D3D11_RESOURCE_MISC_SHARED;
    if (value & RESOURCE_MISC_TEXTURECUBE)
      _flag |= D3D11_RESOURCE_MISC_TEXTURECUBE;
    if (value & RESOURCE_MISC_INDIRECT_ARGS)
      _flag |= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
    if (value & RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
      _flag |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
    if (value & RESOURCE_MISC_BUFFER_STRUCTURED)
      _flag |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    if (value & RESOURCE_MISC_TILED)
      _flag |= D3D11_RESOURCE_MISC_TILED;

    return _flag;
  }
  ezUInt32 _ParseColorWriteMask(ezBitflags<ezRHIColorWriteMask> value)
  {
    ezUInt32 _flag = 0;

    if (value == D3D11_COLOR_WRITE_ENABLE_ALL)
    {
      return D3D11_COLOR_WRITE_ENABLE_ALL;
    }
    else
    {
      if (value.IsSet(ezRHIColorWriteMask::Red))
        _flag |= D3D11_COLOR_WRITE_ENABLE_RED;
      if (value.IsSet(ezRHIColorWriteMask::Green))
        _flag |= D3D11_COLOR_WRITE_ENABLE_GREEN;
      if (value.IsSet(ezRHIColorWriteMask::Blue))
        _flag |= D3D11_COLOR_WRITE_ENABLE_BLUE;
      if (value.IsSet(ezRHIColorWriteMask::Alpha))
        _flag |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
    }

    return _flag;
  }
  D3D11_FILTER _ConvertFilter(ezEnum<ezRHIFilter> value)
  {
    switch (value)
    {
      case ezRHIFilter::FILTER_MIN_MAG_MIP_POINT:
        return D3D11_FILTER_MIN_MAG_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_MIN_MAG_POINT_MIP_LINEAR:
        return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT:
        return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_MIN_POINT_MAG_MIP_LINEAR:
        return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_MIN_LINEAR_MAG_MIP_POINT:
        return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
        return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_MIN_MAG_LINEAR_MIP_POINT:
        return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_MIN_MAG_MIP_LINEAR:
        return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_ANISOTROPIC:
        return D3D11_FILTER_ANISOTROPIC;
        break;
      case ezRHIFilter::FILTER_COMPARISON_MIN_MAG_MIP_POINT:
        return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
        return D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
        return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
        return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
        return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
        return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
        return D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_COMPARISON_MIN_MAG_MIP_LINEAR:
        return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_COMPARISON_ANISOTROPIC:
        return D3D11_FILTER_COMPARISON_ANISOTROPIC;
        break;
      case ezRHIFilter::FILTER_MINIMUM_MIN_MAG_MIP_POINT:
        return D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR:
        return D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
        return D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR:
        return D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT:
        return D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
        return D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT:
        return D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_MINIMUM_MIN_MAG_MIP_LINEAR:
        return D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_MINIMUM_ANISOTROPIC:
        return D3D11_FILTER_MINIMUM_ANISOTROPIC;
        break;
      case ezRHIFilter::FILTER_MAXIMUM_MIN_MAG_MIP_POINT:
        return D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR:
        return D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
        return D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR:
        return D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT:
        return D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
        return D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT:
        return D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT;
        break;
      case ezRHIFilter::FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR:
        return D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR;
        break;
      case ezRHIFilter::FILTER_MAXIMUM_ANISOTROPIC:
        return D3D11_FILTER_MAXIMUM_ANISOTROPIC;
        break;
      default:
        break;
    }
    return D3D11_FILTER_MIN_MAG_MIP_POINT;
  }
  D3D11_TEXTURE_ADDRESS_MODE _ConvertTextureAddressMode(ezEnum<ezRHITextureAddressMode> value)
  {
    switch (value)
    {
      case ezRHITextureAddressMode::Wrap:
        return D3D11_TEXTURE_ADDRESS_WRAP;
        break;
      case ezRHITextureAddressMode::Mirror:
        return D3D11_TEXTURE_ADDRESS_MIRROR;
        break;
      case ezRHITextureAddressMode::Clamp:
        return D3D11_TEXTURE_ADDRESS_CLAMP;
        break;
      case ezRHITextureAddressMode::Border:
        return D3D11_TEXTURE_ADDRESS_BORDER;
        break;
      default:
        break;
    }
    return D3D11_TEXTURE_ADDRESS_WRAP;
  }
  D3D11_COMPARISON_FUNC _ConvertComparisonFunc(ezEnum<ezRHIComparisonFunc> value)
  {
    switch (value)
    {
      case ezRHIComparisonFunc::Never:
        return D3D11_COMPARISON_NEVER;
        break;
      case ezRHIComparisonFunc::Less:
        return D3D11_COMPARISON_LESS;
        break;
      case ezRHIComparisonFunc::Equal:
        return D3D11_COMPARISON_EQUAL;
        break;
      case ezRHIComparisonFunc::LessEqual:
        return D3D11_COMPARISON_LESS_EQUAL;
        break;
      case ezRHIComparisonFunc::Greater:
        return D3D11_COMPARISON_GREATER;
        break;
      case ezRHIComparisonFunc::NotEqual:
        return D3D11_COMPARISON_NOT_EQUAL;
        break;
      case ezRHIComparisonFunc::GreaterEqual:
        return D3D11_COMPARISON_GREATER_EQUAL;
        break;
      case ezRHIComparisonFunc::Always:
        return D3D11_COMPARISON_ALWAYS;
        break;
      default:
        break;
    }
    return D3D11_COMPARISON_NEVER;
  }
  D3D11_FILL_MODE _ConvertFillMode(ezEnum<ezRHIFillMode> value)
  {
    switch (value)
    {
      case ezRHIFillMode::Wireframe:
        return D3D11_FILL_WIREFRAME;
        break;
      case ezRHIFillMode::Solid:
        return D3D11_FILL_SOLID;
        break;
      default:
        break;
    }
    return D3D11_FILL_WIREFRAME;
  }
  D3D11_CULL_MODE _ConvertCullMode(ezEnum<ezRHICullMode> value)
  {
    switch (value)
    {
      case ezRHICullMode::None:
        return D3D11_CULL_NONE;
        break;
      case ezRHICullMode::Front:
        return D3D11_CULL_FRONT;
        break;
      case ezRHICullMode::Back:
        return D3D11_CULL_BACK;
        break;
      default:
        break;
    }
    return D3D11_CULL_NONE;
  }
  D3D11_DEPTH_WRITE_MASK _ConvertDepthWriteMask(ezEnum<ezRHIDepthWriteMask> value)
  {
    switch (value)
    {
      case ezRHIDepthWriteMask::Zero:
        return D3D11_DEPTH_WRITE_MASK_ZERO;
        break;
      case ezRHIDepthWriteMask::All:
        return D3D11_DEPTH_WRITE_MASK_ALL;
        break;
      default:
        break;
    }
    return D3D11_DEPTH_WRITE_MASK_ZERO;
  }
  D3D11_STENCIL_OP _ConvertStencilOp(ezEnum<ezRHIStencilOp> value)
  {
    switch (value)
    {
      case ezRHIStencilOp::Keep:
        return D3D11_STENCIL_OP_KEEP;
        break;
      case ezRHIStencilOp::Zero:
        return D3D11_STENCIL_OP_ZERO;
        break;
      case ezRHIStencilOp::Replace:
        return D3D11_STENCIL_OP_REPLACE;
        break;
      case ezRHIStencilOp::IncrementSaturated:
        return D3D11_STENCIL_OP_INCR_SAT;
        break;
      case ezRHIStencilOp::DecrementSaturated:
        return D3D11_STENCIL_OP_DECR_SAT;
        break;
      case ezRHIStencilOp::Invert:
        return D3D11_STENCIL_OP_INVERT;
        break;
      case ezRHIStencilOp::Increment:
        return D3D11_STENCIL_OP_INCR;
        break;
      case ezRHIStencilOp::Decrement:
        return D3D11_STENCIL_OP_DECR;
        break;
      default:
        break;
    }
    return D3D11_STENCIL_OP_KEEP;
  }
  D3D11_BLEND _ConvertBlend(ezEnum<ezRHIBlendFactor> value)
  {
    switch (value)
    {
      case ezRHIBlendFactor::Zero:
        return D3D11_BLEND_ZERO;
        break;
      case ezRHIBlendFactor::One:
        return D3D11_BLEND_ONE;
        break;
      case ezRHIBlendFactor::SourceColor:
        return D3D11_BLEND_SRC_COLOR;
        break;
      case ezRHIBlendFactor::InverseSourceColor:
        return D3D11_BLEND_INV_SRC_COLOR;
        break;
      case ezRHIBlendFactor::SourceAlpha:
        return D3D11_BLEND_SRC_ALPHA;
        break;
      case ezRHIBlendFactor::InverseSourceAlpha:
        return D3D11_BLEND_INV_SRC_ALPHA;
        break;
      case ezRHIBlendFactor::DestinationAlpha:
        return D3D11_BLEND_DEST_ALPHA;
        break;
      case ezRHIBlendFactor::InverseDestinationAlpha:
        return D3D11_BLEND_INV_DEST_ALPHA;
        break;
      case ezRHIBlendFactor::DestinationColor:
        return D3D11_BLEND_DEST_COLOR;
        break;
      case ezRHIBlendFactor::InverseDestinationColor:
        return D3D11_BLEND_INV_DEST_COLOR;
        break;
      case ezRHIBlendFactor::SourceAlphaSaturated:
        return D3D11_BLEND_SRC_ALPHA_SAT;
        break;
      case ezRHIBlendFactor::BlendFactor:
        return D3D11_BLEND_BLEND_FACTOR;
        break;
      case ezRHIBlendFactor::InverseBlendFactor:
        return D3D11_BLEND_INV_BLEND_FACTOR;
        break;
      case ezRHIBlendFactor::Source1Color:
        return D3D11_BLEND_SRC1_COLOR;
        break;
      case ezRHIBlendFactor::InverseSource1Color:
        return D3D11_BLEND_INV_SRC1_COLOR;
        break;
      case ezRHIBlendFactor::Source1Alpha:
        return D3D11_BLEND_SRC1_ALPHA;
        break;
      case ezRHIBlendFactor::InverseSource1Alpha:
        return D3D11_BLEND_INV_SRC1_ALPHA;
        break;
      default:
        break;
    }
    return D3D11_BLEND_ZERO;
  }
  D3D11_BLEND_OP _ConvertBlendOp(ezEnum<ezRHIBlendOp> value)
  {
    switch (value)
    {
      case ezRHIBlendOp::Add:
        return D3D11_BLEND_OP_ADD;
        break;
      case ezRHIBlendOp::Subtract:
        return D3D11_BLEND_OP_SUBTRACT;
        break;
      case ezRHIBlendOp::ReverseSubtract:
        return D3D11_BLEND_OP_REV_SUBTRACT;
        break;
      case ezRHIBlendOp::Min:
        return D3D11_BLEND_OP_MIN;
        break;
      case ezRHIBlendOp::Max:
        return D3D11_BLEND_OP_MAX;
        break;
      default:
        break;
    }
    return D3D11_BLEND_OP_ADD;
  }
  D3D11_USAGE _ConvertUsage(ezBitflags<ezRHIUsage> value)
  {
    switch (value.GetValue())
    {
      case ezRHIUsage::Default:
        return D3D11_USAGE_DEFAULT;
        break;
      case ezRHIUsage::Immutable:
        return D3D11_USAGE_IMMUTABLE;
        break;
      case ezRHIUsage::Dynamic:
        return D3D11_USAGE_DYNAMIC;
        break;
      case ezRHIUsage::Staging:
        return D3D11_USAGE_STAGING;
        break;
      default:
        break;
    }
    return D3D11_USAGE_DEFAULT;
  }
  D3D11_INPUT_CLASSIFICATION _ConvertInputClassification(ezEnum<ezRHIInputClassification> value)
  {
    switch (value)
    {
      case ezRHIInputClassification::PerVertexData:
        return D3D11_INPUT_PER_VERTEX_DATA;
        break;
      case ezRHIInputClassification::PerInstanceData:
        return D3D11_INPUT_PER_INSTANCE_DATA;
        break;
      default:
        break;
    }
    return D3D11_INPUT_PER_VERTEX_DATA;
  }
  DXGI_FORMAT _ConvertFormat(ezEnum<ezRHIFormat> value)
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
      case ezRHIFormat::R24G8_TYPELESS:
        return DXGI_FORMAT_R24G8_TYPELESS;
        break;
      case ezRHIFormat::D24_UNORM_S8_UINT:
        return DXGI_FORMAT_D24_UNORM_S8_UINT;
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
  D3D11_TEXTURE1D_DESC _ConvertTextureDesc1D(const TextureDesc* pDesc)
  {
    D3D11_TEXTURE1D_DESC desc;
    desc.Width = pDesc->Width;
    desc.MipLevels = pDesc->MipLevels;
    desc.ArraySize = pDesc->ArraySize;
    desc.Format = _ConvertFormat(pDesc->Format);
    desc.Usage = _ConvertUsage(pDesc->Usage);
    desc.BindFlags = _ParseBindFlags(pDesc->BindFlags);
    desc.CPUAccessFlags = _ParseCPUAccessFlags(pDesc->CPUAccessFlags);
    desc.MiscFlags = _ParseResourceMiscFlags(pDesc->MiscFlags);

    return desc;
  }
  D3D11_TEXTURE2D_DESC _ConvertTextureDesc2D(const TextureDesc* pDesc)
  {
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = pDesc->Width;
    desc.Height = pDesc->Height;
    desc.MipLevels = pDesc->MipLevels;
    desc.ArraySize = pDesc->ArraySize;
    desc.Format = _ConvertFormat(pDesc->Format);
    desc.SampleDesc.Count = pDesc->SampleCount;
    desc.SampleDesc.Quality = 0;
    desc.Usage = _ConvertUsage(pDesc->Usage);
    desc.BindFlags = _ParseBindFlags(pDesc->BindFlags);
    desc.CPUAccessFlags = _ParseCPUAccessFlags(pDesc->CPUAccessFlags);
    desc.MiscFlags = _ParseResourceMiscFlags(pDesc->MiscFlags);

    return desc;
  }
  D3D11_TEXTURE3D_DESC _ConvertTextureDesc3D(const TextureDesc* pDesc)
  {
    D3D11_TEXTURE3D_DESC desc;
    desc.Width = pDesc->Width;
    desc.Height = pDesc->Height;
    desc.Depth = pDesc->Depth;
    desc.MipLevels = pDesc->MipLevels;
    desc.Format = _ConvertFormat(pDesc->Format);
    desc.Usage = _ConvertUsage(pDesc->Usage);
    desc.BindFlags = _ParseBindFlags(pDesc->BindFlags);
    desc.CPUAccessFlags = _ParseCPUAccessFlags(pDesc->CPUAccessFlags);
    desc.MiscFlags = _ParseResourceMiscFlags(pDesc->MiscFlags);

    return desc;
  }
  D3D11_SUBRESOURCE_DATA _ConvertSubresourceData(const SubresourceData& pInitialData)
  {
    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = pInitialData.pSysMem;
    data.SysMemPitch = pInitialData.SysMemPitch;
    data.SysMemSlicePitch = pInitialData.SysMemSlicePitch;

    return data;
  }
  constexpr ezUInt32 _ParseBindFlags_Inv(ezUInt32 value)
  {
    ezUInt32 _flag = 0;

    if (value & D3D11_BIND_VERTEX_BUFFER)
      _flag |= BIND_VERTEX_BUFFER;
    if (value & D3D11_BIND_INDEX_BUFFER)
      _flag |= BIND_INDEX_BUFFER;
    if (value & D3D11_BIND_CONSTANT_BUFFER)
      _flag |= BIND_CONSTANT_BUFFER;
    if (value & D3D11_BIND_SHADER_RESOURCE)
      _flag |= BIND_SHADER_RESOURCE;
    if (value & D3D11_BIND_STREAM_OUTPUT)
      _flag |= BIND_STREAM_OUTPUT;
    if (value & D3D11_BIND_RENDER_TARGET)
      _flag |= BIND_RENDER_TARGET;
    if (value & D3D11_BIND_DEPTH_STENCIL)
      _flag |= BIND_DEPTH_STENCIL;
    if (value & D3D11_BIND_UNORDERED_ACCESS)
      _flag |= BIND_UNORDERED_ACCESS;

    return _flag;
  }
  constexpr ezUInt32 _ParseCPUAccessFlags_Inv(ezUInt32 value)
  {
    ezUInt32 _flag = 0;

    if (value & D3D11_CPU_ACCESS_WRITE)
      _flag |= CPU_ACCESS_WRITE;
    if (value & D3D11_CPU_ACCESS_READ)
      _flag |= CPU_ACCESS_READ;

    return _flag;
  }
  constexpr ezUInt32 _ParseResourceMiscFlags_Inv(ezUInt32 value)
  {
    ezUInt32 _flag = 0;

    if (value & D3D11_RESOURCE_MISC_SHARED)
      _flag |= RESOURCE_MISC_SHARED;
    if (value & D3D11_RESOURCE_MISC_TEXTURECUBE)
      _flag |= RESOURCE_MISC_TEXTURECUBE;
    if (value & D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS)
      _flag |= RESOURCE_MISC_INDIRECT_ARGS;
    if (value & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
      _flag |= RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
    if (value & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
      _flag |= RESOURCE_MISC_BUFFER_STRUCTURED;
    if (value & D3D11_RESOURCE_MISC_TILED)
      _flag |= RESOURCE_MISC_TILED;

    return _flag;
  }
  ezEnum<ezRHIFormat> _ConvertFormat_Inv(DXGI_FORMAT value)
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
      case DXGI_FORMAT_R24G8_TYPELESS:
        return ezRHIFormat::R24G8_TYPELESS;
        break;
      case DXGI_FORMAT_D24_UNORM_S8_UINT:
        return ezRHIFormat::D24_UNORM_S8_UINT;
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
  ezEnum<ezRHIUsage> _ConvertUsage_Inv(D3D11_USAGE value)
  {
    switch (value)
    {
      case D3D11_USAGE_DEFAULT:
        return ezRHIUsage::Default;
        break;
      case D3D11_USAGE_IMMUTABLE:
        return ezRHIUsage::Immutable;
        break;
      case D3D11_USAGE_DYNAMIC:
        return ezRHIUsage::Dynamic;
        break;
      case D3D11_USAGE_STAGING:
        return ezRHIUsage::Staging;
        break;
      default:
        break;
    }
    return ezRHIUsage::Default;
  }
  TextureDesc _ConvertTextureDesc_Inv(const D3D11_TEXTURE1D_DESC* pDesc)
  {
    TextureDesc desc;
    desc.Width = pDesc->Width;
    desc.MipLevels = pDesc->MipLevels;
    desc.ArraySize = pDesc->ArraySize;
    desc.Format = _ConvertFormat_Inv(pDesc->Format);
    desc.Usage = _ConvertUsage_Inv(pDesc->Usage);
    desc.BindFlags = _ParseBindFlags_Inv(pDesc->BindFlags);
    desc.CPUAccessFlags = _ParseCPUAccessFlags_Inv(pDesc->CPUAccessFlags);
    desc.MiscFlags = _ParseResourceMiscFlags_Inv(pDesc->MiscFlags);

    return desc;
  }
  TextureDesc _ConvertTextureDesc_Inv(const D3D11_TEXTURE2D_DESC* pDesc)
  {
    TextureDesc desc;
    desc.Width = pDesc->Width;
    desc.Height = pDesc->Height;
    desc.MipLevels = pDesc->MipLevels;
    desc.ArraySize = pDesc->ArraySize;
    desc.Format = _ConvertFormat_Inv(pDesc->Format);
    desc.SampleCount = pDesc->SampleDesc.Count;
    desc.Usage = _ConvertUsage_Inv(pDesc->Usage);
    desc.BindFlags = _ParseBindFlags_Inv(pDesc->BindFlags);
    desc.CPUAccessFlags = _ParseCPUAccessFlags_Inv(pDesc->CPUAccessFlags);
    desc.MiscFlags = _ParseResourceMiscFlags_Inv(pDesc->MiscFlags);

    return desc;
  }
  TextureDesc _ConvertTextureDesc_Inv(const D3D11_TEXTURE3D_DESC* pDesc)
  {
    TextureDesc desc;
    desc.Width = pDesc->Width;
    desc.Height = pDesc->Height;
    desc.Depth = pDesc->Depth;
    desc.MipLevels = pDesc->MipLevels;
    desc.Format = _ConvertFormat_Inv(pDesc->Format);
    desc.Usage = _ConvertUsage_Inv(pDesc->Usage);
    desc.BindFlags = _ParseBindFlags_Inv(pDesc->BindFlags);
    desc.CPUAccessFlags = _ParseCPUAccessFlags_Inv(pDesc->CPUAccessFlags);
    desc.MiscFlags = _ParseResourceMiscFlags_Inv(pDesc->MiscFlags);

    return desc;
  }
  Resource_DX11* to_internal(const GPUResource* param)
  {
    return static_cast<Resource_DX11*>(param->internal_state.get());
  }
  Resource_DX11* to_internal(const GPUBuffer* param)
  {
    return static_cast<Resource_DX11*>(param->internal_state.get());
  }
  Texture_DX11* to_internal(const Texture* param)
  {
    return static_cast<Texture_DX11*>(param->internal_state.get());
  }
  InputLayout_DX11* to_internal(const InputLayout* param)
  {
    return static_cast<InputLayout_DX11*>(param->internal_state.get());
  }
  BlendState_DX11* to_internal(const BlendState* param)
  {
    return static_cast<BlendState_DX11*>(param->internal_state.get());
  }
  DepthStencilState_DX11* to_internal(const DepthStencilState* param)
  {
    return static_cast<DepthStencilState_DX11*>(param->internal_state.get());
  }
  RasterizerState_DX11* to_internal(const RasterizerState* param)
  {
    return static_cast<RasterizerState_DX11*>(param->internal_state.get());
  }
  Sampler_DX11* to_internal(const Sampler* param)
  {
    return static_cast<Sampler_DX11*>(param->internal_state.get());
  }
  Query_DX11* to_internal(const GPUQuery* param)
  {
    return static_cast<Query_DX11*>(param->internal_state.get());
  }
} // namespace DX11_Internal
