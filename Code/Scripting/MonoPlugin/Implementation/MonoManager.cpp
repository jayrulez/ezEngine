// https://www.mono-project.com/docs/advanced/embedding/
#include <MonoPlugin/Mono/MonoAssembly.h>
#include <MonoPlugin/MonoManager.h>
#include <MonoPluginPCH.h>

#include <mono/jit/jit.h>
#include <mono/jit/mono-private-unstable.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/class.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/threads.h>
#include "..\MonoManager.h"

EZ_IMPLEMENT_SINGLETON(ezMonoManager);

ezMonoManager::ezMonoManager()
  : m_SingletonRegistrar(this)
  , m_bIsInitialized(false)
  , m_pRootDomain(nullptr)
{
}

ezMonoManager::~ezMonoManager()
{
}

void ezMonoManager::Startup(const ezArrayMap<ezString, ezString>& trustedPlatformAssemblies)
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


EZ_STATICLINK_FILE(MonoPlugin, MonoPlugin_Implementation_MonoManager);
