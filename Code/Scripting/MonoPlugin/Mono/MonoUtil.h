//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include <MonoPlugin/MonoPluginDLL.h>
#include <MonoPlugin/MonoPluginPCH.h>

class EZ_MONOPLUGIN_DLL ezMonoUtil
{
public:
  ///	\brief Converts a Mono (managed) string to a native wide string.
  static ezStringWChar MonoToWideString(MonoString* str);

  /// \brief Converts a Mono (managed) string to a native narrow string.
  static ezString MonoToString(MonoString* str);

  ///	\brief Converts a native wide string to a Mono (managed) string.
  static MonoString* WideStringToMono(const ezStringWChar& str);

  ///	\brief Converts a native narrow string to a Mono (managed) string.
  static MonoString* StringToMono(const ezString& str);

  ///	\brief Outputs name and namespace for the type of the specified object.
  static void GetClassName(MonoObject* obj, ezString& classNamespace, ezString& typeName);

  ///	\brief Outputs name and namespace for the specified type.
  static void GetClassName(::MonoClass* monoClass, ezString& classNamespace, ezString& typeName);

  ///	\brief Outputs name and namespace for the specified type.
  static void GetClassName(MonoReflectionType* monoReflType, ezString& ns, ezString& typeName);

  /// \brief Throws a native exception if the provided object is a valid managed exception.
  static void ThrowIfException(MonoException* exception);

  /// \brief Throws a native exception if the provided object is a valid managed exception.
  static void ThrowIfException(MonoObject* exception);
};
