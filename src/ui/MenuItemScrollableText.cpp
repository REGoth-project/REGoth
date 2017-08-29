//
// Created by kapitaenzufall on 19.08.17.
//

#include "MenuItemScrollableText.h"
#include "Menu.h"
#include "zFont.h"
#include "MenuItem.h"
#include <engine/BaseEngine.h>
#include <utils/logger.h>

using namespace UI;

MenuItemTypes::MenuItemScrollableText::MenuItemScrollableText(  Engine::BaseEngine& e, UI::Menu& baseMenu,
                                            const Daedalus::GameState::MenuItemHandle& scriptHandle)
                                            : MenuItemText(e, baseMenu, scriptHandle)
{
	m_FirstDrawnLine = 0;
	m_NumberOfFormattedEntries = 0;
}

void MenuItemTypes::MenuItemScrollableText::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
	if (m_IsHidden)
		return;

    MenuItem::update(dt, mstate, config);

    if(!m_ScriptHandle.isValid() )
        return;

    Daedalus::GEngineClasses::C_Menu_Item& item = getItemScriptData();
    const UI::zFont* fnt = m_Engine.getFontCache().getFont(item.fontName);

    if(!fnt)
        return;

    // Get position of the text
    Math::float2 absTranslation = getAbsoluteTranslation();
    int px = (int) (absTranslation.x * config.state.viewWidth + 0.5f);
    int py = (int) (absTranslation.y * config.state.viewHeight + 0.5f);

	if (!item.text[0].empty())
	{
		// Menu has a size of 640x480 and the center text has a width of 500
		unsigned int px_offset = (640 - 500) / 2;
		py += px_offset;

		// Find first line which should be drawn
		unsigned int i, k, lineCounter = 0;
		for (i = 0; i < m_FormattedEntries.size(); i++) {
			for (k = 0; k < m_FormattedEntries.at(i).size(); k++) {
				if (lineCounter == m_FirstDrawnLine)
					goto foundEntry;
				else
					lineCounter++;
			}
		}
	    foundEntry:

		// Draw 19 lines of text inside the content viewer
		unsigned int drawnLines = 0;
		while (drawnLines < 19) {
			drawText(m_FormattedEntries.at(i).at(k), px + px_offset, py, A_TopLeft, config, item.fontName);

			if (m_FormattedEntries.at(i).size() - 1 > k) {
				k++;
			} else if (m_FormattedEntries.size() - 1 > i) {
				k = 0;
				i++;
			} else break;

			py += fnt->getFontHeight();
			drawnLines++;
		}
	}
}

void UI::MenuItemTypes::MenuItemScrollableText::setTopicEntries(std::vector<std::string> &entries)
{
	Daedalus::GEngineClasses::C_Menu_Item& item = getItemScriptData();
	const UI::zFont* fnt = m_Engine.getFontCache().getFont(item.fontName);

	if (!fnt)
		return;

	// Because the content viewer view instance is reused for 
	// each topic entries we have to clear all entries first
	m_FormattedEntries.clear();
	m_NumberOfFormattedEntries = 0;
	m_FirstDrawnLine = 0;

	for (unsigned int i=0; i<entries.size(); i++)
	{
		std::vector<std::string> text = fnt->layoutText(entries.at(i), 500);
		m_NumberOfFormattedEntries += text.size();
		m_FormattedEntries.push_back( std::move(text) );

		// Add line separator not to the last entry
		if (i + 1 < entries.size()) {
			std::vector<std::string> sep;
			sep.push_back("---");
			m_NumberOfFormattedEntries += sep.size();
			m_FormattedEntries.push_back(std::move(sep));
		}
	}
}

void UI::MenuItemTypes::MenuItemScrollableText::scrollDown()
{
	if(m_NumberOfFormattedEntries > m_FirstDrawnLine + 1)
		m_FirstDrawnLine++;
}

void UI::MenuItemTypes::MenuItemScrollableText::scrollUp()
{
	if(m_FirstDrawnLine > 0)
		m_FirstDrawnLine--;
}

