#include <RHIVulkanPCH.h>

#include <RHIVulkan/SwapChain/VKSwapChain.h>
#include <RHIVulkan/CommandQueue/VKCommandQueue.h>
#include <RHIVulkan/Fence/VKTimelineSemaphore.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Adapter/VKAdapter.h>
#include <RHIVulkan/Instance/VKInstance.h>
#include <RHIVulkan/Resource/VKResource.h>
#include <RHIVulkan/CommandList/VKCommandList.h>
#include <Core/System/Window.h>

ezRHIVKSwapchain::ezRHIVKSwapchain(ezRHIVKCommandQueue& commandQueue, const ezRHISwapChainCreationDescription& desc)
  : m_CommandQueue{commandQueue}
  , m_Device{commandQueue.GetDevice()}
{
  ezUInt32 width = desc.m_Window->GetClientAreaSize().width;
  ezUInt32 height = desc.m_Window->GetClientAreaSize().height;

  ezRHIVKAdapter& adapter = m_Device.GetAdapter();
  ezRHIVKInstance& instance = adapter.GetInstance();

  vk::Win32SurfaceCreateInfoKHR surfaceDesc = {};
  surfaceDesc.hinstance = GetModuleHandle(nullptr);
  surfaceDesc.hwnd = reinterpret_cast<HWND>(desc.m_Window->GetNativeWindowHandle());
  m_Surface = instance.GetInstance().createWin32SurfaceKHRUnique(surfaceDesc);

  auto surfaceFormats = adapter.GetPhysicalDevice().getSurfaceFormatsKHR(m_Surface.get());
  EZ_ASSERT_ALWAYS(!surfaceFormats.empty(), "");

  if (surfaceFormats.front().format != vk::Format::eUndefined)
    m_SwapChainColorFormat = surfaceFormats.front().format;

  vk::ColorSpaceKHR colorSpace = surfaceFormats.front().colorSpace;

  vk::SurfaceCapabilitiesKHR surfaceCapabilities = {};
  EZ_ASSERT_ALWAYS(adapter.GetPhysicalDevice().getSurfaceCapabilitiesKHR(m_Surface.get(), &surfaceCapabilities) == vk::Result::eSuccess, "");

  EZ_ASSERT_ALWAYS(surfaceCapabilities.currentExtent.width == width, "");
  EZ_ASSERT_ALWAYS(surfaceCapabilities.currentExtent.height == height, "");

  vk::Bool32 isSupportedSurface = VK_FALSE;
  vk::Result result = adapter.GetPhysicalDevice().getSurfaceSupportKHR(commandQueue.GetQueueFamilyIndex(), m_Surface.get(), &isSupportedSurface);
  EZ_ASSERT_ALWAYS(isSupportedSurface, "");

  auto modes = adapter.GetPhysicalDevice().getSurfacePresentModesKHR(m_Surface.get());

  vk::SwapchainCreateInfoKHR swapChainCreateInfo = {};
  swapChainCreateInfo.surface = m_Surface.get();
  swapChainCreateInfo.minImageCount = desc.m_FrameCount;
  swapChainCreateInfo.imageFormat = m_SwapChainColorFormat;
  swapChainCreateInfo.imageColorSpace = colorSpace;
  swapChainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;
  swapChainCreateInfo.imageArrayLayers = 1;
  swapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;
  swapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
  swapChainCreateInfo.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
  swapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
  if (desc.m_Vsync)
  {
    if (std::count(modes.begin(), modes.end(), vk::PresentModeKHR::eFifoRelaxed))
      swapChainCreateInfo.presentMode = vk::PresentModeKHR::eFifoRelaxed;
    else
      swapChainCreateInfo.presentMode = vk::PresentModeKHR::eFifo;
  }
  else
  {
    if (std::count(modes.begin(), modes.end(), vk::PresentModeKHR::eMailbox))
      swapChainCreateInfo.presentMode = vk::PresentModeKHR::eMailbox;
    else
      swapChainCreateInfo.presentMode = vk::PresentModeKHR::eImmediate;
  }
  swapChainCreateInfo.clipped = true;

  m_SwapChain = m_Device.GetDevice().createSwapchainKHRUnique(swapChainCreateInfo);

  std::vector<vk::Image> images = m_Device.GetDevice().getSwapchainImagesKHR(m_SwapChain.get());

  m_CommandList = m_Device.CreateCommandList(ezRHICommandListType::Graphics);
  for (ezUInt32 i = 0; i < desc.m_FrameCount; ++i)
  {
    ezRHIVKResource* res = EZ_DEFAULT_NEW(ezRHIVKResource, m_Device);
    res->m_Format = GetFormat();
    res->m_Image.m_Image = images[i];
    res->m_Image.m_Format = m_SwapChainColorFormat;
    res->m_Image.m_Size = vk::Extent2D(1u * width, 1u * height);
    res->m_ResourceType = ezRHIResourceType::Texture;
    res->m_IsBackBuffer = true;
    ezDynamicArray<ezRHIResourceBarrierDescription> barriers;
    barriers.PushBack({res, ezRHIResourceState::kUndefined, ezRHIResourceState::Present});
    m_CommandList->ResourceBarrier(barriers);
    res->SetInitialState(ezRHIResourceState::Present);
    m_BackBuffers.PushBack(res);
  }
  m_CommandList->Close();

  vk::SemaphoreCreateInfo semaphoreCreateInfo = {};
  m_ImageAvailableSemaphore = m_Device.GetDevice().createSemaphoreUnique(semaphoreCreateInfo);
  m_RenderingFinishedSemaphore = m_Device.GetDevice().createSemaphoreUnique(semaphoreCreateInfo);
  m_Fence = m_Device.CreateFence(0);
  ezDynamicArray<ezRHICommandList*> commandLists;
  commandLists.PushBack(m_CommandList);
  commandQueue.ExecuteCommandLists(commandLists);
  commandQueue.Signal(m_Fence, 1);
}

