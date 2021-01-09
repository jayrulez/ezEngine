//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include <MonoPlugin/Mono/MonoProperty.h>
#include <MonoPlugin/Mono/MonoMethod.h>
#include <MonoPlugin/Mono/MonoClass.h>
#include <MonoPlugin/Mono/MonoManager.h>
#include <mono/jit/jit.h>
#include <mono/metadata/class.h>

ezMonoProperty::ezMonoProperty(MonoProperty* monoProp)
  : m_pProperty(monoProp)
  , m_pReturnType(nullptr)
  , m_bIsIndexed(false)
  , m_bIsFullyInitialized(false)
{
  m_pGetMethod = mono_property_get_get_method(m_pProperty);
  m_pSetMethod = mono_property_get_set_method(m_pProperty);

  m_Name = mono_property_get_name(m_pProperty);
}

MonoObject* ezMonoProperty::Get(MonoObject* instance) const
{
  if (m_pGetMethod == nullptr)
    return nullptr;

  return mono_runtime_invoke(m_pGetMethod, instance, nullptr, nullptr);
}

void ezMonoProperty::Set(MonoObject* instance, void* value) const
{
  if (m_pSetMethod == nullptr)
    return;

  void* args[1];
  args[0] = value;
  mono_runtime_invoke(m_pSetMethod, instance, args, nullptr);
}

MonoObject* ezMonoProperty::GetIndexed(MonoObject* instance, ezUInt32 index) const
{
  void* args[1];
  args[0] = &index;
  return mono_runtime_invoke(m_pGetMethod, instance, args, nullptr);
}

void ezMonoProperty::SetIndexed(MonoObject* instance, ezUInt32 index, void* value) const
{
  void* args[2];
  args[0] = &index;
  args[1] = value;
  mono_runtime_invoke(m_pSetMethod, instance, args, nullptr);
}

bool ezMonoProperty::IsIndexed() const
{
  if (!m_bIsFullyInitialized)
    InitializeDeferred();

  return m_bIsIndexed;
}

ezMonoClass* ezMonoProperty::GetReturnType() const
{
  if (!m_bIsFullyInitialized)
    InitializeDeferred();

  return m_pReturnType;
}

bool ezMonoProperty::HasAttribute(ezMonoClass* monoClass)
{
  // TODO - Consider caching custom attributes or just initializing them all at load

  MonoClass* parentClass = mono_property_get_parent(m_pProperty);
  MonoCustomAttrInfo* attrInfo = mono_custom_attrs_from_property(parentClass, m_pProperty);
  if (attrInfo == nullptr)
    return false;

  bool hasAttr = mono_custom_attrs_has_attr(attrInfo, monoClass->GetMonoClass()) != 0;

  mono_custom_attrs_free(attrInfo);

  return hasAttr;
}

MonoObject* ezMonoProperty::GetAttribute(ezMonoClass* monoClass)
{
  // TODO - Consider caching custom attributes or just initializing them all at load

  MonoClass* parentClass = mono_property_get_parent(m_pProperty);
  MonoCustomAttrInfo* attrInfo = mono_custom_attrs_from_property(parentClass, m_pProperty);
  if (attrInfo == nullptr)
    return nullptr;

  MonoObject* foundAttr = nullptr;
  if (mono_custom_attrs_has_attr(attrInfo, monoClass->GetMonoClass()))
    foundAttr = mono_custom_attrs_get_attr(attrInfo, monoClass->GetMonoClass());

  mono_custom_attrs_free(attrInfo);
  return foundAttr;
}

ezMonoMemberVisibility ezMonoProperty::GetVisibility()
{
  ezMonoMemberVisibility getterVisibility = ezMonoMemberVisibility::Public;
  if (m_pGetMethod)
  {
    ezMonoMethod getterWrapper(m_pGetMethod);
    getterVisibility = getterWrapper.GetVisibility();
  }

  ezMonoMemberVisibility setterVisibility = ezMonoMemberVisibility::Public;
  if (m_pSetMethod)
  {
    ezMonoMethod setterWrapper(m_pSetMethod);
    setterVisibility = setterWrapper.GetVisibility();
  }

  if (getterVisibility < setterVisibility)
    return getterVisibility;

  return setterVisibility;
}

void ezMonoProperty::InitializeDeferred() const
{
  if (m_pGetMethod != nullptr)
  {
    MonoMethodSignature* signature = mono_method_signature(m_pGetMethod);

    MonoType* returnType = mono_signature_get_return_type(signature);
    if (returnType != nullptr)
    {
      ::MonoClass* returnClass = mono_class_from_mono_type(returnType);
      if (returnClass != nullptr)
        m_pReturnType = ezMonoManager::GetSingleton()->FindClass(returnClass);
    }

    ezUInt32 numParams = mono_signature_get_param_count(signature);
    m_bIsIndexed = numParams == 1;
  }
  else if (m_pSetMethod != nullptr)
  {
    MonoMethodSignature* signature = mono_method_signature(m_pSetMethod);

    MonoType* returnType = mono_signature_get_return_type(signature);
    if (returnType != nullptr)
    {
      ::MonoClass* returnClass = mono_class_from_mono_type(returnType);
      if (returnClass != nullptr)
        m_pReturnType = ezMonoManager::GetSingleton()->FindClass(returnClass);
    }

    ezUInt32 numParams = mono_signature_get_param_count(signature);
    m_bIsIndexed = numParams == 2;
  }

  m_bIsFullyInitialized = true;
}
