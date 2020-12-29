#include <DotNetPlugin/DotNetPluginDLL.h>
#include <DotNetPluginPCH.h>

#include <Foundation/Configuration/Plugin.h>
#include <Foundation/Configuration/Startup.h>
#include <Foundation/Types/UniquePtr.h>

#include <DotNetPlugin/Hosting/DotNetHost.h>

static ezUniquePtr<ezDotNetHost> s_DotNetHost;

// clang-format off
EZ_BEGIN_SUBSYSTEM_DECLARATION(DotNet, DotNetPlugin)

  BEGIN_SUBSYSTEM_DEPENDENCIES
    "Foundation",
    "Core"
  END_SUBSYSTEM_DEPENDENCIES

  ON_CORESYSTEMS_STARTUP
  {
    s_DotNetHost = EZ_DEFAULT_NEW(ezDotNetHost);
    ezDotNetHost::GetSingleton()->Startup();
  }

  ON_CORESYSTEMS_SHUTDOWN
  {
    ezDotNetHost::GetSingleton()->Shutdown();
    s_DotNetHost.Clear();
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



EZ_STATICLINK_FILE(DotNetPlugin, DotNetPlugin_Startup);
