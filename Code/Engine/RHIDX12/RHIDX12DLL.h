#pragma once

#include <Foundation/Basics.h>
#include <RHI/RHIDLL.h>

// Configure the DLL Import/Export Define
#if EZ_ENABLED(EZ_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_RHIDX12_LIB
#    define EZ_RHIDX12_DLL __declspec(dllexport)
#  else
#    define EZ_RHIDX12_DLL __declspec(dllimport)
#  endif
#else
#  define EZ_RHIDX12_DLL
#endif