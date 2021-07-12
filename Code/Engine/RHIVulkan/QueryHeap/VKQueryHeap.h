#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/QueryHeap/QueryHeap.h>

class VKDevice;

class VKQueryHeap : public QueryHeap
{
public:
  VKQueryHeap(VKDevice& device, QueryHeapType type, uint32_t count);

  QueryHeapType GetType() const override;

  vk::QueryType GetQueryType() const;
  vk::QueryPool GetQueryPool() const;

private:
  VKDevice& m_device;
  vk::UniqueQueryPool m_query_pool;
  vk::QueryType m_query_type;
};
