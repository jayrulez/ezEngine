#pragma once

#include <Foundation/Basics.h>
#include <RHI/RHIDLL.h>

// Configure the DLL Import/Export Define
#if EZ_ENABLED(EZ_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_RHIDX11_LIB
#    define EZ_RHIDX11_DLL __declspec(dllexport)
#  else
#    define EZ_RHIDX11_DLL __declspec(dllimport)
#  endif
#else
#  define EZ_RHIDX11_DLL
#endif


#define EZ_RHI_DX11_RELEASE(d3dobj)                                                                                                                  \
  do                                                                                                                                                 \
  {                                                                                                                                                  \
    if ((d3dobj) != nullptr)                                                                                                                         \
    {                                                                                                                                                \
      (d3dobj)->Release();                                                                                                                           \
      (d3dobj) = nullptr;                                                                                                                            \
    }                                                                                                                                                \
  } while (0)