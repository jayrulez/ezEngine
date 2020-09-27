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


#include <UIPlugin/TGUI/Loading/WidgetFactory.hpp>
#include <UIPlugin/TGUI/Widgets/BitmapButton.hpp>
#include <UIPlugin/TGUI/Widgets/Button.hpp>
#include <UIPlugin/TGUI/Widgets/ChatBox.hpp>
#include <UIPlugin/TGUI/Widgets/CheckBox.hpp>
#include <UIPlugin/TGUI/Widgets/ChildWindow.hpp>
#include <UIPlugin/TGUI/Widgets/ComboBox.hpp>
#include <UIPlugin/TGUI/Widgets/EditBox.hpp>
#include <UIPlugin/TGUI/Widgets/Grid.hpp>
#include <UIPlugin/TGUI/Widgets/Group.hpp>
#include <UIPlugin/TGUI/Widgets/HorizontalLayout.hpp>
#include <UIPlugin/TGUI/Widgets/HorizontalWrap.hpp>
#include <UIPlugin/TGUI/Widgets/Knob.hpp>
#include <UIPlugin/TGUI/Widgets/Label.hpp>
#include <UIPlugin/TGUI/Widgets/ListBox.hpp>
#include <UIPlugin/TGUI/Widgets/ListView.hpp>
#include <UIPlugin/TGUI/Widgets/MenuBar.hpp>
#include <UIPlugin/TGUI/Widgets/MessageBox.hpp>
#include <UIPlugin/TGUI/Widgets/Panel.hpp>
#include <UIPlugin/TGUI/Widgets/Picture.hpp>
#include <UIPlugin/TGUI/Widgets/ProgressBar.hpp>
#include <UIPlugin/TGUI/Widgets/RadioButton.hpp>
#include <UIPlugin/TGUI/Widgets/RadioButtonGroup.hpp>
#include <UIPlugin/TGUI/Widgets/RangeSlider.hpp>
#include <UIPlugin/TGUI/Widgets/ScrollablePanel.hpp>
#include <UIPlugin/TGUI/Widgets/Scrollbar.hpp>
#include <UIPlugin/TGUI/Widgets/SeparatorLine.hpp>
#include <UIPlugin/TGUI/Widgets/Slider.hpp>
#include <UIPlugin/TGUI/Widgets/SpinButton.hpp>
#include <UIPlugin/TGUI/Widgets/SpinControl.hpp>
#include <UIPlugin/TGUI/Widgets/Tabs.hpp>
#include <UIPlugin/TGUI/Widgets/TabContainer.hpp>
#include <UIPlugin/TGUI/Widgets/TextArea.hpp>
#include <UIPlugin/TGUI/Widgets/TreeView.hpp>
#include <UIPlugin/TGUI/Widgets/VerticalLayout.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    std::map<String, std::function<Widget::Ptr()>> WidgetFactory::m_constructFunctions =
    {
        {"BitmapButton", std::make_shared<BitmapButton>},
        {"Button", std::make_shared<Button>},
        {"ChatBox", std::make_shared<ChatBox>},
        {"CheckBox", std::make_shared<CheckBox>},
        {"ChildWindow", std::make_shared<ChildWindow>},
        {"ClickableWidget", std::make_shared<ClickableWidget>},
        {"ComboBox", std::make_shared<ComboBox>},
        {"EditBox", std::make_shared<EditBox>},
        {"Grid", std::make_shared<Grid>},
        {"Group", std::make_shared<Group>},
        {"HorizontalLayout", std::make_shared<HorizontalLayout>},
        {"HorizontalWrap", std::make_shared<HorizontalWrap>},
        {"Knob", std::make_shared<Knob>},
        {"Label", std::make_shared<Label>},
        {"ListBox", std::make_shared<ListBox>},
        {"ListView", std::make_shared<ListView>},
        {"MenuBar", std::make_shared<MenuBar>},
        {"MessageBox", std::make_shared<MessageBox>},
        {"Panel", std::make_shared<Panel>},
        {"Picture", std::make_shared<Picture>},
        {"ProgressBar", std::make_shared<ProgressBar>},
        {"RadioButton", std::make_shared<RadioButton>},
        {"RadioButtonGroup", std::make_shared<RadioButtonGroup>},
        {"RangeSlider", std::make_shared<RangeSlider>},
        {"ScrollablePanel", std::make_shared<ScrollablePanel>},
        {"Scrollbar", std::make_shared<Scrollbar>},
        {"SeparatorLine", std::make_shared<SeparatorLine>},
        {"Slider", std::make_shared<Slider>},
        {"SpinButton", std::make_shared<SpinButton>},
        {"SpinControl", std::make_shared<SpinControl>},
        {"Tabs", std::make_shared<Tabs>},
        {"TabContainer", std::make_shared<TabContainer>},
        {"TextArea", std::make_shared<TextArea>},
        {"TreeView", std::make_shared<TreeView>},
        {"VerticalLayout", std::make_shared<VerticalLayout>}
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetFactory::setConstructFunction(const String& type, const std::function<Widget::Ptr()>& constructor)
    {
        m_constructFunctions[type] = constructor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::function<Widget::Ptr()>& WidgetFactory::getConstructFunction(const String& type)
    {
        return m_constructFunctions[type];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
