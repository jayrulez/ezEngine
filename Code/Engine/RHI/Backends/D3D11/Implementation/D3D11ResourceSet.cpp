#include <RHI/Backends/D3D11/D3D11ResourceSet.h>

ezString D3D11ResourceSet::GetName() const
{
  return Name;
}

void D3D11ResourceSet::SetName(const ezString& name)
{
  Name = name;
}

bool D3D11ResourceSet::IsDisposed() const
{
  return Disposed;
}

void D3D11ResourceSet::Dispose()
{
  if (!Disposed)
  {
    Disposed = true;
  }
}

D3D11ResourceSet::D3D11ResourceSet(const RHIResourceSetDescription& description)
  : RHIResourceSet(description)
{
  Resources.SetCountUninitialized(description.BoundResources.GetCount());
  Resources.GetArrayPtr().CopyFrom(description.BoundResources.GetArrayPtr());

  Layout = reinterpret_cast<D3D11ResourceLayout*>(description.Layout);
}

D3D11ResourceLayout* D3D11ResourceSet::GetLayout() const
{
  return Layout;
}
