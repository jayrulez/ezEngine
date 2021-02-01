#pragma once

#include <AngelscriptPlugin/AngelscriptPluginDLL.h>
#include <Foundation/Configuration/Singleton.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Strings/String.h>

#include <Core/Scripting/AngelscriptHelper.h>

#ifdef BUILDSYSTEM_ENABLE_ANGELSCRIPT_SUPPORT

class ezAngelscriptBuilder;
class ezStringFactory;

class EZ_ANGELSCRIPTPLUGIN_DLL ezAngelscriptManager
{
  EZ_DECLARE_SINGLETON(ezAngelscriptManager);

public:
  ezAngelscriptManager();
  ~ezAngelscriptManager();
  void Startup();
  void Shutdown();
  void Test();

private:
  static void RegisterString(asIScriptEngine* scriptEngine);
  static void RegisterArray(asIScriptEngine* scriptEngine);
  static void RegisterCore(asIScriptEngine* scriptEngine);

private:
  ezStringFactory* GetStringFactory() const;

private:
  bool m_Initialized;
  asIScriptEngine* m_pScriptEngine;
  ezAngelscriptBuilder* m_pScriptBuilder;
  ezStringFactory* m_pStringFactory;
};

#endif
