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

  virtual void Dispose() override;

  virtual bool GetSignaled() override;

  /// <summary>
  /// Sets this instance to the unsignaled state.
  /// </summary>
  virtual bool Reset() override;

public:
  D3D11Fence(bool signaled);

  ezThreadSignal& GetResetSignal() const;

  void Set();

  bool Wait(ezUInt64 nanosecondTimeout);
};
