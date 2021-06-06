#pragma once

#include <RHI/Descriptors/Descriptors.h>
#include <RHI/RHIDLL.h>

class EZ_RHI_DLL ezRHISwapChain : public ezRHIObject
{
public:
  const ezRHISwapChainDesc& GetDesc() const
  {
    return m_Desc;
  }

  inline ezRHITexture* GetBackBufferTexture() const;

protected:
  ezRHISwapChain(const ezRHISwapChainDesc& desc);
  virtual ~ezRHISwapChain();

  virtual ezResult InitPlatform(ezRHIDevice* pDevice) = 0;
  virtual ezResult DeInitPlatform(ezRHIDevice* pDevice) = 0;

protected:
  ezRHITexture* m_pBackBufferTexture;
  ezRHISwapChainDesc m_Desc;
};

#include <RHI/Device/Implementation/SwapChain_inl.h>
