#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <d3d11_1.h>

namespace D3D11FormatUtils
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

  D3D11_PRIMITIVE_TOPOLOGY RHIToD3D11PrimitiveTopology(ezEnum<RHIPrimitiveTopology> primitiveTopology);

  D3D11_FILL_MODE RHIToD3D11FillMode(ezEnum<RHIPolygonFillMode> fillMode);

  D3D11_CULL_MODE RHIToD3D11CullMode(ezEnum<RHIFaceCullMode> cullingMode);

  DXGI_FORMAT ToDxgiFormat(ezEnum<RHIVertexElementFormat> format);

  D3D11_COMPARISON_FUNC RHIToD3D11ComparisonFunc(ezEnum<RHIComparisonKind> comparisonKind);

  D3D11_TEXTURE_ADDRESS_MODE RHIToD3D11AddressMode(ezEnum<RHISamplerAddressMode> mode);

  DXGI_FORMAT GetDepthFormat(ezEnum<RHIPixelFormat> format);
} // namespace D3D11FormatUtils

class D3D11Texture;

namespace D3D11Util
{
  int ComputeSubresource(ezUInt32 mipLevel, ezUInt32 mipLevelCount, ezUInt32 arrayLayer);

  D3D11_SHADER_RESOURCE_VIEW_DESC GetSrvDesc(
    D3D11Texture* tex,
    ezUInt32 baseMipLevel,
    ezUInt32 levelCount,
    ezUInt32 baseArrayLayer,
    ezUInt32 layerCount,
    ezEnum<RHIPixelFormat> format);

  ezInt32 GetSyncInterval(bool syncToVBlank);
} // namespace D3D11Util

namespace D3D11DeviceUtils
{
  static bool IsSdkLayersAvailable();

  ezResult CreateDevice(IDXGIAdapter* adapter, D3D_DRIVER_TYPE type, D3D11_CREATE_DEVICE_FLAG flags, D3D_FEATURE_LEVEL featureLevels[], ID3D11Device* device);
  ezResult CreateDevice(IDXGIAdapter* adapter, D3D_DRIVER_TYPE type, D3D11_CREATE_DEVICE_FLAG flags, D3D_FEATURE_LEVEL featureLevels[], ID3D11Device* device, ID3D11DeviceContext* immediateContext);
  ezResult CreateDevice(IDXGIAdapter* adapter, D3D_DRIVER_TYPE type, D3D11_CREATE_DEVICE_FLAG flags, D3D_FEATURE_LEVEL featureLevels[], ID3D11Device* device, D3D_FEATURE_LEVEL& featureLevel, ID3D11DeviceContext* immediateContext);

  ezResult CreateDevice(ezUInt32* adapter, D3D_DRIVER_TYPE type, D3D11_CREATE_DEVICE_FLAG flags, D3D_FEATURE_LEVEL featureLevels[], ID3D11Device* device);
  ezResult CreateDevice(ezUInt32* adapter, D3D_DRIVER_TYPE type, D3D11_CREATE_DEVICE_FLAG flags, D3D_FEATURE_LEVEL featureLevels[], ID3D11Device* device, ID3D11DeviceContext* immediateContext);
  ezResult CreateDevice(ezUInt32* adapter, D3D_DRIVER_TYPE type, D3D11_CREATE_DEVICE_FLAG flags, D3D_FEATURE_LEVEL featureLevels[], ID3D11Device* device, D3D_FEATURE_LEVEL& featureLevel, ID3D11DeviceContext* immediateContext);
}
