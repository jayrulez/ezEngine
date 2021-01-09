//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//

// https://www.mono-project.com/docs/advanced/embedding/
#include <MonoPlugin/Mono/MonoAssembly.h>
#include <MonoPlugin/Mono/MonoManager.h>
#include <MonoPluginPCH.h>

#include "..\MonoManager.h"
#include <mono/jit/jit.h>
#include <mono/jit/mono-private-unstable.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/class.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/threads.h>

EZ_IMPLEMENT_SINGLETON(ezMonoManager);

ezMonoManager::ezMonoManager(const ezArrayMap<ezString, ezString>& trustedPlatformAssemblies)
  : m_SingletonRegistrar(this)
  , m_bIsInitialized(false)
  , m_pRootDomain(nullptr)
  , m_pCoreLibAssembly(nullptr)
{
  if (m_bIsInitialized)
    return;

  ezStringBuilder tpaList(ezOSFile::GetApplicationDirectory(), "System.Private.CoreLib.dll");
  tpaList.MakePathSeparatorsNative();

  for (auto& tpa : trustedPlatformAssemblies)
  {
    tpaList.Append(ezPathUtils::OsSpecificPathStringSeparator);
    tpaList.Append(tpa.value.GetData());
  }

  const char* propertyKeys[] = {"TRUSTED_PLATFORM_ASSEMBLIES"};
  const char* propertyValues[] = {tpaList.GetData()};
  ezInt32 propertyCount = sizeof(propertyKeys) / sizeof(propertyKeys[0]);

  monovm_initialize(propertyCount, (const char**)&propertyKeys, (const char**)&propertyValues);

  const char* monoVersion = "v4.0.30319";
  const char* rootDomainName = "ezMono";

  m_pRootDomain = mono_jit_init_version(rootDomainName, monoVersion);
  if (!m_pRootDomain)
  {
    EZ_REPORT_FAILURE("Failed to initialize Mono runtime.");
    return;
  }

  mono_config_parse(nullptr);

  mono_thread_set_main(mono_thread_current());

  m_pCoreLibAssembly = EZ_DEFAULT_NEW(ezMonoAssembly, "", "System.Private.CoreLib");
  m_pCoreLibAssembly->LoadFromImage(mono_get_corlib());
  m_Assemblies[m_pCoreLibAssembly->GetName()] = m_pCoreLibAssembly;

  m_bIsInitialized = true;
}

ezMonoManager::~ezMonoManager()
{
  Shutdown();
}

void ezMonoManager::Shutdown()
{
  if (m_bIsInitialized)
  {
    for (auto& assembly : m_Assemblies)
    {
      EZ_DEFAULT_DELETE(assembly.value);
    }

    m_Assemblies.Clear();

    if (m_pRootDomain)
    {
      mono_jit_cleanup(m_pRootDomain);
      m_pRootDomain = nullptr;
    }
  }
}

ezMonoAssembly* ezMonoManager::GetAssembly(const ezString& name) const
{
  ezUInt32 assemblyIndex = m_Assemblies.Find(name);
  if (assemblyIndex != ezInvalidIndex)
  {
    return m_Assemblies.GetValue(assemblyIndex);
  }
  return nullptr;
}

ezMonoAssembly* ezMonoManager::LoadAssembly(const ezString& path, const ezString& name)
{
  ezMonoAssembly* assembly = nullptr;

  ezUInt32 index = m_Assemblies.Find(name);
  if (index != ezInvalidIndex)
  {
    assembly = m_Assemblies.GetValue(index);
  }
  else
  {
    assembly = EZ_DEFAULT_NEW(ezMonoAssembly, path, name);
    m_Assemblies[name] = assembly;
  }

  if (!assembly->m_bIsLoaded)
  {
    assembly->Load();
  }

  return assembly;
}

ezMonoClass* ezMonoManager::FindClass(const ezString& classNamespace, const ezString& typeName)
{
  ezMonoClass* monoClass = nullptr;
  for (auto& assembly : m_Assemblies)
  {
    monoClass = assembly.value->GetClass(classNamespace, typeName);
    if (monoClass != nullptr)
      return monoClass;
  }

  return nullptr;
}

ezMonoClass* ezMonoManager::FindClass(MonoClass* monoClass)
{
  ezMonoClass* ezMonoClass = nullptr;
  for (auto& assembly : m_Assemblies)
  {
    ezMonoClass = assembly.value->GetClass(monoClass);
    if (ezMonoClass != nullptr)
      return ezMonoClass;
  }

  return nullptr;
}


EZ_STATICLINK_FILE(MonoPlugin, MonoPlugin_Implementation_MonoManager);
