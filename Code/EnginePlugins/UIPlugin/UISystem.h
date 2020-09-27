#pragma once

#include <Foundation/Configuration/Singleton.h>
#include <Foundation/Communication/Event.h>
#include <Foundation/Containers/DynamicArray.h>
#include <Core/Input/InputManager.h>

#include <UIPlugin/TGUI/Backend/Backend.hpp>
#include <UIPlugin/TGUI/Backend/BackendFont.hpp>
#include <UIPlugin/TGUI/Backend/BackendText.hpp>
#include <UIPlugin/TGUI/Backend/BackendTexture.hpp>
#include <UIPlugin/TGUI/Backend/BackendRenderTarget.hpp>
#include <UIPlugin/TGUI/Backend/Gui.hpp>

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

protected:
  EZ_MAKE_SUBSYSTEM_STARTUP_FRIEND(Foundation, UISystem);

private:
  void HandleInput(const ezInputManager::InputEventData& eventData);
  void OnInputActionChanged(const ezInputManager::InputEventData& eventData);
void OnInputSlotChanged(const ezInputManager::InputEventData& eventData);
void RegisterInputAction(const char* szAction, const ezInputActionConfig& config);
  
  static ezEventSubscriptionID s_InputHandlerSubscriberID;
};
