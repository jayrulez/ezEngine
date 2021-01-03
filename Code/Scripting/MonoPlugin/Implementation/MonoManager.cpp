// https://www.mono-project.com/docs/advanced/embedding/

#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/IO/OSFile.h>
#include <MonoPlugin/Mono/MonoAssembly.h>
#include <MonoPlugin/MonoManager.h>
#include <MonoPluginPCH.h>

#include <mono/jit/jit.h>
#include <mono/jit/mono-private-unstable.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/class.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/threads.h>

EZ_IMPLEMENT_SINGLETON(ezMonoManager);

ezMonoManager::ezMonoManager()
  : m_SingletonRegistrar(this)
  , m_bIsInitialized(false)
  , m_pRootDomain(nullptr)
  , m_pEngineDomain(nullptr)
{
}

ezMonoManager::~ezMonoManager()
{
}

void ezMonoManager::Startup()
{
  if (m_bIsInitialized)
    return;

  ezStringBuilder tpaList(ezOSFile::GetApplicationDirectory(), "System.Private.CoreLib.dll");
  tpaList.MakePathSeparatorsNative();

  const char* propertyKeys[] = {"TRUSTED_PLATFORM_ASSEMBLIES"};
  const char* propertyValues[] = {tpaList.GetData()};
  ezInt32 propertyCount = sizeof(propertyKeys) / sizeof(propertyKeys[0]);

  monovm_initialize(propertyCount, (const char**)&propertyKeys, (const char**)&propertyValues);

  const char* monoVersion = "v4.0.30319";
  const char* rootDomainName = "ezMonoRoot";

  m_pRootDomain = mono_jit_init_version(rootDomainName, monoVersion);
  if (!m_pRootDomain)
  {
    EZ_REPORT_FAILURE("Failed to initialize Mono runtime.");
    return;
  }

  mono_config_parse(nullptr);

  mono_thread_set_main(mono_thread_current());

  m_Assemblies.Insert("System.Private.CoreLib", EZ_DEFAULT_NEW(ezMonoAssembly, mono_get_corlib()));

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

    //if (m_pEngineDomain)
    //{
    //  mono_jit_cleanup(m_pEngineDomain);
    //  m_pEngineDomain = nullptr;
    //}

    if (m_pRootDomain)
    {
      mono_jit_cleanup(m_pRootDomain);
      m_pRootDomain = nullptr;
    }
  }
}

ezMonoAssembly* ezMonoManager::LoadAssembly(const ezString& path, const ezString& name)
{
  ezMonoAssembly* assembly = nullptr;

  if (m_pEngineDomain == nullptr)
  {
    m_pEngineDomain = mono_domain_create();
    if (!m_pEngineDomain)
    {
      EZ_REPORT_FAILURE("Failed to create domain.");
      return nullptr;
    }

    if (!mono_domain_set(m_pEngineDomain, true))
    {
      EZ_REPORT_FAILURE("Failed to set domain.");
      return nullptr;
    }
  }

  ezUInt32 index = m_Assemblies.Find(name);
  if (index != ezInvalidIndex)
  {
    assembly = m_Assemblies.GetValue(index);
  }
  else
  {
    assembly = EZ_DEFAULT_NEW(ezMonoAssembly, path, name);
    m_Assemblies.Insert(name, assembly);
  }

  if (!assembly->m_bIsLoaded)
  {
    assembly->Load();
  }

  return assembly;
}


EZ_STATICLINK_FILE(MonoPlugin, MonoPlugin_Implementation_MonoManager);
