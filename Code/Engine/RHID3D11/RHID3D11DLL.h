#pragma once

#include <Foundation/Basics.h>
#include <RHI/RHIDLL.h>

// Configure the DLL Import/Export Define
#if EZ_ENABLED(EZ_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_RHID3D11_LIB
#    define EZ_RHID3D11_DLL __declspec(dllexport)
#  else
#    define EZ_RHID3D11_DLL __declspec(dllimport)
#  endif
#else
#  define EZ_RHID3D11_DLL
#endif