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


#ifndef TGUI_TEXTURE_MANAGER_HPP
#define TGUI_TEXTURE_MANAGER_HPP

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <UIPlugin/TGUI/TextureData.hpp>
#include <memory>
#include <list>
#include <map>
#include <UIPlugin/UIPluginDLL.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class Texture;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class EZ_UIPLUGIN_DLL TextureManager
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Loads a texture
        ///
        /// @param texture    The texture object to store the loaded image
        /// @param filename   Filename of the image to load
        /// @param partRect   Load only part of the image. Don't pass this parameter if you want to load the full image
        /// @param smooth     Enable smoothing on the texture
        ///
        /// The second time you call this function with the same filename, the previously loaded image will be reused.
        ///
        /// @return Texture data when loaded successfully, nullptr otherwise
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<TextureData> getTexture(Texture& texture, const String& filename, const UIntRect& partRect = UIntRect(0, 0, 0, 0), bool smooth = false);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Shares the image with another texture
        ///
        /// @param textureDataToCopy  The original texture data that will now be reused
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void copyTexture(std::shared_ptr<TextureData> textureDataToCopy);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes the texture
        ///
        /// @param textureDataToRemove  The texture data that should be removed
        ///
        /// When no other texture is using the same image then the image will be removed from memory.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void removeTexture(std::shared_ptr<TextureData> textureDataToRemove);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        static std::map<String, std::list<TextureDataHolder>> m_imageMap;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_TEXTURE_MANAGER_HPP
