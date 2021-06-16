#include <RendererVkPCH.h>

#include <Core/System/Window.h>
#include <Foundation/Basics/Platform/Win/IncludeWindows.h>
#include <RendererVk/Device/DeviceVk.h>
#include <RendererVk/Device/SwapChainVk.h>

#include <Foundation/Basics/Platform/Win/HResultUtils.h>

using QueueType = ezInternal::Vk::QueueType;

ezGALSwapChainVk::ezGALSwapChainVk(const ezGALSwapChainCreationDescription& Description)
  : ezGALSwapChain(Description)
  , m_VkSurface{VK_NULL_HANDLE}
  , m_VkSurfaceCapabilities{VK_NULL_HANDLE}
  , m_VkSwapChain{VK_NULL_HANDLE}
  , m_VkSwapChainImageFormat{}
  , m_VkSwapChainExtent{}
  , m_pDeviceVk{nullptr}
{
}

ezGALSwapChainVk::~ezGALSwapChainVk() {}

ezResult ezGALSwapChainVk::InitPlatform(ezGALDevice* pDevice)
{
  m_pDeviceVk = static_cast<ezGALDeviceVk*>(pDevice);

  VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
  surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  surfaceCreateInfo.hwnd = (HWND)m_Description.m_pWindow->GetNativeWindowHandle();
  surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);

  if (vkCreateWin32SurfaceKHR(m_pDeviceVk->GetVkInstance(), &surfaceCreateInfo, nullptr, &m_VkSurface) != VK_SUCCESS)
  {
    ezLog::Error("Failed to create window surface.");
    return EZ_FAILURE;
  }

  VkBool32 presentSupported = VK_FALSE;
  ezInt32 presentQueueFamilyIndex = m_pDeviceVk->GetVkPresentQueueFamilyIndex();

  if (vkGetPhysicalDeviceSurfaceSupportKHR(m_pDeviceVk->GetVkPhysicalDevice(), presentQueueFamilyIndex, m_VkSurface, &presentSupported) != VK_SUCCESS)
  {
    return EZ_FAILURE;
  }

  if (presentSupported == VK_FALSE)
  {
    ezLog::Error("The surface does not support present with the provided queue family.");
    return EZ_FAILURE;
  }

  if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pDeviceVk->GetVkPhysicalDevice(), m_VkSurface, &m_VkSurfaceCapabilities) != VK_SUCCESS)
  {
    return EZ_FAILURE;
  }

  ezUInt32 formatCount = 0;

  if (vkGetPhysicalDeviceSurfaceFormatsKHR(m_pDeviceVk->GetVkPhysicalDevice(), m_VkSurface, &formatCount, nullptr) != VK_SUCCESS)
  {
    return EZ_FAILURE;
  }

  if (formatCount != 0)
  {
    m_VkSurfaceFormats.SetCountUninitialized(formatCount);

    if (vkGetPhysicalDeviceSurfaceFormatsKHR(m_pDeviceVk->GetVkPhysicalDevice(), m_VkSurface, &formatCount, m_VkSurfaceFormats.GetData()) != VK_SUCCESS)
    {
      return EZ_FAILURE;
    }
  }

  ezUInt32 presentModeCount;
  if (vkGetPhysicalDeviceSurfacePresentModesKHR(m_pDeviceVk->GetVkPhysicalDevice(), m_VkSurface, &presentModeCount, nullptr) != VK_SUCCESS)
  {
    return EZ_FAILURE;
  }

  if (presentModeCount != 0)
  {
    m_VkPresentationModes.SetCountUninitialized(presentModeCount);
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(m_pDeviceVk->GetVkPhysicalDevice(), m_VkSurface, &presentModeCount, m_VkPresentationModes.GetData()) != VK_SUCCESS)
    {
      return EZ_FAILURE;
    }
  }

  bool swapChainAdwquate = !m_VkSurfaceFormats.IsEmpty() && !m_VkPresentationModes.IsEmpty();

  VkSurfaceFormatKHR surfaceFormat = m_VkSurfaceFormats[0];

  for (const auto& availableFormat : m_VkSurfaceFormats)
  {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
    {
      surfaceFormat = availableFormat;
      break;
    }
  }

  VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

  for (const auto& availablePresentMode : m_VkPresentationModes)
  {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
    {
      presentMode = availablePresentMode;
      break;
    }
  }

  VkExtent2D extent;
  if (m_VkSurfaceCapabilities.currentExtent.width != ezMath::MaxValue<ezUInt32>())
  {
    extent = m_VkSurfaceCapabilities.currentExtent;
  }
  else
  {
    ezUInt32 width = m_Description.m_pWindow->GetClientAreaSize().width;
    ezUInt32 height = m_Description.m_pWindow->GetClientAreaSize().height;
    extent = { width, height };

    extent.width = ezMath::Clamp(extent.width, m_VkSurfaceCapabilities.minImageExtent.width, m_VkSurfaceCapabilities.maxImageExtent.width);
    extent.height = ezMath::Clamp(extent.height, m_VkSurfaceCapabilities.minImageExtent.height, m_VkSurfaceCapabilities.maxImageExtent.height);
  }

  ezUInt32 imageCount = m_VkSurfaceCapabilities.minImageCount + 1;
  if (m_VkSurfaceCapabilities.maxImageCount > 0 && imageCount > m_VkSurfaceCapabilities.maxImageCount)
  {
    imageCount = m_VkSurfaceCapabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR swapChainCreateInfo{};
  swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapChainCreateInfo.surface = m_VkSurface;
  swapChainCreateInfo.minImageCount = imageCount;
  swapChainCreateInfo.imageFormat = surfaceFormat.format;
  swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
  swapChainCreateInfo.imageExtent = extent;
  swapChainCreateInfo.imageArrayLayers = 1;
  swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  ezDynamicArray<ezInt32> queueFamilyIndices(m_pDeviceVk->GetVkQueueFamilyIndices());
  if (queueFamilyIndices.IndexOf(presentQueueFamilyIndex) == ezInvalidIndex)
  {
    queueFamilyIndices.PushBack(presentQueueFamilyIndex);
  }

  if (queueFamilyIndices[QueueType::Graphics] != presentQueueFamilyIndex)
  {
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapChainCreateInfo.queueFamilyIndexCount = queueFamilyIndices.GetCount();
    swapChainCreateInfo.pQueueFamilyIndices = reinterpret_cast<const ezUInt32*>(queueFamilyIndices.GetData());
  }
  else
  {
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo.queueFamilyIndexCount = 0; // Optional
    swapChainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
  }

  swapChainCreateInfo.preTransform = m_VkSurfaceCapabilities.currentTransform;
  swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapChainCreateInfo.presentMode = presentMode;
  swapChainCreateInfo.clipped = VK_TRUE;
  swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(m_pDeviceVk->GetVkDevice(), &swapChainCreateInfo, nullptr, &m_VkSwapChain) != VK_SUCCESS)
  {
    ezLog::Error("Failed to create swap chain.");
    return EZ_FAILURE;
  }

  vkGetSwapchainImagesKHR(m_pDeviceVk->GetVkDevice(), m_VkSwapChain, &imageCount, nullptr);
  m_VkSwapChainImages.SetCountUninitialized(imageCount);
  vkGetSwapchainImagesKHR(m_pDeviceVk->GetVkDevice(), m_VkSwapChain, &imageCount, m_VkSwapChainImages.GetData());

  m_VkSwapChainImageFormat = surfaceFormat.format;
  m_VkSwapChainExtent = extent;

  m_VkSwapChainImageViews.SetCountUninitialized(m_VkSwapChainImages.GetCount());
  for (ezUInt32 i = 0; i < m_VkSwapChainImages.GetCount(); i++)
  {
    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = m_VkSwapChainImages[i];
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = m_VkSwapChainImageFormat;

    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_pDeviceVk->GetVkDevice(), &imageViewCreateInfo, nullptr, &m_VkSwapChainImageViews[i]) != VK_SUCCESS)
    {
      ezLog::Error("Failed to create image views.");
      return EZ_FAILURE;
    }
  }

  return EZ_SUCCESS;
}

ezResult ezGALSwapChainVk::DeInitPlatform(ezGALDevice* pDevice)
{
  if (m_pDeviceVk)
  {
    for (auto imageView : m_VkSwapChainImageViews)
    {
      vkDestroyImageView(m_pDeviceVk->GetVkDevice(), imageView, nullptr);
    }

    vkDestroySwapchainKHR(m_pDeviceVk->GetVkDevice(), m_VkSwapChain, nullptr);
    vkDestroySurfaceKHR(m_pDeviceVk->GetVkInstance(), m_VkSurface, nullptr);
  }
  return EZ_SUCCESS;
}



EZ_STATICLINK_FILE(RendererVk, RendererVk_Device_Implementation_SwapChainVk);
