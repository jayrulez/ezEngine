#include <ScriptEnginePCH.h>

EZ_STATICLINK_LIBRARY(ScriptEngine)
{
  if (bReturn)
    return;

  EZ_STATICLINK_REFERENCE(ScriptEngine_WorldModule_Implementation_ScriptEngineWorldModule);
}
