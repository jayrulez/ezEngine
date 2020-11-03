#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <dxgi.h>
#include <dxgiformat.h>

#include <d3d11_1.h>

namespace D3D11Formats
{
  DXGI_FORMAT ToDxgiFormat(ezEnum<RHIPixelFormat> format, bool depthFormat);

  DXGI_FORMAT GetTypelessFormat(DXGI_FORMAT format);

  D3D11_BIND_FLAG RHIToD3D11BindFlags(ezBitflags<RHIBufferUsage> usage);

  ezBitflags<RHITextureUsage> GetRHIUsage(D3D11_BIND_FLAG bindFlags, D3D11_CPU_ACCESS_FLAG cpuFlags, D3D11_RESOURCE_MISC_FLAG optionFlags);

  bool IsUnsupportedFormat(ezEnum<RHIPixelFormat> format);

  DXGI_FORMAT GetViewFormat(DXGI_FORMAT format);

  D3D11_BLEND RHIToD3D11Blend(ezEnum<RHIBlendFactor> factor);

  DXGI_FORMAT ToDxgiFormat(ezEnum<RHIIndexFormat> format);

  D3D11_STENCIL_OP RHIToD3D11StencilOperation(ezEnum<RHIStencilOperation> op);

  ezEnum<RHIPixelFormat> ToRHIFormat(DXGI_FORMAT format);

  D3D11_BLEND_OP RHIToD3D11BlendOperation(ezEnum<RHIBlendFunction> function);

  D3D11_FILTER ToD3D11Filter(ezEnum<RHISamplerFilter> filter, bool isComparison);

  D3D11_MAP RHIToD3D11MapMode(bool isDynamic, ezEnum<RHIMapMode> mode);

  D3D_PRIMITIVE_TOPOLOGY RHIToD3D11PrimitiveTopology(ezEnum<RHIPrimitiveTopology> primitiveTopology);

  D3D11_FILL_MODE RHIToD3D11FillMode(ezEnum<RHIPolygonFillMode> fillMode);

  D3D11_CULL_MODE RHIToD3D11CullMode(ezEnum<RHIFaceCullMode> cullingMode);

  DXGI_FORMAT ToDxgiFormat(ezEnum<RHIVertexElementFormat> format);

  D3D11_COMPARISON_FUNC RHIToD3D11ComparisonFunc(ezEnum<RHIComparisonKind> comparisonKind);

  D3D11_TEXTURE_ADDRESS_MODE RHIToD3D11AddressMode(ezEnum<RHISamplerAddressMode> mode);

  DXGI_FORMAT GetDepthFormat(ezEnum<RHIPixelFormat> format);
} // namespace D3D11Formats
