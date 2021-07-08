#pragma once

#include <Foundation/Basics.h>
#include <RHI/RHIDLL.h>

//#define VOLK_IMPLEMENTATION 1

#include<volk/volk.h>

//#include <vulkan/vulkan.h>

// Configure the DLL Import/Export Define
#if EZ_ENABLED(EZ_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_RHIVULKAN_LIB
#    define EZ_RHIVULKAN_DLL __declspec(dllexport)
#  else
#    define EZ_RHIVULKAN_DLL __declspec(dllimport)
#  endif
#else
#  define EZ_RHIVULKAN_DLL
#endif


EZ_DEFINE_AS_POD_TYPE(VkLayerProperties);
EZ_DEFINE_AS_POD_TYPE(VkSurfaceFormatKHR);
EZ_DEFINE_AS_POD_TYPE(VkQueueFamilyProperties);
EZ_DEFINE_AS_POD_TYPE(VkExtensionProperties);
EZ_DEFINE_AS_POD_TYPE(VkPresentModeKHR);
EZ_DEFINE_AS_POD_TYPE(VkBufferCopy);
EZ_DEFINE_AS_POD_TYPE(VkPhysicalDeviceFragmentShadingRateKHR);
