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

    std::vector<std::string> names = SavegameManager::gatherAvailableSavegames();

    // There are 15 labels in the original menus
    // Slot 0 is for the current game, so skip it
    for(int i=1;i<names.size();i++)
    {
        std::string sym = "MENUITEM_SAVE_SLOT" + std::to_string(i);
        assert(m_pVM->getDATFile().hasSymbolName(sym));
        
        if(!names[i].empty())
            getItemScriptData(sym).text[0] = names[i];
        else 
            getItemScriptData(sym).text[0] = "---";
    }
}

void Menu_Save::onCustomAction(const std::string& action)
{
    if(action.find("MENUITEM_SAVE_SLOT") != std::string::npos)
    {
        // Some random titles from http://www.ruggenberg.nl/titels.html
        std::vector<std::string> randomTitles = {
            "Fallen Something", 
            "The Luscious Darkness", 
            "The Fire's Dragon",
            "The Edge of the Sorcerer",
            "Guardian in the Servant",
            "Smooth Bride",
            "The Red Prince",
            "Mists of End",
            "The Gate's Night",
            "The Rose of the Beginning",
            "Luck in the Memory",
            "What Dreams",
            "The Seventh Mists",
            "History of Voyage",
            "The Return's Sorcerer",
            "Doors in the Bridge",
            "Shadowy World",
            "Time of Truth",
            "The Planet's Petals",
            "The Stone of the Angel",
            "Flight in the Stream",
            "Forgotten Child",
            "Deep Storm"};

        // Find the corresponding number
        std::string numStr = action.substr(std::string("MENUITEM_SAVE_SLOT").size());
        int idx = std::stoi(numStr);

        // Lock to number of savegames
        assert(idx > 0 && idx < 16);

        // TODO: Should be writing to a temp-directory first, before messing with the save-files already existing
        // Clean data from old savegame, so we don't load into worlds we haven't been to yet
        Engine::SavegameManager::clearSavegame(idx);

        // Write information about the current game-state
        Engine::SavegameManager::SavegameInfo info;
        info.name = randomTitles[rand() % randomTitles.size()];
        info.world = Utils::stripExtension(m_Engine.getMainWorld().get().getZenFile());
        info.timePlayed = 0;
        Engine::SavegameManager::writeSavegameInfo(idx, info);

        json j;
        m_Engine.getMainWorld().get().exportWorld(j);

        // Save
        Engine::SavegameManager::writeWorld(idx, info.world, Utils::iso_8859_1_to_utf8(j.dump(4)));

        // Update list of savegames, for testing
        gatherAvailableSavegames();
    }
}
