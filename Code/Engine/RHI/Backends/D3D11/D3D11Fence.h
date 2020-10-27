#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Threading/ThreadSignal.h>
#include <RHI/Resources/Fence.h>

class D3D11Fence : public RHIFence
{
private:
  ezThreadSignal* Signal = nullptr;

  bool Disposed = false;
  ezString Name;

public:
  virtual ezString GetName() const override { return Name; }

  virtual void SetName(const ezString& name) override
  {
    Name = name;
  }

  virtual bool IsDisposed() const override { return Disposed; }

  virtual void Dispose() override
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

  virtual bool GetSignaled() override
  {
    return Signal->WaitForSignal(ezTime::Zero()) == ezThreadSignal::WaitResult::Signaled;
  }

  /// <summary>
  /// Sets this instance to the unsignaled state.
  /// </summary>
  virtual bool Reset() override
  {
    Signal->ClearSignal();
  }

public:
  D3D11Fence(bool signaled)
  {
    ezThreadSignal sigManual(ezThreadSignal::Mode::ManualReset);
    Signal = &sigManual;

    if (signaled)
    {
      Signal->RaiseSignal();
    }
  }

  ezThreadSignal& GetResetSignal() const
  {
    return *Signal;
  }

  void Set()
  {
    Signal->RaiseSignal();
  }

  bool Wait(ezUInt64 nanosecondTimeout)
  {
    ezUInt64 timeout = ezMath::Min(ezMath::MaxValue<ezUInt64>(), nanosecondTimeout / 1000000);

    return Signal->WaitForSignal(ezTime::Nanoseconds(nanosecondTimeout)) == ezThreadSignal::WaitResult::Signaled;
  }
};
