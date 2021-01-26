#pragma once

#include <Core/CoreDLL.h>
#include <Foundation/Configuration/Singleton.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Strings/String.h>
#include <Foundation/Time/Timestamp.h>

#include <HashLink/src/hl.h>
extern "C"
{
#include <HashLink/src/hlmodule.h>
}

#ifdef BUILDSYSTEM_ENABLE_HASHLINK_SUPPORT

class EZ_CORE_DLL ezHashLinkManager
{
  EZ_DECLARE_SINGLETON(ezHashLinkManager);

public:
  ezHashLinkManager();
  ~ezHashLinkManager();
  void Startup(ezString file, bool debugWait = false);
  void Shutdown();
  ezResult Run();
  void Test();
  void Test2();

private:
  friend static bool CheckReload(ezHashLinkManager* pHashLinkManager);

private:
  bool m_Initialized;
  hl_code* m_pCode;
  hl_module* m_pModule;
  ezString m_File;
  ezTimestamp m_FileModifiedTime;
  ezInt32 m_DebugPort;
  bool m_bEnableHotReload;
};

#endif
