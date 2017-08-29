//
// Created by kapitaenzufall on 19.08.17.
//

#pragma once

#include <ZenLib/daedalus/DaedalusGameState.h>
#include "MenuItem.h"
#include "View.h"

namespace UI
{
    class Menu;
    class ImageView;

    namespace MenuItemTypes
    {
        class MenuItemScrollableText : public MenuItemText
        {
        public:
			MenuItemScrollableText(Engine::BaseEngine& e, Menu& baseMenu, const Daedalus::GameState::MenuItemHandle& scriptHandle);

            /**
             * Updates/draws the UI-Views
             * @param dt time since last frame
             * @param mstate mouse-state
             */
            void update(double dt, Engine::Input::MouseState &mstate, Render::RenderConfig &config) override;

			/*
			 * Set the topic entries
			 * @param entries The entries which should be added
			 */
			void setTopicEntries(std::vector<std::string> &entries);

			/*
			* Scroll the entries one line down
			*/
			void scrollDown();

			/*
			* Scroll the entries one line up
			*/
			void scrollUp();

		private:
			std::vector<std::vector<std::string>> m_FormattedEntries;
			unsigned int m_NumberOfFormattedEntries;
			unsigned int m_FirstDrawnLine;
        };
    }
}
