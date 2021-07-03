
#pragma once

#include <RHI/Renderer/Descriptors/Descriptors.h>
#include <RHI/Renderer/Basics.h>

class EZ_RHI_DLL ezRHISwapChain : public ezRHIObject<ezRHISwapChainCreationDescription>
{
public:
  /// \brief Returns a handle to the back buffer texture.
  ///
  /// \attention Some RenderSystems do not support reading the swap chain's back buffer texture. Those will return an invalid handle.
  inline ezRHITextureHandle GetBackBufferTexture() const;

protected:
  ezRHISwapChain(const ezRHISwapChainCreationDescription& Description);

  virtual ~ezRHISwapChain();

  virtual ezResult InitPlatform(ezRHIDevice* pDevice) = 0;

  virtual ezResult DeInitPlatform(ezRHIDevice* pDevice);

  ezRHITextureHandle m_hBackBufferTexture;
};

#include <RHI/Renderer/Device/Implementation/SwapChain_inl.h>
