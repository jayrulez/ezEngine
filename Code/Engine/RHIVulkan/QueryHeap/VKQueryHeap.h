#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHIVulkan/VKBasics.h>

#include <RHI/QueryHeap/QueryHeap.h>

class ezRHIVKQueryHeap : public ezRHIQueryHeap
{
public:
  ezRHIVKQueryHeap(ezRHIVKDevice& device, ezRHIQueryHeapType type, ezUInt32 count);

  ezRHIQueryHeapType GetType() const override;

  vk::QueryType GetQueryType() const;
  vk::QueryPool GetQueryPool() const;

private:
  ezRHIVKDevice& m_Device;
  vk::UniqueQueryPool m_QueryPool;
  vk::QueryType m_QueryType;
};
