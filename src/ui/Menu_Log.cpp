//
// Created by kapitaenzufall on 30.07.17.
//

#include "Menu_Log.h"
#include "MenuItem.h"
#include "MenuItemListbox.h"
#include "MenuItemScrollableText.h"
#include <daedalus/DaedalusGameState.h>
#include <engine/GameEngine.h>
#include <logic/LogManager.h>

using namespace UI;

Menu_Log::Menu_Log(Engine::GameEngine& e)
    : Menu(e)
{
    m_LogStatus = EMenuLogStatus::CategorySelection;
}

Menu_Log* Menu_Log::create(Engine::GameEngine& e)
{
    Menu_Log* s = new Menu_Log(e);
    s->initializeInstance("MENU_LOG");

    // Hide content viewer on log opening
    s->findMenuItem("MENU_ITEM_CONTENT_VIEWER")->setHidden(true);

    s->setTimeAndDay();
    s->initializeLogEntries();

    return s;
}

void Menu_Log::setTimeAndDay()
{
    if (!m_MenuHandle.isValid())
        return;

    getItemScriptData("MENU_ITEM_TIME").text[0] = m_Engine.getGameClock().getTimeOfDayFormatted();
    getItemScriptData("MENU_ITEM_DAY").text[0] = m_Engine.getGameClock().getDayFormatted();
}

void Menu_Log::initializeLogEntries()
{
    if (!m_MenuHandle.isValid())
        return;

    MenuItemTypes::MenuItemListbox* listbox_running;
    MenuItemTypes::MenuItemListbox* listbox_failed;
    MenuItemTypes::MenuItemListbox* listbox_old;
    MenuItemTypes::MenuItemListbox* listbox_info;

    // Find listboxes
    for (auto& item : m_Items)
        if (item.second->getItemScriptData().instanceSymbol == getItemScriptData("MENU_ITEM_LIST_MISSIONS_ACT").instanceSymbol)
            listbox_running = dynamic_cast<MenuItemTypes::MenuItemListbox*>(item.second);
        else if (item.second->getItemScriptData().instanceSymbol == getItemScriptData("MENU_ITEM_LIST_MISSIONS_FAILED").instanceSymbol)
            listbox_failed = dynamic_cast<MenuItemTypes::MenuItemListbox*>(item.second);
        else if (item.second->getItemScriptData().instanceSymbol == getItemScriptData("MENU_ITEM_LIST_MISSIONS_OLD").instanceSymbol)
            listbox_old = dynamic_cast<MenuItemTypes::MenuItemListbox*>(item.second);
        else if (item.second->getItemScriptData().instanceSymbol == getItemScriptData("MENU_ITEM_LIST_LOG").instanceSymbol)
            listbox_info = dynamic_cast<MenuItemTypes::MenuItemListbox*>(item.second);

    // Set topic entries to listboxes
    const std::map<std::string, Daedalus::GameState::LogTopic>& log = m_Engine.getSession().getLogManager().getPlayerLog();
    for (auto& entry : log)
    {
        if (entry.second.section == Daedalus::GameState::LogTopic::ESection::LT_Mission)
        {
            switch (entry.second.status)
            {
                case Daedalus::GameState::LogTopic::ELogStatus::LS_Running:
                    listbox_running->addTopic(entry.first, entry.second);
                    break;
                case Daedalus::GameState::LogTopic::ELogStatus::LS_Failed:
                    listbox_failed->addTopic(entry.first, entry.second);
                    break;
                case Daedalus::GameState::LogTopic::ELogStatus::LS_Obsolete:
                    LogInfo() << "Found mission with status LS_Obsolete. This is current not supported!";
                    break;
                case Daedalus::GameState::LogTopic::ELogStatus::LS_Success:
                    listbox_old->addTopic(entry.first, entry.second);
                    break;
            }
        }
        else if (entry.second.section == Daedalus::GameState::LogTopic::ESection::LT_Note)
        {
            listbox_info->addTopic(entry.first, entry.second);
        }
    }

    // Make actual mission listbox visible
    listbox_running->setHidden(false);
}

