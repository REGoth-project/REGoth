#include "Menu_Save.h"
#include "MenuItem.h"
#include <engine/BaseEngine.h>
#include "Hud.h"
#include "Menu_Main.h"
#include <logic/SavegameManager.h>
#include <daedalus/DaedalusVM.h>
#include <engine/World.h>

using namespace UI;

Menu_Save::Menu_Save(Engine::BaseEngine& e) : Menu(e)
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
    // Slot 0 is for the current game, so skip it
    for(unsigned i=1;i<names.size();i++)
    {
        std::string sym = "MENUITEM_SAVE_SLOT" + std::to_string(i);
        assert(m_pVM->getDATFile().hasSymbolName(sym));
        
        if(names[i] != nullptr)
            getItemScriptData(sym).text[0] = *names[i];
        else 
            getItemScriptData(sym).text[0] = "---";
    }
}

void Menu_Save::onCustomAction(const std::string& action)
{
    if(action.find("MENUITEM_SAVE_SLOT") != std::string::npos)
    {
		std::string numStr = action.substr(std::string("MENUITEM_SAVE_SLOT").size());
		int idx = std::stoi(numStr);
		assert(idx > 0 && idx < Engine::SavegameManager::maxSlots());

		if (!m_isWaitingForSaveName)
		{
            const std::string saveGameName = Engine::SavegameManager::isSavegameAvailable(idx) ? Engine::SavegameManager::readSavegameInfo(idx).name : "";

			m_isWaitingForSaveName = true;
			m_SaveName = saveGameName;
			m_MenuItemSaveSlot = "MENUITEM_SAVE_SLOT" + std::to_string(idx);
			getItemScriptData(m_MenuItemSaveSlot).text[0] = "_";
		}
		else
		{
			m_isWaitingForSaveName = false;
            Engine::SavegameManager::saveToSaveGameSlot(idx, m_SaveName);
			// close menus after saving
			getHud().popAllMenus();
		}
    }
}

void Menu_Save::onInputAction(EInputAction action)
{
    if (!m_isWaitingForSaveName)
	Menu::onInputAction(action);
    else
    {
	switch (action)
	{
	    case IA_Accept:
		if(!m_SelectableItems.empty()) performSelectAction(m_SelectableItems[m_SelectedItem]);
		break;

	    case IA_Backspace:
		if (m_SaveName.size() > 0)
		    m_SaveName.pop_back();

	    default:
		break;
	}
    }
}

void Menu_Save::onTextInput(const std::string& text)
{
    if (m_isWaitingForSaveName && m_SaveName.size() <= 32) // Arbitrary length
    {
		m_SaveName += text;
		getItemScriptData(m_MenuItemSaveSlot).text[0] = m_SaveName + "_";
    }
}
