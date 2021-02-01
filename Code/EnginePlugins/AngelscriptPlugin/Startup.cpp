#include <AngelscriptPluginPCH.h>

#include <AngelscriptPlugin/AngelscriptManager.h>
#include <Foundation/Configuration/Startup.h>
#include <Foundation/Types/UniquePtr.h>

static ezUniquePtr<ezAngelscriptManager> s_AngelscriptManager;

// clang-format off
EZ_BEGIN_SUBSYSTEM_DECLARATION(Angelscript, AngelscriptPlugin)

  BEGIN_SUBSYSTEM_DEPENDENCIES
    "Foundation",
    "Core"
  END_SUBSYSTEM_DEPENDENCIES

  ON_CORESYSTEMS_STARTUP
  {
    s_AngelscriptManager = EZ_DEFAULT_NEW(ezAngelscriptManager);
    s_AngelscriptManager->Startup();
  }

  ON_CORESYSTEMS_SHUTDOWN
  {
    s_AngelscriptManager->Shutdown();
  }

  ON_HIGHLEVELSYSTEMS_STARTUP
  {
  }

  ON_HIGHLEVELSYSTEMS_SHUTDOWN
  {
  }

EZ_END_SUBSYSTEM_DECLARATION;
// clang-format on
