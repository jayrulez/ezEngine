#pragma once

#include <Core/CoreDLL.h>
#include <Foundation/Configuration/Singleton.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Strings/String.h>
#include <Foundation/Time/Timestamp.h>
#include <HashLink/src/hl.h>
#include <HashLink/src/hlmodule.h>

#ifdef BUILDSYSTEM_ENABLE_HASHLINK_SUPPORT


struct ezHLContext
{
  EZ_DECLARE_POD_TYPE();

  hl_code* code;
  hl_module* module;
  vdynamic* ret;
  const char* filePath;
  ezTimestamp fileModifiedTime;
};

class EZ_CORE_DLL ezHashLinkManager
{
  EZ_DECLARE_SINGLETON(ezHashLinkManager);

public:
  ezHashLinkManager();
  ~ezHashLinkManager();
  void Startup();
  void Shutdown();
  ezResult RunModule(const char* modulePath, bool hotReload = false, bool debug = false);

private:
  bool m_Initialized;
  ezDynamicArray<ezHLContext> m_pContexts;
};

#endif
