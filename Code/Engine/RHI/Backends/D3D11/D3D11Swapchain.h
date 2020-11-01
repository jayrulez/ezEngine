#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/Swapchain.h>
#include <RHI/Resources/SwapchainSource.h>

class D3D11Texture;
class D3D11GraphicsDevice;
class D3D11CommandList;
class D3D11Framebuffer;
struct IDXGISwapChain;

#include <dxgi.h>
#include <dxgiformat.h>

class D3D11Swapchain : public RHISwapchain
{
private:
  D3D11GraphicsDevice* GraphicsDevice = nullptr;
  std::optional<ezEnum<RHIPixelFormat>> DepthFormat;
  IDXGISwapChain* DXGISwapChain = nullptr;
  bool VSync;
  ezUInt32 SyncInterval;
  D3D11Framebuffer* Framebuffer;
  D3D11Texture* DepthTexture;
  float PixelScale = 1.f;
  ezMutex ReferencedCLsMutex;
  ezHashSet<D3D11CommandList*> ReferencedCLs;
  DXGI_FORMAT ColorFormat = DXGI_FORMAT_UNKNOWN;

  bool Disposed = false;
  ezString Name;

public:
  virtual ezString GetName() const override;

  virtual void SetName(const ezString& name) override;
  virtual bool IsDisposed() const override;
  virtual void Dispose() override;

  virtual RHIFramebuffer* GetFramebuffer() const override;

  virtual void Resize(ezUInt32 width, ezUInt32 height) override;

  virtual bool GetSyncToVerticalBlank() const override;

  virtual void SetSyncToVerticalBlank(bool value) override;

public:
  IDXGISwapChain* GetIDXGISwapChain() const;

  ezUInt32 GetSyncInterval() const;

  void AddCommandListReference(D3D11CommandList* cl);

  void RemoveCommandListReference(D3D11CommandList* cl);

  D3D11Swapchain(D3D11GraphicsDevice* graphicsDevice, const RHISwapchainDescription& description);
};
