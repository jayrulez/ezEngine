#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Configuration/Plugin.h>

// Configure the DLL Import/Export Define
#if EZ_ENABLED(EZ_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_RHISHADERCOMPILERHLSL_LIB
#    define EZ_RHISHADERCOMPILERHLSL_DLL __declspec(dllexport)
#  else
#    define EZ_RHISHADERCOMPILERHLSL_DLL __declspec(dllimport)
#  endif
#else
#  define EZ_RHISHADERCOMPILERHLSL_DLL
#endif
