#pragma once

#include <HashLinkPlugin/HashLinkPluginDLL.h>
#include <Foundation/Configuration/Singleton.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Strings/String.h>
#include <Foundation/Time/Timestamp.h>

#include <Core/Scripting/HashLinkHelper.h>

#ifdef BUILDSYSTEM_ENABLE_HASHLINK_SUPPORT

class EZ_HASHLINKPLUGIN_DLL ezHashLinkManager
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
  friend static bool check_reload(ezHashLinkManager* pHashLinkManager);

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
