#pragma once

#include <RHI/Descriptors/Descriptors.h>
#include <RHI/Device/SwapChain.h>
#include <RHIDX11/RHIDX11DLL.h>

class ezRHIDX11SwapChain : public ezRHISwapChain
{
public:
  EZ_ALWAYS_INLINE Microsoft::WRL::ComPtr<IDXGISwapChain1> GetDXGISwapChain()
  {
    return m_pSwapChain;
  }

protected:
  friend class ezRHIDX11Device;
  friend class ezMemoryUtils;

  ezRHIDX11SwapChain(const ezRHISwapChainDesc& desc);
  virtual ~ezRHIDX11SwapChain();

  virtual ezResult InitPlatform(ezRHIDevice* pDevice) override;

  virtual ezResult DeInitPlatform(ezRHIDevice* pDevice) override;

protected:
  Microsoft::WRL::ComPtr<IDXGISwapChain1> m_pSwapChain;
  ezRHITexture* m_pBackBufferTexture;
};
