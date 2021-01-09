//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include <MonoPlugin/Mono/MonoClass.h>
#include <MonoPlugin/Mono/MonoMethod.h>
#include <MonoPlugin/Mono/MonoUtil.h>
#include <MonoPlugin/Mono/MonoManager.h>
#include <mono/jit/jit.h>
#include <mono/metadata/attrdefs.h>

ezMonoMethod::ezMonoMethod(MonoMethod* method)
  : m_pMethod(method)
  , m_pCachedReturnType(nullptr)
  , m_pCachedParameters(nullptr)
  , m_CachedParameterCount(0)
  , m_bIsStatic(false)
  , m_bHasCachedSignature(false)
{
}

ezMonoMethod::~ezMonoMethod()
{
  if (m_pCachedParameters != nullptr)
    EZ_DEFAULT_DELETE_RAW_BUFFER(m_pCachedParameters);
}

MonoObject* ezMonoMethod::Invoke(MonoObject* instance, void** params)
{
  MonoObject* exception = nullptr;
  MonoObject* retVal = mono_runtime_invoke(m_pMethod, instance, params, &exception);

  ezMonoUtil::ThrowIfException(exception);
  return retVal;
}

MonoObject* ezMonoMethod::InvokeVirtual(MonoObject* instance, void** params)
{
  ::MonoMethod* virtualMethod = mono_object_get_virtual_method(instance, m_pMethod);

  MonoObject* exception = nullptr;
  MonoObject* retVal = mono_runtime_invoke(virtualMethod, instance, params, &exception);

  ezMonoUtil::ThrowIfException(exception);
  return retVal;
}

void* ezMonoMethod::GetThunk() const
{
  return mono_method_get_unmanaged_thunk(m_pMethod);
}

ezString ezMonoMethod::GetName() const
{
  return ezString(mono_method_get_name(m_pMethod));
}

ezMonoClass* ezMonoMethod::GetReturnType() const
{
  if (!m_bHasCachedSignature)
    CacheSignature();

  return m_pCachedReturnType;
}

ezUInt32 ezMonoMethod::GetParameterCount() const
{
  if (!m_bHasCachedSignature)
    CacheSignature();

  return m_CachedParameterCount;
}

ezMonoClass* ezMonoMethod::GetParameterType(ezUInt32 paramIdx) const
{
  if (!m_bHasCachedSignature)
    CacheSignature();

  if (paramIdx >= m_CachedParameterCount)
  {
    EZ_REPORT_FAILURE("Parameter index out of range. Valid range is [0, {0}]", m_CachedParameterCount - 1);
    return nullptr;
  }

  return m_pCachedParameters[paramIdx];
}

bool ezMonoMethod::IsStatic() const
{
  if (!m_bHasCachedSignature)
    CacheSignature();

  return m_bIsStatic;
}

bool ezMonoMethod::HasAttribute(ezMonoClass* monoClass) const
{
  // TODO - Consider caching custom attributes or just initializing them all at load

  MonoCustomAttrInfo* attrInfo = mono_custom_attrs_from_method(m_pMethod);
  if (attrInfo == nullptr)
    return false;

  bool hasAttr = mono_custom_attrs_has_attr(attrInfo, monoClass->GetMonoClass()) != 0;

  mono_custom_attrs_free(attrInfo);

  return hasAttr;
}

MonoObject* ezMonoMethod::GetAttribute(ezMonoClass* monoClass) const
{
  // TODO - Consider caching custom attributes or just initializing them all at load

  MonoCustomAttrInfo* attrInfo = mono_custom_attrs_from_method(m_pMethod);
  if (attrInfo == nullptr)
    return nullptr;

  MonoObject* foundAttr = nullptr;
  if (mono_custom_attrs_has_attr(attrInfo, monoClass->GetMonoClass()))
    foundAttr = mono_custom_attrs_get_attr(attrInfo, monoClass->GetMonoClass());

  mono_custom_attrs_free(attrInfo);
  return foundAttr;
}

ezMonoMemberVisibility ezMonoMethod::GetVisibility()
{
  uint32_t flags = mono_method_get_flags(m_pMethod, nullptr) & MONO_METHOD_ATTR_ACCESS_MASK;

  if (flags == MONO_METHOD_ATTR_PRIVATE)
    return ezMonoMemberVisibility::Private;
  else if (flags == MONO_METHOD_ATTR_FAM_AND_ASSEM)
    return ezMonoMemberVisibility::ProtectedInternal;
  else if (flags == MONO_METHOD_ATTR_ASSEM)
    return ezMonoMemberVisibility::Internal;
  else if (flags == MONO_METHOD_ATTR_FAMILY)
    return ezMonoMemberVisibility::Protected;
  else if (flags == MONO_METHOD_ATTR_PUBLIC)
    return ezMonoMemberVisibility::Public;

  EZ_ASSERT_ALWAYS(false, "Unknown visibility modifier.");
  return ezMonoMemberVisibility::Private;
}

void ezMonoMethod::CacheSignature() const
{
  MonoMethodSignature* methodSignature = mono_method_signature(m_pMethod);

  MonoType* returnType = mono_signature_get_return_type(methodSignature);
  if (returnType != nullptr)
  {
    MonoClass* returnClass = mono_class_from_mono_type(returnType);
    if (returnClass != nullptr)
      m_pCachedReturnType = ezMonoManager::GetSingleton()->FindClass(returnClass);
  }

  m_CachedParameterCount = (ezUInt32)mono_signature_get_param_count(methodSignature);
  if (m_pCachedParameters != nullptr)
  {
    EZ_DEFAULT_DELETE_RAW_BUFFER(m_pCachedParameters);
    m_pCachedParameters = nullptr;
  }

  if (m_CachedParameterCount > 0)
  {
    m_pCachedParameters = (ezMonoClass**)EZ_DEFAULT_NEW_RAW_BUFFER(ezMonoClass, m_CachedParameterCount * sizeof(ezMonoClass*));

    void* iter = nullptr;
    for (ezUInt32 i = 0; i < m_CachedParameterCount; i++)
    {
      MonoType* curParamType = mono_signature_get_params(methodSignature, &iter);
      MonoClass* rawClass = mono_class_from_mono_type(curParamType);
      m_pCachedParameters[i] = ezMonoManager::GetSingleton()->FindClass(rawClass);
    }
  }

  m_bIsStatic = !mono_signature_is_instance(methodSignature);
  m_bHasCachedSignature = true;
}
