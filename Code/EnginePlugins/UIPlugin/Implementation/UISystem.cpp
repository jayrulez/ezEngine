#include <Foundation/Logging/Log.h>
#include <UIPlugin/UISystem.h>
#include <functional>

EZ_IMPLEMENT_SINGLETON(ezUISystem);

ezEventSubscriptionID ezUISystem::s_InputHandlerSubscriberID = 0;

ezUISystem::ezUISystem()
  : m_SingletonRegistrar(this)
{
}

ezUISystem::~ezUISystem()
{
}

void ezUISystem::Startup()
{
  ezInputActionConfig cfg;

  //cfg.m_sInputSlotTrigger[0] = ezInputSlot_MouseButton0;
  //RegisterInputAction("MouseLeftClick", cfg);

  cfg.m_sInputSlotTrigger[0] = ezInputSlot_MouseDblClick0;
  RegisterInputAction("MouseLeftDoubleClick", cfg);

  s_InputHandlerSubscriberID = ezInputManager::AddEventHandler(std::bind(&ezUISystem::HandleInput, this, std::placeholders::_1));
}

void ezUISystem::Shutdown()
{
  if (s_InputHandlerSubscriberID != 0)
    ezInputManager::RemoveEventHandler(s_InputHandlerSubscriberID);
}

void ezUISystem::Update()
{
}

void ezUISystem::HandleInput(const ezInputManager::InputEventData& eventData)
{
  switch (eventData.m_EventType)
  {
    case ezInputManager::InputEventData::InputActionChanged:
      OnInputActionChanged(eventData);
      break;
    case ezInputManager::InputEventData::InputSlotChanged:
      OnInputSlotChanged(eventData);
      break;

    default:
      break;
  }
}

void ezUISystem::OnInputActionChanged(const ezInputManager::InputEventData& eventData)
{
  ezLog::Debug("UIPlugin: Action inputSet:{} inputAction:{}", eventData.m_szInputSet, eventData.m_szInputAction);
}

void ezUISystem::OnInputSlotChanged(const ezInputManager::InputEventData& eventData)
{
  //ezLog::Debug("UIPlugin: Slot inputSlot:{} inputSet:{}}", eventData.m_szInputSlot, eventData.m_szInputSet);
}

void ezUISystem::RegisterInputAction(const char* szAction, const ezInputActionConfig& config)
{
  ezInputManager::SetInputActionConfig("UI", szAction, config, true);
}
