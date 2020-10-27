#pragma once

#include <RHI/Backends/D3D11/D3D11Framebuffer.h>

D3D11Framebuffer::D3D11Framebuffer(ID3D11Device* device, const RHIFramebufferDescription& description)
  : RHIFramebuffer(description.DepthTarget, description.ColorTargets)
{
  if (description.DepthTarget)
  {
    D3D11Texture* d3dDepthTarget = dynamic_cast<D3D11Texture*>(description.DepthTarget.value().Target);
    EZ_VERIFY(d3dDepthTarget != nullptr, "Depth Target Texture is not a valid D3D11Texture.");

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Format = D3D11FormatUtils::GetDepthFormat(d3dDepthTarget->GetFormat());

    if (d3dDepthTarget->GetArrayLayers())
    {
      if (d3dDepthTarget->GetSampleCount() == RHITextureSampleCount::Count1)
      {
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Texture2D.MipSlice = description.DepthTarget.value().MipLevel;
      }
      else
      {
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
      }
    }
    else
    {
      if (d3dDepthTarget->GetSampleCount() == RHITextureSampleCount::Count1)
      {
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
        dsvDesc.Texture2DArray.FirstArraySlice = description.DepthTarget.value().ArrayLayer;
        dsvDesc.Texture2DArray.ArraySize = 1;
        dsvDesc.Texture2DArray.MipSlice = description.DepthTarget.value().MipLevel;
      }
      else
      {
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
        dsvDesc.Texture2DMSArray.FirstArraySlice = (int)description.DepthTarget.value().ArrayLayer;
        dsvDesc.Texture2DMSArray.ArraySize = 1;
      }
    }

    HRESULT hr = device->CreateDepthStencilView(d3dDepthTarget->GetDeviceTexture(), &dsvDesc, &DepthStencilView);
  }

  if (description.ColorTargets.GetCount() > 0)
  {
    RenderTargetViews.SetCountUninitialized(description.ColorTargets.GetCount());

    for (ezUInt32 i = 0; i < RenderTargetViews.GetCount(); i++)
    {
      D3D11Texture* d3dColorTarget = dynamic_cast<D3D11Texture*>(description.ColorTargets[i].Target);
      EZ_ASSERT_ALWAYS(d3dColorTarget != nullptr, "Color target is null.");

      D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
      rtvDesc.Format = D3D11FormatUtils::ToDxgiFormat(d3dColorTarget->GetFormat(), false);


      if (d3dColorTarget->GetArrayLayers() > 1 || (d3dColorTarget->GetUsage() & RHITextureUsage::Cubemap) != 0)
      {
        if (d3dColorTarget->GetSampleCount() == RHITextureSampleCount::Count1)
        {

          /*
            UINT MipSlice;
    UINT FirstArraySlice;
    UINT ArraySize;
            */
          rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;

          rtvDesc.Texture2DArray.ArraySize = 1;
          rtvDesc.Texture2DArray.FirstArraySlice = description.ColorTargets[i].ArrayLayer;
          rtvDesc.Texture2DArray.MipSlice = description.ColorTargets[i].MipLevel;
        }
        else
        {
          rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
          rtvDesc.Texture2DMSArray.ArraySize = 1;
          rtvDesc.Texture2DMSArray.FirstArraySlice = description.ColorTargets[i].ArrayLayer;
        }
      }
      else
      {
        if (d3dColorTarget->GetSampleCount() == RHITextureSampleCount::Count1)
        {
          rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
          rtvDesc.Texture2D.MipSlice = description.ColorTargets[i].MipLevel;
        }
        else
        {
          rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
        }
      }
      HRESULT hr = device->CreateRenderTargetView(d3dColorTarget->GetDeviceTexture(), &rtvDesc, &RenderTargetViews[i]);
    }
  }
  else
  {
    RenderTargetViews.Clear();
  }
}
