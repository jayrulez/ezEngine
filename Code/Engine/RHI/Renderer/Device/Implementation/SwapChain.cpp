#include <RHIPCH.h>

#include <RHI/Renderer/Device/Device.h>
#include <RHI/Renderer/Device/SwapChain.h>

ezRHISwapChain::ezRHISwapChain(const ezRHISwapChainCreationDescription& Description)
  : ezRHIObject(Description)
{
}

ezRHISwapChain::~ezRHISwapChain() {}


ezResult ezRHISwapChain::DeInitPlatform(ezRHIDevice* pDevice)
{
  if (!m_hBackBufferTexture.IsInvalidated())
  {
    pDevice->DestroyTexture(m_hBackBufferTexture);
    m_hBackBufferTexture.Invalidate();
  }

  return EZ_SUCCESS;
}


EZ_STATICLINK_FILE(RendererFoundation, RendererFoundation_Device_Implementation_SwapChain);
