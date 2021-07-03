#include <RHIDX12/Device/DXDevice.h>
#include <RHIDX12/Fence/DXFence.h>
#include <RHIDX12/Utilities/DXUtility.h>
#include <dxgi1_6.h>
#include <directx/d3d12.h>

DXFence::DXFence(DXDevice& device, uint64_t initial_value)
  : m_device(device)
{
  EZ_ASSERT_ALWAYS(device.GetDevice()->CreateFence(initial_value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)) == S_OK, "");
  m_fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

uint64_t DXFence::GetCompletedValue()
{
  return m_fence->GetCompletedValue();
}

void DXFence::Wait(uint64_t value)
{
  if (GetCompletedValue() < value)
  {
    EZ_ASSERT_ALWAYS(m_fence->SetEventOnCompletion(value, m_fence_event) == S_OK, "");
    WaitForSingleObjectEx(m_fence_event, INFINITE, FALSE);
  }
}

void DXFence::Signal(uint64_t value)
{
  m_fence->Signal(value);
}

ComPtr<ID3D12Fence> DXFence::GetFence()
{
  return m_fence;
}
