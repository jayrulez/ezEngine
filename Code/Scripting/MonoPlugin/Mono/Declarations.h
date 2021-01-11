#pragma once

#include <MonoPlugin/MonoPluginDLL.h>
#include <MonoPlugin/MonoPluginPCH.h>

#if EZ_ENABLED(EZ_PLATFORM_WINDOWS)
#  include <Foundation/Basics/Platform/Win/MinWindows.h>
#  define EZ_MONO_THUNKCALL EZ_WINDOWS_CALLBACK
#else
#  define EZ_MONO_THUNKCALL
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

/// \brief A list of all valid Mono primitive types.
enum class ezMonoPrimitiveType
{
  Boolean,
  Char,
  I8,
  U8,
  I16,
  U16,
  I32,
  U32,
  I64,
  U64,
  R32,
  R64,
  String,
  ValueType,
  Class,
  Array,
  Generic,
  Unknown
};

/// \brief Returns the level of member visibility in the class.
/// Note: Elements must be ordered from most to least hidden
enum class ezMonoMemberVisibility
{
  Private,
  Protected,
  Internal,
  ProtectedInternal,
  Public
};
