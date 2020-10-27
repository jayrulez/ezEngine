#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Backends/D3D11/D3D11ResourceLayout.h>

#include <RHI/Resources/ResourceSet.h>

class D3D11ResourceSet : public RHIResourceSet
{
private:
  ezDynamicArray<RHIDeviceResource*> Resources;
  D3D11ResourceLayout* Layout = nullptr;

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

public:
  D3D11ResourceSet(const RHIResourceSetDescription& description)
    : RHIResourceSet(description)
  {
    Resources.SetCountUninitialized(description.BoundResources.GetCount());
    Resources.GetArrayPtr().CopyFrom(description.BoundResources.GetArrayPtr());

    Layout = reinterpret_cast<D3D11ResourceLayout*>(description.Layout);
  }
};
