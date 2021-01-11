#include <MonoPlugin/MonoPluginDLL.h>
#include <MonoPluginPCH.h>

#include <Foundation/Configuration/Plugin.h>
#include <Foundation/Configuration/Startup.h>
#include <Foundation/Types/UniquePtr.h>
#include <MonoPlugin/Mono/MonoAssembly.h>
#include <MonoPlugin/Mono/MonoClass.h>
#include <MonoPlugin/Mono/MonoField.h>
#include <MonoPlugin/Mono/MonoManager.h>
#include <MonoPlugin/Mono/MonoProperty.h>

static ezUniquePtr<ezMonoManager> s_MonoManager;

// clang-format off
EZ_BEGIN_SUBSYSTEM_DECLARATION(Scripting, MonoPlugin)

  BEGIN_SUBSYSTEM_DEPENDENCIES
    "Core"
  END_SUBSYSTEM_DEPENDENCIES

  ON_CORESYSTEMS_STARTUP
  {
    s_MonoManager = EZ_DEFAULT_NEW(ezMonoManager);

    const char* appDir = ezOSFile::GetApplicationDirectory();
    //ezStringBuilder assembliesDir = ezPathUtils::GetFileDirectory(appDir);
    ezStringBuilder assembliesDir(appDir, "Assemblies/net5.0/");

    ezArrayMap<ezString, ezString> trustedPlatformAssemblies;
    ezStringBuilder engineAssemblyPath = ezStringBuilder(assembliesDir, "ezEngine.Managed.dll");
    engineAssemblyPath.MakePathSeparatorsNative();
    trustedPlatformAssemblies["ezEngine.Managed"] = engineAssemblyPath;
    ezMonoManager::GetSingleton()->Initialize(trustedPlatformAssemblies);

    auto assembly = ezMonoManager::GetSingleton()->LoadAssembly(engineAssemblyPath, "ezEngine.Managed");
    auto classes = assembly->GetAllClasses();
    auto klass = assembly->GetClass("ezEngine.Managed", "Class1");
    auto fields = klass->GetAllFields();
    auto properties = klass->GetAllProperties();
    auto methods = klass->GetAllMethods();
    auto attributes = klass->GetAllAttributes();

    assembly->Invoke("Class1::HelloWorld");
  }

  ON_CORESYSTEMS_SHUTDOWN
  {
    //ezMonoManager::GetSingleton()->UnloadAll();
    s_MonoManager.Clear();
  }

  ON_HIGHLEVELSYSTEMS_STARTUP
  {
  }

  ON_HIGHLEVELSYSTEMS_SHUTDOWN
  {
  }

EZ_END_SUBSYSTEM_DECLARATION;
// clang-format on

//////////////////////////////////////////////////////////////////////////
ezPlugin g_Plugin(false);
//////////////////////////////////////////////////////////////////////////



EZ_STATICLINK_FILE(MonoPlugin, MonoPlugin_Startup);
