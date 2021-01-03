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

class ezMonoAssembly;
class ezMonoClass;
class ezMonoMethod;
class ezMonoField;
class ezMonoProperty;

// Mono types used for embedding
typedef struct _MonoClass MonoClass;
typedef struct _MonoDomain MonoDomain;
typedef struct _MonoImage MonoImage;
typedef struct _MonoAssembly MonoAssembly;
typedef struct _MonoMethod MonoMethod;
typedef struct _MonoProperty MonoProperty;
typedef struct _MonoObject MonoObject;
typedef struct _MonoString MonoString;
typedef struct _MonoArray MonoArray;
typedef struct _MonoReflectionType MonoReflectionType;
typedef struct _MonoException MonoException;
typedef struct _MonoClassField MonoClassField;
