#pragma once
#include <RHI/View/View.h>
#include <RHIDX12/Resource/DXResource.h>
#include <RHIDX12/CPUDescriptorPool/DXCPUDescriptorHandle.h>
#include <RHIDX12/GPUDescriptorPool/DXGPUDescriptorPoolRange.h>

class DXDevice;
class DXResource;

class DXView : public View
{
public:
    DXView(DXDevice& device, const ezSharedPtr<DXResource>& resource, const ViewDesc& view_desc);
    ezSharedPtr<Resource> GetResource() override;
    uint32_t GetDescriptorId() const override;
    uint32_t GetBaseMipLevel() const override;
    uint32_t GetLevelCount() const override;
    uint32_t GetBaseArrayLayer() const override;
    uint32_t GetLayerCount() const override;

    D3D12_CPU_DESCRIPTOR_HANDLE GetHandle();

private:
    void CreateView();
    void CreateSRV();
    void CreateRAS();
    void CreateUAV();
    void CreateRTV();
    void CreateDSV();
    void CreateCBV();
    void CreateSampler();

    DXDevice& m_device;
    ezSharedPtr<DXResource> m_resource;
    ViewDesc m_view_desc;
    ezSharedPtr<DXCPUDescriptorHandle> m_handle;
    ezSharedPtr<DXGPUDescriptorPoolRange> m_range;
};
