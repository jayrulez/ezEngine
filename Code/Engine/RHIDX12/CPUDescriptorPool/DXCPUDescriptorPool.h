#pragma once
#include <RHIDX12/CPUDescriptorPool/DXCPUDescriptorHandle.h>
#include <RHIDX12/CPUDescriptorPool/DXCPUDescriptorPoolTyped.h>
#include <RHI/Instance/BaseTypes.h>
#include <RHIDX12/Utilities/DXUtility.h>
#include <algorithm>
#include <map>
#include <memory>
#include <wrl.h>
//#include <directx/d3d12.h>
#include <DirectX-Headers/include/directx/d3d12.h>
using namespace Microsoft::WRL;

class DXDevice;

class DXCPUDescriptorPool
{
public:
    DXCPUDescriptorPool(DXDevice& device);
    std::shared_ptr<DXCPUDescriptorHandle> AllocateDescriptor(ViewType view_type);

private:
    DXCPUDescriptorPoolTyped& SelectHeap(ViewType view_type);

    DXDevice& m_device;
    DXCPUDescriptorPoolTyped m_resource;
    DXCPUDescriptorPoolTyped m_sampler;
    DXCPUDescriptorPoolTyped m_rtv;
    DXCPUDescriptorPoolTyped m_dsv;
};
