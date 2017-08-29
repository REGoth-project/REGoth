//
// Created by kapitaenzufall on 30.07.17.
//

#pragma once

#include <memory>
#include "Menu.h"

namespace UI
{
    class Menu_Log : public Menu
    {
        Menu_Log(Engine::BaseEngine& e);

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
        static Menu_Log* create(Engine::BaseEngine& e);

    private:

		/**
		* Set the time and day text to the view
		*/
		void setTimeAndDay();

		/**
		* Find a menu item by an instance string
		*/
		MenuItem * findMenuItem( const std::string &instance );
    };
}