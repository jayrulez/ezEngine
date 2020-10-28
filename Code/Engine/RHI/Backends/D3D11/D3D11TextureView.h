#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/TextureView.h>

#include <d3d11_1.h>

class D3D11TextureView : public RHITextureView
{
private:
  ID3D11ShaderResourceView* ShaderResourceView = nullptr;
  ID3D11UnorderedAccessView* UnorderedAccessView = nullptr;

  bool Disposed = false;
  ezString Name;

public:
  virtual ezString GetName() const override { return Name; }

  virtual void SetName(const ezString& name) override
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
  virtual bool IsDisposed() const override { return Disposed; }
  virtual void Dispose() override
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

public:
  D3D11TextureView(D3D11GraphicsDevice* graphicsDevice, const RHITextureViewDescription& description);

  ID3D11ShaderResourceView* GetShaderResourceView() const
  {
    return ShaderResourceView;
  }

  ID3D11UnorderedAccessView* GetUnorderedAccessView() const
  {
    return UnorderedAccessView;
  }
};
