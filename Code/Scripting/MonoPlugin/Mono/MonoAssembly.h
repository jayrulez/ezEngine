#pragma once

#include <MonoPlugin/MonoPluginDLL.h>
#include <MonoPlugin/MonoPluginPCH.h>

class EZ_MONOPLUGIN_DLL ezMonoAssembly
{
private:
  friend class ezMonoManager;
  friend class ezMemoryUtils;

  ezMonoAssembly(const char* path, const ezString& name);
  ezMonoAssembly(MonoImage* image);

public:
  ~ezMonoAssembly();
  const ezString& GetName() const { return m_Name; }

  ezMonoClass* GetClass(const ezString& classNamespace, const ezString& name) const;
  ezMonoClass* GetClass(MonoClass* monoClass) const;
  const ezDynamicArray<ezMonoClass*>& GetClasses() const;
  void Invoke(const ezString& methodName);

private:
  ezMonoClass* GetClass(const ezString& classNamespace, const ezString& name, MonoClass* monoClass) const;
  void Load();
  void Unload();
  bool IsGenericClass(const ezString& name) const;

private:
  MonoAssembly* m_pMonoAssembly;
  MonoImage* m_pMonoImage;

  mutable ezArrayMap<ezString, ezMonoClass*> m_Classes;
  mutable ezArrayMap<ezString, MonoClass*> m_MonoClasses;

  mutable bool m_bHasCachedClasses;
  mutable ezDynamicArray<ezMonoClass*> m_CachedClasses;

  ezString m_Name;
  const char* m_Path;
  bool m_bIsLoaded;
  bool m_bIsDependency;
  ezDynamicArray<ezUInt8> m_DebugData;
};
