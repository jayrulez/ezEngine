#include <RHIDX12/Device/DXDevice.h>
#include <RHIDX12/GPUDescriptorPool/DXGPUDescriptorPoolTyped.h>
#include <directx/d3dx12.h>

DXGPUDescriptorPoolTyped::DXGPUDescriptorPoolTyped(DXDevice& device, D3D12_DESCRIPTOR_HEAP_TYPE type)
  : m_device(device)
  , m_type(type)
  , m_offset(0)
  , m_size(0)
{
}

DXGPUDescriptorPoolRange DXGPUDescriptorPoolTyped::Allocate(ezUInt32 count)
{
  auto index = m_empty_ranges.LowerBound(count);
  if (index != ezInvalidIndex)
  {
    ezUInt32 offset = m_empty_ranges[m_empty_ranges.GetKey(index)];
    ezUInt32 size = m_empty_ranges.GetKey(index);

    // todo: update when ez can remove all entries with the given key
    while (m_empty_ranges.RemoveAndCopy(m_empty_ranges.GetKey(index)))
    {
    }
    return DXGPUDescriptorPoolRange(*this, m_device, m_heap, m_cpu_handle, m_gpu_handle, m_heap_readable, m_cpu_handle_readable, offset, size, m_device.GetDevice()->GetDescriptorHandleIncrementSize(m_type), m_type);
  }
  if (m_offset + count > m_size)
    ResizeHeap(ezMath::Max(m_offset + count, 2 * (m_size + 1)));
  m_offset += count;
  return DXGPUDescriptorPoolRange(*this, m_device, m_heap, m_cpu_handle, m_gpu_handle, m_heap_readable, m_cpu_handle_readable, m_offset - count, count, m_device.GetDevice()->GetDescriptorHandleIncrementSize(m_type), m_type);
}

void DXGPUDescriptorPoolTyped::ResizeHeap(ezUInt32 req_size)
{
  if (m_size >= req_size)
    return;

  ComPtr<ID3D12DescriptorHeap> heap;
  ComPtr<ID3D12DescriptorHeap> heap_readable;
  D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
  heap_desc.NumDescriptors = static_cast<uint32_t>(req_size);
  heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
  heap_desc.Type = m_type;
  EZ_ASSERT_ALWAYS(m_device.GetDevice()->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&heap)) == S_OK, "");

  heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  EZ_ASSERT_ALWAYS(m_device.GetDevice()->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&heap_readable)) == S_OK, "");

  if (m_size > 0)
  {
    m_device.GetDevice()->CopyDescriptorsSimple(m_size, heap_readable->GetCPUDescriptorHandleForHeapStart(), m_cpu_handle_readable, m_type);
    m_device.GetDevice()->CopyDescriptorsSimple(m_size, heap->GetCPUDescriptorHandleForHeapStart(), m_cpu_handle_readable, m_type);
  }

  m_size = heap_desc.NumDescriptors;
  m_heap = heap;
  m_heap_readable = heap_readable;
  m_cpu_handle = m_heap->GetCPUDescriptorHandleForHeapStart();
  m_gpu_handle = m_heap->GetGPUDescriptorHandleForHeapStart();
  m_cpu_handle_readable = m_heap_readable->GetCPUDescriptorHandleForHeapStart();
}

void DXGPUDescriptorPoolTyped::OnRangeDestroy(ezUInt32 offset, ezUInt32 size)
{
  m_empty_ranges.Insert(size, offset);
}

void DXGPUDescriptorPoolTyped::ResetHeap()
{
  m_offset = 0;
}

ComPtr<ID3D12DescriptorHeap> DXGPUDescriptorPoolTyped::GetHeap()
{
  return m_heap;
}
