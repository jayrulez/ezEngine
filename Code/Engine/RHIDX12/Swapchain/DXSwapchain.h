#pragma once
#include <RHI/Swapchain/Swapchain.h>
#include <RHI/Resource/Resource.h>
#include <cstdint>
#include <vector>
#include <dxgi1_4.h>
#include <directx/d3d12.h>
#include <wrl.h>
using namespace Microsoft::WRL;

class DXCommandQueue;

class DXSwapchain
    : public Swapchain
{
public:
    DXSwapchain(DXCommandQueue& command_queue, Window window, uint32_t width, uint32_t height, uint32_t frame_count, bool vsync);
    ezRHIResourceFormat::Enum GetFormat() const override;
    ezSharedPtr<Resource> GetBackBuffer(uint32_t buffer) override;
    uint32_t NextImage(const ezSharedPtr<Fence>& fence, uint64_t signal_value) override;
    void Present(const ezSharedPtr<Fence>& fence, uint64_t wait_value) override;

private:
    DXCommandQueue& m_command_queue;
    bool m_vsync;
    ComPtr<IDXGISwapChain3> m_swap_chain;
    std::vector<ezSharedPtr<Resource>> m_back_buffers;
};
