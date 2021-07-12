#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Types/UniquePtr.h>
#include <RHI/RHIDLL.h>

#include <vulkan/vulkan.hpp>

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
