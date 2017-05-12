#include "Menu_Load.h"
#include <logic/SavegameManager.h>
#include <utils/logger.h>
#include <utils/Utils.h>
#include "Hud.h"
#include <engine/BaseEngine.h>

using namespace UI;

Menu_Load::Menu_Load(Engine::BaseEngine& e) : Menu(e)
{
}

Menu_Load::~Menu_Load()
{
}

Menu_Load* Menu_Load::create(Engine::BaseEngine& e)
{
    Menu_Load* s = new Menu_Load(e);
    s->initializeInstance("MENU_SAVEGAME_LOAD");

    s->gatherAvailableSavegames();
    return s;
}

void Menu_Load::gatherAvailableSavegames()
{
    using namespace Engine;
    using namespace Daedalus;
    using namespace GEngineClasses;

    auto names = SavegameManager::gatherAvailableSavegames();

    // gothic 1: Slot 1-15. gothic 2: Slot 1-20 + Slot0 (quicksave)
    for(unsigned i=0;i<names.size();i++)
    {
        std::string sym = "MENUITEM_LOAD_SLOT" + std::to_string(i);
        bool slotSymbolFound = m_pVM->getDATFile().hasSymbolName(sym);
        if (!slotSymbolFound && i == 0)
            continue; // Gothic 1 does not have a quicksave slot in the load-menu

        assert(slotSymbolFound);
        std::string displayName;
        if(names[i] != nullptr)
        {
            // Toggle selectable depending on whether we actually have a slot here
            getItemScriptData(sym).flags |= C_Menu_Item::IT_SELECTABLE;
            displayName = *names[i];
        }else 
        {
            getItemScriptData(sym).flags &= ~C_Menu_Item::IT_SELECTABLE;
            displayName = Menu_Load::EMPTY_SLOT_DISPLAYNAME;
        }
        // never overwrite displayname of quicksave slot (i==0)
        if (i != 0)
            getItemScriptData(sym).text[0] = displayName;
    }
}

void Menu_Load::onCustomAction(const std::string& action)
{
    if(action == "SAVEGAME_LOAD")
    {
        using namespace Engine;

        // Load actions don't give us the index. We have to check the name for that...
        size_t sym = getItemScriptData(m_SelectableItems[m_SelectedItem]).instanceSymbol;
        std::string name = m_pVM->getDATFile().getSymbolByIndex(sym).name;

        // Find the corresponding number
        std::string numStr = name.substr(std::string("MENUITEM_SAVE_SLOT").size());
        int idx = std::stoi(numStr);
        std::string error = SavegameManager::loadSaveGameSlot(idx);
        if (!error.empty()){
            LogWarn() << error;
            return;
        }

        // Close menu_load & menu_main after loading the world
        getHud().popAllMenus();
    }
}
