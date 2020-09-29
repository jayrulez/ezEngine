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


#ifndef TGUI_TREE_VIEW_HPP
#define TGUI_TREE_VIEW_HPP

#include <UIPlugin/TGUI/CopiedSharedPtr.hpp>
#include <UIPlugin/TGUI/Widgets/Scrollbar.hpp>
#include <UIPlugin/TGUI/Renderers/TreeViewRenderer.hpp>
#include <UIPlugin/TGUI/Text.hpp>
#include <UIPlugin/UIPluginDLL.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Tree view widget
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class EZ_UIPLUGIN_DLL TreeView : public Widget
    {
    public:

        typedef std::shared_ptr<TreeView> Ptr;
        typedef std::shared_ptr<const TreeView> ConstPtr;

        /// @brief Read-only node representation used by getNodes
        struct ConstNode
        {
            bool expanded;
            String text;
            std::vector<ConstNode> nodes;
        };

        /// @brief Internal representation of a node
        struct Node
        {
            Text text;
            unsigned depth = 0;
            bool expanded = true;
            Node* parent;
            std::vector<std::shared_ptr<Node>> nodes;
        };


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Default constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TreeView();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Copy constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TreeView(const TreeView&);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Move constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TreeView(TreeView&&) = default;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Overload of copy assignment operator
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TreeView& operator=(const TreeView&);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Overload of move assignment operator
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TreeView& operator=(TreeView&&) = default;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new tree view widget
        /// @return The new tree view
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static TreeView::Ptr create();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Makes a copy of another tree view
        /// @param treeView  The other tree view
        /// @return The new tree view
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static TreeView::Ptr copy(TreeView::ConstPtr treeView);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the renderer, which gives access to functions that determine how the widget is displayed
        /// @return Temporary pointer to the renderer that may be shared with other widgets using the same renderer
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TreeViewRenderer* getSharedRenderer();
        const TreeViewRenderer* getSharedRenderer() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the renderer, which gives access to functions that determine how the widget is displayed
        /// @return Temporary pointer to the renderer
        /// @warning After calling this function, the widget has its own copy of the renderer and it will no longer be shared.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TreeViewRenderer* getRenderer();
        const TreeViewRenderer* getRenderer() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the size of the tree view
        /// @param size  The new size of the tree view
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setSize(const Layout2d& size) override;
        using Widget::setSize;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Adds a new item to the tree view
        ///
        /// @param hierarchy     Hierarchy of items, with the last item being the leaf item
        /// @param createParents Should the hierarchy be created if it did not exist yet?
        ///
        /// @return True when the item was added (always the case if createParents is true)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool addItem(const std::vector<String>& hierarchy, bool createParents = true);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Expands the given item
        ///
        /// @param hierarchy  Hierarchy of items, identifying the node that has to be expanded
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void expand(const std::vector<String>& hierarchy);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Expands all items
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void expandAll();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Collapses the given item
        ///
        /// @param hierarchy  Hierarchy of items, identifying the node that has to be collapsed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void collapse(const std::vector<String>& hierarchy);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Collapses all items
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void collapseAll();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Selects an item in the tree view
        ///
        /// @param hierarchy  Hierarchy of items, with the last item being the leaf item, which is being selected
        ///
        /// @return True when the item was selected, false when hierarchy was incorrect
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool selectItem(const std::vector<String>& hierarchy);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Deselect the item if one was selected
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void deselectItem();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes an item
        ///
        /// @param hierarchy  Hierarchy of items, identifying the node to be removed
        /// @param removeParentsWhenEmpty  Also delete the parent of the deleted item if it has no other children
        ///
        /// @return True when the item existed and was removed, false when hierarchy was incorrect
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool removeItem(const std::vector<String>& hierarchy, bool removeParentsWhenEmpty = true);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes all items
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void removeAllItems();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the selected item
        /// @return Hierarchy of items, identifying the selected node, or an empty list when no item was selected
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<String> getSelectedItem() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the nodes in the tree view
        /// @return List of nodes
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<ConstNode> getNodes() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the height of the items in the tree view
        /// @param itemHeight  The size of a single item in the tree view
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setItemHeight(unsigned int itemHeight);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the height of the items in the tree view
        /// @return The item height
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int getItemHeight() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the text size of the items
        ///
        /// @param textSize  The size size of the text
        ///
        /// This will not change the height that each item has.
        /// When passing 0 to this function, the text will be auto-sized to nicely fit inside this item height.
        ///
        /// @see setItemHeight
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setTextSize(unsigned int textSize) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the thumb position of the vertical scrollbar
        ///
        /// @param value  New value of the vertical scrollbar
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setVerticalScrollbarValue(unsigned int value);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the thumb position of the vertical scrollbar
        ///
        /// @return Value of the vertical scrollbar
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int getVerticalScrollbarValue() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the thumb position of the horizontal scrollbar
        ///
        /// @param value  New value of the horizontal scrollbar
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setHorizontalScrollbarValue(unsigned int value);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the thumb position of the horizontal scrollbar
        ///
        /// @return Value of the horizontal scrollbar
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int getHorizontalScrollbarValue() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns whether the mouse position (which is relative to the parent widget) lies on top of the widget
        ///
        /// @return Is the mouse on top of the widget?
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool isMouseOnWidget(Vector2f pos) const override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void leftMousePressed(Vector2f pos) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void leftMouseReleased(Vector2f pos) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void rightMousePressed(Vector2f pos) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void mouseMoved(Vector2f pos) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool mouseWheelScrolled(float delta, Vector2f pos) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void mouseNoLongerOnWidget() override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void leftMouseButtonNoLongerDown() override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void markNodesDirty();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Draw the widget to a render target
        ///
        /// @param target Render target to draw to
        /// @param states Current render states
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void draw(BackendRenderTargetBase& target, RenderStates states) const override;



        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Retrieves a signal based on its name
        ///
        /// @param signalName  Name of the signal
        ///
        /// @return Signal that corresponds to the name
        ///
        /// @throw Exception when the name does not match any signal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Signal& getSignal(String signalName) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Function called when one of the properties of the renderer is changed
        ///
        /// @param property  Name of the property that was changed
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void rendererChanged(const String& property) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Saves the widget as a tree node in order to save it to a file
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::unique_ptr<DataIO::Node> save(SavingRenderersMap& renderers) const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Loads the widget from a tree of nodes
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Returns the size without the borders
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Vector2f getInnerSize() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Updates the bounds of the icons
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateIconBounds();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // This function is called every frame with the time passed since the last frame.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool updateTime(Duration elapsedTime) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Makes a copy of the widget
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Widget::Ptr clone() const override
        {
            return std::make_shared<TreeView>(*this);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateTextColors(std::vector<std::shared_ptr<Node>>& nodes);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void createNode(std::vector<std::shared_ptr<Node>>& menus, Node* parent, const String& text);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Node* findParentNode(const std::vector<String>& hierarchy, unsigned int parentIndex, std::vector<std::shared_ptr<Node>>& nodes, Node* parent, bool createParents);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Expands or collapses one of the visible items
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void toggleNodeInternal(std::size_t index);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Expands or collapses a node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool expandOrCollapse(const std::vector<String>& hierarchy, bool expand);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Helper function to load the items from a text file
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void loadItems(const std::unique_ptr<DataIO::Node>& node, std::vector<std::shared_ptr<Node>>& items, Node* parent);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Rebuilds the list of visible items and positions the texts
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int updateVisibleNodes(std::vector<std::shared_ptr<Node>>& nodes, Node* selectedNode, float textPadding, unsigned int pos);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Updates the text colors of the selected and hovered items
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateSelectedAndHoveringItemColors();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Updates the text color of the hovered item
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateHoveredItem(int item);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Updates the text color of the selected item
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateSelectedItem(int item);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public:

        SignalItemHierarchy onItemSelect = {"ItemSelected"};   //!< An node was selected in the tree view. Optional parameter: selected node
        SignalItemHierarchy onDoubleClick = {"DoubleClicked"}; //!< A leaf node was double clicked. Optional parameter: selected node
        SignalItemHierarchy onExpand = {"Expanded"};           //!< A branch node was expanded in the tree view. Optional parameter: expanded node
        SignalItemHierarchy onCollapse = {"Collapsed"};        //!< A branch node was collapsed in the tree view. Optional parameter: collapsed node
        SignalItemHierarchy onRightClick = {"RightClicked"};   //!< A node was right clicked. Optional parameter: node below mouse

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        // This contains the nodes of the tree
        std::vector<std::shared_ptr<Node>> m_nodes;
        std::vector<std::shared_ptr<Node>> m_visibleNodes;

        int m_selectedItem = -1;
        int m_hoveredItem = -1;

        // The size must be stored
        unsigned int m_itemHeight = 0;
        unsigned int m_requestedTextSize = 0;
        float m_maxRight = 0;

        Vector2f m_iconBounds;

        CopiedSharedPtr<ScrollbarChildWidget> m_verticalScrollbar;
        CopiedSharedPtr<ScrollbarChildWidget> m_horizontalScrollbar;


        bool m_possibleDoubleClick = false;
        int m_doubleClickNodeIndex = -1;

        Sprite    m_spriteBranchExpanded;
        Sprite    m_spriteBranchCollapsed;
        Sprite    m_spriteLeaf;

        // Cached renderer properties
        Borders   m_bordersCached;
        Borders   m_paddingCached;
        Color     m_borderColorCached;
        Color     m_backgroundColorCached;
        Color     m_textColorCached;
        Color     m_textColorHoverCached;
        Color     m_selectedTextColorCached;
        Color     m_selectedTextColorHoverCached;
        Color     m_selectedBackgroundColorCached;
        Color     m_selectedBackgroundColorHoverCached;
        Color     m_backgroundColorHoverCached;
        TextStyles m_textStyleCached;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_TREE_VIEW_HPP
