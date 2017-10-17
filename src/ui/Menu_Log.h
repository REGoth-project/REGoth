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
        enum EMenuLogStatus
        {
            CategorySelection,
            TopicSelection,
            ContentViewer
        };

        /**
         * To be called when one of the given actions were triggered
         * @param action Input action
         */
        bool onInputAction(EInputAction action) override;

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
         * Initialize the listboxes with the log entries
         */
        void initializeLogEntries();

        /**
         * Find a menu item by an instance string
         * @return The found menu item or nullptr
         */
        MenuItem* findMenuItem(const std::string& instance);

        /**
         * This member is used to indicate which
         * view is current effected by an input action
         */
        EMenuLogStatus m_LogStatus;
    };
}