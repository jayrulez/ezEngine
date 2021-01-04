#include <MonoPlugin/MonoPluginDLL.h>
#include <MonoPluginPCH.h>

#include <Foundation/Configuration/Plugin.h>
#include <Foundation/Configuration/Startup.h>
#include <Foundation/Types/UniquePtr.h>
#include <MonoPlugin/MonoManager.h>

static ezUniquePtr<ezMonoManager> s_MonoManager;

// clang-format off
EZ_BEGIN_SUBSYSTEM_DECLARATION(Scripting, MonoPlugin)

  BEGIN_SUBSYSTEM_DEPENDENCIES
    "Core"
  END_SUBSYSTEM_DEPENDENCIES

  ON_CORESYSTEMS_STARTUP
  {
    const char* appDir = ezOSFile::GetApplicationDirectory();
    const ezStringView assembliesDir = ezPathUtils::GetFileDirectory(appDir);

    s_MonoManager = EZ_DEFAULT_NEW(ezMonoManager);
    ezArrayMap<ezString, ezString> trustedPlatformAssemblies;
    trustedPlatformAssemblies.Insert("ezEngine.Managed", "ezEngine.Managed.dll");

    ezMonoManager::GetSingleton()->Startup(trustedPlatformAssemblies);
    //ezMonoManager::GetSingleton()->LoadAssembly("", "");
  }

  ON_CORESYSTEMS_SHUTDOWN
  {
    ezMonoManager::GetSingleton()->Shutdown();
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
