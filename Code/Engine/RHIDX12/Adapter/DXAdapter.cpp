#include <RHIDX12/Adapter/DXAdapter.h>
#include <RHIDX12/Device/DXDevice.h>
#include <RHIDX12/Utilities/DXUtility.h>
#include <dxgi1_6.h>
#include <directx/d3d12.h>

DXAdapter::DXAdapter(DXInstance& instance, const ComPtr<IDXGIAdapter1>& adapter)
    : m_instance(instance)
    , m_adapter(adapter)
{
    DXGI_ADAPTER_DESC desc = {};
    adapter->GetDesc(&desc);
    m_name = ezStringUtf8(desc.Description);
}

const ezString& DXAdapter::GetName() const
{
    return m_name;
}

std::shared_ptr<Device> DXAdapter::CreateDevice()
{
    return std::make_shared<DXDevice>(*this);
}

DXInstance& DXAdapter::GetInstance()
{
    return m_instance;
}

ComPtr<IDXGIAdapter1> DXAdapter::GetAdapter()
{
    return m_adapter;
}
