#include <RHI/Backends/D3D11/D3D11Texture.h>
#include <RHI/Backends/D3D11/D3D11Util.h>

namespace D3D11Util
{
  ezUInt32 ComputeSubresource(ezUInt32 mipLevel, ezUInt32 mipLevelCount, ezUInt32 arrayLayer)
  {
    return (arrayLayer * mipLevelCount) + mipLevel;
  }

  D3D11_SHADER_RESOURCE_VIEW_DESC GetSrvDesc(
    D3D11Texture* tex,
    ezUInt32 baseMipLevel,
    ezUInt32 levelCount,
    ezUInt32 baseArrayLayer,
    ezUInt32 layerCount,
    ezEnum<RHIPixelFormat> format)
  {
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

    srvDesc.Format = D3D11Formats::GetViewFormat(D3D11Formats::ToDxgiFormat(format, (tex->GetUsage() & RHITextureUsage::DepthStencil) != 0));

    if ((tex->GetUsage() & RHITextureUsage::Cubemap) == RHITextureUsage::Cubemap)
    {
      if (tex->GetArrayLayers() == 1)
      {
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
        srvDesc.TextureCube.MostDetailedMip = baseMipLevel;
        srvDesc.TextureCube.MipLevels = levelCount;
      }
      else
      {
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
        srvDesc.TextureCubeArray.MostDetailedMip = baseMipLevel;
        srvDesc.TextureCubeArray.MipLevels = levelCount;
        srvDesc.TextureCubeArray.First2DArrayFace = baseArrayLayer;
        srvDesc.TextureCubeArray.NumCubes = tex->GetArrayLayers();
      }
    }
    else if (tex->GetDepth() == 1)
    {
      if (tex->GetArrayLayers() == 1)
      {
        if (tex->GetType() == RHITextureType::Texture1D)
        {
          srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
          srvDesc.Texture1D.MostDetailedMip = baseMipLevel;
          srvDesc.Texture1D.MipLevels = levelCount;
        }
        else
        {
          if (tex->GetSampleCount() == RHITextureSampleCount::Count1)
            srvDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
          else
            srvDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMS;
          srvDesc.Texture2D.MostDetailedMip = baseMipLevel;
          srvDesc.Texture2D.MipLevels = levelCount;
        }
      }
      else
      {
        if (tex->GetType() == RHITextureType::Texture1D)
        {
          srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
          srvDesc.Texture1DArray.MostDetailedMip = baseMipLevel;
          srvDesc.Texture1DArray.MipLevels = levelCount;
          srvDesc.Texture1DArray.FirstArraySlice = baseArrayLayer;
          srvDesc.Texture1DArray.ArraySize = layerCount;
        }
        else
        {
          srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
          srvDesc.Texture2DArray.MostDetailedMip = baseMipLevel;
          srvDesc.Texture2DArray.MipLevels = levelCount;
          srvDesc.Texture2DArray.FirstArraySlice = baseArrayLayer;
          srvDesc.Texture2DArray.ArraySize = layerCount;
        }
      }
    }
    else
    {
      srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
      srvDesc.Texture3D.MostDetailedMip = baseMipLevel;
      srvDesc.Texture3D.MipLevels = levelCount;
    }

    return srvDesc;
  }

  ezInt32 GetSyncInterval(bool syncToVBlank)
  {
    return syncToVBlank ? 1 : 0;
  }
  D3D11_DEPTH_STENCILOP_DESC ToD3D11StencilOpDesc(const RHIStencilBehaviorDescription& sbd)
  {
    D3D11_DEPTH_STENCILOP_DESC desc;
    desc.StencilFunc = D3D11Formats::RHIToD3D11ComparisonFunc(sbd.Comparison);
    desc.StencilPassOp = D3D11Formats::RHIToD3D11StencilOperation(sbd.Pass);
    desc.StencilFailOp = D3D11Formats::RHIToD3D11StencilOperation(sbd.Fail);
    desc.StencilDepthFailOp = D3D11Formats::RHIToD3D11StencilOperation(sbd.DepthFail);

    return desc;
  }
} // namespace D3D11Util

namespace D3D11DeviceUtils
{
  bool IsSdkLayersAvailable()
  {
    return true;
  }

  ezResult CreateDevice(IDXGIAdapter* adapter, D3D_DRIVER_TYPE type, D3D11_CREATE_DEVICE_FLAG flags, D3D_FEATURE_LEVEL featureLevels[], ID3D11Device* device)
  {
    return EZ_SUCCESS;
  }
  ezResult CreateDevice(IDXGIAdapter* adapter, D3D_DRIVER_TYPE type, D3D11_CREATE_DEVICE_FLAG flags, D3D_FEATURE_LEVEL featureLevels[], ID3D11Device* device, ID3D11DeviceContext* immediateContext)
  {
    return EZ_SUCCESS;
  }
  ezResult CreateDevice(IDXGIAdapter* adapter, D3D_DRIVER_TYPE type, D3D11_CREATE_DEVICE_FLAG flags, D3D_FEATURE_LEVEL featureLevels[], ID3D11Device* device, D3D_FEATURE_LEVEL& featureLevel, ID3D11DeviceContext* immediateContext)
  {
    return EZ_SUCCESS;
  }

  ezResult CreateDevice(ezUInt32* adapter, D3D_DRIVER_TYPE type, D3D11_CREATE_DEVICE_FLAG flags, D3D_FEATURE_LEVEL featureLevels[], ID3D11Device* device)
  {
    return EZ_SUCCESS;
  }
  ezResult CreateDevice(ezUInt32* adapter, D3D_DRIVER_TYPE type, D3D11_CREATE_DEVICE_FLAG flags, D3D_FEATURE_LEVEL featureLevels[], ID3D11Device* device, ID3D11DeviceContext* immediateContext)
  {
    return EZ_SUCCESS;
  }
  ezResult CreateDevice(ezUInt32* adapter, D3D_DRIVER_TYPE type, D3D11_CREATE_DEVICE_FLAG flags, D3D_FEATURE_LEVEL featureLevels[], ID3D11Device* device, D3D_FEATURE_LEVEL& featureLevel, ID3D11DeviceContext* immediateContext)
  {
    return EZ_SUCCESS;
  }
} // namespace D3D11DeviceUtils
