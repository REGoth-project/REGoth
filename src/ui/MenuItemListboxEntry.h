//
// Created by kapitaenzufall on 07.08.17
//

#pragma once

#include <engine/BaseEngine.h>
#include <ui/MenuItem.h>

namespace UI
{
    class Menu;

    namespace MenuItemTypes
    {
        class MenuItemListboxEntry : public MenuItemText
        {
        public:
            MenuItemListboxEntry(Engine::BaseEngine& e, Menu& baseMenu, const Daedalus::GameState::MenuItemHandle& scriptHandle, std::string& topic_name, Daedalus::GameState::LogTopic& topic);

            /**
             * Get the entries of this listbox entry
             */
            std::vector<std::string> getEntries();

            /**
             * Draw this entry
             */
            void drawEntry(int px, int py, EAlign alignment, Render::RenderConfig& config, const std::string& font, boolean highlight);

        private:
            /**
             * Return the normal or highlighted font of this item.
             * @param Whether the returned font is the highlighted variant or not
             */
            std::string setFontHighlighted(bool value);

            Daedalus::GameState::LogTopic m_Topic;
            std::string m_TopicName;
            boolean is_Selected;
        };
    }
}
