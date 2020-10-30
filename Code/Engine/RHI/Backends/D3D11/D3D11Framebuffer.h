#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Descriptors/FramebufferDescription.h>
#include <RHI/Resources/Framebuffer.h>

struct ID3D11Device;
struct ID3D11DepthStencilView;
struct ID3D11RenderTargetView;

class D3D11Swapchain;

class D3D11Framebuffer : public RHIFramebuffer
{
private:
  ezDynamicArray<ID3D11RenderTargetView*> RenderTargetViews;
  ID3D11DepthStencilView* DepthStencilView = nullptr;

  D3D11Swapchain* Swapchain = nullptr;

private:
  bool Disposed = false;
  ezString Name;

public:
  virtual ezString GetName() const override { return Name; }

  virtual void SetName(const ezString& name) override;
  virtual bool IsDisposed() const override { return Disposed; }
  virtual void Dispose() override;

public:
  ezDynamicArray<ID3D11RenderTargetView*> GetRenderTargetViews() const;

  ID3D11DepthStencilView* GetDepthStencilView() const;

  D3D11Framebuffer(ID3D11Device* device, const RHIFramebufferDescription& description);

public:
  //TODO: Make private and expost to friends

  D3D11Swapchain* GetSwapchain() const;

  void SetSwapchain(D3D11Swapchain* swapchain);
};
