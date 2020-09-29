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


#include <RendererFoundation/Device/Device.h>
#include <TGUIPlugin/Backend/Backend.hpp>
#include <TGUIPlugin/Backend/BackendRenderTarget.hpp>
#include <TGUIPlugin/Backend/BackendText.hpp>
#include <TGUIPlugin/Backend/BackendTexture.hpp>
#include <TGUIPlugin/TGUI/Container.hpp>


#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  BackendRenderTarget::BackendRenderTarget(ezView* view)
    : m_pView(view)
  {
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  BackendRenderTarget::~BackendRenderTarget()
  {
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  ezView* BackendRenderTarget::getView() const
  {
    return m_pView;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  void BackendRenderTarget::setView(FloatRect view, FloatRect viewport)
  {
    if (m_pView == nullptr)
      return;

    ezRectFloat viewPort = m_pView->GetViewport();

    m_projectionTransform = Transform();
    m_projectionTransform.translate({-1 - (2.f * (view.left / view.width) * (viewport.width / view.width)),
      1 + (2.f * (view.top / view.height) * (viewport.height / view.height))});
    m_projectionTransform.scale({2.f / m_viewWidth * (viewport.width / view.width),
      -2.f / m_viewHeight * (viewport.height / view.height)});

    m_viewRect = view;
    m_viewport = {static_cast<int>(viewport.left), static_cast<int>(viewport.top), static_cast<int>(viewport.width), static_cast<int>(viewport.height)};
    m_viewportEz = {viewport.left, viewport.top, viewport.width, viewport.height};
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  void BackendRenderTarget::drawGui(const std::shared_ptr<RootContainer>& root)
  {
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  void BackendRenderTarget::drawWidget(const RenderStates& states, const std::shared_ptr<Widget>& widget)
  {
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  void BackendRenderTarget::addClippingLayer(const RenderStates& states, FloatRect rect)
  {
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  void BackendRenderTarget::removeClippingLayer()
  {
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  void BackendRenderTarget::drawSprite(const RenderStates& states, const Sprite& sprite)
  {
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  void BackendRenderTarget::drawText(const RenderStates& states, const Text& text)
  {
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  void BackendRenderTarget::drawTriangles(const RenderStates& states, const Vertex* vertices, std::size_t vertexCount, const int* indices, std::size_t indexCount)
  {
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  void BackendRenderTarget::drawCircle(const RenderStates& states, float size, const Color& backgroundColor, float borderThickness, const Color& borderColor)
  {
  }
} // namespace tgui

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
