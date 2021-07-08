#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHIVulkan/VKBasics.h>

#include <RHI/SwapChain/SwapChain.h>

class ezRHIVKSwapchain
  : public ezRHISwapChain
{
public:
  ezRHIVKSwapchain(ezRHIVKCommandQueue& commandQueue, const ezRHISwapChainCreationDescription& desc);
  ~ezRHIVKSwapchain();
  ezRHIResourceFormat::Enum GetFormat() const override;
  ezRHIResource* GetBackBuffer(ezUInt32 buffer) override;
  ezUInt32 NextImage(const ezRHIFence* fence, ezUInt64 signalValue) override;
  void Present(const ezRHIFence* fence, ezUInt64 waitValue) override;

private:
  ezRHIVKCommandQueue& m_CommandQueue;
  ezRHIVKDevice& m_Device;
  vk::UniqueSurfaceKHR m_Surface;
  vk::Format m_SwapChainColorFormat = vk::Format::eB8G8R8Unorm;
  vk::UniqueSwapchainKHR m_SwapChain;
  ezDynamicArray<ezRHIResource*> m_BackBuffers;
  ezUInt32 m_FrameIndex = 0;
  vk::UniqueSemaphore m_ImageAvailableSemaphore;
  vk::UniqueSemaphore m_RenderingFinishedSemaphore;
  ezRHICommandList* m_CommandList;
  ezRHIFence* m_Fence;
};
