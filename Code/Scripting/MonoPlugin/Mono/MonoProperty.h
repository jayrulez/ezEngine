//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//

#pragma once

#include <MonoPlugin/MonoPluginDLL.h>
#include <MonoPlugin/MonoPluginPCH.h>
#include <MonoPlugin/Mono/Declarations.h>

class EZ_MONOPLUGIN_DLL ezMonoProperty
{
public:
  /// \brief 	Returns the name of the property.
  const ezString& getName() const { return m_Name; }

  /// \brief  Returns a boxed value contained in the property in the specified instance.
  ///
  /// \brief \param[in]	instance	Object instance to access the property on. Can be null for static properties.
  ///
  /// \return					A boxed value of the property.
  MonoObject* Get(MonoObject* instance) const;

  /// \brief  Sets a value of the property in the specified instance.
  ///
  /// \param[in]	instance	Object instance to access the property on. Can be null for static properties.
  /// \param[in]	value		Value to set on the property. For value type it should be a pointer to the value and for
  ///							reference type it should be a pointer to MonoObject.
  void Set(MonoObject* instance, void* value) const;

  /// \brief Returns a boxed value contained in the property in the specified instance. Used for properties with indexers.
  ///
  /// \param[in]	instance	Object instance to access the property on. Can be null for static properties.
  /// \param[in]	index		Index of the value to retrieve.
  ///
  /// \return					A boxed value of the property.
  MonoObject* GetIndexed(MonoObject* instance, ezUInt32 index) const;

  /// \brief Sets a value of the property in the specified instance. Used for properties with indexers.
  ///
  /// \param[in]	instance	Object instance to access the property on. Can be null for static properties.
  /// \param[in]	index		Index of the value to set.
  /// \param[in]	value		Value to set on the property. For value type it should be a pointer to the value and for
  ///							reference type it should be a pointer to MonoObject.
  void SetIndexed(MonoObject* instance, ezUInt32 index, void* value) const;

  /// \brief  Checks does the property contains indexed data, or just a single value.
  bool IsIndexed() const;

  /// \brief 	Returns the data type the property holds.
  ezMonoClass* GetReturnType() const;

  /// \brief 	Checks if property has an attribute of the specified type.
  bool HasAttribute(ezMonoClass* monoClass);

  /// \brief Returns an instance of an attribute of the specified type.
  ///Returns null if the property doesn't have such an attribute.
  MonoObject* GetAttribute(ezMonoClass* monoClass);

  /// \brief Returns property visibility in the class.
  /// If getter/setter methods have different visibility, the more restrictive one is returned.
  ezMonoMemberVisibility GetVisibility();

private:
  friend class ezMonoClass;
  friend class ezMemoryUtils;

  ezMonoProperty(MonoProperty* monoProp);

  /// \brief Some property data is not initialized by default on creation (with the assumption it will never be accessed).
  /// This method will initialize that data.
  void InitializeDeferred() const;

private:
  ezString m_Name;
  MonoProperty* m_pProperty;
  MonoMethod* m_pGetMethod;
  MonoMethod* m_pSetMethod;

  mutable ezMonoClass* m_pReturnType;
  mutable bool m_bIsIndexed;
  mutable bool m_bIsFullyInitialized;
};
