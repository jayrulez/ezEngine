#include <MonoPlugin/Mono/MonoClass.h>
#include <MonoPlugin/Mono/MonoField.h>
#include <MonoPlugin/Mono/MonoMethod.h>
#include <MonoPlugin/Mono/MonoProperty.h>
#include <mono/metadata/class.h>
#include <mono/metadata/debug-helpers.h>

bool ezMonoClass::MethodId::operator==(const ezMonoClass::MethodId& other) const
{
  return Name == other.Name && ParameterCount == other.ParameterCount;
}

bool ezMonoClass::MethodId::operator!=(const ezMonoClass::MethodId& other) const
{
  return !(*this == other);
}

bool ezMonoClass::MethodId::operator<(const ezMonoClass::MethodId& other) const
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

  auto iterFind = std::find(begin(m_Methods), end(m_Methods), methodId);
  if (iterFind != end(m_Methods))
    return iterFind->value;

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

  auto iterFind = std::find(begin(m_Methods), end(m_Methods), methodId);
  if (iterFind != end(m_Methods))
    return iterFind->value;

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
  auto iterFind = std::find(begin(m_Fields), end(m_Fields), name);
  if (iterFind != end(m_Fields))
    return iterFind->value;

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
  auto iterFind = std::find(begin(m_Properties), end(m_Properties), name);
  if (iterFind != end(m_Properties))
    return iterFind->value;

  MonoProperty* property = mono_class_get_property_from_name(m_pMonoClass, name.GetData());
  if (property == nullptr)
    return nullptr;

  ezMonoProperty* newProperty = EZ_DEFAULT_NEW(ezMonoProperty, property);
  m_Properties[name] = newProperty;

  return newProperty;
}
