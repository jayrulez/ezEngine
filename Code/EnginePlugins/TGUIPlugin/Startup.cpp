#include <TGUIPluginPCH.h>

#include <Core/ResourceManager/ResourceManager.h>
#include <Foundation/Configuration/Startup.h>
#include <TGUIPlugin/UISystem.h>

// clang-format off
EZ_BEGIN_SUBSYSTEM_DECLARATION(UI, UIPlugin)

  BEGIN_SUBSYSTEM_DEPENDENCIES
    "Foundation", 
    "Core",
    "InputManager",
    "RendererCore"
  END_SUBSYSTEM_DEPENDENCIES

  ON_CORESYSTEMS_STARTUP
  {

  }

  ON_CORESYSTEMS_SHUTDOWN
  {
  }

  ON_HIGHLEVELSYSTEMS_STARTUP
  {
    ezUISystem::GetSingleton()->Startup();
  }

  ON_HIGHLEVELSYSTEMS_SHUTDOWN
  {
    ezUISystem::GetSingleton()->Shutdown();
  }

EZ_END_SUBSYSTEM_DECLARATION;
// clang-format on


EZ_STATICLINK_FILE(UIPlugin, UIPlugin_Startup);
