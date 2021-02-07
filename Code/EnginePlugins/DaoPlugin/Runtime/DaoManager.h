#pragma once

#include <Foundation/Configuration/Singleton.h>

#include <DaoPlugin/DaoPluginDLL.h>

#ifdef BUILDSYSTEM_ENABLE_DAO_SUPPORT

struct DaoVmSpace;
struct DaoNamespace;

class EZ_DAOPLUGIN_DLL ezDaoManager
{
  EZ_DECLARE_SINGLETON(ezDaoManager);

public:
  ezDaoManager();
  ~ezDaoManager();
  void Startup();
  void Shutdown();
  void Test();

private:
  bool m_bInitialized;
  DaoVmSpace* m_pVMSpace;
  DaoNamespace* m_pEngineNamespace;
};

#endif
