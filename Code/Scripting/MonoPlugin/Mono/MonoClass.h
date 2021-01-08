#pragma once

#include <MonoPlugin/MonoPluginDLL.h>
#include <MonoPlugin/MonoPluginPCH.h>

class EZ_MONOPLUGIN_DLL ezMonoClass
{
public:
  struct MethodId
  {
    EZ_DECLARE_POD_TYPE();

    ezString Name;
    ezUInt32 ParameterCount;

    MethodId() = default;
    MethodId(const ezString& name, ezUInt32 parameterCount)
      : Name(name)
      , ParameterCount(parameterCount)
    {
    }

    bool operator==(const ezMonoClass::MethodId& other) const;
    bool operator!=(const ezMonoClass::MethodId& other) const;
    bool operator<(const ezMonoClass::MethodId& other) const;
    ezUInt32 GetHashCode() const;
  };

public:
  ~ezMonoClass();

  const ezString& GetNamespace() const { return m_Namespace; }
  const ezString& GetTypeName() const { return m_TypeName; }
  const ezString& GetFullName() const { return m_FullName; }

  ezMonoMethod* GetMethod(const ezString& name, ezUInt32 parameterCount = 0) const;
  ezMonoMethod* GetMethodWithSignature(const ezString& name, const ezString& signature) const;
  const ezDynamicArray<ezMonoMethod*>& GetAllMethods() const;
  MonoObject* InvokeMethod(const ezString& name, MonoObject* instance = nullptr, void** params = nullptr, ezUInt32 parameterCount = 0);

  bool HasField(const ezString& name) const;
  ezMonoField* GetField(const ezString& name) const;
  const ezDynamicArray<ezMonoField*>& GetAllFields() const;

  ezMonoProperty* GetProperty(const ezString& name) const;
  const ezDynamicArray<ezMonoProperty*>& GetAllProperties() const;

  bool HasAttribute(ezMonoClass* monoClass) const;
  MonoObject* GetAttribute(ezMonoClass* monoClass) const;
  ezDynamicArray<ezMonoClass*> GetAllAttributes() const;

  ezMonoClass* GetBaseClass() const;

  bool IsSubClassOf(const ezMonoClass* monoClass) const;

  bool IsInstanceOfType(MonoObject* object) const;

  ezUInt32 GetInstanceSize() const;


  void AddInternalCall(const ezString& name, const void* method);

  MonoObject* CreateInstance(bool construct = true) const;

  MonoObject* CreateInstance(void** params, ezUInt32 parameterCount);

  MonoObject* CreateInstance(const ezString& ctorSignature, void** params);

  /**	Returns the internal mono representation of the class. */
  MonoClass* GetMonoClass() const { return m_pMonoClass; }

  static void Construct(MonoObject* object);

private:
  friend class ezMonoAssembly;
  friend class ezMemoryUtils;

  ezMonoClass(const ezString& classNamespace, const ezString& type, MonoClass* monoClass, const ezMonoAssembly* parentAssembly);

private:
  MonoClass* m_pMonoClass;

  const ezMonoAssembly* m_pParentAssembly;
  ezString m_Namespace;
  ezString m_TypeName;
  ezString m_FullName;

  mutable ezArrayMap<ezString, ezMonoMethod*> m_Methods;
  mutable ezArrayMap<ezString, ezMonoField*> m_Fields;
  mutable ezArrayMap<ezString, ezMonoProperty*> m_Properties;

  mutable bool m_bHasCachedFields;
  mutable ezDynamicArray<ezMonoField*> m_CachedFieldList;

  mutable bool m_bHasCachedProperties;
  mutable ezDynamicArray<ezMonoProperty*> m_CachedPropertyList;

  mutable bool m_bHasCachedMethods;
  mutable ezDynamicArray<ezMonoMethod*> m_CachedMethodList;
};

template <>
struct ezHashHelper<ezMonoClass::MethodId>
{
  EZ_ALWAYS_INLINE static ezUInt32 Hash(const ezMonoClass::MethodId& value) { return value.GetHashCode(); }

  EZ_ALWAYS_INLINE static bool Equal(const ezMonoClass::MethodId& a, const ezMonoClass::MethodId& b) { return a == b; }
};
