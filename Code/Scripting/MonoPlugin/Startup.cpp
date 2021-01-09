#include <MonoPlugin/MonoPluginDLL.h>
#include <MonoPluginPCH.h>

#include <Foundation/Configuration/Plugin.h>
#include <Foundation/Configuration/Startup.h>
#include <Foundation/Types/UniquePtr.h>
#include <MonoPlugin/Mono/MonoManager.h>

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

    ezArrayMap<ezString, ezString> trustedPlatformAssemblies;
    trustedPlatformAssemblies["ezEngine.Managed"] = "ezEngine.Managed.dll";
    
    s_MonoManager = EZ_DEFAULT_NEW(ezMonoManager, trustedPlatformAssemblies);
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
