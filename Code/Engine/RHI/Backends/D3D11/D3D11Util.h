#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Descriptors/StencilBehaviorDescription.h>

#include <dxgi.h>
#include <dxgiformat.h>

#include <d3d11_1.h>

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

  D3D11_DEPTH_STENCILOP_DESC ToD3D11StencilOpDesc(const RHIStencilBehaviorDescription& sbd);
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
