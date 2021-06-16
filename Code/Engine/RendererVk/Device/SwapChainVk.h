
#pragma once

#include <RendererFoundation/Descriptors/Descriptors.h>
#include <RendererFoundation/Device/SwapChain.h>
#include <RendererVk/RendererVkDLL.h>

class ezGALSwapChainVk : public ezGALSwapChain
{
public:
  const VkExtent2D& GetVkSwapChainExtent() const;

protected:
  friend class ezGALDeviceVk;
  friend class ezMemoryUtils;

  ezGALSwapChainVk(const ezGALSwapChainCreationDescription& Description);

  virtual ~ezGALSwapChainVk();

  virtual ezResult InitPlatform(ezGALDevice* pDevice) override;

  virtual ezResult DeInitPlatform(ezGALDevice* pDevice) override;

private:
  VkQueue m_VkPresentQueue;
  VkSurfaceKHR m_VkSurface;
  VkSurfaceCapabilitiesKHR m_VkSurfaceCapabilities;
  ezDynamicArray<VkSurfaceFormatKHR> m_VkSurfaceFormats;
  ezDynamicArray<VkPresentModeKHR> m_VkPresentationModes;
  VkSwapchainKHR m_VkSwapChain;

  ezDynamicArray<VkImage> m_VkSwapChainImages;
  VkFormat m_VkSwapChainImageFormat;
  VkExtent2D m_VkSwapChainExtent;
  ezDynamicArray<VkImageView> m_VkSwapChainImageViews;

  ezGALDeviceVk* m_pDeviceVk;
};

#include <RendererVk/Device/Implementation/SwapChainVk_inl.h>
