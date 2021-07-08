#include <RHIVulkanPCH.h>

#include <RHIVulkan/QueryHeap/VKQueryHeap.h>
#include <RHIVulkan/Device/VKDevice.h>

ezRHIVKQueryHeap::ezRHIVKQueryHeap(ezRHIVKDevice& device, ezRHIQueryHeapType type, ezUInt32 count)
  : m_Device{device}
{
  EZ_ASSERT_ALWAYS(type == ezRHIQueryHeapType::AccelerationStructureCompactedSize, "");

  m_QueryType = vk::QueryType::eAccelerationStructureCompactedSizeKHR;
  vk::QueryPoolCreateInfo desc = {};
  desc.queryCount = count;
  desc.queryType = m_QueryType;
  m_QueryPool = m_Device.GetDevice().createQueryPoolUnique(desc);
}

ezRHIQueryHeapType ezRHIVKQueryHeap::GetType() const
{
  return ezRHIQueryHeapType::AccelerationStructureCompactedSize;
}

vk::QueryType ezRHIVKQueryHeap::GetQueryType() const
{
  return m_QueryType;
}

vk::QueryPool ezRHIVKQueryHeap::GetQueryPool() const
{
  return m_QueryPool.get();
}
