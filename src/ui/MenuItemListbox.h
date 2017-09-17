//
// Created by kapitaenzufall on 06.08.17
//

#pragma once

#include <engine/BaseEngine.h>
#include <ui/MenuItemListboxEntry.h>

namespace UI
{
    class Menu;

    namespace MenuItemTypes
    {
        class MenuItemListbox : public MenuItem
        {
        public:
            MenuItemListbox(Engine::BaseEngine& e, Menu& baseMenu, const Daedalus::GameState::MenuItemHandle& scriptHandle);

            /**
             * Updates/draws the UI-Views
             * @param dt time since last frame
             * @param mstate mouse-state
             */
            void update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config) override;

            /**
             * Add a topic
             * @param topic_name The name of the topic
             * @param topic The topic containing the topic entries
             */
            void addTopic(std::string topic_name, const Daedalus::GameState::LogTopic& topic);

            /*
             * Returns true if this listbox has at least one topic entry, false otherwise
             */
            bool hasTopics();

            /*
             * Set the focus of view to this listbox by highlighting an entry
             * @param focus Whether to set the focus or not
             */
            void setFocus(bool focus);

            /**
             * Return the topic entries of the current selected topic
             */
            std::vector<std::string> getSelectedTopicEntries();

            /**
             * Select the topic among the current selected one.
             */
            void selectNextItem();

            /**
             * Select the topic above the current selected one.
             */
            void selectPreviousItem();

        private:
            bool m_Focus;
            unsigned int m_Selection;
            std::vector<MenuItemListboxEntry*> m_Entries;
        };
    }
}
