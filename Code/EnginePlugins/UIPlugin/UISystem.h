#pragma once

#include <Core/Input/InputManager.h>
#include <Foundation/Communication/Event.h>
#include <Foundation/Configuration/Singleton.h>
#include <Foundation/Containers/DynamicArray.h>
#include <RendererCore/RenderWorld/RenderWorld.h>

#include <UIPlugin/TGUI/Backend/Backend.hpp>
#include <UIPlugin/TGUI/Backend/BackendFont.hpp>
#include <UIPlugin/TGUI/Backend/BackendRenderTarget.hpp>
#include <UIPlugin/TGUI/Backend/BackendText.hpp>
#include <UIPlugin/TGUI/Backend/BackendTexture.hpp>
#include <UIPlugin/TGUI/Backend/Gui.hpp>

#include <UIPlugin/TGUI/TGUI.hpp>

#include <UIPlugin/UIPluginDLL.h>

class EZ_UIPLUGIN_DLL ezUISystem
{
  EZ_DECLARE_SINGLETON(ezUISystem);

public:
  ezUISystem();
  virtual ~ezUISystem();

  void Startup();
  void Shutdown();

  void Update();
  void Draw(const ezRenderWorldRenderEvent& e);

  std::shared_ptr<tgui::Gui> GetGui() const
  {
    return m_Gui;
  }

private:
  void HandleInput(const ezInputManager::InputEventData& eventData);

private:
  std::shared_ptr<tgui::Gui> m_Gui = nullptr;
  static ezEventSubscriptionID s_InputHandlerSubscriberID;
  static ezEventSubscriptionID s_RenderEventSubscriberID;

protected:
  EZ_MAKE_SUBSYSTEM_STARTUP_FRIEND(Foundation, UISystem);
};
