#pragma once
#include <RHI/Swapchain/Swapchain.h>
#include <RHIVulkan/Resource/VKResource.h>
#include <memory>
#include <vector>
#include <RHIVulkan/RHIVulkanDLL.h>

class VKDevice;
class VKCommandQueue;
class CommandList;
class Fence;

class VKSwapchain
  : public Swapchain
{
public:
  VKSwapchain(VKCommandQueue& command_queue, Window window, uint32_t width, uint32_t height, uint32_t frame_count, bool vsync);
  ~VKSwapchain();
  ezRHIResourceFormat::Enum GetFormat() const override;
  std::shared_ptr<Resource> GetBackBuffer(uint32_t buffer) override;
  uint32_t NextImage(const std::shared_ptr<Fence>& fence, uint64_t signal_value) override;
  void Present(const std::shared_ptr<Fence>& fence, uint64_t wait_value) override;

private:
  VKCommandQueue& m_command_queue;
  VKDevice& m_device;
  VkSurfaceKHR m_surface;
  VkFormat m_swapchain_color_format = VkFormat::VK_FORMAT_B8G8R8_UNORM;
  VkSwapchainKHR m_swapchain;
  std::vector<std::shared_ptr<Resource>> m_back_buffers;
  uint32_t m_frame_index = 0;
  VkSemaphore m_image_available_semaphore;
  VkSemaphore m_rendering_finished_semaphore;
  std::shared_ptr<CommandList> m_command_list;
  std::shared_ptr<Fence> m_fence;
};
