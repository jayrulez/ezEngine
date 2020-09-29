#pragma once

#include <Core/Input/InputManager.h>
#include <Foundation/Communication/Event.h>
#include <Foundation/Configuration/Singleton.h>
#include <Foundation/Containers/DynamicArray.h>
#include <RendererCore/RenderWorld/RenderWorld.h>

#include <TGUIPlugin/Backend/Backend.hpp>
#include <TGUIPlugin/Backend/BackendFont.hpp>
#include <TGUIPlugin/Backend/BackendRenderTarget.hpp>
#include <TGUIPlugin/Backend/BackendText.hpp>
#include <TGUIPlugin/Backend/BackendTexture.hpp>
#include <TGUIPlugin/Backend/Gui.hpp>

#include <TGUIPlugin/TGUI/TGUI.hpp>

#include <TGUIPlugin/TGUIPluginDLL.h>

class EZ_TGUIPLUGIN_DLL ezUISystem
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
