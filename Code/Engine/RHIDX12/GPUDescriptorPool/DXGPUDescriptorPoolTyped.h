#pragma once
#include <RHIDX12/GPUDescriptorPool/DXGPUDescriptorPoolRange.h>
#include <RHI/Instance/BaseTypes.h>
#include <RHIDX12/Utilities/DXUtility.h>
#include <algorithm>
#include <map>
#include <memory>
#include <wrl.h>
//#include <directx/d3d12.h>
#include <DirectX-Headers/include/directx/d3d12.h>
using namespace Microsoft::WRL;

class DXDevice;

class DXGPUDescriptorPoolTyped
{
public:
    DXGPUDescriptorPoolTyped(DXDevice& device, D3D12_DESCRIPTOR_HEAP_TYPE type);
  DXGPUDescriptorPoolRange Allocate(ezUInt32 count);
    void ResizeHeap(ezUInt32 req_size);
  void OnRangeDestroy(ezUInt32 offset, ezUInt32 size);
    void ResetHeap();
    ComPtr<ID3D12DescriptorHeap> GetHeap();

private:
    DXDevice& m_device;
    D3D12_DESCRIPTOR_HEAP_TYPE m_type;
    ezUInt32 m_offset;
    ezUInt32 m_size;
    ComPtr<ID3D12DescriptorHeap> m_heap;
    D3D12_CPU_DESCRIPTOR_HANDLE m_cpu_handle;
    D3D12_GPU_DESCRIPTOR_HANDLE m_gpu_handle;
    ComPtr<ID3D12DescriptorHeap> m_heap_readable;
    D3D12_CPU_DESCRIPTOR_HANDLE m_cpu_handle_readable;
    std::multimap<ezUInt32, ezUInt32> m_empty_ranges;
};
