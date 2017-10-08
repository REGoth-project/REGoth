#include "Menu_Save.h"
#include "Hud.h"
#include "MenuItem.h"
#include "Menu_Main.h"
#include <daedalus/DaedalusVM.h>
#include <engine/BaseEngine.h>
#include <engine/World.h>
#include <logic/SavegameManager.h>

using namespace UI;

Menu_Save::Menu_Save(Engine::BaseEngine& e)
    : Menu(e)
{
}

Menu_Save::~Menu_Save()
{
}

Menu_Save* Menu_Save::create(Engine::BaseEngine& e)
{
    Menu_Save* s = new Menu_Save(e);
    s->initializeInstance("MENU_SAVEGAME_SAVE");

    s->gatherAvailableSavegames();

    return s;
}

void Menu_Save::performSelectAction(Daedalus::GameState::MenuItemHandle item)
{
    MenuItem* iData = m_Items[item];

    using namespace Daedalus::GEngineClasses::MenuConstants;

    Menu::performSelectAction(item);
}

void Menu_Save::gatherAvailableSavegames()
{
    using namespace Engine;

    auto names = SavegameManager::gatherAvailableSavegames();

    // There are 15/20 labels in the original menus
    for (unsigned i = 0; i < names.size(); i++)
    {
        std::string sym = "MENUITEM_SAVE_SLOT" + std::to_string(i);
        bool slotSymbolFound = m_pVM->getDATFile().hasSymbolName(sym);
        if (!slotSymbolFound && i == 0)
            continue;  // Gothic 1+2 do not have a quicksave slot in the save-menu
        assert(slotSymbolFound);

        if (names[i] != nullptr)
            getItemScriptData(sym).text[0] = *names[i];
        else
            getItemScriptData(sym).text[0] = Menu_Save::EMPTY_SLOT_DISPLAYNAME;
    }
}

void Menu_Save::onCustomAction(const std::string& action)
{
    if (action.find("MENUITEM_SAVE_SLOT") != std::string::npos)
    {
        std::string numStr = action.substr(std::string("MENUITEM_SAVE_SLOT").size());
        int index = std::stoi(numStr);
        assert(index >= 0 && index < Engine::SavegameManager::maxSlots());

        if (!m_isWaitingForSaveName)
        {
            m_isWaitingForSaveName = true;
            m_MenuItemSaveSlot = "MENUITEM_SAVE_SLOT" + std::to_string(index);

            const std::string saveGameName = Engine::SavegameManager::isSavegameAvailable(index) ? Engine::SavegameManager::readSavegameInfo(index).name : "";
            m_SaveName.clear();

#ifdef ANDROID
            m_SaveName = "Slot " + std::to_string(index);
#endif

            onTextInput(saveGameName);
        }
        else
        {
            m_isWaitingForSaveName = false;
            Engine::SavegameManager::saveToSlot(index, m_SaveName);
            // close menus after saving
            getHud().popAllMenus();
        }
    }
}

bool Menu_Save::onInputAction(EInputAction action)
{
    if (!m_isWaitingForSaveName)
        return Menu::onInputAction(action);
    else
    {
        switch (action)
        {
            case IA_Accept:
                if (!m_SelectableItems.empty())
                    performSelectAction(m_SelectableItems[m_SelectedItem]);
                break;

            case IA_Backspace:
                if (m_SaveName.size() > 0)
                    m_SaveName.pop_back();
                break;
            case IA_Close:
                m_isWaitingForSaveName = false;
                // restore displayname of current slot
                gatherAvailableSavegames();
                break;
            default:
                break;
        }
    }
    return false;
}

void Menu_Save::onTextInput(const std::string& text)
{
    if (m_isWaitingForSaveName && m_SaveName.size() <= 32)  // Arbitrary length
    {
        m_SaveName += text;
        getItemScriptData(m_MenuItemSaveSlot).text[0] = m_SaveName + "_";
    }
}
