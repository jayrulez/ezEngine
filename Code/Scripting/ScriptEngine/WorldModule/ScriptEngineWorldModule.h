#pragma once

#include <Core/World/WorldModule.h>
#include <ScriptEngine/ScriptEngineDLL.h>

class EZ_SCRIPTENGINE_DLL ezScriptEngineWorldModule : public ezWorldModule
{
  EZ_DECLARE_WORLD_MODULE();
  EZ_ADD_DYNAMIC_REFLECTION(ezScriptEngineWorldModule, ezWorldModule);

public:
  ezScriptEngineWorldModule(ezWorld* pWorld);
  ~ezScriptEngineWorldModule();

  virtual void Initialize() override;
  virtual void Deinitialize() override;
};