ezRHIVKSwapchain::~ezRHIVKSwapchain()
{
  m_Fence->Wait(1);
  EZ_DEFAULT_DELETE(m_Fence);  // todo: use device destroy
  EZ_DEFAULT_DELETE(m_CommandList); // todo: use device destroy
}

ezRHIResourceFormat::Enum ezRHIVKSwapchain::GetFormat() const
{
  return VKUtilities::ToEngineFormat(m_SwapChainColorFormat);
}

ezRHIResource* ezRHIVKSwapchain::GetBackBuffer(ezUInt32 buffer)
{
  return m_BackBuffers[buffer];
}

ezUInt32 ezRHIVKSwapchain::NextImage(const ezRHIFence* fence, ezUInt64 signalValue)
{
  vk::Result result = m_Device.GetDevice().acquireNextImageKHR(m_SwapChain.get(), UINT64_MAX, m_ImageAvailableSemaphore.get(), nullptr, &m_FrameIndex);

  decltype(auto) vkFence = fence->As<ezRHIVKTimelineSemaphore>();
  ezUInt64 tmp = ezMath::MaxValue<ezUInt64>();
  vk::TimelineSemaphoreSubmitInfo timelineInfo = {};
  timelineInfo.waitSemaphoreValueCount = 1;
  timelineInfo.pWaitSemaphoreValues = &tmp;
  timelineInfo.signalSemaphoreValueCount = 1;
  timelineInfo.pSignalSemaphoreValues = &signalValue;

  vk::SubmitInfo signalSubmitInfo = {};
  signalSubmitInfo.pNext = &timelineInfo;
  signalSubmitInfo.waitSemaphoreCount = 1;
  signalSubmitInfo.pWaitSemaphores = &m_ImageAvailableSemaphore.get();

  vk::PipelineStageFlags waitDstStageMask = vk::PipelineStageFlagBits::eTransfer;
  signalSubmitInfo.pWaitDstStageMask = &waitDstStageMask;
  signalSubmitInfo.signalSemaphoreCount = 1;
  signalSubmitInfo.pSignalSemaphores = &vkFence->GetFence();
  result = m_CommandQueue.GetQueue().submit(1, &signalSubmitInfo, {});

  return m_FrameIndex;
}

void ezRHIVKSwapchain::Present(const ezRHIFence* fence, ezUInt64 waitValue)
{
  decltype(auto) vkFence = fence->As<ezRHIVKTimelineSemaphore>();
  ezUInt64 tmp = ezMath::MaxValue<ezUInt64>();

  vk::TimelineSemaphoreSubmitInfo timelineInfo = {};
  timelineInfo.waitSemaphoreValueCount = 1;
  timelineInfo.pWaitSemaphoreValues = &waitValue;
  timelineInfo.signalSemaphoreValueCount = 1;
  timelineInfo.pSignalSemaphoreValues = &tmp;

  vk::SubmitInfo signalSubmitInfo = {};
  signalSubmitInfo.pNext = &timelineInfo;
  signalSubmitInfo.waitSemaphoreCount = 1;
  signalSubmitInfo.pWaitSemaphores = &vkFence->GetFence();

  vk::PipelineStageFlags waitDstStageMask = vk::PipelineStageFlagBits::eTransfer;
  signalSubmitInfo.pWaitDstStageMask = &waitDstStageMask;
  signalSubmitInfo.signalSemaphoreCount = 1;
  signalSubmitInfo.pSignalSemaphores = &m_RenderingFinishedSemaphore.get();
  vk::Result result = m_CommandQueue.GetQueue().submit(1, &signalSubmitInfo, {});

  vk::PresentInfoKHR present_info = {};
  present_info.swapchainCount = 1;
  present_info.pSwapchains = &m_SwapChain.get();
  present_info.pImageIndices = &m_FrameIndex;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = &m_RenderingFinishedSemaphore.get();
  result = m_CommandQueue.GetQueue().presentKHR(present_info);
}
