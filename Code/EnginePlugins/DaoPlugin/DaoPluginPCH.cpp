#include <DaoPluginPCH.h>

#include <DaoPlugin/DaoPluginDLL.h>

#include <Foundation/Configuration/Plugin.h>

ezPlugin g_Plugin(false);

EZ_STATICLINK_LIBRARY(DaoPlugin)
{
  if (bReturn)
    return;

  EZ_STATICLINK_REFERENCE(DaoPlugin_Bindings_Implementation_Log);
  EZ_STATICLINK_REFERENCE(DaoPlugin_Runtime_Implementation_DaoManager);
  EZ_STATICLINK_REFERENCE(DaoPlugin_Startup);
}
