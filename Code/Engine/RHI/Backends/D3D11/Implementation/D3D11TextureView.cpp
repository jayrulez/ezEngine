#include <RHI/Backends/D3D11/D3D11GraphicsDevice.h>
#include <RHI/Backends/D3D11/D3D11TextureView.h>
#include <RHI/Util.h>

#include <d3d11_1.h>

ezString D3D11TextureView::GetName() const
{
  return Name;
}

void D3D11TextureView::SetName(const ezString& name)
{
  Name = name;

  if (ShaderResourceView != nullptr)
  {
    ezStringBuilder sb(name.GetData(), "_SRV");
    ShaderResourceView->SetPrivateData(WKPDID_D3DDebugObjectName, sb.GetCharacterCount(), sb.GetData());
  }
  if (UnorderedAccessView != nullptr)
  {
    ezStringBuilder sb(name.GetData(), "_UAV");
    UnorderedAccessView->SetPrivateData(WKPDID_D3DDebugObjectName, sb.GetCharacterCount(), sb.GetData());
  }
}

bool D3D11TextureView::IsDisposed() const
{
  return Disposed;
}

void D3D11TextureView::Dispose()
{
  if (!Disposed)
  {
    if (ShaderResourceView != nullptr)
    {
      ShaderResourceView->Release();
      ShaderResourceView = nullptr;
    }
    if (UnorderedAccessView != nullptr)
    {
      UnorderedAccessView->Release();
      UnorderedAccessView = nullptr;
    }
    Disposed = true;
  }
}

D3D11TextureView::D3D11TextureView(D3D11GraphicsDevice* graphicsDevice, const RHITextureViewDescription& description)
  : RHITextureView(description)
{
  ID3D11Device* device = graphicsDevice->GetDevice();
  D3D11Texture* d3dTex = Util::AssertSubtype<RHITexture, D3D11Texture>(description.Target);

  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = D3D11Util::GetSrvDesc(
    d3dTex,
    description.BaseMipLevel,
    description.MipLevels,
    description.BaseArrayLayer,
    description.ArrayLayers,
    this->GetFormat());

  HRESULT hr = device->CreateShaderResourceView(d3dTex->GetDeviceTexture(), &srvDesc, &ShaderResourceView);

  if ((d3dTex->GetUsage() & RHITextureUsage::Storage) == RHITextureUsage::Storage)
  {
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    uavDesc.Format = D3D11Formats::GetViewFormat(d3dTex->GetDxgiFormat());

    if ((d3dTex->GetUsage() & RHITextureUsage::Cubemap) == RHITextureUsage::Cubemap)
    {
      EZ_ASSERT_NOT_IMPLEMENTED;
    }
    else if (d3dTex->GetDepth() == 1)
    {
      if (d3dTex->GetArrayLayers() == 1)
      {
        if (d3dTex->GetType() == RHITextureType::Texture1D)
        {
          uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
          uavDesc.Texture1D.MipSlice = description.BaseMipLevel;
        }
        else
        {
          uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
          uavDesc.Texture2D.MipSlice = description.BaseMipLevel;
        }
      }
      else
      {
        if (d3dTex->GetType() == RHITextureType::Texture1D)
        {
          uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
          uavDesc.Texture1DArray.MipSlice = description.BaseMipLevel;
          uavDesc.Texture1DArray.FirstArraySlice = description.BaseArrayLayer;
          uavDesc.Texture1DArray.ArraySize = description.ArrayLayers;
        }
        else
        {
          uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
          uavDesc.Texture2DArray.MipSlice = description.BaseMipLevel;
          uavDesc.Texture2DArray.FirstArraySlice = description.BaseArrayLayer;
          uavDesc.Texture2DArray.ArraySize = description.ArrayLayers;
        }
      }
    }
    else
    {
      uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
      uavDesc.Texture3D.MipSlice = description.BaseMipLevel;
      uavDesc.Texture3D.FirstWSlice = description.BaseArrayLayer;
      uavDesc.Texture3D.WSize = description.ArrayLayers;
    }

    hr = device->CreateUnorderedAccessView(d3dTex->GetDeviceTexture(), &uavDesc, &UnorderedAccessView);
  }
}

ID3D11ShaderResourceView* D3D11TextureView::GetShaderResourceView() const
{
  return ShaderResourceView;
}

ID3D11UnorderedAccessView* D3D11TextureView::GetUnorderedAccessView() const
{
  return UnorderedAccessView;
}
