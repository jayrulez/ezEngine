#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/CommandList.h>

class D3D11CommandList : public RHICommandList
{
private:
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
      Disposed = true;
    }
  }

  D3D11CommandList(D3D11GraphicsDevice* graphicsDevice, const RHICommandListDescription& description)
    : RHICommandList(description, graphicsDevice->GetFeatures(), graphicsDevice->GetUniformBufferMinOffsetAlignment(), graphicsDevice->GetStructuredBufferMinOffsetAlignment())
  {
  }
};