MenuItem* UI::Menu_Log::findMenuItem(const std::string& instance)
{
    MenuItem* menuItem = nullptr;

    for (auto& item : m_Items)
        if (item.second->getItemScriptData().instanceSymbol == getItemScriptData(instance).instanceSymbol)
        {
            menuItem = item.second;
            break;
        }

    return menuItem;
}

bool Menu_Log::onInputAction(EInputAction action)
{
    bool baseclassClose = false;

    // Only effect the selection of the menu if the listbox
    // is not in focus or the content viewer open
    if ((m_LogStatus == EMenuLogStatus::CategorySelection) || (action == IA_Close))
        baseclassClose = Menu::onInputAction(action);

    // Separate the keywords from the current selected action
    std::vector<std::string> result;
    std::istringstream iss(m_Items[m_SelectableItems[m_SelectedItem]]->getItemScriptData().onSelAction_S[0]);
    for (std::string s; iss >> s;)
        result.push_back(s);

    // Get current listbox
    MenuItemTypes::MenuItemListbox* listbox;
    if (result[0] == "EFFECTS")
    {
        // Find current selected listbox and hide all other listboxes
        for (auto& item : m_Items)
            if (item.second->getItemScriptData().instanceSymbol == getItemScriptData(result[1]).instanceSymbol)
            {
                listbox = dynamic_cast<MenuItemTypes::MenuItemListbox*>(item.second);
                listbox->setHidden(false);
            }
            else if (item.second->getItemScriptData().type == Daedalus::GEngineClasses::C_Menu_Item::MENU_ITEM_LISTBOX)
            {
                item.second->setHidden(true);
            }
    }

    // React on input action
    switch (action)
    {
        case IA_Up:
            if (m_LogStatus == EMenuLogStatus::TopicSelection)
            {
                listbox->selectPreviousItem();
            }
            else if (m_LogStatus == EMenuLogStatus::ContentViewer)
            {
                MenuItemTypes::MenuItemScrollableText* menu_item;
                menu_item = dynamic_cast<MenuItemTypes::MenuItemScrollableText*>(findMenuItem("MENU_ITEM_CONTENT_VIEWER"));
                menu_item->scrollUp();
            }
            break;

        case IA_Down:
            if (m_LogStatus == EMenuLogStatus::TopicSelection)
            {
                listbox->selectNextItem();
            }
            else if (m_LogStatus == EMenuLogStatus::ContentViewer)
            {
                MenuItemTypes::MenuItemScrollableText* menu_item;
                menu_item = dynamic_cast<MenuItemTypes::MenuItemScrollableText*>(findMenuItem("MENU_ITEM_CONTENT_VIEWER"));
                menu_item->scrollDown();
            }
            break;

        case IA_Left:
            if (m_LogStatus != EMenuLogStatus::ContentViewer)
            {
                m_LogStatus = EMenuLogStatus::CategorySelection;
                listbox->setFocus(false);
            }
            break;

        case IA_Right:
            if (listbox->hasTopics() && (m_LogStatus != EMenuLogStatus::ContentViewer))
            {
                m_LogStatus = EMenuLogStatus::TopicSelection;
                listbox->setFocus(true);
            }
            break;

        case IA_Accept:
            if (m_LogStatus == EMenuLogStatus::TopicSelection)
            {
                std::vector<std::string> entries = listbox->getSelectedTopicEntries();

                MenuItemTypes::MenuItemScrollableText* menu_item;
                menu_item = dynamic_cast<MenuItemTypes::MenuItemScrollableText*>(findMenuItem("MENU_ITEM_CONTENT_VIEWER"));
                menu_item->setTopicEntries(entries);
                menu_item->setFontHighlighted(true);
                menu_item->setHidden(false);
                m_LogStatus = EMenuLogStatus::ContentViewer;
            }
            break;

        case IA_Close:
            MenuItem* menu_item = findMenuItem("MENU_ITEM_CONTENT_VIEWER");
            if (!menu_item->isHidden())
            {
                menu_item->setHidden(true);
                m_LogStatus = EMenuLogStatus::TopicSelection;
                return false;
            }
    }

    // Close this menu if either the parent function wants to close or this function
    return baseclassClose || (action == IA_ToggleLogMenu);
}
