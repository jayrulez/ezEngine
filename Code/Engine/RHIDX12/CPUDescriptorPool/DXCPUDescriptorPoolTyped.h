#pragma once
#include <RHIDX12/CPUDescriptorPool/DXCPUDescriptorHandle.h>
#include <RHI/Instance/BaseTypes.h>
#include <RHIDX12/Utilities/DXUtility.h>
#include <algorithm>
#include <memory>
#include <wrl.h>
#include <directx/d3dx12.h>
using namespace Microsoft::WRL;

class DXDevice;

class DXCPUDescriptorPoolTyped
{
public:
    DXCPUDescriptorPoolTyped(DXDevice& device, D3D12_DESCRIPTOR_HEAP_TYPE type);
    std::shared_ptr<DXCPUDescriptorHandle> Allocate(size_t count);
    void ResizeHeap(size_t req_size);

private:
    DXDevice& m_device;
    D3D12_DESCRIPTOR_HEAP_TYPE m_type;
    size_t m_offset;
    size_t m_size;
    ComPtr<ID3D12DescriptorHeap> m_heap;
    D3D12_CPU_DESCRIPTOR_HANDLE m_cpu_handle;
};
