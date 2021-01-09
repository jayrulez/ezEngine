//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include <MonoPlugin/Mono/MonoClass.h>
#include <MonoPlugin/Mono/MonoField.h>
#include <MonoPlugin/Mono/MonoMethod.h>
#include <MonoPlugin/Mono/MonoProperty.h>
#include <MonoPlugin/Mono/MonoUtil.h>
#include <MonoPlugin/Mono/MonoManager.h>
#include <mono/metadata/class.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/reflection.h>

bool ezMonoClass::MethodId::operator==(const MethodId& other) const
{
  return Name == other.Name && ParameterCount == other.ParameterCount;
}

bool ezMonoClass::MethodId::operator!=(const MethodId& other) const
{
  return !(*this == other);
}

bool ezMonoClass::MethodId::operator<(const MethodId& other) const
{
  return GetHashCode() < other.GetHashCode();
}

ezUInt32 ezMonoClass::MethodId::GetHashCode() const
{
  ezUInt32 hash = 0;

  hash = ezHashingUtils::xxHash32String(Name, hash);
  hash = ezHashingUtils::xxHash32(&ParameterCount, sizeof(ParameterCount), hash);

  return hash;
}

ezMonoClass::ezMonoClass(const ezString& classNamespace, const ezString& type, MonoClass* monoClass, const ezMonoAssembly* parentAssembly)
  : m_pParentAssembly(parentAssembly)
  , m_pMonoClass(monoClass)
  , m_Namespace(classNamespace)
  , m_TypeName(type)
  , m_bHasCachedFields(false)
  , m_bHasCachedProperties(false)
  , m_bHasCachedMethods(false)
{
  m_FullName = ezStringBuilder(classNamespace, ".", type);
}

ezMonoClass::~ezMonoClass()
{
  for (auto& mapEntry : m_Methods)
  {
    EZ_DEFAULT_DELETE(mapEntry.value);
  }
  m_Methods.Clear();


  for (auto& mapEntry : m_Fields)
  {
    EZ_DEFAULT_DELETE(mapEntry.value);
  }
  m_Fields.Clear();


  for (auto& mapEntry : m_Properties)
  {
    EZ_DEFAULT_DELETE(mapEntry.value);
  }
  m_Properties.Clear();
}

ezMonoMethod* ezMonoClass::GetMethod(const ezString& name, ezUInt32 parameterCount) const
{
  MethodId methodId(name, parameterCount);

  ezUInt32 methodIndex = m_Methods.Find(methodId);
  if (methodIndex != ezInvalidIndex)
    return m_Methods.GetValue(methodIndex);

  MonoMethod* method = mono_class_get_method_from_name(m_pMonoClass, name.GetData(), (ezInt32)parameterCount);
  if (method == nullptr)
    return nullptr;

  ezMonoMethod* newMethod = EZ_DEFAULT_NEW(ezMonoMethod, method);
  m_Methods[methodId] = newMethod;

  return newMethod;
}

ezMonoMethod* ezMonoClass::GetMethodWithSignature(const ezString& name, const ezString& signature) const
{
  MethodId methodId(ezStringBuilder(name, "(", signature, ")"), 0);

  ezUInt32 methodIndex = m_Methods.Find(methodId);
  if (methodIndex != ezInvalidIndex)
    return m_Methods.GetValue(methodIndex);

  MonoMethod* method;
  void* iter = nullptr;

  const char* rawName = name;
  const char* rawSig = signature;

  while ((method = mono_class_get_methods(m_pMonoClass, &iter)))
  {
    if (ezStringUtils::Compare(rawName, mono_method_get_name(method)) == 0)
    {
      const char* currentSig = mono_signature_get_desc(mono_method_signature(method), false);
      if (ezStringUtils::Compare(rawSig, currentSig) == 0)
      {
        ezMonoMethod* newMethod = EZ_DEFAULT_NEW(ezMonoMethod, method);
        m_Methods[methodId] = newMethod;

        return newMethod;
      }
    }
  }

  return nullptr;
}

