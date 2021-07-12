#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHIVulkan/RHIVulkanDLL.h>

namespace VKUtils
{
  vk::Format ToVkFormat(ezRHIResourceFormat::Enum value);
  ezRHIResourceFormat::Enum ToEngineFormat(vk::Format value);
}
