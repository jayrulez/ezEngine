#pragma once

#include <Foundation/Basics.h>

// Configure the DLL Import/Export Define
#if EZ_ENABLED(EZ_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_MONOPLUGIN_LIB
#    define EZ_MONOPLUGIN_DLL __declspec(dllexport)
#  else
#    define EZ_MONOPLUGIN_DLL __declspec(dllimport)
#  endif
#else
#  define EZ_MONOPLUGIN_DLL
#endif
