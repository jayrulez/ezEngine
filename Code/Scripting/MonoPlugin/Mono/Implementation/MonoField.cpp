//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include <MonoPlugin/Mono/MonoField.h>
#include <MonoPlugin/Mono/MonoClass.h>
#include <MonoPlugin/Mono/MonoManager.h>
#include <mono/metadata/class.h>
#include <mono/metadata/attrdefs.h>
#include <mono/jit/jit.h>

ezMonoField::ezMonoField(MonoClassField* field)
  : m_Field(field)
  , m_FieldType(nullptr)
{
  m_Name = mono_field_get_name(m_Field);
}

ezMonoClass* ezMonoField::GetType()
{
  if (m_FieldType != nullptr)
    return m_FieldType;

  MonoType* monoType = mono_field_get_type(m_Field);
  MonoClass* fieldClass = mono_class_from_mono_type(monoType);
  if (fieldClass == nullptr)
    return nullptr;

  m_FieldType = ezMonoManager::GetSingleton()->FindClass(fieldClass);

  return m_FieldType;
}

void ezMonoField::Get(MonoObject* instance, void* outValue)
{
  mono_field_get_value(instance, m_Field, outValue);
}

MonoObject* ezMonoField::GetBoxed(MonoObject* instance)
{
  return mono_field_get_value_object(ezMonoManager::GetSingleton()->GetDomain(), m_Field, instance);
}

void ezMonoField::Set(MonoObject* instance, void* value)
{
  mono_field_set_value(instance, m_Field, value);
}

bool ezMonoField::HasAttribute(ezMonoClass* monoClass)
{
  // TODO - Consider caching custom attributes or just initializing them all at load

  MonoClass* parentClass = mono_field_get_parent(m_Field);
  MonoCustomAttrInfo* attrInfo = mono_custom_attrs_from_field(parentClass, m_Field);
  if (attrInfo == nullptr)
    return false;

  bool hasAttr = mono_custom_attrs_has_attr(attrInfo, monoClass->GetMonoClass()) != 0;

  mono_custom_attrs_free(attrInfo);

  return hasAttr;
}

MonoObject* ezMonoField::GetAttribute(ezMonoClass* monoClass)
{
  // TODO - Consider caching custom attributes or just initializing them all at load

  MonoClass* parentClass = mono_field_get_parent(m_Field);
  MonoCustomAttrInfo* attrInfo = mono_custom_attrs_from_field(parentClass, m_Field);
  if (attrInfo == nullptr)
    return nullptr;

  MonoObject* foundAttr = nullptr;
  if (mono_custom_attrs_has_attr(attrInfo, monoClass->GetMonoClass()))
    foundAttr = mono_custom_attrs_get_attr(attrInfo, monoClass->GetMonoClass());

  mono_custom_attrs_free(attrInfo);
  return foundAttr;
}

ezMonoMemberVisibility ezMonoField::GetVisibility()
{
  ezUInt32 flags = mono_field_get_flags(m_Field) & MONO_FIELD_ATTR_FIELD_ACCESS_MASK;

  if (flags == MONO_FIELD_ATTR_PRIVATE)
    return ezMonoMemberVisibility::Private;
  else if (flags == MONO_FIELD_ATTR_FAM_AND_ASSEM)
    return ezMonoMemberVisibility::ProtectedInternal;
  else if (flags == MONO_FIELD_ATTR_ASSEMBLY)
    return ezMonoMemberVisibility::Internal;
  else if (flags == MONO_FIELD_ATTR_FAMILY)
    return ezMonoMemberVisibility::Protected;
  else if (flags == MONO_FIELD_ATTR_PUBLIC)
    return ezMonoMemberVisibility::Public;

  EZ_ASSERT_ALWAYS(false, "Unknown visibility modifier.");
  return ezMonoMemberVisibility::Private;
}

bool ezMonoField::IsStatic()
{
  ezUInt32 flags = mono_field_get_flags(m_Field);

  return (flags & MONO_FIELD_ATTR_STATIC) != 0;
}
