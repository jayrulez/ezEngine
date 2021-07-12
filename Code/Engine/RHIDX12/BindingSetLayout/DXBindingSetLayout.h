#pragma once
#include <RHI/BindingSetLayout/BindingSetLayout.h>
#include <RHIDX12/Program/DXProgram.h>
#include <directx/d3d12.h>
#include <wrl.h>
using namespace Microsoft::WRL;

class DXDevice;

struct BindingLayout
{
  D3D12_DESCRIPTOR_HEAP_TYPE heap_type;
  ezUInt32 heap_offset;
};

struct DescriptorTableDesc
{
  D3D12_DESCRIPTOR_HEAP_TYPE heap_type;
  ezUInt32 heap_offset;
  bool bindless;
  bool is_compute;
};

class DXBindingSetLayout
  : public BindingSetLayout
{
public:
  DXBindingSetLayout(DXDevice& device, const std::vector<BindKey>& descs);

  const ezMap<D3D12_DESCRIPTOR_HEAP_TYPE, ezUInt32>& GetHeapDescs() const;
  const ezMap<BindKey, BindingLayout>& GetLayout() const;
  const ezMap<uint32_t, DescriptorTableDesc>& GetDescriptorTables() const;
  const ComPtr<ID3D12RootSignature>& /* DXBindingSetLayout::*/ GetRootSignature() const;

private:
  DXDevice& m_device;
  ezMap<D3D12_DESCRIPTOR_HEAP_TYPE, ezUInt32> m_heap_descs;
  ezMap<BindKey, BindingLayout> m_layout;
  ezMap<uint32_t, DescriptorTableDesc> m_descriptor_tables;
  ComPtr<ID3D12RootSignature> m_root_signature;
};
