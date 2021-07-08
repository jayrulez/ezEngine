#pragma once
#include <RHI/QueryHeap/QueryHeap.h>
#include <RHIVulkan/RHIVulkanDLL.h>

class VKDevice;

class VKQueryHeap : public QueryHeap
{
public:
  VKQueryHeap(VKDevice& device, QueryHeapType type, uint32_t count);

  QueryHeapType GetType() const override;

  VkQueryType GetQueryType() const;
  VkQueryPool GetQueryPool() const;

private:
  VKDevice& m_device;
  VkQueryPool m_query_pool;
  VkQueryType m_query_type;
};
