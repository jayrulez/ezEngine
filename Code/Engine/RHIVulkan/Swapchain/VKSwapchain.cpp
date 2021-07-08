#include <RHIVulkanPCH.h>

#include <RHIVulkan/Adapter/VKAdapter.h>
#include <RHIVulkan/CommandQueue/VKCommandQueue.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Fence/VKTimelineSemaphore.h>
#include <RHIVulkan/Instance/VKInstance.h>
#include <RHIVulkan/Resource/VKResource.h>
#include <RHIVulkan/Swapchain/VKSwapchain.h>
#include <RHIVulkan/Utilities/VKUtility.h>

#include <Foundation/Basics/Platform/Win/IncludeWindows.h>

VKSwapchain::VKSwapchain(VKCommandQueue& command_queue, Window window, uint32_t width, uint32_t height, uint32_t frame_count, bool vsync)
  : m_command_queue(command_queue)
  , m_device(command_queue.GetDevice())
{
  VKAdapter& adapter = m_device.GetAdapter();
  VKInstance& instance = adapter.GetInstance();

  VkWin32SurfaceCreateInfoKHR surface_desc = {};
  surface_desc.hinstance = GetModuleHandle(nullptr);
  surface_desc.hwnd = reinterpret_cast<HWND>(window);
  vkCreateWin32SurfaceKHR(instance.GetInstance(), &surface_desc, nullptr, &m_surface);

  ezUInt32 formatsCount = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(adapter.GetPhysicalDevice(), m_surface, &formatsCount, nullptr);

  ezDynamicArray<VkSurfaceFormatKHR> surface_formats;
  surface_formats.SetCount(formatsCount);
  vkGetPhysicalDeviceSurfaceFormatsKHR(adapter.GetPhysicalDevice(), m_surface, &formatsCount, surface_formats.GetData());

  EZ_ASSERT_ALWAYS(!surface_formats.IsEmpty(), "");

  if (surface_formats[0].format != VkFormat::VK_FORMAT_UNDEFINED)
    m_swapchain_color_format = surface_formats[0].format;

  VkColorSpaceKHR color_space = surface_formats[0].colorSpace;

  VkSurfaceCapabilitiesKHR surface_capabilities = {};
  EZ_ASSERT_ALWAYS(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(adapter.GetPhysicalDevice(), m_surface, &surface_capabilities) == VK_SUCCESS, "");

  EZ_ASSERT_ALWAYS(surface_capabilities.currentExtent.width == width, "");
  EZ_ASSERT_ALWAYS(surface_capabilities.currentExtent.height == height, "");

  VkBool32 is_supported_surface = VK_FALSE;
  VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(adapter.GetPhysicalDevice(), command_queue.GetQueueFamilyIndex(), m_surface, &is_supported_surface);
  EZ_ASSERT_ALWAYS(is_supported_surface, "");

  ezUInt32 presentationModesCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(adapter.GetPhysicalDevice(), m_surface, &presentationModesCount, nullptr);

  ezDynamicArray<VkPresentModeKHR> modes;
  modes.SetCount(presentationModesCount);
  vkGetPhysicalDeviceSurfacePresentModesKHR(adapter.GetPhysicalDevice(), m_surface, &presentationModesCount, modes.GetData());

  VkSwapchainCreateInfoKHR swap_chain_create_info = {};
  swap_chain_create_info.sType = VkStructureType::VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swap_chain_create_info.surface = m_surface;
  swap_chain_create_info.minImageCount = frame_count;
  swap_chain_create_info.imageFormat = m_swapchain_color_format;
  swap_chain_create_info.imageColorSpace = color_space;
  swap_chain_create_info.imageExtent = surface_capabilities.currentExtent;
  swap_chain_create_info.imageArrayLayers = 1;
  swap_chain_create_info.imageUsage = VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  swap_chain_create_info.imageSharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
  swap_chain_create_info.preTransform = VkSurfaceTransformFlagBitsKHR::VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  swap_chain_create_info.compositeAlpha = VkCompositeAlphaFlagBitsKHR::VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  if (vsync)
  {
    if (modes.Contains(VkPresentModeKHR::VK_PRESENT_MODE_FIFO_RELAXED_KHR))
      swap_chain_create_info.presentMode = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_RELAXED_KHR;
    else
      swap_chain_create_info.presentMode = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
  }
  else
  {
    if (modes.Contains(VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR))
      swap_chain_create_info.presentMode = VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR;
    else
      swap_chain_create_info.presentMode = VkPresentModeKHR::VK_PRESENT_MODE_IMMEDIATE_KHR;
  }
  swap_chain_create_info.clipped = true;

  vkCreateSwapchainKHR(m_device.GetDevice(), &swap_chain_create_info, nullptr, &m_swapchain);

  ezUInt32 swapchainImagesCount = 0;
  vkGetSwapchainImagesKHR(m_device.GetDevice(), m_swapchain, &swapchainImagesCount, nullptr);

  ezDynamicArray<VkImage> images;
  images.SetCount(swapchainImagesCount);
  vkGetSwapchainImagesKHR(m_device.GetDevice(), m_swapchain, &swapchainImagesCount, images.GetData());

  m_command_list = m_device.CreateCommandList(CommandListType::kGraphics);
  for (uint32_t i = 0; i < frame_count; ++i)
  {
    std::shared_ptr<VKResource> res = std::make_shared<VKResource>(m_device);
    res->format = GetFormat();
    res->image.res = images[i];
    res->image.format = m_swapchain_color_format;
    res->image.size = VkExtent2D{1u * width, 1u * height};
    res->resource_type = ResourceType::kTexture;
    res->is_back_buffer = true;
    m_command_list->ResourceBarrier({{res, ResourceState::kUndefined, ResourceState::kPresent}});
    res->SetInitialState(ResourceState::kPresent);
    m_back_buffers.emplace_back(res);
  }
  m_command_list->Close();

  VkSemaphoreCreateInfo semaphore_create_info = {};
  semaphore_create_info.sType = VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  vkCreateSemaphore(m_device.GetDevice(), &semaphore_create_info, nullptr, &m_image_available_semaphore);
  vkCreateSemaphore(m_device.GetDevice(), &semaphore_create_info, nullptr, &m_rendering_finished_semaphore);
  m_fence = m_device.CreateFence(0);
  command_queue.ExecuteCommandLists({m_command_list});
  command_queue.Signal(m_fence, 1);
}

