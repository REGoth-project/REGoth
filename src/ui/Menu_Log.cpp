//
// Created by kapitaenzufall on 30.07.17.
//

#include "Menu_Log.h"
#include "MenuItem.h"
#include <engine/BaseEngine.h>
#include <daedalus/DaedalusGameState.h>

using namespace UI;

Menu_Log::Menu_Log(Engine::BaseEngine& e) : Menu(e)
{
}


Menu_Log* Menu_Log::create(Engine::BaseEngine& e)
{
	Menu_Log* s = new Menu_Log(e);
    s->initializeInstance("MENU_LOG");

	// Hide content viewer on log opening
	s->findMenuItem("MENU_ITEM_CONTENT_VIEWER")->setHidden(true);

    return s;
}


MenuItem * UI::Menu_Log::findMenuItem(const std::string & instance)
{
	for (auto iterator = m_Items.begin(); iterator != m_Items.end(); iterator++)
		if (iterator->second->getItemScriptData().instanceSymbol == getItemScriptData(instance).instanceSymbol)
			return iterator->second;
}


bool Menu_Log::onInputAction(EInputAction action)
{
	bool baseclassClose = false;

    // Close this menu if either the parent function wants to close or this function
    return baseclassClose || (action == IA_ToggleLogMenu);
}
