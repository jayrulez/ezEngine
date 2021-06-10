#pragma once

#include <Foundation/Basics.h>
#include <RendererFoundation/RendererFoundationDLL.h>

// Configure the DLL Import/Export Define
#if EZ_ENABLED(EZ_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_RENDERERVK_LIB
#    define EZ_RENDERERVK_DLL __declspec(dllexport)
#  else
#    define EZ_RENDERERVK_DLL __declspec(dllimport)
#  endif
#else
#  define EZ_RENDERERVK_DLL
#endif
