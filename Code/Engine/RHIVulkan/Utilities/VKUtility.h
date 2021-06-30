#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>
#include <vulkan/vulkan.hpp>

namespace VKUtils
{
  vk::Format ToVkFormat(ezRHIResourceFormat::Enum value);
  ezRHIResourceFormat::Enum ToEngineFormat(vk::Format value);
}

//#define CUSTOM_FAILED(hr) ((static_cast<VkResult>((hr))) != VK_SUCCESS)
//#ifdef _WIN32
//#include "DXUtility.h"
//#else
//#define ASSERT( isTrue, ... ) (void)(isTrue)
//#define WARN_ONCE_IF( isTrue, ... ) (void)(isTrue)
//#define WARN_ONCE_IF_NOT( isTrue, ... ) (void)(isTrue)
//#define ERROR( msg, ... )
//#define DEBUGPRINT( msg, ... ) do {} while(0)
//#define ASSERT_SUCCEEDED( hr, ... ) (void)(hr)
//#endif
