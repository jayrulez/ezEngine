#include <RHI/Device/SwapChain.h>

ezRHISwapChain::ezRHISwapChain(const ezRHISwapChainDesc& desc)
  : m_Desc{desc}
  , m_pBackBufferTexture{nullptr}
{
}

ezRHISwapChain::~ezRHISwapChain()
{
}
