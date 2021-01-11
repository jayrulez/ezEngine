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

ezMonoManager::ezMonoManager()
  : m_SingletonRegistrar(this)
  , m_bIsInitialized(false)
  , m_pRootDomain(nullptr)
  //, m_pScriptDomain(nullptr)
  , m_pCoreLibAssembly(nullptr)
{
}

ezMonoManager::~ezMonoManager()
{
  UnloadAll();
}

void ezMonoManager::Initialize(const ezArrayMap<ezString, ezString>& trustedPlatformAssemblies)
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

void ezMonoManager::UnloadAll()
{
  if (m_bIsInitialized)
  {
    for (auto& assembly : m_Assemblies)
    {
      assembly.value->Unload();
      EZ_DEFAULT_DELETE(assembly.value);
    }

    m_Assemblies.Clear();

    UnloadScriptDomain();

    if (m_pRootDomain)
    {
      mono_jit_cleanup(m_pRootDomain);
      m_pRootDomain = nullptr;
    }

    // Make sure to explicitly clear this meta-data, as it contains structures allocated from other dynamic libraries,
    // which will likely get unloaded right after shutdown
    m_ScriptMetaData.Clear();

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

  //if (m_pScriptDomain == nullptr)
  //{
  //  m_pScriptDomain = mono_domain_create();
  //  if (m_pScriptDomain == nullptr)
  //  {
  //    EZ_REPORT_FAILURE("Failed to create script domain.");
  //    return nullptr;
  //  }

  //  if (!mono_domain_set(m_pScriptDomain, true))
  //  {
  //    EZ_REPORT_FAILURE("Failed to set script domain.");
  //    return nullptr;
  //  }
  //}

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
    InitializeScriptTypes(*assembly);
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

void ezMonoManager::UnloadScriptDomain()
{
  //if (m_pScriptDomain != nullptr)
  //{
  //  mono_domain_set(mono_get_root_domain(), true);

  //  MonoObject* exception = nullptr;
  //  mono_domain_unload(m_pScriptDomain/*, &exception*/);

  //  if (exception != nullptr)
  //    ezMonoUtil::ThrowIfException(exception);

  //  m_pScriptDomain = nullptr;
  //}

  for (auto& assemblyEntry : m_Assemblies)
  {
    assemblyEntry.value->Unload();

    // CoreLib assembly persists domain unload since it's in the root domain. However we make sure to clear its
    // class list as it could contain generic instances that use types from other assemblies.

    //if (ezStringUtils::IsEqual(assemblyEntry.key, m_pCoreLibAssembly->GetName()) != 0)
    //  EZ_DEFAULT_DELETE(assemblyEntry.value);

    // Metas hold references to various assembly objects that were just deleted, so clear them
    ezDynamicArray<ScriptMetaInfo>& typeMetas = m_ScriptMetaData[assemblyEntry.key];
    for (auto& entry : typeMetas)
    {
      entry.m_MetaData->m_ScriptClass = nullptr;
      entry.m_MetaData->m_ScriptClass = nullptr;
    }
  }

  m_Assemblies.Clear();
  //m_Assemblies[m_pCoreLibAssembly->GetName()] = m_pCoreLibAssembly;
}

void ezMonoManager::RegisterScriptType(ezScriptMeta* metaData, const ezScriptMeta& localMetaData)
{
  if (m_bIsInitialized)
  {
    EZ_REPORT_FAILURE("RegisterScriptType cannot be called after the runtime is already initialized.");
    return;
  }

  ezDynamicArray<ScriptMetaInfo>& mMetas = m_ScriptMetaData[localMetaData.m_Assembly];
  mMetas.PushBack({metaData, localMetaData});
}

void ezMonoManager::InitializeScriptTypes(ezMonoAssembly& assembly)
{
  // Fully initialize all types that use this assembly
  ezDynamicArray<ScriptMetaInfo>& typeMetas = m_ScriptMetaData[assembly.m_Name];
  for (auto& entry : typeMetas)
  {
    ezScriptMeta* meta = entry.m_MetaData;
    *meta = entry.m_LocalMetaData;

    meta->m_ScriptClass = assembly.GetClass(meta->m_Namespace, meta->m_Name);
    if (meta->m_ScriptClass == nullptr)
    {
      EZ_REPORT_FAILURE("Unable to find class of type: '{0}::{1}'", meta->m_Namespace, meta->m_Name);
      return;
    }

    if (meta->m_ScriptClass->HasField("mNativeInstance"))
      meta->m_NativeInstance = meta->m_ScriptClass->GetField("mNativeInstance");
    else
      meta->m_NativeInstance = nullptr;

    meta->m_InitCallback();
  }
}


EZ_STATICLINK_FILE(MonoPlugin, MonoPlugin_Implementation_MonoManager);
