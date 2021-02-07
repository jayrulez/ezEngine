#include <HashLinkPluginPCH.h>

#include <Foundation/Configuration/Startup.h>
#include <Foundation/Types/UniquePtr.h>
#include <HashLinkPlugin/HashLinkManager.h>

//static ezUniquePtr<ezHashLinkManager> s_HashLinkManager;

// clang-format off
EZ_BEGIN_SUBSYSTEM_DECLARATION(HashLink, HashLinkPlugin)

  BEGIN_SUBSYSTEM_DEPENDENCIES
    "Foundation",
    "Core"
  END_SUBSYSTEM_DEPENDENCIES

  ON_CORESYSTEMS_STARTUP
  {
    //s_HashLinkManager = EZ_DEFAULT_NEW(ezHashLinkManager);
  }

  ON_CORESYSTEMS_SHUTDOWN
  {
    //s_HashLinkManager->Shutdown();
  }

  ON_HIGHLEVELSYSTEMS_STARTUP
  {
    //s_HashLinkManager->Startup("E:\\dev\\HaxeProjects\\test2\\New Project\\bin\\NewProject.hl");
    //s_HashLinkManager->Run().IgnoreResult();
  }

  ON_HIGHLEVELSYSTEMS_SHUTDOWN
  {
  }

EZ_END_SUBSYSTEM_DECLARATION;
// clang-format on
