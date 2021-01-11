//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//

// https://www.mono-project.com/docs/advanced/embedding/
#include <MonoPlugin/Mono/MonoAssembly.h>
#include <MonoPlugin/Mono/MonoManager.h>
#include <MonoPluginPCH.h>
#include <mono/jit/jit.h>
#include <mono/jit/mono-private-unstable.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/class.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/threads.h>
#include <mono/utils/mono-logger.h>

EZ_IMPLEMENT_SINGLETON(ezMonoManager);

void OnMonoLogCallback(const char* logDomain, const char* logLevel, const char* message, mono_bool fatal, void* userData)
{
  static const char* monoErrorLevels[] =
    {
      nullptr,
      "error",
      "critical",
      "warning",
      "message",
      "info",
      "debug"};

  ezUInt32 errorLevel = 0;
  if (logLevel != nullptr)
  {
    for (ezUInt32 i = 1; i < 7; i++)
    {
      if (strcmp(monoErrorLevels[i], logLevel) == 0)
      {
        errorLevel = i;
        break;
      }
    }
  }

  if (errorLevel == 0)
  {
    ezLog::Error("Mono: {0} in domain {1}", message, logDomain);
  }
  else if (errorLevel <= 2)
  {
    ezLog::Error("Mono: {0} in domain {1} [{2}]", message, logDomain, logLevel);
  }
  else if (errorLevel <= 3)
  {
    ezLog::Warning("Mono: {0} in domain {1} [{2}]", message, logDomain, logLevel);
  }
  else
  {
    ezLog::Info("Mono: {0} in domain {1} [{2}]", message, logDomain, logLevel);
  }
}

void OnMonoPrintCallback(const char* string, mono_bool isStdout)
{
  ezLog::Warning("Mono warning: {0}", string);
}

void OnMonoPrintErrorCallback(const char* string, mono_bool isStdout)
{
  ezLog::Error("Mono error: {0}", string);
}

ezMonoManager::ezMonoManager(const ezArrayMap<ezString, ezString>& trustedPlatformAssemblies)
  : m_SingletonRegistrar(this)
  , m_bIsInitialized(false)
  , m_pRootDomain(nullptr)
  , m_pCoreLibAssembly(nullptr)
{
  if (m_bIsInitialized)
    return;

  ezStringBuilder tpaList(ezOSFile::GetApplicationDirectory(), "Mono/System.Private.CoreLib/System.Private.CoreLib.dll");
  tpaList.MakePathSeparatorsNative();

  for (auto& tpa : trustedPlatformAssemblies)
  {
    tpaList.Append(ezPathUtils::OsSpecificPathStringSeparator);
    tpaList.Append(tpa.value.GetData());
  }

  const char* propertyKeys[] = {"TRUSTED_PLATFORM_ASSEMBLIES"};
  const char* propertyValues[] = {tpaList.GetData()};
  ezInt32 propertyCount = sizeof(propertyKeys) / sizeof(propertyKeys[0]);

  ezStringBuilder assembliesDir(ezOSFile::GetApplicationDirectory(), "Mono/runtime");
  assembliesDir.MakePathSeparatorsNative();

  mono_set_assemblies_path(assembliesDir.GetData());

#if EZ_ENABLED(EZ_COMPILE_FOR_DEBUG)
  mono_debug_init(MONO_DEBUG_FORMAT_MONO);

  const char* options[] = {
    "--soft-breakpoints",
    "--debugger-agent=transport=dt_socket,address=127.0.0.1:17615,embedding=1,server=y,suspend=n",
    "--debug-domain-unload",

    // GC options:
    // check-remset-consistency: Makes sure that write barriers are properly issued in native code, and therefore
    //    all old->new generation references are properly present in the remset. This is easy to mess up in native
    //    code by performing a simple memory copy without a barrier, so it's important to keep the option on.
    // verify-before-collections: Unusure what exactly it does, but it sounds like it could help track down
    //    things like accessing released/moved objects, or attempting to release handles for an unloaded domain.
    // xdomain-checks: Makes sure that no references are left when a domain is unloaded.
    "--gc-debug=check-remset-consistency,verify-before-collections,xdomain-checks"};
  mono_jit_parse_options(4, (char**)options);
  mono_trace_set_level_string("warning"); // Note: Switch to "debug" for detailed output, disabled for now due to spam
#else
  mono_trace_set_level_string("warning");
#endif

  mono_trace_set_log_handler(OnMonoLogCallback, this);
  mono_trace_set_print_handler(OnMonoPrintCallback);
  mono_trace_set_printerr_handler(OnMonoPrintErrorCallback);

  mono_config_parse(nullptr);
  monovm_initialize(propertyCount, (const char**)&propertyKeys, (const char**)&propertyValues);

  const char* monoVersion = "v4.0.30319";
  const char* rootDomainName = "ezMono";

  m_pRootDomain = mono_jit_init_version(rootDomainName, monoVersion);
  if (!m_pRootDomain)
  {
    EZ_REPORT_FAILURE("Failed to initialize Mono runtime.");
    return;
  }

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
    m_bIsInitialized = false;
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
