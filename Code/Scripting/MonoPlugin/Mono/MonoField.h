//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//

#pragma once

#include <MonoPlugin/MonoPluginDLL.h>
#include <MonoPlugin/MonoPluginPCH.h>

class EZ_MONOPLUGIN_DLL ezMonoField
{
public:
  /// \brief	Returns the name of the field.
  const ezString& GetName() const { return m_Name; }

  /// \brief Returns the class representing the type of data the field holds.
  ezMonoClass* GetType();

  /// \brief Retrieves value currently set in the field on the specified object instance. If field is static object instance
  /// can be null.
  ///
  /// \note
  /// Value will be a pointer to raw data type for value types (for example int, float), and a MonoObject* for
  /// reference types.
  void Get(MonoObject* instance, void* outValue);

  /// \brief Retrieves value currently set in the field on the specified object instance.
  ///
  /// If field is static object instance can be null. If returned value is a value type it will be boxed.
  MonoObject* GetBoxed(MonoObject* instance);

  /// \brief Sets a value for the field on the specified object instance.
  /// If field is static object instance can be null.
  ///
  /// \note
  /// Value should be a pointer to raw data type for value types (for example int, float), and a MonoObject* for
  /// reference types.
  void Set(MonoObject* instance, void* value);

  ///	Checks if field has an attribute of the specified type.
  bool HasAttribute(ezMonoClass* monoClass);

  /// \brief Returns an instance of an attribute of the specified type.
  ///
  /// Returns null if the field doesn't have such an attribute.
  MonoObject* GetAttribute(ezMonoClass* monoClass);

  /// \brief Returns field visibility in the class.
  ezMonoMemberVisibility GetVisibility();

  /// \brief Query if the field is static.
  bool IsStatic();

private:
  friend class ezMonoClass;
  friend class ezMemoryUtils;

  ezMonoField(MonoClassField* field);

private:
  ezString m_Name;
  MonoClassField* m_Field;
  ezMonoClass* m_FieldType;
};
