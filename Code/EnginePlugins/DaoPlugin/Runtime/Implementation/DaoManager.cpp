#include <DaoPluginPCH.h>
#include <Dao/kernel/dao.h>
#include <Dao/kernel/daoNamespace.h>

#include <DaoPlugin/Runtime/DaoManager.h>

EZ_IMPLEMENT_SINGLETON(ezDaoManager);

ezDaoManager::ezDaoManager()
  : m_SingletonRegistrar{this}
  , m_bInitialized{false}
  , m_pVMSpace{nullptr}
  , m_pEngineNamespace{nullptr}
{
}

ezDaoManager::~ezDaoManager()
{
}

void ezDaoManager::Startup()
{
  if (m_bInitialized)
    return;

  m_pVMSpace = DaoInit(nullptr);
  m_pEngineNamespace = DaoVmSpace_GetNamespace(m_pVMSpace, "ezEngine");
  DaoOnLoad(m_pVMSpace, m_pEngineNamespace);

  DaoNamespace* daoNamespace = DaoVmSpace_GetNamespace(m_pVMSpace, "dao");

  DaoNamespace_AddParent(daoNamespace, m_pEngineNamespace);

  m_bInitialized = true;
}

void ezDaoManager::Shutdown()
{
  if (!m_bInitialized)
    return;

  DaoQuit();

  m_bInitialized = false;
}

void ezDaoManager::Test()
{
  auto x = DaoVmSpace_CurrentLoadingPath(m_pVMSpace);
  auto ns = DaoVmSpace_Load(m_pVMSpace, "E:\\dev\\ez2\\Data\\Samples\\ScriptingSample\\test.dao");
}


EZ_STATICLINK_FILE(DaoPlugin, DaoPlugin_Runtime_Implementation_DaoManager);