const ezDynamicArray<ezMonoMethod*>& ezMonoClass::GetAllMethods() const
{
  if (m_bHasCachedMethods)
    return m_CachedMethodList;

  m_CachedMethodList.Clear();

  void* iter = nullptr;
  MonoMethod* curClassMethod = mono_class_get_methods(m_pMonoClass, &iter);
  while (curClassMethod != nullptr)
  {
    MonoMethodSignature* sig = mono_method_signature(curClassMethod);

    const char* sigDesc = mono_signature_get_desc(sig, false);
    const char* methodName = mono_method_get_name(curClassMethod);

    ezMonoMethod* curMethod = GetMethodWithSignature(methodName, sigDesc);
    m_CachedMethodList.PushBack(curMethod);

    curClassMethod = mono_class_get_methods(m_pMonoClass, &iter);
  }

  m_bHasCachedMethods = true;
  return m_CachedMethodList;
}

MonoObject* ezMonoClass::InvokeMethod(const ezString& name, MonoObject* instance, void** params, ezUInt32 parameterCount)
{
  return GetMethod(name, parameterCount)->Invoke(instance, params);
}

bool ezMonoClass::HasField(const ezString& name) const
{
  MonoClassField* field = mono_class_get_field_from_name(m_pMonoClass, name.GetData());

  return field != nullptr;
}

ezMonoField* ezMonoClass::GetField(const ezString& name) const
{
  ezUInt32 fieldIndex = m_Fields.Find(name);
  if (fieldIndex != ezInvalidIndex)
    return m_Fields.GetValue(fieldIndex);

  MonoClassField* field = mono_class_get_field_from_name(m_pMonoClass, name.GetData());
  if (field == nullptr)
    return nullptr;

  ezMonoField* newField = EZ_DEFAULT_NEW(ezMonoField, field);
  m_Fields[name] = newField;

  return newField;
}

const ezDynamicArray<ezMonoField*>& ezMonoClass::GetAllFields() const
{
  if (m_bHasCachedFields)
    return m_CachedFieldList;

  m_CachedFieldList.Clear();

  void* iter = nullptr;
  MonoClassField* curClassField = mono_class_get_fields(m_pMonoClass, &iter);
  while (curClassField != nullptr)
  {
    const char* fieldName = mono_field_get_name(curClassField);
    ezMonoField* curField = GetField(fieldName);

    m_CachedFieldList.PushBack(curField);

    curClassField = mono_class_get_fields(m_pMonoClass, &iter);
  }

  m_bHasCachedFields = true;
  return m_CachedFieldList;
}

ezMonoProperty* ezMonoClass::GetProperty(const ezString& name) const
{
  ezUInt32 propertyIndex = m_Properties.Find(name);
  if (propertyIndex != ezInvalidIndex)
    return m_Properties.GetValue(propertyIndex);

  MonoProperty* property = mono_class_get_property_from_name(m_pMonoClass, name.GetData());
  if (property == nullptr)
    return nullptr;

  ezMonoProperty* newProperty = EZ_DEFAULT_NEW(ezMonoProperty, property);
  m_Properties[name] = newProperty;

  return newProperty;
}

const ezDynamicArray<ezMonoProperty*>& ezMonoClass::GetAllProperties() const
{
  if (m_bHasCachedProperties)
    return m_CachedPropertyList;

  m_CachedPropertyList.Clear();

  void* iter = nullptr;
  MonoProperty* curClassProperty = mono_class_get_properties(m_pMonoClass, &iter);
  while (curClassProperty != nullptr)
  {
    const char* propertyName = mono_property_get_name(curClassProperty);
    ezMonoProperty* curProperty = GetProperty(propertyName);

    m_CachedPropertyList.PushBack(curProperty);

    curClassProperty = mono_class_get_properties(m_pMonoClass, &iter);
  }

  m_bHasCachedProperties = true;
  return m_CachedPropertyList;
}

bool ezMonoClass::HasAttribute(ezMonoClass* monoClass) const
{
  // TODO - Consider caching custom attributes or just initializing them all at load

  MonoCustomAttrInfo* attrInfo = mono_custom_attrs_from_class(m_pMonoClass);
  if (attrInfo == nullptr)
    return false;

  bool hasAttr = mono_custom_attrs_has_attr(attrInfo, monoClass->GetMonoClass()) != 0;

  mono_custom_attrs_free(attrInfo);

  return hasAttr;
}

