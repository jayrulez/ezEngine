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

#ifndef TGUI_GLOBAL_HPP
#define TGUI_GLOBAL_HPP

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <UIPlugin/TGUI/Config.hpp>
#include <UIPlugin/TGUI/Exception.hpp>
#include <UIPlugin/TGUI/Duration.hpp>
#include <UIPlugin/TGUI/Font.hpp>
#include <cstdint>
#include <string>
#include <memory>

#ifdef TGUI_DEBUG
    #include <iostream>
#endif
#include <UIPluginDLL.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Namespace that contains all TGUI functions and classes
namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Sets the default text size for all widgets created after calling the function
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    EZ_UIPLUGIN_DLL void setGlobalTextSize(unsigned int textSize);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Retrieves the default text size used for all new widgets
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    EZ_UIPLUGIN_DLL unsigned int getGlobalTextSize();


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Sets the double-click time for the mouse
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    EZ_UIPLUGIN_DLL void setDoubleClickTime(Duration duration);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Retrieves the double-click time for the mouse
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    EZ_UIPLUGIN_DLL Duration getDoubleClickTime();


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Sets a new resource path
    ///
    /// This pathname is placed in front of every filename that is used to load a resource.
    ///
    /// @param path  New resource path
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    EZ_UIPLUGIN_DLL void setResourcePath(const String& path);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Returns the resource path
    ///
    /// This pathname is placed in front of every filename that is used to load a resource.
    ///
    /// @return The current resource path
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    EZ_UIPLUGIN_DLL const String& getResourcePath();


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Changes the blink rate of the cursor in edit fields such as EditBox and TextArea
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    EZ_UIPLUGIN_DLL void setEditCursorBlinkRate(Duration blinkRate);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Returns the blink rate of the cursor in edit fields such as EditBox and TextArea
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    EZ_UIPLUGIN_DLL Duration getEditCursorBlinkRate();


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_GLOBAL_HPP

