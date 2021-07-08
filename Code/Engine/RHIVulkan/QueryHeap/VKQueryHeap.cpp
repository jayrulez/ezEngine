#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/QueryHeap/VKQueryHeap.h>

VKQueryHeap::VKQueryHeap(VKDevice& device, QueryHeapType type, uint32_t count)
  : m_device(device)
{
  assert(type == QueryHeapType::kAccelerationStructureCompactedSize);
  m_query_type = VkQueryType::VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR;
  VkQueryPoolCreateInfo desc = {};
  desc.sType = VkStructureType::VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
  desc.queryCount = count;
  desc.queryType = m_query_type;
  vkCreateQueryPool(m_device.GetDevice(), &desc, nullptr, &m_query_pool);
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
  return m_query_pool;
}
