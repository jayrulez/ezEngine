#pragma once

#include <MonoPlugin/MonoPluginDLL.h>
#include <MonoPlugin/MonoPluginPCH.h>

class EZ_MONOPLUGIN_DLL ezMonoClass
{
public:
  ~ezMonoClass();

  const ezString& GetNamespace() const { return m_Namespace; }
  const ezString& GetTypeName() const { return m_TypeName; }
  const ezString& GetFullName() const { return m_FullName; }

  ezMonoMethod* GetMethod(const ezString& name, ezUInt32 parameterCount = 0) const;
  ezMonoField* GetField(const ezString& name) const;
  ezMonoField* GetProperty(const ezString& name) const;
  MonoObject* GetAttribute(ezMonoClass* monoClass) const;
  ezMonoClass* GetBaseClass() const;

  ezMonoMethod* GetMethodWithSignature(const ezString& name, const ezString& signature) const;

  const ezDynamicArray<ezMonoField*>& GetAllFields() const;

  const ezDynamicArray<ezMonoProperty*>& GetAllProperties() const;

  const ezDynamicArray<ezMonoMethod*>& GetAllMethods() const;

  ezDynamicArray<ezMonoClass*> GetAllAttributes() const;

  bool HasAttribute(ezMonoClass* monoClass) const;

  bool HasField(const ezString& name) const;

  bool IsSubClassOf(const ezMonoClass* monoClass) const;

  bool IsInstanceOfType(MonoObject* object) const;

  ezUInt32 GetInstanceSize() const;

  MonoObject* InvokeMethod(const ezString& name, MonoObject* instance = nullptr, void** params = nullptr, ezUInt32 parameterCount = 0);

  void AddInternalCall(const ezString& name, const void* method);

  MonoObject* CreateInstance(bool construct = true) const;

  MonoObject* CreateInstance(void** params, ezUInt32 parameterCount);

  MonoObject* CreateInstance(const ezString& ctorSignature, void** params);

  /**	Returns the internal mono representation of the class. */
  ::MonoClass* GetMonoClass() const { return m_pMonoClass; }

  static void construct(MonoObject* object);

private:
  friend class ezMonoAssembly;

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
