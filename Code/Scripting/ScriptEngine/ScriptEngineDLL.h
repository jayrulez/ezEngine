#pragma once

#include <Foundation/Basics.h>

// Configure the DLL Import/Export Define
#if EZ_ENABLED(EZ_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_SCRIPTENGINE_LIB
#    define EZ_SCRIPTENGINE_DLL __declspec(dllexport)
#  else
#    define EZ_SCRIPTENGINE_DLL __declspec(dllimport)
#  endif
#else
#  define EZ_SCRIPTENGINE_DLL
#endif
