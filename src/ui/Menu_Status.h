//
// Created by desktop on 21.12.16.
//

#pragma once

#include <memory>
#include "Menu.h"

namespace UI
{
    class Menu_Status : public Menu
    {
        Menu_Status(Engine::GameEngine& e);

    public:
        enum EAttribute
        {
            // Note: Numbers correspond to item-names, like: MENU_ITEM_ATTRIBUTE_1 for STR
            A_STR = 1,
            A_DEX = 2,
            A_MANA = 3,
            A_HEALTH = 4
        };

        /**
         * Sets the value for an attribute
         * @param type Type of attribute this is
         * @param value Current value
         * @param max Max value (Ignored for STR and DEX)
         */
        void setAttribute(EAttribute type, int value, int max = 0);

        /**
         * Setters
         */
        void setGuild(const std::string& guild);
        void setLevel(int level);
        void setExperience(int xp);
        void setLearnPoints(int lp);
        void setExperienceNext(int xpNext);

        /**
         * To be called when one of the given actions were triggered
         * @param action Input action
         */
        virtual bool onInputAction(EInputAction action) override;

        /**
         * Creates an instance of this class and appends it to the root UI-View
         * @return Instance of the class. Don't forget to delete!
         */
        static Menu_Status* create(Engine::GameEngine& e);

    public:
    protected:
    };
}
