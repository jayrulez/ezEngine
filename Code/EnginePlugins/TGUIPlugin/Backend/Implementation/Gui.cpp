/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <TGUIPlugin/TGUI/Backend.hpp>
#include <TGUIPlugin/Backend/Gui.hpp>
#include <TGUIPlugin/TGUI/Timer.hpp>
#include <TGUIPlugin/TGUI/ToolTip.hpp>

#include <TGUIPlugin/Backend/Backend.hpp>
#include <TGUIPlugin/Backend/BackendRenderTarget.hpp>

#include <RendererCore/RenderWorld/RenderWorld.h>
#include <thread>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  static bool ConvertEventDataToEvent(const ezInputManager::InputEventData& eventData, Event& event)
  {
    return false;
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  Gui::Gui()
  {
    init();
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  Gui::Gui(ezView* view)
  {
    init();
    setView(view);
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  void Gui::setView(ezView* view)
  {
    std::shared_ptr<Backend> backend = std::dynamic_pointer_cast<Backend>(getBackend());
    m_renderTarget = backend->createGuiRenderTarget(this, view);

    updateContainerSize();
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  bool Gui::handleEvent(const ezInputManager::InputEventData& eventData)
  {

    if (eventData.m_EventType != ezInputManager::InputEventData::InputSlotChanged)
      return false;

    Event event;
    if (!ConvertEventDataToEvent(eventData, event))
      return false;

    return handleEvent(event);
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  void Gui::draw()
  {
    if (m_drawUpdatesTime)
      updateTime();

    EZ_ASSERT_ALWAYS(m_renderTarget != nullptr, "Gui must be given an ezView (either at construction or via setView function) before calling draw()");
    m_renderTarget->drawGui(m_container);
  }

  void Gui::init()
  {
    if (!isBackendSet())
    {
      setBackend(std::make_shared<Backend>());
      getBackend()->setDestroyOnLastGuiDetatch(true);
    }

    GuiBase::init();
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  void Gui::updateContainerSize()
  {
    EZ_ASSERT_ALWAYS(m_renderTarget != nullptr, "Gui must be given an ezView (either at construction or via setView function) before calling updateContainerSize()");

    ezRectFloat viewPort = m_renderTarget->getView()->GetViewport();

    m_viewport.updateParentSize({viewPort.width, viewPort.height});
    m_view.updateParentSize({m_viewport.getWidth(), m_viewport.getHeight()});
    m_renderTarget->setView(m_view.getRect(), m_viewport.getRect());
    m_container->setSize(Vector2f{m_view.getWidth(), m_view.getHeight()});

    GuiBase::updateContainerSize();
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace tgui

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
