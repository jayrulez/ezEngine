#include <ScriptEngine/WorldModule/ScriptEngineWorldModule.h>
#include <ScriptEnginePCH.h>

// clang-format off
EZ_IMPLEMENT_WORLD_MODULE(ezScriptEngineWorldModule);
EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezScriptEngineWorldModule, 1, ezRTTINoAllocator)
EZ_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

ezScriptEngineWorldModule::ezScriptEngineWorldModule(ezWorld* pWorld)
  : ezWorldModule(pWorld)
{
}

ezScriptEngineWorldModule::~ezScriptEngineWorldModule()
{
}

void ezScriptEngineWorldModule::Initialize()
{
}

void ezScriptEngineWorldModule::Deinitialize()
{
}


EZ_STATICLINK_FILE(ScriptEngine, ScriptEngine_WorldModule_Implementation_ScriptEngineWorldModule);

