#include <DotNetPluginPCH.h>
#include <DotNetPlugin/DotNetPluginDLL.h>

EZ_STATICLINK_LIBRARY(DotNetPlugin)
{
  if (bReturn)
    return;

  EZ_STATICLINK_REFERENCE(DotNetPlugin_Hosting_Implementation_DotNetHost);
  EZ_STATICLINK_REFERENCE(DotNetPlugin_Startup);
}
