#pragma once
#include <RHI/Instance/BaseTypes.h>
#include <RHIDX12/GPUDescriptorPool/DXGPUDescriptorPoolRange.h>
#include <RHIDX12/GPUDescriptorPool/DXGPUDescriptorPoolTyped.h>
#include <RHIDX12/Utilities/DXUtility.h>
#include <algorithm>
#include <map>
#include <memory>
#include <wrl.h>
//#include <directx/d3d12.h>
#include <DirectX-Headers/include/directx/d3d12.h>
using namespace Microsoft::WRL;

class DXDevice;

class DXGPUDescriptorPool
{
public:
  DXGPUDescriptorPool(DXDevice& device);
  DXGPUDescriptorPoolRange Allocate(D3D12_DESCRIPTOR_HEAP_TYPE descriptor_type, ezUInt32 count);
  ComPtr<ID3D12DescriptorHeap> GetHeap(D3D12_DESCRIPTOR_HEAP_TYPE descriptor_type);

private:
  DXDevice& m_device;
  DXGPUDescriptorPoolTyped m_shader_resource;
  DXGPUDescriptorPoolTyped m_shader_sampler;
};
