#pragma once

#include <RHI/Descriptors/Descriptors.h>
#include <RHI/Resources/ResourceFormats.h>
#include <RHIDX11/RHIDX11DLL.h>

#include <DXGI1_3.h>
#include <d3d11_3.h>
#include <wrl/client.h> // ComPtr

namespace ezRHIDX11Utils
{
  // Engine -> Native converters

  ezUInt32 _ParseBindFlags(ezBitflags<ezRHIBindFlags> value)
  {
    ezUInt32 _flag = 0;

    if (value.IsSet(ezRHIBindFlags::VertexBuffer))
      _flag |= D3D11_BIND_VERTEX_BUFFER;
    if (value.IsSet(ezRHIBindFlags::IndexBuffer))
      _flag |= D3D11_BIND_INDEX_BUFFER;
    if (value.IsSet(ezRHIBindFlags::ConstantBuffer))
      _flag |= D3D11_BIND_CONSTANT_BUFFER;
    if (value.IsSet(ezRHIBindFlags::ShaderResource))
      _flag |= D3D11_BIND_SHADER_RESOURCE;
    if (value.IsSet(ezRHIBindFlags::StreamOutput))
      _flag |= D3D11_BIND_STREAM_OUTPUT;
    if (value.IsSet(ezRHIBindFlags::RenderTarget))
      _flag |= D3D11_BIND_RENDER_TARGET;
    if (value.IsSet(ezRHIBindFlags::DepthStencil))
      _flag |= D3D11_BIND_DEPTH_STENCIL;
    if (value.IsSet(ezRHIBindFlags::UnorderedAccess))
      _flag |= D3D11_BIND_UNORDERED_ACCESS;

    return _flag;
  }
  ezUInt32 _ParseCPUAccessFlags(ezBitflags<ezRHICPUAccessFlags> value)
  {
    ezUInt32 _flag = 0;

    if (value.IsSet(ezRHICPUAccessFlags::Write))
      _flag |= D3D11_CPU_ACCESS_WRITE;
    if (value.IsSet(ezRHICPUAccessFlags::Read))
      _flag |= D3D11_CPU_ACCESS_READ;

    return _flag;
  }
  ezUInt32 _ParseResourceMiscFlags(ezBitflags<ezRHIResourceMiscFlags> value)
  {
    ezUInt32 _flag = 0;

    if (value.IsSet(ezRHIResourceMiscFlags::Shared))
      _flag |= D3D11_RESOURCE_MISC_SHARED;
    if (value.IsSet(ezRHIResourceMiscFlags::TextureCube))
      _flag |= D3D11_RESOURCE_MISC_TEXTURECUBE;
    if (value.IsSet(ezRHIResourceMiscFlags::IndirectArgs))
      _flag |= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
    if (value.IsSet(ezRHIResourceMiscFlags::BufferAllowRawViews))
      _flag |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
    if (value.IsSet(ezRHIResourceMiscFlags::BufferStructured))
      _flag |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    if (value.IsSet(ezRHIResourceMiscFlags::Tiled))
      _flag |= D3D11_RESOURCE_MISC_TILED;

    return _flag;
  }
  constexpr ezUInt32 _ParseColorWriteMask(ezUInt32 value)
  {
    ezUInt32 _flag = 0;

    if (value == D3D11_COLOR_WRITE_ENABLE_ALL)
    {
      return D3D11_COLOR_WRITE_ENABLE_ALL;
    }
    else
    {
      if (value & ezRHIColorWriteMask::Red)
        _flag |= D3D11_COLOR_WRITE_ENABLE_RED;
      if (value & ezRHIColorWriteMask::Green)
        _flag |= D3D11_COLOR_WRITE_ENABLE_GREEN;
      if (value & ezRHIColorWriteMask::Blue)
        _flag |= D3D11_COLOR_WRITE_ENABLE_BLUE;
      if (value & ezRHIColorWriteMask::Alpha)
        _flag |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
    }

    return _flag;
  }

constexpr D3D11_FILTER _ConvertFilter(ezRHITextureFilterMode::Enum value)
  {
    switch (value)
    {
      case ezRHITextureFilterMode::MinMagMipPoint:
        return D3D11_FILTER_MIN_MAG_MIP_POINT;
        break;
      case ezRHITextureFilterMode::MinMagPointMipLinear:
        return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
        break;
      case ezRHITextureFilterMode::MinPointMagLinearMipPoint:
        return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
        break;
      case ezRHITextureFilterMode::MinPointMagMipLinear:
        return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
        break;
      case ezRHITextureFilterMode::MinLinearMagMipPoint:
        return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
        break;
      case ezRHITextureFilterMode::MinLinearMagPointMipLinear:
        return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        break;
      case ezRHITextureFilterMode::MinMagLinearMipPoint:
        return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        break;
      case ezRHITextureFilterMode::MinMagMipLinear:
        return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        break;
      case ezRHITextureFilterMode::Anisotropic:
        return D3D11_FILTER_ANISOTROPIC;
        break;
      case ezRHITextureFilterMode::ComparisonMinMagMipPoint:
        return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
        break;
      case ezRHITextureFilterMode::ComparisonMinMagPointMipLinear:
        return D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
        break;
      case ezRHITextureFilterMode::ComparisonMinPointMagLinearMipPoint:
        return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
        break;
      case ezRHITextureFilterMode::ComparisonMinPointMagMipLinear:
        return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
        break;
      case ezRHITextureFilterMode::ComparisonMinLinearMagMipPoint:
        return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
        break;
      case ezRHITextureFilterMode::ComparisonMinLinearMagPointMipLinear:
        return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        break;
      case ezRHITextureFilterMode::ComparisonMinMagLinearMipPoint:
        return D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
        break;
      case ezRHITextureFilterMode::ComparisonMinMagMipLinear:
        return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
        break;
      case ezRHITextureFilterMode::ComparisonAnisotropic:
        return D3D11_FILTER_COMPARISON_ANISOTROPIC;
        break;
      case ezRHITextureFilterMode::MinimumMinMagMipPoint:
        return D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT;
        break;
      case ezRHITextureFilterMode::MinimumMinMagPointMipLinear:
        return D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR;
        break;
      case ezRHITextureFilterMode::MinimumMinPointMagLinearMipPoint:
        return D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
        break;
      case ezRHITextureFilterMode::MinimumMinPointMagMipLinear:
        return D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR;
        break;
      case ezRHITextureFilterMode::MinimumMinLinearMagMipPoint:
        return D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT;
        break;
      case ezRHITextureFilterMode::MinimumMinLinearMagPointMipLinear:
        return D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        break;
      case ezRHITextureFilterMode::MinimumMinMagLinearMipPoint:
        return D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT;
        break;
      case ezRHITextureFilterMode::MinimumMinMagMipLinear:
        return D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
        break;
      case ezRHITextureFilterMode::MinimumAnisotropic:
        return D3D11_FILTER_MINIMUM_ANISOTROPIC;
        break;
      case ezRHITextureFilterMode::MaximumMinMagMipPoint:
        return D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT;
        break;
      case ezRHITextureFilterMode::MaximumMinMagPointMipLinear:
        return D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR;
        break;
      case ezRHITextureFilterMode::MaximumMinPointMagLinearMipPoint:
        return D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
        break;
      case ezRHITextureFilterMode::MaximumMinPointMagMipLinear:
        return D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR;
        break;
      case ezRHITextureFilterMode::MaximumMinLinearMagMipPoint:
        return D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT;
        break;
      case ezRHITextureFilterMode::MaximumMinLinearMagPointMipLinear:
        return D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        break;
      case ezRHITextureFilterMode::MaximumMinMagLinearMipPoint:
        return D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT;
        break;
      case ezRHITextureFilterMode::MaximumMinMagMipLinear:
        return D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR;
        break;
      case ezRHITextureFilterMode::MaximumAnisotropic:
        return D3D11_FILTER_MAXIMUM_ANISOTROPIC;
        break;
      default:
        break;
    }
    return D3D11_FILTER_MIN_MAG_MIP_POINT;
  }
  constexpr D3D11_TEXTURE_ADDRESS_MODE _ConvertTextureAddressMode(ezRHITextureAddressMode::Enum value)
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
  constexpr D3D11_COMPARISON_FUNC _ConvertComparisonFunc(ezRHICompareFunc::Enum value)
  {
    switch (value)
    {
      case ezRHICompareFunc::Never:
        return D3D11_COMPARISON_NEVER;
        break;
      case ezRHICompareFunc::Less:
        return D3D11_COMPARISON_LESS;
        break;
      case ezRHICompareFunc::Equal:
        return D3D11_COMPARISON_EQUAL;
        break;
      case ezRHICompareFunc::LessEqual:
        return D3D11_COMPARISON_LESS_EQUAL;
        break;
      case ezRHICompareFunc::Greater:
        return D3D11_COMPARISON_GREATER;
        break;
      case ezRHICompareFunc::NotEqual:
        return D3D11_COMPARISON_NOT_EQUAL;
        break;
      case ezRHICompareFunc::GreaterEqual:
        return D3D11_COMPARISON_GREATER_EQUAL;
        break;
      case ezRHICompareFunc::Always:
        return D3D11_COMPARISON_ALWAYS;
        break;
      default:
        break;
    }
    return D3D11_COMPARISON_NEVER;
  }
  constexpr D3D11_FILL_MODE _ConvertFillMode(ezRHIFillMode::Enum value)
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
  constexpr D3D11_CULL_MODE _ConvertCullMode(ezRHICullMode::Enum value)
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
  constexpr D3D11_DEPTH_WRITE_MASK _ConvertDepthWriteMask(ezRHIDepthWriteMask::Enum value)
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
  constexpr D3D11_STENCIL_OP _ConvertStencilOp(ezRHIStencilOp::Enum value)
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
  constexpr D3D11_BLEND _ConvertBlend(ezRHIBlend::Enum value)
  {
    switch (value)
    {
      case ezRHIBlend::Zero:
        return D3D11_BLEND_ZERO;
        break;
      case ezRHIBlend::One:
        return D3D11_BLEND_ONE;
        break;
      case ezRHIBlend::SrcColor:
        return D3D11_BLEND_SRC_COLOR;
        break;
      case ezRHIBlend::InvSrcColor:
        return D3D11_BLEND_INV_SRC_COLOR;
        break;
      case ezRHIBlend::SrcAlpha:
        return D3D11_BLEND_SRC_ALPHA;
        break;
      case ezRHIBlend::InvSrcAlpha:
        return D3D11_BLEND_INV_SRC_ALPHA;
        break;
      case ezRHIBlend::DestAlpha:
        return D3D11_BLEND_DEST_ALPHA;
        break;
      case ezRHIBlend::InvDestAlpha:
        return D3D11_BLEND_INV_DEST_ALPHA;
        break;
      case ezRHIBlend::DestColor:
        return D3D11_BLEND_DEST_COLOR;
        break;
      case ezRHIBlend::InvDestColor:
        return D3D11_BLEND_INV_DEST_COLOR;
        break;
      case ezRHIBlend::SrcAlphaSaturated:
        return D3D11_BLEND_SRC_ALPHA_SAT;
        break;
      case ezRHIBlend::BlendFactor:
        return D3D11_BLEND_BLEND_FACTOR;
        break;
      case ezRHIBlend::InvBlendFactor:
        return D3D11_BLEND_INV_BLEND_FACTOR;
        break;
      case ezRHIBlend::Src1Color:
        return D3D11_BLEND_SRC1_COLOR;
        break;
      case ezRHIBlend::InvSrc1Color:
        return D3D11_BLEND_INV_SRC1_COLOR;
        break;
      case ezRHIBlend::Src1Alpha:
        return D3D11_BLEND_SRC1_ALPHA;
        break;
      case ezRHIBlend::InvSrc1Alpha:
        return D3D11_BLEND_INV_SRC1_ALPHA;
        break;
      default:
        break;
    }
    return D3D11_BLEND_ZERO;
  }
  constexpr D3D11_BLEND_OP _ConvertBlendOp(ezRHIBlendOp::Enum value)
  {
    switch (value)
    {
      case ezRHIBlendOp::Add:
        return D3D11_BLEND_OP_ADD;
        break;
      case ezRHIBlendOp::Subtract:
        return D3D11_BLEND_OP_SUBTRACT;
        break;
      case ezRHIBlendOp::RevSubtract:
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
  constexpr D3D11_USAGE _ConvertUsage(ezRHIResourceUsage::Enum value)
  {
    switch (value)
    {
      case ezRHIResourceUsage::Default:
        return D3D11_USAGE_DEFAULT;
        break;
      case ezRHIResourceUsage::Immutable:
        return D3D11_USAGE_IMMUTABLE;
        break;
      case ezRHIResourceUsage::Dynamic:
        return D3D11_USAGE_DYNAMIC;
        break;
      case ezRHIResourceUsage::Staging:
        return D3D11_USAGE_STAGING;
        break;
      default:
        break;
    }
    return D3D11_USAGE_DEFAULT;
  }
  constexpr D3D11_INPUT_CLASSIFICATION _ConvertInputClassification(ezRHIInputClassification::Enum value)
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
  constexpr DXGI_FORMAT _ConvertFormat(ezRHIResourceFormat::Enum value)
  {
    switch (value)
    {
      case ezRHIResourceFormat::UNKNOWN:
        return DXGI_FORMAT_UNKNOWN;
        break;
      case ezRHIResourceFormat::R32G32B32A32_FLOAT:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
        break;
      case ezRHIResourceFormat::R32G32B32A32_UINT:
        return DXGI_FORMAT_R32G32B32A32_UINT;
        break;
      case ezRHIResourceFormat::R32G32B32A32_SINT:
        return DXGI_FORMAT_R32G32B32A32_SINT;
        break;
      case ezRHIResourceFormat::R32G32B32_FLOAT:
        return DXGI_FORMAT_R32G32B32_FLOAT;
        break;
      case ezRHIResourceFormat::R32G32B32_UINT:
        return DXGI_FORMAT_R32G32B32_UINT;
        break;
      case ezRHIResourceFormat::R32G32B32_SINT:
        return DXGI_FORMAT_R32G32B32_SINT;
        break;
      case ezRHIResourceFormat::R16G16B16A16_FLOAT:
        return DXGI_FORMAT_R16G16B16A16_FLOAT;
        break;
      case ezRHIResourceFormat::R16G16B16A16_UNORM:
        return DXGI_FORMAT_R16G16B16A16_UNORM;
        break;
      case ezRHIResourceFormat::R16G16B16A16_UINT:
        return DXGI_FORMAT_R16G16B16A16_UINT;
        break;
      case ezRHIResourceFormat::R16G16B16A16_SNORM:
        return DXGI_FORMAT_R16G16B16A16_SNORM;
        break;
      case ezRHIResourceFormat::R16G16B16A16_SINT:
        return DXGI_FORMAT_R16G16B16A16_SINT;
        break;
      case ezRHIResourceFormat::R32G32_FLOAT:
        return DXGI_FORMAT_R32G32_FLOAT;
        break;
      case ezRHIResourceFormat::R32G32_UINT:
        return DXGI_FORMAT_R32G32_UINT;
        break;
      case ezRHIResourceFormat::R32G32_SINT:
        return DXGI_FORMAT_R32G32_SINT;
        break;
      case ezRHIResourceFormat::R32G8X24_TYPELESS:
        return DXGI_FORMAT_R32G8X24_TYPELESS;
        break;
      case ezRHIResourceFormat::D32_FLOAT_S8X24_UINT:
        return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        break;
      case ezRHIResourceFormat::R10G10B10A2_UNORM:
        return DXGI_FORMAT_R10G10B10A2_UNORM;
        break;
      case ezRHIResourceFormat::R10G10B10A2_UINT:
        return DXGI_FORMAT_R10G10B10A2_UINT;
        break;
      case ezRHIResourceFormat::R11G11B10_FLOAT:
        return DXGI_FORMAT_R11G11B10_FLOAT;
        break;
      case ezRHIResourceFormat::R8G8B8A8_UNORM:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
        break;
      case ezRHIResourceFormat::R8G8B8A8_UNORM_SRGB:
        return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        break;
      case ezRHIResourceFormat::R8G8B8A8_UINT:
        return DXGI_FORMAT_R8G8B8A8_UINT;
        break;
      case ezRHIResourceFormat::R8G8B8A8_SNORM:
        return DXGI_FORMAT_R8G8B8A8_SNORM;
        break;
      case ezRHIResourceFormat::R8G8B8A8_SINT:
        return DXGI_FORMAT_R8G8B8A8_SINT;
        break;
      case ezRHIResourceFormat::R16G16_FLOAT:
        return DXGI_FORMAT_R16G16_FLOAT;
        break;
      case ezRHIResourceFormat::R16G16_UNORM:
        return DXGI_FORMAT_R16G16_UNORM;
        break;
      case ezRHIResourceFormat::R16G16_UINT:
        return DXGI_FORMAT_R16G16_UINT;
        break;
      case ezRHIResourceFormat::R16G16_SNORM:
        return DXGI_FORMAT_R16G16_SNORM;
        break;
      case ezRHIResourceFormat::R16G16_SINT:
        return DXGI_FORMAT_R16G16_SINT;
        break;
      case ezRHIResourceFormat::R32_TYPELESS:
        return DXGI_FORMAT_R32_TYPELESS;
        break;
      case ezRHIResourceFormat::D32_FLOAT:
        return DXGI_FORMAT_D32_FLOAT;
        break;
      case ezRHIResourceFormat::R32_FLOAT:
        return DXGI_FORMAT_R32_FLOAT;
        break;
      case ezRHIResourceFormat::R32_UINT:
        return DXGI_FORMAT_R32_UINT;
        break;
      case ezRHIResourceFormat::R32_SINT:
        return DXGI_FORMAT_R32_SINT;
        break;
      case ezRHIResourceFormat::R24G8_TYPELESS:
        return DXGI_FORMAT_R24G8_TYPELESS;
        break;
      case ezRHIResourceFormat::D24_UNORM_S8_UINT:
        return DXGI_FORMAT_D24_UNORM_S8_UINT;
        break;
      case ezRHIResourceFormat::R8G8_UNORM:
        return DXGI_FORMAT_R8G8_UNORM;
        break;
      case ezRHIResourceFormat::R8G8_UINT:
        return DXGI_FORMAT_R8G8_UINT;
        break;
      case ezRHIResourceFormat::R8G8_SNORM:
        return DXGI_FORMAT_R8G8_SNORM;
        break;
      case ezRHIResourceFormat::R8G8_SINT:
        return DXGI_FORMAT_R8G8_SINT;
        break;
      case ezRHIResourceFormat::R16_TYPELESS:
        return DXGI_FORMAT_R16_TYPELESS;
        break;
      case ezRHIResourceFormat::R16_FLOAT:
        return DXGI_FORMAT_R16_FLOAT;
        break;
      case ezRHIResourceFormat::D16_UNORM:
        return DXGI_FORMAT_D16_UNORM;
        break;
      case ezRHIResourceFormat::R16_UNORM:
        return DXGI_FORMAT_R16_UNORM;
        break;
      case ezRHIResourceFormat::R16_UINT:
        return DXGI_FORMAT_R16_UINT;
        break;
      case ezRHIResourceFormat::R16_SNORM:
        return DXGI_FORMAT_R16_SNORM;
        break;
      case ezRHIResourceFormat::R16_SINT:
        return DXGI_FORMAT_R16_SINT;
        break;
      case ezRHIResourceFormat::R8_UNORM:
        return DXGI_FORMAT_R8_UNORM;
        break;
      case ezRHIResourceFormat::R8_UINT:
        return DXGI_FORMAT_R8_UINT;
        break;
      case ezRHIResourceFormat::R8_SNORM:
        return DXGI_FORMAT_R8_SNORM;
        break;
      case ezRHIResourceFormat::R8_SINT:
        return DXGI_FORMAT_R8_SINT;
        break;
      case ezRHIResourceFormat::BC1_UNORM:
        return DXGI_FORMAT_BC1_UNORM;
        break;
      case ezRHIResourceFormat::BC1_UNORM_SRGB:
        return DXGI_FORMAT_BC1_UNORM_SRGB;
        break;
      case ezRHIResourceFormat::BC2_UNORM:
        return DXGI_FORMAT_BC2_UNORM;
        break;
      case ezRHIResourceFormat::BC2_UNORM_SRGB:
        return DXGI_FORMAT_BC2_UNORM_SRGB;
        break;
      case ezRHIResourceFormat::BC3_UNORM:
        return DXGI_FORMAT_BC3_UNORM;
        break;
      case ezRHIResourceFormat::BC3_UNORM_SRGB:
        return DXGI_FORMAT_BC3_UNORM_SRGB;
        break;
      case ezRHIResourceFormat::BC4_UNORM:
        return DXGI_FORMAT_BC4_UNORM;
        break;
      case ezRHIResourceFormat::BC4_SNORM:
        return DXGI_FORMAT_BC4_SNORM;
        break;
      case ezRHIResourceFormat::BC5_UNORM:
        return DXGI_FORMAT_BC5_UNORM;
        break;
      case ezRHIResourceFormat::BC5_SNORM:
        return DXGI_FORMAT_BC5_SNORM;
        break;
      case ezRHIResourceFormat::B8G8R8A8_UNORM:
        return DXGI_FORMAT_B8G8R8A8_UNORM;
        break;
      case ezRHIResourceFormat::B8G8R8A8_UNORM_SRGB:
        return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        break;
      case ezRHIResourceFormat::BC6H_UF16:
        return DXGI_FORMAT_BC6H_UF16;
        break;
      case ezRHIResourceFormat::BC6H_SF16:
        return DXGI_FORMAT_BC6H_SF16;
        break;
      case ezRHIResourceFormat::BC7_UNORM:
        return DXGI_FORMAT_BC7_UNORM;
        break;
      case ezRHIResourceFormat::BC7_UNORM_SRGB:
        return DXGI_FORMAT_BC7_UNORM_SRGB;
        break;
    }
    return DXGI_FORMAT_UNKNOWN;
  }

  inline D3D11_TEXTURE1D_DESC _ConvertTextureDesc1D(const ezRHITextureDesc* pDesc)
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
  inline D3D11_TEXTURE2D_DESC _ConvertTextureDesc2D(const ezRHITextureDesc* pDesc)
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
  inline D3D11_TEXTURE3D_DESC _ConvertTextureDesc3D(const ezRHITextureDesc* pDesc)
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
  inline D3D11_SUBRESOURCE_DATA _ConvertSubresourceData(const ezRHISubresourceData& pInitialData)
  {
    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = pInitialData.pSysMem;
    data.SysMemPitch = pInitialData.SysMemPitch;
    data.SysMemSlicePitch = pInitialData.SysMemSlicePitch;

    return data;
  }


  // Native -> Engine converters

  ezBitflags<ezRHIBindFlags> _ParseBindFlags_Inv(ezUInt32 value)
  {
    ezBitflags<ezRHIBindFlags> _flag = ezRHIBindFlags::Default;

    if (value & D3D11_BIND_VERTEX_BUFFER)
      _flag |= ezRHIBindFlags::VertexBuffer;
    if (value & D3D11_BIND_INDEX_BUFFER)
      _flag |= ezRHIBindFlags::IndexBuffer;
    if (value & D3D11_BIND_CONSTANT_BUFFER)
      _flag |= ezRHIBindFlags::ConstantBuffer;
    if (value & D3D11_BIND_SHADER_RESOURCE)
      _flag |= ezRHIBindFlags::ShaderResource;
    if (value & D3D11_BIND_STREAM_OUTPUT)
      _flag |= ezRHIBindFlags::StreamOutput;
    if (value & D3D11_BIND_RENDER_TARGET)
      _flag |= ezRHIBindFlags::RenderTarget;
    if (value & D3D11_BIND_DEPTH_STENCIL)
      _flag |= ezRHIBindFlags::DepthStencil;
    if (value & D3D11_BIND_UNORDERED_ACCESS)
      _flag |= ezRHIBindFlags::UnorderedAccess;

    return _flag;
  }
  ezBitflags<ezRHICPUAccessFlags> _ParseCPUAccessFlags_Inv(ezUInt32 value)
  {
    ezBitflags<ezRHICPUAccessFlags> _flag = ezRHICPUAccessFlags::Default;

    if (value & D3D11_CPU_ACCESS_WRITE)
      _flag |= ezRHICPUAccessFlags::Write;
    if (value & D3D11_CPU_ACCESS_READ)
      _flag |= ezRHICPUAccessFlags::Read;

    return _flag;
  }
  ezBitflags<ezRHIResourceMiscFlags> _ParseResourceMiscFlags_Inv(ezUInt32 value)
  {
    ezBitflags<ezRHIResourceMiscFlags> flags = ezRHIResourceMiscFlags::Default;

    if (value & D3D11_RESOURCE_MISC_SHARED)
      flags |= ezRHIResourceMiscFlags::Shared;
    if (value & D3D11_RESOURCE_MISC_TEXTURECUBE)
      flags |= ezRHIResourceMiscFlags::TextureCube;
    if (value & D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS)
      flags |= ezRHIResourceMiscFlags::IndirectArgs;
    if (value & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
      flags |= ezRHIResourceMiscFlags::BufferAllowRawViews;
    if (value & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
      flags |= ezRHIResourceMiscFlags::BufferStructured;
    if (value & D3D11_RESOURCE_MISC_TILED)
      flags |= ezRHIResourceMiscFlags::Tiled;

    return flags;
  }

  constexpr ezRHIResourceFormat::Enum _ConvertFormat_Inv(DXGI_FORMAT value)
  {
    switch (value)
    {
      case DXGI_FORMAT_UNKNOWN:
        return ezRHIResourceFormat::UNKNOWN;
        break;
      case DXGI_FORMAT_R32G32B32A32_FLOAT:
        return ezRHIResourceFormat::R32G32B32A32_FLOAT;
        break;
      case DXGI_FORMAT_R32G32B32A32_UINT:
        return ezRHIResourceFormat::R32G32B32A32_UINT;
        break;
      case DXGI_FORMAT_R32G32B32A32_SINT:
        return ezRHIResourceFormat::R32G32B32A32_SINT;
        break;
      case DXGI_FORMAT_R32G32B32_FLOAT:
        return ezRHIResourceFormat::R32G32B32_FLOAT;
        break;
      case DXGI_FORMAT_R32G32B32_UINT:
        return ezRHIResourceFormat::R32G32B32_UINT;
        break;
      case DXGI_FORMAT_R32G32B32_SINT:
        return ezRHIResourceFormat::R32G32B32_SINT;
        break;
      case DXGI_FORMAT_R16G16B16A16_FLOAT:
        return ezRHIResourceFormat::R16G16B16A16_FLOAT;
        break;
      case DXGI_FORMAT_R16G16B16A16_UNORM:
        return ezRHIResourceFormat::R16G16B16A16_UNORM;
        break;
      case DXGI_FORMAT_R16G16B16A16_UINT:
        return ezRHIResourceFormat::R16G16B16A16_UINT;
        break;
      case DXGI_FORMAT_R16G16B16A16_SNORM:
        return ezRHIResourceFormat::R16G16B16A16_SNORM;
        break;
      case DXGI_FORMAT_R16G16B16A16_SINT:
        return ezRHIResourceFormat::R16G16B16A16_SINT;
        break;
      case DXGI_FORMAT_R32G32_FLOAT:
        return ezRHIResourceFormat::R32G32_FLOAT;
        break;
      case DXGI_FORMAT_R32G32_UINT:
        return ezRHIResourceFormat::R32G32_UINT;
        break;
      case DXGI_FORMAT_R32G32_SINT:
        return ezRHIResourceFormat::R32G32_SINT;
        break;
      case DXGI_FORMAT_R32G8X24_TYPELESS:
        return ezRHIResourceFormat::R32G8X24_TYPELESS;
        break;
      case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        return ezRHIResourceFormat::D32_FLOAT_S8X24_UINT;
        break;
      case DXGI_FORMAT_R10G10B10A2_UNORM:
        return ezRHIResourceFormat::R10G10B10A2_UNORM;
        break;
      case DXGI_FORMAT_R10G10B10A2_UINT:
        return ezRHIResourceFormat::R10G10B10A2_UINT;
        break;
      case DXGI_FORMAT_R11G11B10_FLOAT:
        return ezRHIResourceFormat::R11G11B10_FLOAT;
        break;
      case DXGI_FORMAT_R8G8B8A8_UNORM:
        return ezRHIResourceFormat::R8G8B8A8_UNORM;
        break;
      case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        return ezRHIResourceFormat::R8G8B8A8_UNORM_SRGB;
        break;
      case DXGI_FORMAT_R8G8B8A8_UINT:
        return ezRHIResourceFormat::R8G8B8A8_UINT;
        break;
      case DXGI_FORMAT_R8G8B8A8_SNORM:
        return ezRHIResourceFormat::R8G8B8A8_SNORM;
        break;
      case DXGI_FORMAT_R8G8B8A8_SINT:
        return ezRHIResourceFormat::R8G8B8A8_SINT;
        break;
      case DXGI_FORMAT_R16G16_FLOAT:
        return ezRHIResourceFormat::R16G16_FLOAT;
        break;
      case DXGI_FORMAT_R16G16_UNORM:
        return ezRHIResourceFormat::R16G16_UNORM;
        break;
      case DXGI_FORMAT_R16G16_UINT:
        return ezRHIResourceFormat::R16G16_UINT;
        break;
      case DXGI_FORMAT_R16G16_SNORM:
        return ezRHIResourceFormat::R16G16_SNORM;
        break;
      case DXGI_FORMAT_R16G16_SINT:
        return ezRHIResourceFormat::R16G16_SINT;
        break;
      case DXGI_FORMAT_R32_TYPELESS:
        return ezRHIResourceFormat::R32_TYPELESS;
        break;
      case DXGI_FORMAT_D32_FLOAT:
        return ezRHIResourceFormat::D32_FLOAT;
        break;
      case DXGI_FORMAT_R32_FLOAT:
        return ezRHIResourceFormat::R32_FLOAT;
        break;
      case DXGI_FORMAT_R32_UINT:
        return ezRHIResourceFormat::R32_UINT;
        break;
      case DXGI_FORMAT_R32_SINT:
        return ezRHIResourceFormat::R32_SINT;
        break;
      case DXGI_FORMAT_R24G8_TYPELESS:
        return ezRHIResourceFormat::R24G8_TYPELESS;
        break;
      case DXGI_FORMAT_D24_UNORM_S8_UINT:
        return ezRHIResourceFormat::D24_UNORM_S8_UINT;
        break;
      case DXGI_FORMAT_R8G8_UNORM:
        return ezRHIResourceFormat::R8G8_UNORM;
        break;
      case DXGI_FORMAT_R8G8_UINT:
        return ezRHIResourceFormat::R8G8_UINT;
        break;
      case DXGI_FORMAT_R8G8_SNORM:
        return ezRHIResourceFormat::R8G8_SNORM;
        break;
      case DXGI_FORMAT_R8G8_SINT:
        return ezRHIResourceFormat::R8G8_SINT;
        break;
      case DXGI_FORMAT_R16_TYPELESS:
        return ezRHIResourceFormat::R16_TYPELESS;
        break;
      case DXGI_FORMAT_R16_FLOAT:
        return ezRHIResourceFormat::R16_FLOAT;
        break;
      case DXGI_FORMAT_D16_UNORM:
        return ezRHIResourceFormat::D16_UNORM;
        break;
      case DXGI_FORMAT_R16_UNORM:
        return ezRHIResourceFormat::R16_UNORM;
        break;
      case DXGI_FORMAT_R16_UINT:
        return ezRHIResourceFormat::R16_UINT;
        break;
      case DXGI_FORMAT_R16_SNORM:
        return ezRHIResourceFormat::R16_SNORM;
        break;
      case DXGI_FORMAT_R16_SINT:
        return ezRHIResourceFormat::R16_SINT;
        break;
      case DXGI_FORMAT_R8_UNORM:
        return ezRHIResourceFormat::R8_UNORM;
        break;
      case DXGI_FORMAT_R8_UINT:
        return ezRHIResourceFormat::R8_UINT;
        break;
      case DXGI_FORMAT_R8_SNORM:
        return ezRHIResourceFormat::R8_SNORM;
        break;
      case DXGI_FORMAT_R8_SINT:
        return ezRHIResourceFormat::R8_SINT;
        break;
      case DXGI_FORMAT_BC1_UNORM:
        return ezRHIResourceFormat::BC1_UNORM;
        break;
      case DXGI_FORMAT_BC1_UNORM_SRGB:
        return ezRHIResourceFormat::BC1_UNORM_SRGB;
        break;
      case DXGI_FORMAT_BC2_UNORM:
        return ezRHIResourceFormat::BC2_UNORM;
        break;
      case DXGI_FORMAT_BC2_UNORM_SRGB:
        return ezRHIResourceFormat::BC2_UNORM_SRGB;
        break;
      case DXGI_FORMAT_BC3_UNORM:
        return ezRHIResourceFormat::BC3_UNORM;
        break;
      case DXGI_FORMAT_BC3_UNORM_SRGB:
        return ezRHIResourceFormat::BC3_UNORM_SRGB;
        break;
      case DXGI_FORMAT_BC4_UNORM:
        return ezRHIResourceFormat::BC4_UNORM;
        break;
      case DXGI_FORMAT_BC4_SNORM:
        return ezRHIResourceFormat::BC4_SNORM;
        break;
      case DXGI_FORMAT_BC5_UNORM:
        return ezRHIResourceFormat::BC5_UNORM;
        break;
      case DXGI_FORMAT_BC5_SNORM:
        return ezRHIResourceFormat::BC5_SNORM;
        break;
      case DXGI_FORMAT_B8G8R8A8_UNORM:
        return ezRHIResourceFormat::B8G8R8A8_UNORM;
        break;
      case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        return ezRHIResourceFormat::B8G8R8A8_UNORM_SRGB;
        break;
      case DXGI_FORMAT_BC6H_UF16:
        return ezRHIResourceFormat::BC6H_UF16;
        break;
      case DXGI_FORMAT_BC6H_SF16:
        return ezRHIResourceFormat::BC6H_SF16;
        break;
      case DXGI_FORMAT_BC7_UNORM:
        return ezRHIResourceFormat::BC7_UNORM;
        break;
      case DXGI_FORMAT_BC7_UNORM_SRGB:
        return ezRHIResourceFormat::BC7_UNORM_SRGB;
        break;
    }
    return ezRHIResourceFormat::UNKNOWN;
  }
  constexpr ezRHIResourceUsage::Enum _ConvertUsage_Inv(D3D11_USAGE value)
  {
    switch (value)
    {
      case D3D11_USAGE_DEFAULT:
        return ezRHIResourceUsage::Default;
        break;
      case D3D11_USAGE_IMMUTABLE:
        return ezRHIResourceUsage::Immutable;
        break;
      case D3D11_USAGE_DYNAMIC:
        return ezRHIResourceUsage::Dynamic;
        break;
      case D3D11_USAGE_STAGING:
        return ezRHIResourceUsage::Staging;
        break;
      default:
        break;
    }
    return ezRHIResourceUsage::Default;
  }

  inline ezRHITextureDesc _ConvertTextureDesc_Inv(const D3D11_TEXTURE1D_DESC* pDesc)
  {
    ezRHITextureDesc desc;
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
  inline ezRHITextureDesc _ConvertTextureDesc_Inv(const D3D11_TEXTURE2D_DESC* pDesc)
  {
    ezRHITextureDesc desc;
    desc.Width = pDesc->Width;
    desc.Height = pDesc->Height;
    desc.MipLevels = pDesc->MipLevels;
    desc.ArraySize = pDesc->ArraySize;
    desc.Format = _ConvertFormat_Inv(pDesc->Format);
    desc.SampleCount = (ezRHIMSAASampleCount::Enum)pDesc->SampleDesc.Count;
    desc.Usage = _ConvertUsage_Inv(pDesc->Usage);
    desc.BindFlags = _ParseBindFlags_Inv(pDesc->BindFlags);
    desc.CPUAccessFlags = _ParseCPUAccessFlags_Inv(pDesc->CPUAccessFlags);
    desc.MiscFlags = _ParseResourceMiscFlags_Inv(pDesc->MiscFlags);

    return desc;
  }
  inline ezRHITextureDesc _ConvertTextureDesc_Inv(const D3D11_TEXTURE3D_DESC* pDesc)
  {
    ezRHITextureDesc desc;
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
} // namespace ezRHIDX11Utils
