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

    std::vector<std::string> names = SavegameManager::gatherAvailableSavegames();

    // There are 15 labels in the original menus
    // Slot 0 is for the current game, so skip it
    for(int i=1;i<names.size();i++)
    {
        std::string sym = "MENUITEM_LOAD_SLOT" + std::to_string(i);
        assert(m_pVM->getDATFile().hasSymbolName(sym));
        
        if(!names[i].empty())
        {
            // Toggle selectable depending on whether we actually have a slot here
            getItemScriptData(sym).flags |= C_Menu_Item::IT_SELECTABLE;
            getItemScriptData(sym).text[0] = names[i];
        }else 
        {
            getItemScriptData(sym).flags &= ~C_Menu_Item::IT_SELECTABLE;
            getItemScriptData(sym).text[0] = "";
        }
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

        // Lock to number of savegames
        assert(idx > 0 && idx < 16);

        if(!SavegameManager::isSavegameAvailable(idx))
        {
            LogInfo() << "Savegame at slot " << idx << " not available!";
            return;
        }

        // Read general information about the saved game. Most importantly the world the player saved in
        SavegameManager::SavegameInfo info = SavegameManager::readSavegameInfo(idx);

        std::string worldPath = SavegameManager::buildWorldPath(idx, info.world);

        // Sanity check, if we really got a safe for this world. Otherwise we would end up in the fresh version
        // if it was missing. Also, IF the player saved there, there should be a save for this.
        if(!Utils::getFileSize(worldPath))
        {
            LogWarn() << "Target world-file invalid: " << worldPath;
            return;
        }

        m_Engine.loadWorld(info.world + ".zen", worldPath);

        // Close menu after loading the world
        getHud().popMenu();
    }
}
