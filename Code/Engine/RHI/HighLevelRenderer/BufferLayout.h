#pragma once
#include <RHI/RHIDLL.h>
#include <RHI/HighLevelRenderer/RenderDevice.h>

struct BufferLayout
{
    size_t dst_buffer_size;
    std::vector<size_t> data_size;
    std::vector<size_t> src_offset;
    std::vector<size_t> dst_offset;
};

class EZ_RHI_DLL ViewProvider : public DeferredView
{
public:
    ViewProvider(RenderDevice& device, const uint8_t* src_data, BufferLayout& layout);
    ezSharedPtr<ResourceLazyViewDesc> GetView(RenderCommandList& command_list) override;
    void OnDestroy(ResourceLazyViewDesc& view_desc) override;

private:
    bool SyncData();

    RenderDevice& m_device;
    const uint8_t* m_src_data;
    BufferLayout& m_layout;
    std::vector<uint8_t> m_dst_data;
    ezSharedPtr<ResourceLazyViewDesc> m_last_view;
    std::vector<ezSharedPtr<Resource>> m_free_resources;
};

template<typename T>
class ConstantBuffer : public T
{
public:
    ConstantBuffer(RenderDevice& device, BufferLayout& layout)
    {
        T& data = static_cast<T&>(*this);
        m_view_provider = EZ_DEFAULT_NEW(ViewProvider, device, reinterpret_cast<const uint8_t*>(&data), layout);
    }

    operator ezSharedPtr<DeferredView>& ()
    {
        return m_view_provider;
    }

private:
    ezSharedPtr<DeferredView> m_view_provider;
};