VKSwapchain::~VKSwapchain()
{
  m_fence->Wait(1);
}

ezRHIResourceFormat::Enum VKSwapchain::GetFormat() const
{
  return VKUtils::ToEngineFormat(m_swapchain_color_format);
}

std::shared_ptr<Resource> VKSwapchain::GetBackBuffer(uint32_t buffer)
{
  return m_back_buffers[buffer];
}

uint32_t VKSwapchain::NextImage(const std::shared_ptr<Fence>& fence, uint64_t signal_value)
{
  VkResult res = vkAcquireNextImageKHR(m_device.GetDevice(), m_swapchain, UINT64_MAX, m_image_available_semaphore, nullptr, &m_frame_index);

  decltype(auto) vk_fence = fence->As<VKTimelineSemaphore>();
  uint64_t tmp = ezMath::MaxValue<ezUInt64>();
  VkTimelineSemaphoreSubmitInfo timeline_info = {};
  timeline_info.sType = VkStructureType::VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
  timeline_info.waitSemaphoreValueCount = 1;
  timeline_info.pWaitSemaphoreValues = &tmp;
  timeline_info.signalSemaphoreValueCount = 1;
  timeline_info.pSignalSemaphoreValues = &signal_value;

  VkSubmitInfo signal_submit_info = {};
  signal_submit_info.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
  signal_submit_info.pNext = &timeline_info;
  signal_submit_info.waitSemaphoreCount = 1;
  signal_submit_info.pWaitSemaphores = &m_image_available_semaphore;
  VkPipelineStageFlags waitDstStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT;
  signal_submit_info.pWaitDstStageMask = &waitDstStageMask;
  signal_submit_info.signalSemaphoreCount = 1;
  signal_submit_info.pSignalSemaphores = &vk_fence.GetFence();
  res = vkQueueSubmit(m_command_queue.GetQueue(), 1, &signal_submit_info, {});

  return m_frame_index;
}

void VKSwapchain::Present(const std::shared_ptr<Fence>& fence, uint64_t wait_value)
{
  decltype(auto) vk_fence = fence->As<VKTimelineSemaphore>();
  uint64_t tmp = std::numeric_limits<uint64_t>::max();
  VkTimelineSemaphoreSubmitInfo timeline_info = {};
  timeline_info.sType = VkStructureType::VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
  timeline_info.waitSemaphoreValueCount = 1;
  timeline_info.pWaitSemaphoreValues = &wait_value;
  timeline_info.signalSemaphoreValueCount = 1;
  timeline_info.pSignalSemaphoreValues = &tmp;

  VkSubmitInfo signal_submit_info = {};
  signal_submit_info.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
  signal_submit_info.pNext = &timeline_info;
  signal_submit_info.waitSemaphoreCount = 1;
  signal_submit_info.pWaitSemaphores = &vk_fence.GetFence();
  VkPipelineStageFlags waitDstStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT;
  signal_submit_info.pWaitDstStageMask = &waitDstStageMask;
  signal_submit_info.signalSemaphoreCount = 1;
  signal_submit_info.pSignalSemaphores = &m_rendering_finished_semaphore;
  VkResult res = vkQueueSubmit(m_command_queue.GetQueue() , 1, &signal_submit_info, {});

  VkPresentInfoKHR present_info = {};
  present_info.sType = VkStructureType::VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.swapchainCount = 1;
  present_info.pSwapchains = &m_swapchain;
  present_info.pImageIndices = &m_frame_index;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = &m_rendering_finished_semaphore;
  res = vkQueuePresentKHR(m_command_queue.GetQueue(), &present_info);
}
