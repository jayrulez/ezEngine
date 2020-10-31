#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Backends/D3D11/D3D11ResourceLayout.h>
#include <RHI/Descriptors/ResourceSetDescription.h>
#include <RHI/Resources/Resource.h>
#include <RHI/Resources/ResourceSet.h>


class D3D11ResourceSet : public RHIResourceSet
{
private:
  ezDynamicArray<RHIResource*> Resources;
  D3D11ResourceLayout* Layout = nullptr;

  bool Disposed = false;
  ezString Name;

public:
  virtual ezString GetName() const override;

  virtual void SetName(const ezString& name) override;
  virtual bool IsDisposed() const override;
  virtual void Dispose() override;

public:
  D3D11ResourceSet(const RHIResourceSetDescription& description);

  D3D11ResourceLayout* GetLayout() const;
};
