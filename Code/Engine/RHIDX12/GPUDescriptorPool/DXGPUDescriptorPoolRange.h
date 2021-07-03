#pragma once
#include <RHI/Instance/BaseTypes.h>
#include <RHIDX12/Utilities/DXUtility.h>
#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <wrl.h>
#include <directx/d3d12.h>
using namespace Microsoft::WRL;

class DXDevice;
class DXGPUDescriptorPoolTyped;

class DXGPUDescriptorPoolRange
{
public:
  using Ptr = std::shared_ptr<DXGPUDescriptorPoolRange>;
  DXGPUDescriptorPoolRange(
    DXGPUDescriptorPoolTyped& pool,
    DXDevice& device,
    ComPtr<ID3D12DescriptorHeap>& heap,
    D3D12_CPU_DESCRIPTOR_HANDLE& cpu_handle,
    D3D12_GPU_DESCRIPTOR_HANDLE& gpu_handle,
    ComPtr<ID3D12DescriptorHeap>& heap_readable,
    D3D12_CPU_DESCRIPTOR_HANDLE& cpu_handle_readable,
    ezUInt32 offset,
    ezUInt32 size,
    uint32_t increment_size,
    D3D12_DESCRIPTOR_HEAP_TYPE type);
  DXGPUDescriptorPoolRange(DXGPUDescriptorPoolRange&& oth);
  ~DXGPUDescriptorPoolRange();
  void CopyCpuHandle(ezUInt32 dst_offset, D3D12_CPU_DESCRIPTOR_HANDLE handle);
  D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(ezUInt32 offset = 0) const;

  const ComPtr<ID3D12DescriptorHeap>& GetHeap() const;

  ezUInt32 GetSize() const;
  ezUInt32 GetOffset() const;

private:
  D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle, ezUInt32 offset = 0) const;

  std::reference_wrapper<DXGPUDescriptorPoolTyped> m_pool;
  std::reference_wrapper<DXDevice> m_device;
  std::reference_wrapper<ComPtr<ID3D12DescriptorHeap>> m_heap;
  std::reference_wrapper<D3D12_CPU_DESCRIPTOR_HANDLE> m_cpu_handle;
  std::reference_wrapper<D3D12_GPU_DESCRIPTOR_HANDLE> m_gpu_handle;
  std::reference_wrapper<ComPtr<ID3D12DescriptorHeap>> m_heap_readable;
  std::reference_wrapper<D3D12_CPU_DESCRIPTOR_HANDLE> m_cpu_handle_readable;
  ezUInt32 m_offset;
  ezUInt32 m_size;
  uint32_t m_increment_size;
  D3D12_DESCRIPTOR_HEAP_TYPE m_type;
  std::unique_ptr<DXGPUDescriptorPoolRange, std::function<void(DXGPUDescriptorPoolRange*)>> m_callback;
};
