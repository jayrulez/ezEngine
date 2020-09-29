#include <Foundation/Logging/Log.h>
#include <UIPlugin/UISystem.h>
#include <functional>

EZ_IMPLEMENT_SINGLETON(ezUISystem);

ezEventSubscriptionID ezUISystem::s_InputHandlerSubscriberID = 0;
ezEventSubscriptionID ezUISystem::s_RenderEventSubscriberID = 0;

ezUISystem::ezUISystem()
  : m_SingletonRegistrar(this)
{
}

ezUISystem::~ezUISystem()
{
}

void ezUISystem::Startup()
{
  if (!tgui::isBackendSet())
  {
    tgui::setBackend(std::make_shared<tgui::Backend>());
    tgui::getBackend()->setDestroyOnLastGuiDetatch(true);
  }

  ezView* pView = nullptr;

  ezRenderWorld::CreateView("UIView", pView);

  m_Gui = std::make_shared<tgui::Gui>(pView);

  s_InputHandlerSubscriberID = ezInputManager::AddEventHandler(std::bind(&ezUISystem::HandleInput, this, std::placeholders::_1));

  s_RenderEventSubscriberID = ezRenderWorld::GetRenderEvent().AddEventHandler(std::bind(&ezUISystem::Draw, this, std::placeholders::_1));
}

void ezUISystem::Shutdown()
{
  if (s_RenderEventSubscriberID != 0)
    ezRenderWorld::GetRenderEvent().RemoveEventHandler(s_RenderEventSubscriberID);

  if (s_InputHandlerSubscriberID != 0)
    ezInputManager::RemoveEventHandler(s_InputHandlerSubscriberID);

  tgui::setBackend(nullptr);
}

void ezUISystem::Update()
{
  m_Gui->updateTime();
}

void ezUISystem::Draw(const ezRenderWorldRenderEvent& e)
{
  if (e.m_Type == ezRenderWorldRenderEvent::Type::EndRender)
    m_Gui->draw();
}

void ezUISystem::HandleInput(const ezInputManager::InputEventData& eventData)
{
}
