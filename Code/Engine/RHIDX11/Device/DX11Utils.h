#pragma once

#include <RHIDX11/RHIDX11DLL.h>

using namespace Microsoft::WRL;

namespace DX11_Internal
{
  // Engine -> Native converters

  constexpr uint32_t _ParseBindFlags(uint32_t value)
  {
    uint32_t _flag = 0;

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
  constexpr uint32_t _ParseCPUAccessFlags(uint32_t value)
  {
    uint32_t _flag = 0;

    if (value & CPU_ACCESS_WRITE)
      _flag |= D3D11_CPU_ACCESS_WRITE;
    if (value & CPU_ACCESS_READ)
      _flag |= D3D11_CPU_ACCESS_READ;

    return _flag;
  }
  constexpr uint32_t _ParseResourceMiscFlags(uint32_t value)
  {
    uint32_t _flag = 0;

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
  constexpr uint32_t _ParseColorWriteMask(uint32_t value)
  {
    uint32_t _flag = 0;

    if (value == D3D11_COLOR_WRITE_ENABLE_ALL)
    {
      return D3D11_COLOR_WRITE_ENABLE_ALL;
    }
    else
    {
      if (value & COLOR_WRITE_ENABLE_RED)
        _flag |= D3D11_COLOR_WRITE_ENABLE_RED;
      if (value & COLOR_WRITE_ENABLE_GREEN)
        _flag |= D3D11_COLOR_WRITE_ENABLE_GREEN;
      if (value & COLOR_WRITE_ENABLE_BLUE)
        _flag |= D3D11_COLOR_WRITE_ENABLE_BLUE;
      if (value & COLOR_WRITE_ENABLE_ALPHA)
        _flag |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
    }

    return _flag;
  }

  constexpr D3D11_FILTER _ConvertFilter(FILTER value)
  {
    switch (value)
    {
      case FILTER_MIN_MAG_MIP_POINT:
        return D3D11_FILTER_MIN_MAG_MIP_POINT;
        break;
      case FILTER_MIN_MAG_POINT_MIP_LINEAR:
        return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
        break;
      case FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT:
        return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
        break;
      case FILTER_MIN_POINT_MAG_MIP_LINEAR:
        return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
        break;
      case FILTER_MIN_LINEAR_MAG_MIP_POINT:
        return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
        break;
      case FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
        return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        break;
      case FILTER_MIN_MAG_LINEAR_MIP_POINT:
        return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        break;
      case FILTER_MIN_MAG_MIP_LINEAR:
        return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        break;
      case FILTER_ANISOTROPIC:
        return D3D11_FILTER_ANISOTROPIC;
        break;
      case FILTER_COMPARISON_MIN_MAG_MIP_POINT:
        return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
        break;
      case FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
        return D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
        break;
      case FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
        return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
        break;
      case FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
        return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
        break;
      case FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
        return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
        break;
      case FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
        return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        break;
      case FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
        return D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
        break;
      case FILTER_COMPARISON_MIN_MAG_MIP_LINEAR:
        return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
        break;
      case FILTER_COMPARISON_ANISOTROPIC:
        return D3D11_FILTER_COMPARISON_ANISOTROPIC;
        break;
      case FILTER_MINIMUM_MIN_MAG_MIP_POINT:
        return D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT;
        break;
      case FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR:
        return D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR;
        break;
      case FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
        return D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
        break;
      case FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR:
        return D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR;
        break;
      case FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT:
        return D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT;
        break;
      case FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
        return D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        break;
      case FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT:
        return D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT;
        break;
      case FILTER_MINIMUM_MIN_MAG_MIP_LINEAR:
        return D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
        break;
      case FILTER_MINIMUM_ANISOTROPIC:
        return D3D11_FILTER_MINIMUM_ANISOTROPIC;
        break;
      case FILTER_MAXIMUM_MIN_MAG_MIP_POINT:
        return D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT;
        break;
      case FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR:
        return D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR;
        break;
      case FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
        return D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
        break;
      case FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR:
        return D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR;
        break;
      case FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT:
        return D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT;
        break;
      case FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
        return D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        break;
      case FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT:
        return D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT;
        break;
      case FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR:
        return D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR;
        break;
      case FILTER_MAXIMUM_ANISOTROPIC:
        return D3D11_FILTER_MAXIMUM_ANISOTROPIC;
        break;
      default:
        break;
    }
    return D3D11_FILTER_MIN_MAG_MIP_POINT;
  }
  constexpr D3D11_TEXTURE_ADDRESS_MODE _ConvertTextureAddressMode(TEXTURE_ADDRESS_MODE value)
  {
    switch (value)
    {
      case TEXTURE_ADDRESS_WRAP:
        return D3D11_TEXTURE_ADDRESS_WRAP;
        break;
      case TEXTURE_ADDRESS_MIRROR:
        return D3D11_TEXTURE_ADDRESS_MIRROR;
        break;
      case TEXTURE_ADDRESS_CLAMP:
        return D3D11_TEXTURE_ADDRESS_CLAMP;
        break;
      case TEXTURE_ADDRESS_BORDER:
        return D3D11_TEXTURE_ADDRESS_BORDER;
        break;
      default:
        break;
    }
    return D3D11_TEXTURE_ADDRESS_WRAP;
  }
  constexpr D3D11_COMPARISON_FUNC _ConvertComparisonFunc(COMPARISON_FUNC value)
  {
    switch (value)
    {
      case COMPARISON_NEVER:
        return D3D11_COMPARISON_NEVER;
        break;
      case COMPARISON_LESS:
        return D3D11_COMPARISON_LESS;
        break;
      case COMPARISON_EQUAL:
        return D3D11_COMPARISON_EQUAL;
        break;
      case COMPARISON_LESS_EQUAL:
        return D3D11_COMPARISON_LESS_EQUAL;
        break;
      case COMPARISON_GREATER:
        return D3D11_COMPARISON_GREATER;
        break;
      case COMPARISON_NOT_EQUAL:
        return D3D11_COMPARISON_NOT_EQUAL;
        break;
      case COMPARISON_GREATER_EQUAL:
        return D3D11_COMPARISON_GREATER_EQUAL;
        break;
      case COMPARISON_ALWAYS:
        return D3D11_COMPARISON_ALWAYS;
        break;
      default:
        break;
    }
    return D3D11_COMPARISON_NEVER;
  }
  constexpr D3D11_FILL_MODE _ConvertFillMode(FILL_MODE value)
  {
    switch (value)
    {
      case FILL_WIREFRAME:
        return D3D11_FILL_WIREFRAME;
        break;
      case FILL_SOLID:
        return D3D11_FILL_SOLID;
        break;
      default:
        break;
    }
    return D3D11_FILL_WIREFRAME;
  }
  constexpr D3D11_CULL_MODE _ConvertCullMode(CULL_MODE value)
  {
    switch (value)
    {
      case CULL_NONE:
        return D3D11_CULL_NONE;
        break;
      case CULL_FRONT:
        return D3D11_CULL_FRONT;
        break;
      case CULL_BACK:
        return D3D11_CULL_BACK;
        break;
      default:
        break;
    }
    return D3D11_CULL_NONE;
  }
  constexpr D3D11_DEPTH_WRITE_MASK _ConvertDepthWriteMask(DEPTH_WRITE_MASK value)
  {
    switch (value)
    {
      case DEPTH_WRITE_MASK_ZERO:
        return D3D11_DEPTH_WRITE_MASK_ZERO;
        break;
      case DEPTH_WRITE_MASK_ALL:
        return D3D11_DEPTH_WRITE_MASK_ALL;
        break;
      default:
        break;
    }
    return D3D11_DEPTH_WRITE_MASK_ZERO;
  }
  constexpr D3D11_STENCIL_OP _ConvertStencilOp(STENCIL_OP value)
  {
    switch (value)
    {
      case STENCIL_OP_KEEP:
        return D3D11_STENCIL_OP_KEEP;
        break;
      case STENCIL_OP_ZERO:
        return D3D11_STENCIL_OP_ZERO;
        break;
      case STENCIL_OP_REPLACE:
        return D3D11_STENCIL_OP_REPLACE;
        break;
      case STENCIL_OP_INCR_SAT:
        return D3D11_STENCIL_OP_INCR_SAT;
        break;
      case STENCIL_OP_DECR_SAT:
        return D3D11_STENCIL_OP_DECR_SAT;
        break;
      case STENCIL_OP_INVERT:
        return D3D11_STENCIL_OP_INVERT;
        break;
      case STENCIL_OP_INCR:
        return D3D11_STENCIL_OP_INCR;
        break;
      case STENCIL_OP_DECR:
        return D3D11_STENCIL_OP_DECR;
        break;
      default:
        break;
    }
    return D3D11_STENCIL_OP_KEEP;
  }
  constexpr D3D11_BLEND _ConvertBlend(BLEND value)
  {
    switch (value)
    {
      case BLEND_ZERO:
        return D3D11_BLEND_ZERO;
        break;
      case BLEND_ONE:
        return D3D11_BLEND_ONE;
        break;
      case BLEND_SRC_COLOR:
        return D3D11_BLEND_SRC_COLOR;
        break;
      case BLEND_INV_SRC_COLOR:
        return D3D11_BLEND_INV_SRC_COLOR;
        break;
      case BLEND_SRC_ALPHA:
        return D3D11_BLEND_SRC_ALPHA;
        break;
      case BLEND_INV_SRC_ALPHA:
        return D3D11_BLEND_INV_SRC_ALPHA;
        break;
      case BLEND_DEST_ALPHA:
        return D3D11_BLEND_DEST_ALPHA;
        break;
      case BLEND_INV_DEST_ALPHA:
        return D3D11_BLEND_INV_DEST_ALPHA;
        break;
      case BLEND_DEST_COLOR:
        return D3D11_BLEND_DEST_COLOR;
        break;
      case BLEND_INV_DEST_COLOR:
        return D3D11_BLEND_INV_DEST_COLOR;
        break;
      case BLEND_SRC_ALPHA_SAT:
        return D3D11_BLEND_SRC_ALPHA_SAT;
        break;
      case BLEND_BLEND_FACTOR:
        return D3D11_BLEND_BLEND_FACTOR;
        break;
      case BLEND_INV_BLEND_FACTOR:
        return D3D11_BLEND_INV_BLEND_FACTOR;
        break;
      case BLEND_SRC1_COLOR:
        return D3D11_BLEND_SRC1_COLOR;
        break;
      case BLEND_INV_SRC1_COLOR:
        return D3D11_BLEND_INV_SRC1_COLOR;
        break;
      case BLEND_SRC1_ALPHA:
        return D3D11_BLEND_SRC1_ALPHA;
        break;
      case BLEND_INV_SRC1_ALPHA:
        return D3D11_BLEND_INV_SRC1_ALPHA;
        break;
      default:
        break;
    }
    return D3D11_BLEND_ZERO;
  }
  constexpr D3D11_BLEND_OP _ConvertBlendOp(BLEND_OP value)
  {
    switch (value)
    {
      case BLEND_OP_ADD:
        return D3D11_BLEND_OP_ADD;
        break;
      case BLEND_OP_SUBTRACT:
        return D3D11_BLEND_OP_SUBTRACT;
        break;
      case BLEND_OP_REV_SUBTRACT:
        return D3D11_BLEND_OP_REV_SUBTRACT;
        break;
      case BLEND_OP_MIN:
        return D3D11_BLEND_OP_MIN;
        break;
      case BLEND_OP_MAX:
        return D3D11_BLEND_OP_MAX;
        break;
      default:
        break;
    }
    return D3D11_BLEND_OP_ADD;
  }
  constexpr D3D11_USAGE _ConvertUsage(USAGE value)
  {
    switch (value)
    {
      case USAGE_DEFAULT:
        return D3D11_USAGE_DEFAULT;
        break;
      case USAGE_IMMUTABLE:
        return D3D11_USAGE_IMMUTABLE;
        break;
      case USAGE_DYNAMIC:
        return D3D11_USAGE_DYNAMIC;
        break;
      case USAGE_STAGING:
        return D3D11_USAGE_STAGING;
        break;
      default:
        break;
    }
    return D3D11_USAGE_DEFAULT;
  }
  constexpr D3D11_INPUT_CLASSIFICATION _ConvertInputClassification(INPUT_CLASSIFICATION value)
  {
    switch (value)
    {
      case INPUT_PER_VERTEX_DATA:
        return D3D11_INPUT_PER_VERTEX_DATA;
        break;
      case INPUT_PER_INSTANCE_DATA:
        return D3D11_INPUT_PER_INSTANCE_DATA;
        break;
      default:
        break;
    }
    return D3D11_INPUT_PER_VERTEX_DATA;
  }
  constexpr DXGI_FORMAT _ConvertFormat(FORMAT value)
  {
    switch (value)
    {
      case FORMAT_UNKNOWN:
        return DXGI_FORMAT_UNKNOWN;
        break;
      case FORMAT_R32G32B32A32_FLOAT:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
        break;
      case FORMAT_R32G32B32A32_UINT:
        return DXGI_FORMAT_R32G32B32A32_UINT;
        break;
      case FORMAT_R32G32B32A32_SINT:
        return DXGI_FORMAT_R32G32B32A32_SINT;
        break;
      case FORMAT_R32G32B32_FLOAT:
        return DXGI_FORMAT_R32G32B32_FLOAT;
        break;
      case FORMAT_R32G32B32_UINT:
        return DXGI_FORMAT_R32G32B32_UINT;
        break;
      case FORMAT_R32G32B32_SINT:
        return DXGI_FORMAT_R32G32B32_SINT;
        break;
      case FORMAT_R16G16B16A16_FLOAT:
        return DXGI_FORMAT_R16G16B16A16_FLOAT;
        break;
      case FORMAT_R16G16B16A16_UNORM:
        return DXGI_FORMAT_R16G16B16A16_UNORM;
        break;
      case FORMAT_R16G16B16A16_UINT:
        return DXGI_FORMAT_R16G16B16A16_UINT;
        break;
      case FORMAT_R16G16B16A16_SNORM:
        return DXGI_FORMAT_R16G16B16A16_SNORM;
        break;
      case FORMAT_R16G16B16A16_SINT:
        return DXGI_FORMAT_R16G16B16A16_SINT;
        break;
      case FORMAT_R32G32_FLOAT:
        return DXGI_FORMAT_R32G32_FLOAT;
        break;
      case FORMAT_R32G32_UINT:
        return DXGI_FORMAT_R32G32_UINT;
        break;
      case FORMAT_R32G32_SINT:
        return DXGI_FORMAT_R32G32_SINT;
        break;
      case FORMAT_R32G8X24_TYPELESS:
        return DXGI_FORMAT_R32G8X24_TYPELESS;
        break;
      case FORMAT_D32_FLOAT_S8X24_UINT:
        return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        break;
      case FORMAT_R10G10B10A2_UNORM:
        return DXGI_FORMAT_R10G10B10A2_UNORM;
        break;
      case FORMAT_R10G10B10A2_UINT:
        return DXGI_FORMAT_R10G10B10A2_UINT;
        break;
      case FORMAT_R11G11B10_FLOAT:
        return DXGI_FORMAT_R11G11B10_FLOAT;
        break;
      case FORMAT_R8G8B8A8_UNORM:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
        break;
      case FORMAT_R8G8B8A8_UNORM_SRGB:
        return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        break;
      case FORMAT_R8G8B8A8_UINT:
        return DXGI_FORMAT_R8G8B8A8_UINT;
        break;
      case FORMAT_R8G8B8A8_SNORM:
        return DXGI_FORMAT_R8G8B8A8_SNORM;
        break;
      case FORMAT_R8G8B8A8_SINT:
        return DXGI_FORMAT_R8G8B8A8_SINT;
        break;
      case FORMAT_R16G16_FLOAT:
        return DXGI_FORMAT_R16G16_FLOAT;
        break;
      case FORMAT_R16G16_UNORM:
        return DXGI_FORMAT_R16G16_UNORM;
        break;
      case FORMAT_R16G16_UINT:
        return DXGI_FORMAT_R16G16_UINT;
        break;
      case FORMAT_R16G16_SNORM:
        return DXGI_FORMAT_R16G16_SNORM;
        break;
      case FORMAT_R16G16_SINT:
        return DXGI_FORMAT_R16G16_SINT;
        break;
      case FORMAT_R32_TYPELESS:
        return DXGI_FORMAT_R32_TYPELESS;
        break;
      case FORMAT_D32_FLOAT:
        return DXGI_FORMAT_D32_FLOAT;
        break;
      case FORMAT_R32_FLOAT:
        return DXGI_FORMAT_R32_FLOAT;
        break;
      case FORMAT_R32_UINT:
        return DXGI_FORMAT_R32_UINT;
        break;
      case FORMAT_R32_SINT:
        return DXGI_FORMAT_R32_SINT;
        break;
      case FORMAT_R24G8_TYPELESS:
        return DXGI_FORMAT_R24G8_TYPELESS;
        break;
      case FORMAT_D24_UNORM_S8_UINT:
        return DXGI_FORMAT_D24_UNORM_S8_UINT;
        break;
      case FORMAT_R8G8_UNORM:
        return DXGI_FORMAT_R8G8_UNORM;
        break;
      case FORMAT_R8G8_UINT:
        return DXGI_FORMAT_R8G8_UINT;
        break;
      case FORMAT_R8G8_SNORM:
        return DXGI_FORMAT_R8G8_SNORM;
        break;
      case FORMAT_R8G8_SINT:
        return DXGI_FORMAT_R8G8_SINT;
        break;
      case FORMAT_R16_TYPELESS:
        return DXGI_FORMAT_R16_TYPELESS;
        break;
      case FORMAT_R16_FLOAT:
        return DXGI_FORMAT_R16_FLOAT;
        break;
      case FORMAT_D16_UNORM:
        return DXGI_FORMAT_D16_UNORM;
        break;
      case FORMAT_R16_UNORM:
        return DXGI_FORMAT_R16_UNORM;
        break;
      case FORMAT_R16_UINT:
        return DXGI_FORMAT_R16_UINT;
        break;
      case FORMAT_R16_SNORM:
        return DXGI_FORMAT_R16_SNORM;
        break;
      case FORMAT_R16_SINT:
        return DXGI_FORMAT_R16_SINT;
        break;
      case FORMAT_R8_UNORM:
        return DXGI_FORMAT_R8_UNORM;
        break;
      case FORMAT_R8_UINT:
        return DXGI_FORMAT_R8_UINT;
        break;
      case FORMAT_R8_SNORM:
        return DXGI_FORMAT_R8_SNORM;
        break;
      case FORMAT_R8_SINT:
        return DXGI_FORMAT_R8_SINT;
        break;
      case FORMAT_BC1_UNORM:
        return DXGI_FORMAT_BC1_UNORM;
        break;
      case FORMAT_BC1_UNORM_SRGB:
        return DXGI_FORMAT_BC1_UNORM_SRGB;
        break;
      case FORMAT_BC2_UNORM:
        return DXGI_FORMAT_BC2_UNORM;
        break;
      case FORMAT_BC2_UNORM_SRGB:
        return DXGI_FORMAT_BC2_UNORM_SRGB;
        break;
      case FORMAT_BC3_UNORM:
        return DXGI_FORMAT_BC3_UNORM;
        break;
      case FORMAT_BC3_UNORM_SRGB:
        return DXGI_FORMAT_BC3_UNORM_SRGB;
        break;
      case FORMAT_BC4_UNORM:
        return DXGI_FORMAT_BC4_UNORM;
        break;
      case FORMAT_BC4_SNORM:
        return DXGI_FORMAT_BC4_SNORM;
        break;
      case FORMAT_BC5_UNORM:
        return DXGI_FORMAT_BC5_UNORM;
        break;
      case FORMAT_BC5_SNORM:
        return DXGI_FORMAT_BC5_SNORM;
        break;
      case FORMAT_B8G8R8A8_UNORM:
        return DXGI_FORMAT_B8G8R8A8_UNORM;
        break;
      case FORMAT_B8G8R8A8_UNORM_SRGB:
        return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        break;
      case FORMAT_BC6H_UF16:
        return DXGI_FORMAT_BC6H_UF16;
        break;
      case FORMAT_BC6H_SF16:
        return DXGI_FORMAT_BC6H_SF16;
        break;
      case FORMAT_BC7_UNORM:
        return DXGI_FORMAT_BC7_UNORM;
        break;
      case FORMAT_BC7_UNORM_SRGB:
        return DXGI_FORMAT_BC7_UNORM_SRGB;
        break;
    }
    return DXGI_FORMAT_UNKNOWN;
  }

  inline D3D11_TEXTURE1D_DESC _ConvertTextureDesc1D(const TextureDesc* pDesc)
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
  inline D3D11_TEXTURE2D_DESC _ConvertTextureDesc2D(const TextureDesc* pDesc)
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
  inline D3D11_TEXTURE3D_DESC _ConvertTextureDesc3D(const TextureDesc* pDesc)
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
  inline D3D11_SUBRESOURCE_DATA _ConvertSubresourceData(const SubresourceData& pInitialData)
  {
    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = pInitialData.pSysMem;
    data.SysMemPitch = pInitialData.SysMemPitch;
    data.SysMemSlicePitch = pInitialData.SysMemSlicePitch;

    return data;
  }


  // Native -> Engine converters

  constexpr uint32_t _ParseBindFlags_Inv(uint32_t value)
  {
    uint32_t _flag = 0;

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
  constexpr uint32_t _ParseCPUAccessFlags_Inv(uint32_t value)
  {
    uint32_t _flag = 0;

    if (value & D3D11_CPU_ACCESS_WRITE)
      _flag |= CPU_ACCESS_WRITE;
    if (value & D3D11_CPU_ACCESS_READ)
      _flag |= CPU_ACCESS_READ;

    return _flag;
  }
  constexpr uint32_t _ParseResourceMiscFlags_Inv(uint32_t value)
  {
    uint32_t _flag = 0;

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

  constexpr FORMAT _ConvertFormat_Inv(DXGI_FORMAT value)
  {
    switch (value)
    {
      case DXGI_FORMAT_UNKNOWN:
        return FORMAT_UNKNOWN;
        break;
      case DXGI_FORMAT_R32G32B32A32_FLOAT:
        return FORMAT_R32G32B32A32_FLOAT;
        break;
      case DXGI_FORMAT_R32G32B32A32_UINT:
        return FORMAT_R32G32B32A32_UINT;
        break;
      case DXGI_FORMAT_R32G32B32A32_SINT:
        return FORMAT_R32G32B32A32_SINT;
        break;
      case DXGI_FORMAT_R32G32B32_FLOAT:
        return FORMAT_R32G32B32_FLOAT;
        break;
      case DXGI_FORMAT_R32G32B32_UINT:
        return FORMAT_R32G32B32_UINT;
        break;
      case DXGI_FORMAT_R32G32B32_SINT:
        return FORMAT_R32G32B32_SINT;
        break;
      case DXGI_FORMAT_R16G16B16A16_FLOAT:
        return FORMAT_R16G16B16A16_FLOAT;
        break;
      case DXGI_FORMAT_R16G16B16A16_UNORM:
        return FORMAT_R16G16B16A16_UNORM;
        break;
      case DXGI_FORMAT_R16G16B16A16_UINT:
        return FORMAT_R16G16B16A16_UINT;
        break;
      case DXGI_FORMAT_R16G16B16A16_SNORM:
        return FORMAT_R16G16B16A16_SNORM;
        break;
      case DXGI_FORMAT_R16G16B16A16_SINT:
        return FORMAT_R16G16B16A16_SINT;
        break;
      case DXGI_FORMAT_R32G32_FLOAT:
        return FORMAT_R32G32_FLOAT;
        break;
      case DXGI_FORMAT_R32G32_UINT:
        return FORMAT_R32G32_UINT;
        break;
      case DXGI_FORMAT_R32G32_SINT:
        return FORMAT_R32G32_SINT;
        break;
      case DXGI_FORMAT_R32G8X24_TYPELESS:
        return FORMAT_R32G8X24_TYPELESS;
        break;
      case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        return FORMAT_D32_FLOAT_S8X24_UINT;
        break;
      case DXGI_FORMAT_R10G10B10A2_UNORM:
        return FORMAT_R10G10B10A2_UNORM;
        break;
      case DXGI_FORMAT_R10G10B10A2_UINT:
        return FORMAT_R10G10B10A2_UINT;
        break;
      case DXGI_FORMAT_R11G11B10_FLOAT:
        return FORMAT_R11G11B10_FLOAT;
        break;
      case DXGI_FORMAT_R8G8B8A8_UNORM:
        return FORMAT_R8G8B8A8_UNORM;
        break;
      case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        return FORMAT_R8G8B8A8_UNORM_SRGB;
        break;
      case DXGI_FORMAT_R8G8B8A8_UINT:
        return FORMAT_R8G8B8A8_UINT;
        break;
      case DXGI_FORMAT_R8G8B8A8_SNORM:
        return FORMAT_R8G8B8A8_SNORM;
        break;
      case DXGI_FORMAT_R8G8B8A8_SINT:
        return FORMAT_R8G8B8A8_SINT;
        break;
      case DXGI_FORMAT_R16G16_FLOAT:
        return FORMAT_R16G16_FLOAT;
        break;
      case DXGI_FORMAT_R16G16_UNORM:
        return FORMAT_R16G16_UNORM;
        break;
      case DXGI_FORMAT_R16G16_UINT:
        return FORMAT_R16G16_UINT;
        break;
      case DXGI_FORMAT_R16G16_SNORM:
        return FORMAT_R16G16_SNORM;
        break;
      case DXGI_FORMAT_R16G16_SINT:
        return FORMAT_R16G16_SINT;
        break;
      case DXGI_FORMAT_R32_TYPELESS:
        return FORMAT_R32_TYPELESS;
        break;
      case DXGI_FORMAT_D32_FLOAT:
        return FORMAT_D32_FLOAT;
        break;
      case DXGI_FORMAT_R32_FLOAT:
        return FORMAT_R32_FLOAT;
        break;
      case DXGI_FORMAT_R32_UINT:
        return FORMAT_R32_UINT;
        break;
      case DXGI_FORMAT_R32_SINT:
        return FORMAT_R32_SINT;
        break;
      case DXGI_FORMAT_R24G8_TYPELESS:
        return FORMAT_R24G8_TYPELESS;
        break;
      case DXGI_FORMAT_D24_UNORM_S8_UINT:
        return FORMAT_D24_UNORM_S8_UINT;
        break;
      case DXGI_FORMAT_R8G8_UNORM:
        return FORMAT_R8G8_UNORM;
        break;
      case DXGI_FORMAT_R8G8_UINT:
        return FORMAT_R8G8_UINT;
        break;
      case DXGI_FORMAT_R8G8_SNORM:
        return FORMAT_R8G8_SNORM;
        break;
      case DXGI_FORMAT_R8G8_SINT:
        return FORMAT_R8G8_SINT;
        break;
      case DXGI_FORMAT_R16_TYPELESS:
        return FORMAT_R16_TYPELESS;
        break;
      case DXGI_FORMAT_R16_FLOAT:
        return FORMAT_R16_FLOAT;
        break;
      case DXGI_FORMAT_D16_UNORM:
        return FORMAT_D16_UNORM;
        break;
      case DXGI_FORMAT_R16_UNORM:
        return FORMAT_R16_UNORM;
        break;
      case DXGI_FORMAT_R16_UINT:
        return FORMAT_R16_UINT;
        break;
      case DXGI_FORMAT_R16_SNORM:
        return FORMAT_R16_SNORM;
        break;
      case DXGI_FORMAT_R16_SINT:
        return FORMAT_R16_SINT;
        break;
      case DXGI_FORMAT_R8_UNORM:
        return FORMAT_R8_UNORM;
        break;
      case DXGI_FORMAT_R8_UINT:
        return FORMAT_R8_UINT;
        break;
      case DXGI_FORMAT_R8_SNORM:
        return FORMAT_R8_SNORM;
        break;
      case DXGI_FORMAT_R8_SINT:
        return FORMAT_R8_SINT;
        break;
      case DXGI_FORMAT_BC1_UNORM:
        return FORMAT_BC1_UNORM;
        break;
      case DXGI_FORMAT_BC1_UNORM_SRGB:
        return FORMAT_BC1_UNORM_SRGB;
        break;
      case DXGI_FORMAT_BC2_UNORM:
        return FORMAT_BC2_UNORM;
        break;
      case DXGI_FORMAT_BC2_UNORM_SRGB:
        return FORMAT_BC2_UNORM_SRGB;
        break;
      case DXGI_FORMAT_BC3_UNORM:
        return FORMAT_BC3_UNORM;
        break;
      case DXGI_FORMAT_BC3_UNORM_SRGB:
        return FORMAT_BC3_UNORM_SRGB;
        break;
      case DXGI_FORMAT_BC4_UNORM:
        return FORMAT_BC4_UNORM;
        break;
      case DXGI_FORMAT_BC4_SNORM:
        return FORMAT_BC4_SNORM;
        break;
      case DXGI_FORMAT_BC5_UNORM:
        return FORMAT_BC5_UNORM;
        break;
      case DXGI_FORMAT_BC5_SNORM:
        return FORMAT_BC5_SNORM;
        break;
      case DXGI_FORMAT_B8G8R8A8_UNORM:
        return FORMAT_B8G8R8A8_UNORM;
        break;
      case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        return FORMAT_B8G8R8A8_UNORM_SRGB;
        break;
      case DXGI_FORMAT_BC6H_UF16:
        return FORMAT_BC6H_UF16;
        break;
      case DXGI_FORMAT_BC6H_SF16:
        return FORMAT_BC6H_SF16;
        break;
      case DXGI_FORMAT_BC7_UNORM:
        return FORMAT_BC7_UNORM;
        break;
      case DXGI_FORMAT_BC7_UNORM_SRGB:
        return FORMAT_BC7_UNORM_SRGB;
        break;
    }
    return FORMAT_UNKNOWN;
  }
  constexpr USAGE _ConvertUsage_Inv(D3D11_USAGE value)
  {
    switch (value)
    {
      case D3D11_USAGE_DEFAULT:
        return USAGE_DEFAULT;
        break;
      case D3D11_USAGE_IMMUTABLE:
        return USAGE_IMMUTABLE;
        break;
      case D3D11_USAGE_DYNAMIC:
        return USAGE_DYNAMIC;
        break;
      case D3D11_USAGE_STAGING:
        return USAGE_STAGING;
        break;
      default:
        break;
    }
    return USAGE_DEFAULT;
  }

  inline TextureDesc _ConvertTextureDesc_Inv(const D3D11_TEXTURE1D_DESC* pDesc)
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
  inline TextureDesc _ConvertTextureDesc_Inv(const D3D11_TEXTURE2D_DESC* pDesc)
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
  inline TextureDesc _ConvertTextureDesc_Inv(const D3D11_TEXTURE3D_DESC* pDesc)
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


  // Local Helpers:
  const void* const __nullBlob[128] = {}; // this is initialized to nullptrs and used to unbind resources!


  struct Resource_DX11 : public ezRefCounted
  {
    ComPtr<ID3D11Resource> resource;
    ComPtr<ID3D11ShaderResourceView> srv;
    ComPtr<ID3D11UnorderedAccessView> uav;
    ezDynamicArray<ComPtr<ID3D11ShaderResourceView>> subresources_srv;
    ezDynamicArray<ComPtr<ID3D11UnorderedAccessView>> subresources_uav;
  };
  struct Texture_DX11 : public Resource_DX11
  {
    ComPtr<ID3D11RenderTargetView> rtv;
    ComPtr<ID3D11DepthStencilView> dsv;
    ezDynamicArray<ComPtr<ID3D11RenderTargetView>> subresources_rtv;
    ezDynamicArray<ComPtr<ID3D11DepthStencilView>> subresources_dsv;
  };
  struct VertexShader_DX11 : public ezRefCounted
  {
    ComPtr<ID3D11VertexShader> resource;
    ezDynamicArray<uint8_t> shadercode;
  };
  struct HullShader_DX11 : public ezRefCounted
  {
    ComPtr<ID3D11HullShader> resource;
  };
  struct DomainShader_DX11 : public ezRefCounted
  {
    ComPtr<ID3D11DomainShader> resource;
  };
  struct GeometryShader_DX11 : public ezRefCounted
  {
    ComPtr<ID3D11GeometryShader> resource;
  };
  struct PixelShader_DX11 : public ezRefCounted
  {
    ComPtr<ID3D11PixelShader> resource;
  };
  struct ComputeShader_DX11 : public ezRefCounted
  {
    ComPtr<ID3D11ComputeShader> resource;
  };
  struct PipelineState_DX11 : public ezRefCounted
  {
    ComPtr<ID3D11BlendState> bs;
    ComPtr<ID3D11DepthStencilState> dss;
    ComPtr<ID3D11RasterizerState> rs;
    ComPtr<ID3D11InputLayout> il;
  };
  struct Sampler_DX11 : public ezRefCounted
  {
    ComPtr<ID3D11SamplerState> resource;
  };
  struct QueryHeap_DX11 : public ezRefCounted
  {
    ezDynamicArray<ComPtr<ID3D11Query>> resources;
  };
  struct SwapChain_DX11 : public ezRefCounted
  {
    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
  };

  Resource_DX11* to_internal(const GPUResource* param)
  {
    //return *param->internal_state.Downcast<Resource_DX11*>();
    return static_cast<Resource_DX11*>(param->internal_state.Borrow());
  }
  Resource_DX11* to_internal(const GPUBuffer* param)
  {
    return static_cast<Resource_DX11*>(param->internal_state.Borrow());
  }
  Texture_DX11* to_internal(const Texture* param)
  {
    return static_cast<Texture_DX11*>(param->internal_state.Borrow());
  }
  PipelineState_DX11* to_internal(const PipelineState* param)
  {
    return static_cast<PipelineState_DX11*>(param->internal_state.Borrow());
  }
  Sampler_DX11* to_internal(const Sampler* param)
  {
    return static_cast<Sampler_DX11*>(param->internal_state.Borrow());
  }
  QueryHeap_DX11* to_internal(const GPUQueryHeap* param)
  {
    return static_cast<QueryHeap_DX11*>(param->internal_state.Borrow());
  }
  SwapChain_DX11* to_internal(const SwapChain* param)
  {
    return static_cast<SwapChain_DX11*>(param->internal_state.Borrow());
  }
} // namespace DX11_Internal
