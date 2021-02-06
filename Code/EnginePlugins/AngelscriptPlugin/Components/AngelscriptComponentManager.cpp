#include <AngelscriptPlugin/Components/AngelscriptComponent.h>

ezAngelscriptComponentManager::ezAngelscriptComponentManager(ezWorld* pWorld)
  : SUPER(pWorld)
{
}

ezAngelscriptComponentManager::~ezAngelscriptComponentManager() = default;

void ezAngelscriptComponentManager::Initialize()
{
  SUPER::Initialize();

  auto desc = EZ_CREATE_MODULE_UPDATE_FUNCTION_DESC(ezAngelscriptComponentManager::Update, this);
  desc.m_bOnlyUpdateWhenSimulating = true;
  desc.m_Phase = UpdateFunctionDesc::Phase::PreAsync;

  RegisterUpdateFunction(desc);
}

void ezAngelscriptComponentManager::Deinitialize()
{
  SUPER::Deinitialize();
}

void ezAngelscriptComponentManager::OnSimulationStarted()
{
  SUPER::OnSimulationStarted();
}

void ezAngelscriptComponentManager::Update(const ezWorldModule::UpdateContext& context)
{
  EZ_PROFILE_SCOPE("Angelscript Update");

  for (auto it = this->m_ComponentStorage.GetIterator(context.m_uiFirstComponentIndex, context.m_uiComponentCount); it.IsValid(); ++it)
  {
    if (it->IsActiveAndSimulating())
    {
      it->Update();
    }
  }
}
