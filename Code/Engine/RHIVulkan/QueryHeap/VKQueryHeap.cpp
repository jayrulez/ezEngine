#include <RHIVulkan/QueryHeap/VKQueryHeap.h>
#include <RHIVulkan/Device/VKDevice.h>

VKQueryHeap::VKQueryHeap(VKDevice& device, QueryHeapType type, uint32_t count)
    : m_device(device)
{
    assert(type == QueryHeapType::kAccelerationStructureCompactedSize);
    m_query_type = VkQueryType::eAccelerationStructureCompactedSizeKHR;
    VkQueryPoolCreateInfo desc = {};
    desc.queryCount = count;
    desc.queryType = m_query_type;
    m_query_pool = m_device.GetDevice().createQueryPoolUnique(desc);
}

QueryHeapType VKQueryHeap::GetType() const
{
    return QueryHeapType::kAccelerationStructureCompactedSize;
}

VkQueryType VKQueryHeap::GetQueryType() const
{
    return m_query_type;
}

VkQueryPool VKQueryHeap::GetQueryPool() const
{
    return m_query_pool.get();
}
