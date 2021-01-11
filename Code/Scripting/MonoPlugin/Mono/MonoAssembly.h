//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include <MonoPlugin/MonoPluginDLL.h>
#include <MonoPlugin/MonoPluginPCH.h>
#include <MonoPlugin/Mono/Declarations.h>

class EZ_MONOPLUGIN_DLL ezMonoAssembly
{
public:
  struct ClassId
  {
    EZ_DECLARE_POD_TYPE();

    ezString Namespace;
    ezString Name;
    MonoClass* GenericInstance;

    ClassId() = default;
    ClassId(const ezString& classNamespace, ezString name, MonoClass* genericInstance = nullptr)
      : Namespace(classNamespace)
      , Name(name)
      , GenericInstance(genericInstance)
    {
    }

    bool operator==(const ClassId& other) const;
    bool operator!=(const ClassId& other) const;
    bool operator<(const ClassId& other) const;
    ezUInt32 GetHashCode() const;
  };

public:
  ~ezMonoAssembly();
  const ezString& GetName() const { return m_Name; }

  ezMonoClass* GetClass(const ezString& classNamespace, const ezString& name) const;
  ezMonoClass* GetClass(MonoClass* monoClass) const;
  const ezDynamicArray<ezMonoClass*>& GetAllClasses() const;
  void Invoke(const ezString& methodName);

private:
  friend class ezMonoManager;
  friend class ezMemoryUtils;

  ezMonoAssembly(const char* path, const ezString& name);
  ezMonoClass* GetClass(const ezString& classNamespace, const ezString& name, MonoClass* monoClass) const;
  void Load();
  void LoadFromImage(MonoImage* image);
  void Unload();
  bool IsGenericClass(const ezString& name) const;

private:



  ezString m_Name;
  const char* m_Path;
  MonoImage* m_pMonoImage;
  MonoAssembly* m_pMonoAssembly;
  ezDynamicArray<ezUInt8> m_DebugData;
  bool m_bIsLoaded;
  bool m_bIsDependency;

  mutable ezArrayMap<ClassId, ezMonoClass*> m_Classes;
  mutable ezArrayMap<MonoClass*, ezMonoClass*> m_ClassesByMonoClass;

  mutable bool m_bHasCachedClasses;
  mutable ezDynamicArray<ezMonoClass*> m_CachedClasses;
};

template <>
struct ezHashHelper<ezMonoAssembly::ClassId>
{
  EZ_ALWAYS_INLINE static ezUInt32 Hash(const ezMonoAssembly::ClassId& value) { return value.GetHashCode(); }

  EZ_ALWAYS_INLINE static bool Equal(const ezMonoAssembly::ClassId& a, const ezMonoAssembly::ClassId& b) { return a == b; }
};
