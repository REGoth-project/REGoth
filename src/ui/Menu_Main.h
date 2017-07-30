
//
// Created by desktop on 21.12.16.
//

#pragma once

#include <memory>
#include "Menu.h"

namespace UI
{
    class Menu_Main : public Menu
    {
        Menu_Main(Engine::BaseEngine& e);

    public:
        /**
         * To be called when one of the given actions were triggered
         * @param action Input action
         */
        virtual bool onInputAction(EInputAction action) override;

        /**
         * Creates an instance of this class and appends it to the root UI-View
         * @return Instance of the class. Don't forget to delete!
         */
        static Menu_Main* create(Engine::BaseEngine& e);

        /**
         * @return the gametype of the Menu's VM (based on the existence of Gothic 2 specific symbols)
         */
        Daedalus::GameType determineGameType();

        virtual void onCustomAction(const std::string& action) override;

    protected:
    };
}
