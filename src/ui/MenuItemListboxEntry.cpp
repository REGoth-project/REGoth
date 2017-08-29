//
// Created by kapitaenzufall on 07.08.17.
//

#include "MenuItemListboxEntry.h"
#include "Menu.h"
//#include "View.h"
//#include "ImageView.h"
#include "MenuItem.h"
#include "zFont.h"
#include <engine/BaseEngine.h>
#include <utils/logger.h>

using namespace UI;
using namespace MenuItemTypes;

MenuItemTypes::MenuItemListboxEntry::MenuItemListboxEntry(  Engine::BaseEngine& e, UI::Menu& baseMenu,
                                            const Daedalus::GameState::MenuItemHandle& scriptHandle,
											std::string& topic_name,
											Daedalus::GameState::LogTopic &topic)
                                            : MenuItemText(e, baseMenu, scriptHandle), m_TopicName(topic_name), m_Topic(topic)
{
}

std::vector<std::string> MenuItemTypes::MenuItemListboxEntry::getEntries()
{
	return m_Topic.entries;
}

void MenuItemTypes::MenuItemListboxEntry::drawEntry(int px, int py, EAlign alignment, Render::RenderConfig& config, const std::string& font, boolean highlight)
{
    // Highlight the selected listbox entry
	std::string fontBase;
	if(highlight)
		fontBase = setFontHighlighted(true);
	else
		fontBase = setFontHighlighted(false);

	drawText(" " + m_TopicName, px, py, alignment, config, fontBase);

}

std::string MenuItemTypes::MenuItemListboxEntry::setFontHighlighted(bool value)
{
	std::string fontBase = getItemScriptData().fontName;
	fontBase = Utils::stripExtension(fontBase);

	// Strip the _HI, if present
	if (fontBase.substr(fontBase.size() - 3) == "_HI")
		fontBase = fontBase.substr(0, fontBase.size() - 3);

	if (value)
		fontBase += "_HI";

	return fontBase;
}

