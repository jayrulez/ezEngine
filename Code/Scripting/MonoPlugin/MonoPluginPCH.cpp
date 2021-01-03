#include <MonoPluginPCH.h>
#include <MonoPlugin/MonoPluginDLL.h>

EZ_STATICLINK_LIBRARY(MonoPlugin)
{
  if (bReturn)
    return;

  EZ_STATICLINK_REFERENCE(MonoPlugin_Implementation_MonoManager);
  EZ_STATICLINK_REFERENCE(MonoPlugin_Startup);
}
