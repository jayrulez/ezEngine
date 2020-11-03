#include <RHI/Backends/D3D11/D3D11Fence.h>

void D3D11Fence::Dispose()
{
  if (!Disposed)
  {
    if (Signal != nullptr)
    {
      delete Signal;
      Signal = nullptr;
    }
    Disposed = true;
  }
}

bool D3D11Fence::GetSignaled()
{
  return Signal->WaitForSignal(ezTime::Zero()) == ezThreadSignal::WaitResult::Signaled;
}


/// <summary>
/// Sets this instance to the unsignaled state.
/// </summary>

bool D3D11Fence::Reset()
{
  Signal->ClearSignal();
  return true;
}

D3D11Fence::D3D11Fence(bool signaled)
{
  ezThreadSignal sigManual(ezThreadSignal::Mode::ManualReset);
  Signal = &sigManual;

  if (signaled)
  {
    Signal->RaiseSignal();
  }
}

ezThreadSignal* D3D11Fence::GetResetSignal() const
{
  return Signal;
}

void D3D11Fence::Set()
{
  Signal->RaiseSignal();
}

bool D3D11Fence::Wait(ezUInt64 nanosecondTimeout)
{
  return Signal->WaitForSignal(ezTime::Nanoseconds((double)nanosecondTimeout)) == ezThreadSignal::WaitResult::Signaled;
}


EZ_STATICLINK_FILE(RHI, RHI_Backends_D3D11_Implementation_D3D11Fence);