MonoObject* ezMonoClass::GetAttribute(ezMonoClass* monoClass) const
{
  // TODO - Consider caching custom attributes or just initializing them all at load

  MonoCustomAttrInfo* attrInfo = mono_custom_attrs_from_class(m_pMonoClass);
  if (attrInfo == nullptr)
    return nullptr;

  MonoObject* foundAttr = nullptr;
  if (mono_custom_attrs_has_attr(attrInfo, monoClass->GetMonoClass()))
    foundAttr = mono_custom_attrs_get_attr(attrInfo, monoClass->GetMonoClass());

  mono_custom_attrs_free(attrInfo);
  return foundAttr;
}

ezDynamicArray<ezMonoClass*> ezMonoClass::GetAllAttributes() const
{
  // TODO - Consider caching custom attributes or just initializing them all at load
  ezDynamicArray<ezMonoClass*> attributes;

  MonoCustomAttrInfo* attrInfo = mono_custom_attrs_from_class(m_pMonoClass);
  if (attrInfo == nullptr)
    return attributes;

  for (ezInt32 i = 0; i < attrInfo->num_attrs; i++)
  {
    MonoClass* attribClass = mono_method_get_class(attrInfo->attrs[i].ctor);
    ezMonoClass* klass = ezMonoManager::GetSingleton()->FindClass(attribClass);

    if (klass != nullptr)
      attributes.PushBack(klass);
  }

  mono_custom_attrs_free(attrInfo);

  return attributes;
}

ezMonoClass* ezMonoClass::GetBaseClass() const
{
  MonoClass* monoBase = mono_class_get_parent(m_pMonoClass);
  if (monoBase == nullptr)
    return nullptr;

  ezString classNamespace;
  ezString type;
  ezMonoUtil::GetClassName(monoBase, classNamespace, type);

  return ezMonoManager::GetSingleton()->FindClass(classNamespace, type);
}

bool ezMonoClass::IsSubClassOf(const ezMonoClass* monoClass) const
{
  if (monoClass == nullptr)
    return false;

  return mono_class_is_subclass_of(m_pMonoClass, monoClass->m_pMonoClass, true) != 0;
}

bool ezMonoClass::IsInstanceOfType(MonoObject* object) const
{
  if (object == nullptr)
    return false;

  MonoClass* monoClass = mono_object_get_class(object);
  return mono_class_is_subclass_of(monoClass, m_pMonoClass, false) != 0;
}

ezUInt32 ezMonoClass::GetInstanceSize() const
{
  ezUInt32 dummy = 0;

  if (mono_class_is_valuetype(m_pMonoClass))
    return mono_class_value_size(m_pMonoClass, &dummy);

  return mono_class_instance_size(m_pMonoClass);
}

void ezMonoClass::AddInternalCall(const ezString& name, const void* method)
{
  const char* fullMethodName = ezStringBuilder(m_FullName, "::", name).GetData();
  mono_add_internal_call(fullMethodName, method);
}

MonoObject* ezMonoClass::CreateInstance(bool construct) const
{
  MonoObject* obj = mono_object_new(ezMonoManager::GetSingleton()->GetDomain(), m_pMonoClass);

  if (construct)
    mono_runtime_object_init(obj);

  return obj;
}

MonoObject* ezMonoClass::CreateInstance(void** params, ezUInt32 parameterCount)
{
  MonoObject* obj = mono_object_new(ezMonoManager::GetSingleton()->GetDomain(), m_pMonoClass);
  GetMethod(".ctor", parameterCount)->Invoke(obj, params);

  return obj;
}

MonoObject* ezMonoClass::CreateInstance(const ezString& ctorSignature, void** params)
{
  MonoObject* obj = mono_object_new(ezMonoManager::GetSingleton()->GetDomain(), m_pMonoClass);
  GetMethodWithSignature(".ctor", ctorSignature)->Invoke(obj, params);

  return obj;
}

void ezMonoClass::Construct(MonoObject* object)
{
  mono_runtime_object_init(object);
}
