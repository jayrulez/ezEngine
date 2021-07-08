#include <RHIVulkanPCH.h>

#include <RHIVulkan/Swapchain/VKSwapchain.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/CommandQueue/VKCommandQueue.h>
#include <RHIVulkan/Adapter/VKAdapter.h>
#include <RHIVulkan/Instance/VKInstance.h>
#include <RHIVulkan/Fence/VKTimelineSemaphore.h>
#include <RHIVulkan/Utilities/VKUtility.h>
#include <RHIVulkan/Resource/VKResource.h>

#include <Foundation/Basics/Platform/Win/IncludeWindows.h>

VKSwapchain::VKSwapchain(VKCommandQueue& command_queue, Window window, uint32_t width, uint32_t height, uint32_t frame_count, bool vsync)
    : m_command_queue(command_queue)
    , m_device(command_queue.GetDevice())
{
    VKAdapter& adapter = m_device.GetAdapter();
    VKInstance& instance = adapter.GetInstance();

    vk::Win32SurfaceCreateInfoKHR surface_desc = {};
    surface_desc.hinstance = GetModuleHandle(nullptr);
    surface_desc.hwnd = reinterpret_cast<HWND>(window);
    m_surface = instance.GetInstance().createWin32SurfaceKHRUnique(surface_desc);

    auto surface_formats = adapter.GetPhysicalDevice().getSurfaceFormatsKHR(m_surface.get());
    EZ_ASSERT_ALWAYS(!surface_formats.empty(), "");

    if (surface_formats.front().format != vk::Format::eUndefined)
        m_swapchain_color_format = surface_formats.front().format;

    vk::ColorSpaceKHR color_space = surface_formats.front().colorSpace;

    vk::SurfaceCapabilitiesKHR surface_capabilities = {};
    EZ_ASSERT_ALWAYS(adapter.GetPhysicalDevice().getSurfaceCapabilitiesKHR(m_surface.get(), &surface_capabilities) == vk::Result::eSuccess, "");

    EZ_ASSERT_ALWAYS(surface_capabilities.currentExtent.width == width, "");
    EZ_ASSERT_ALWAYS(surface_capabilities.currentExtent.height == height, "");

    vk::Bool32 is_supported_surface = VK_FALSE;
    vk::Result res = adapter.GetPhysicalDevice().getSurfaceSupportKHR(command_queue.GetQueueFamilyIndex(), m_surface.get(), &is_supported_surface);
    EZ_ASSERT_ALWAYS(is_supported_surface, "");

    auto modes = adapter.GetPhysicalDevice().getSurfacePresentModesKHR(m_surface.get());

    vk::SwapchainCreateInfoKHR swap_chain_create_info = {};
    swap_chain_create_info.surface = m_surface.get();
    swap_chain_create_info.minImageCount = frame_count;
    swap_chain_create_info.imageFormat = m_swapchain_color_format;
    swap_chain_create_info.imageColorSpace = color_space;
    swap_chain_create_info.imageExtent = surface_capabilities.currentExtent;
    swap_chain_create_info.imageArrayLayers = 1;
    swap_chain_create_info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;
    swap_chain_create_info.imageSharingMode = vk::SharingMode::eExclusive;
    swap_chain_create_info.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
    swap_chain_create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    if (vsync)
    {
        if (std::count(modes.begin(), modes.end(), vk::PresentModeKHR::eFifoRelaxed))
            swap_chain_create_info.presentMode = vk::PresentModeKHR::eFifoRelaxed;
        else
            swap_chain_create_info.presentMode = vk::PresentModeKHR::eFifo;
    }
    else
    {
        if (std::count(modes.begin(), modes.end(), vk::PresentModeKHR::eMailbox))
            swap_chain_create_info.presentMode = vk::PresentModeKHR::eMailbox;
        else
            swap_chain_create_info.presentMode = vk::PresentModeKHR::eImmediate;
    }
    swap_chain_create_info.clipped = true;

    m_swapchain = m_device.GetDevice().createSwapchainKHRUnique(swap_chain_create_info);

    std::vector<vk::Image> m_images = m_device.GetDevice().getSwapchainImagesKHR(m_swapchain.get());

    m_command_list = m_device.CreateCommandList(CommandListType::kGraphics);
    for (uint32_t i = 0; i < frame_count; ++i)
    {
        std::shared_ptr<VKResource> res = std::make_shared<VKResource>(m_device);
        res->format = GetFormat();
        res->image.res = m_images[i];
        res->image.format = m_swapchain_color_format;
        res->image.size = vk::Extent2D(1u * width, 1u * height);
        res->resource_type = ResourceType::kTexture;
        res->is_back_buffer = true;
        m_command_list->ResourceBarrier({ { res, ResourceState::kUndefined, ResourceState::kPresent } });
        res->SetInitialState(ResourceState::kPresent);
        m_back_buffers.emplace_back(res);
    }
    m_command_list->Close();

    vk::SemaphoreCreateInfo semaphore_create_info = {};
    m_image_available_semaphore = m_device.GetDevice().createSemaphoreUnique(semaphore_create_info);
    m_rendering_finished_semaphore = m_device.GetDevice().createSemaphoreUnique(semaphore_create_info);
    m_fence = m_device.CreateFence(0);
    command_queue.ExecuteCommandLists({ m_command_list });
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
    vk::Result res = m_device.GetDevice().acquireNextImageKHR(m_swapchain.get(), UINT64_MAX, m_image_available_semaphore.get(), nullptr, &m_frame_index);

    decltype(auto) vk_fence = fence->As<VKTimelineSemaphore>();
    uint64_t tmp = ezMath::MaxValue<ezUInt64>();
    vk::TimelineSemaphoreSubmitInfo timeline_info = {};
    timeline_info.waitSemaphoreValueCount = 1;
    timeline_info.pWaitSemaphoreValues = &tmp;
    timeline_info.signalSemaphoreValueCount = 1;
    timeline_info.pSignalSemaphoreValues = &signal_value;
    vk::SubmitInfo signal_submit_info = {};
    signal_submit_info.pNext = &timeline_info;
    signal_submit_info.waitSemaphoreCount = 1;
    signal_submit_info.pWaitSemaphores = &m_image_available_semaphore.get();
    vk::PipelineStageFlags waitDstStageMask = vk::PipelineStageFlagBits::eTransfer;
    signal_submit_info.pWaitDstStageMask = &waitDstStageMask;
    signal_submit_info.signalSemaphoreCount = 1;
    signal_submit_info.pSignalSemaphores = &vk_fence.GetFence();
    res = m_command_queue.GetQueue().submit(1, &signal_submit_info, {});

    return m_frame_index;
}

