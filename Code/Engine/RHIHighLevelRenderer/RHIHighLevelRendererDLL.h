#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Types/Id.h>
#include <Foundation/Types/RefCounted.h>

// Configure the DLL Import/Export Define
#if EZ_ENABLED(EZ_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_RHIHIGHLEVELRENDERER_LIB
#    define EZ_RHIHIGHLEVELRENDERER_DLL __declspec(dllexport)
#  else
#    define EZ_RHIHIGHLEVELRENDERER_DLL __declspec(dllimport)
#  endif
#else
#  define EZ_RHIHIGHLEVELRENDERER_DLL
#endif
