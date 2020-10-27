#pragma once

#include <RHI/Descriptors/DepthStencilStateDescription.h>
#include <RHI/Descriptors/FramebufferDescription.h>
#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/Framebuffer.h>

#include <RHI/Backends/D3D11/D3D11Texture.h>

#include <RHI/Backends/D3D11/D3D11Utils.h>

#include <d3d11.h>

class D3D11Framebuffer : public RHIFramebuffer
{
private:
  ezDynamicArray<ID3D11RenderTargetView*> RenderTargetViews;
  ID3D11DepthStencilView* DepthStencilView = nullptr;

private:
  bool Disposed = false;
  ezString Name;

public:
  virtual ezString GetName() const override { return Name; }

  virtual void SetName(const ezString& name) override
  {
    Name = name;

    for (ezUInt32 i = 0; i < RenderTargetViews.GetCount(); i++)
    {
      ezStringBuilder sb(name.GetData(), "_RTV");
      sb.Append(i);
      RenderTargetViews[i]->SetPrivateData(WKPDID_D3DDebugObjectName, sb.GetCharacterCount(), sb.GetData());
    }
    if (DepthStencilView != nullptr)
    {
      ezStringBuilder sb(name.GetData(), "_DSV");
      DepthStencilView->SetPrivateData(WKPDID_D3DDebugObjectName, sb.GetCharacterCount(), sb.GetData());
    }
  }
  virtual bool IsDisposed() const override { return Disposed; }
  virtual void Dispose() override
  {
    if (!Disposed)
    {
      if (DepthStencilView)
      {
        DepthStencilView->Release();
        for (ID3D11RenderTargetView* rtv : RenderTargetViews)
        {
          rtv->Release();
        }
      }

      Disposed = true;
    }
  }

public:
  ezDynamicArray<ID3D11RenderTargetView*> GetRenderTargetViews() const
  {
    return RenderTargetViews;
  }

  ID3D11DepthStencilView* GetDepthStencilView() const
  {
    return DepthStencilView;
  }

  D3D11Framebuffer(ID3D11Device* device, const RHIFramebufferDescription& description);
};
