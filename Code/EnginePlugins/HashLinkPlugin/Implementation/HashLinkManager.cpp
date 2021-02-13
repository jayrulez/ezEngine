#include <Core/Scripting/HashLinkHelper.h>
#include <Foundation/IO/FileSystem/FileReader.h>
#include <Foundation/IO/OSFile.h>
#include <HashLinkPlugin/HashLinkManager.h>

#ifdef BUILDSYSTEM_ENABLE_HASHLINK_SUPPORT

HL_PRIM hl_type hlt_i32 = {HI32};

EZ_IMPLEMENT_SINGLETON(ezHashLinkManager);

ezHashLinkManager::ezHashLinkManager()
  : m_SingletonRegistrar{this}
  , m_pHelper{nullptr}
{
  m_pHelper = EZ_DEFAULT_NEW(ezHashLinkHelper);
}

ezHashLinkManager::~ezHashLinkManager()
{
  EZ_DEFAULT_DELETE(m_pHelper);
  m_pHelper = nullptr;
}

void ezHashLinkManager::Startup(const ezString& file)
{
  if (!m_pHelper->IsInitialized())
    m_pHelper->Initialize(file);
}

void ezHashLinkManager::Shutdown()
{
  if (m_pHelper != nullptr)
  {
    if (m_pHelper->IsInitialized())
      m_pHelper->Shutdown();
  }
}

void ezHashLinkManager::Test()
{
  if (m_pHelper->IsInitialized())
  {
    ezDynamicArray<vdynamic*> args;
    vdynamic* arg1 = hl_alloc_dynamic(&hlt_i32);
    arg1->v.i = 15;
    vdynamic* arg2 = hl_alloc_dynamic(&hlt_i32);
    arg2->v.i = 134;

    args.PushBack(arg1);
    args.PushBack(arg2);
    vdynamic* obj = m_pHelper->CreateInstance("test2x.TestClass", args);
    if (obj != nullptr)
    {
      vdynamic* ret = m_pHelper->CallMethod(obj, "execute");
    }
  }
}

#endif
