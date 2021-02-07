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
  void Startup(const ezString& file);
  void Shutdown();
  void Test();

private:
  ezHashLinkHelper* m_pHelper;
};

#endif