void VKSwapchain::Present(const std::shared_ptr<Fence>& fence, uint64_t wait_value)
{
    decltype(auto) vk_fence = fence->As<VKTimelineSemaphore>();
    uint64_t tmp = std::numeric_limits<uint64_t>::max();
    vk::TimelineSemaphoreSubmitInfo timeline_info = {};
    timeline_info.waitSemaphoreValueCount = 1;
    timeline_info.pWaitSemaphoreValues = &wait_value;
    timeline_info.signalSemaphoreValueCount = 1;
    timeline_info.pSignalSemaphoreValues = &tmp;
    vk::SubmitInfo signal_submit_info = {};
    signal_submit_info.pNext = &timeline_info;
    signal_submit_info.waitSemaphoreCount = 1;
    signal_submit_info.pWaitSemaphores = &vk_fence.GetFence();
    vk::PipelineStageFlags waitDstStageMask = vk::PipelineStageFlagBits::eTransfer;
    signal_submit_info.pWaitDstStageMask = &waitDstStageMask;
    signal_submit_info.signalSemaphoreCount = 1;
    signal_submit_info.pSignalSemaphores = &m_rendering_finished_semaphore.get();
    vk::Result res = m_command_queue.GetQueue().submit(1, &signal_submit_info, {});

    vk::PresentInfoKHR present_info = {};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &m_swapchain.get();
    present_info.pImageIndices = &m_frame_index;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &m_rendering_finished_semaphore.get();
    res = m_command_queue.GetQueue().presentKHR(present_info);
}