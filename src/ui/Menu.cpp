//
// Created by desktop on 21.12.16.
//

#include "Menu.h"
#include "MenuItem.h"
#include <daedalus/DaedalusVM.h>
#include <utils/Utils.h>
#include <engine/BaseEngine.h>
#include <ZenLib/utils/logger.h>
#include <render/RenderSystem.h>
#include "ImageView.h"
#include "TextView.h"
#include "Hud.h"

UI::Menu::Menu(Engine::BaseEngine& e) : View(e), m_pVM(nullptr)
{
    m_pBackgroundImage = nullptr;
    m_pInfoText = nullptr;
    m_SelectedItem = 0;
}

UI::Menu::~Menu()
{
    delete m_pVM;

    removeChild(m_pBackgroundImage);
    delete m_pBackgroundImage;

    if(m_pInfoText)
    {
        removeChild(m_pInfoText);
        delete m_pInfoText;
    }

    for(auto& item: m_Items)
    {
        removeChild(item.second);
        delete item.second;
    }
}


void UI::Menu::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    if(m_IsHidden)
        return;

    // Check if we want unscaled views
    if((getScriptData().flags & Daedalus::GEngineClasses::C_Menu::MENU_DONTSCALE_DIM) != 0)
    {
        // Gothic menus are normalized to 640,480.
        Math::float2 fac = Math::float2(640.0f / config.state.viewWidth, 480.0f / config.state.viewHeight);

        setSize(Math::float2((getScriptData().dimx / 8192.0f) * fac.x, (getScriptData().dimy / 8192.0f) * fac.y));
    }

    // Center view, if wanted
    if((getScriptData().flags & Daedalus::GEngineClasses::C_Menu::MENU_ALIGN_CENTER) != 0)
    {
        Math::float2 c = Math::float2(0.5f, 0.5f) - m_Size * 0.5f;
        setTranslation(c);
    }else if((getScriptData().flags & Daedalus::GEngineClasses::C_Menu::MENU_DONTSCALE_DIM) != 0)
    {
        // Note: I guess they meant to use MENU_DONTSCALE_POS here, but this is what they do

        // Gothic menus are normalized to 640,480.
        Math::float2 fac = Math::float2(640.0f / config.state.viewWidth, 480.0f / config.state.viewHeight);

        setTranslation(Math::float2((getScriptData().posx / 8192.0f) * fac.x, (getScriptData().posx / 8192.0f) * fac.y));
    }

    
    // Lighten up the selected item
    if(!m_SelectableItems.empty())
    {
        // Make sure we really do have a selectable item active
        unsigned cnt = 0;
        while(!m_Items[m_SelectableItems[m_SelectedItem]]->isSelectable() && cnt != m_SelectableItems.size())
        {
            m_SelectedItem = Utils::mod(m_SelectedItem + 1, static_cast<int>(m_SelectableItems.size()));
            cnt++;
        }

        // Savety
        m_SelectedItem = Utils::mod(m_SelectedItem, static_cast<int>(m_SelectableItems.size()));

        // Disable highlight on all of them
        for(Daedalus::GameState::MenuItemHandle h : m_SelectableItems)
            m_Items[h]->setFontHighlighted(false);

        // Highlight the selected one
        m_Items[m_SelectableItems[m_SelectedItem]]->setFontHighlighted(true);

        // Update info-text
        if((getScriptData().flags & Daedalus::GEngineClasses::C_Menu::MENU_SHOW_INFO) != 0 && m_pInfoText)
        {
            m_pInfoText->setText(getItemScriptData(m_SelectableItems[m_SelectedItem]).text[1]);
        }
    }

    View::update(dt, mstate, config);
}

void UI::Menu::initializeInstance(const std::string& instance)
{
    // Load MENU.DAT in case this hasn't been done yet
    if(!loadMenuDAT())
        return;

    // Create and initialize instance for this very menu
    m_MenuHandle = m_pVM->getGameState().createMenu();

    m_pVM->initializeInstance(ZMemory::toBigHandle(m_MenuHandle),
                              m_pVM->getDATFile().getSymbolIndexByName(instance),
                              Daedalus::IC_Menu);


    // Create background image first
    m_pBackgroundImage = new ImageView(m_Engine);
    m_pBackgroundImage->setTranslation(Math::float2(getScriptData().posx / 8192.0f, getScriptData().posy / 8192.0f));
    m_pBackgroundImage->setSize(Math::float2(getScriptData().dimx / 8192.0f, getScriptData().dimy / 8192.0f));

    Handle::TextureHandle bgr = m_Engine.getEngineTextureAlloc().loadTextureVDF(getScriptData().backPic);
    m_pBackgroundImage->setImage(m_Engine.getEngineTextureAlloc().getTexture(bgr).m_TextureHandle);
    m_pBackgroundImage->setRelativeSize(false); // Scale like this screen
    addChild(m_pBackgroundImage);

    // Create all other items drawn on top
    m_Items = initializeItems();


    if((getScriptData().flags & Daedalus::GEngineClasses::C_Menu::MENU_SHOW_INFO) != 0)
    {
        m_pInfoText = new TextView(m_Engine);
        addChild(m_pInfoText);

        float infoX = 1000.0f / 8192.0f;
        float infoY = 7500.0f / 8192.0f;

        // There could be script-defined values

        if(m_pVM->getDATFile().hasSymbolName("MENU_INFO_X") && m_pVM->getDATFile().hasSymbolName("MENU_INFO_X"))
        {
            Daedalus::PARSymbol& symX =  m_pVM->getDATFile().getSymbolByName("MENU_INFO_X");
            Daedalus::PARSymbol& symY =  m_pVM->getDATFile().getSymbolByName("MENU_INFO_Y");

            infoX = symX.getIntValue() / 8192.0f;
            infoY = symY.getIntValue() / 8192.0f;
        }
        float sX = 1.0f - infoX * 2.0f;
        float sY = 1.0f - infoY;

        infoX += sX / 2;
        // infoY += sY / 2;

        m_pInfoText->setTranslation(Math::float2(infoX, infoY));
        m_pInfoText->setAlignment(A_TopCenter);
        m_pInfoText->setFont(UI::DEFAULT_FONT);
    }
}

bool UI::Menu::loadMenuDAT()
{
    if(m_pVM)
        return true; // Nothing to do, already loaded

    std::string datPath = "/_work/data/Scripts/_compiled/MENU.DAT";
    std::string datFile = Utils::getCaseSensitivePath(datPath, m_Engine.getEngineArgs().gameBaseDirectory);

    if(!Utils::fileExists(datFile))
    {
        LogError() << "Failed to find MENU.DAT at: " << datFile;
        return false;
    }

    // Load DAT-File...
    m_pVM = new Daedalus::DaedalusVM(datFile);
    Daedalus::registerGothicEngineClasses(*m_pVM);

    return true;
}

Daedalus::GEngineClasses::C_Menu& UI::Menu::getScriptData()
{
    return m_pVM->getGameState().getMenu(m_MenuHandle);
}

Daedalus::GEngineClasses::C_Menu_Item& UI::Menu::getItemScriptData(Daedalus::GameState::MenuItemHandle item)
{
    return m_pVM->getGameState().getMenuItem(item);
}

Daedalus::GEngineClasses::C_Menu_Item& UI::Menu::getItemScriptData(const std::string& instance)
{
    auto& sym = m_pVM->getDATFile().getSymbolByName(instance);

    assert(sym.instanceDataClass == Daedalus::IC_MenuItem);

    Daedalus::GameState::MenuItemHandle h = ZMemory::handleCast<Daedalus::GameState::MenuItemHandle>(sym.instanceDataHandle);
    return getItemScriptData(h);
}


std::map<Daedalus::GameState::MenuItemHandle, UI::MenuItem*> UI::Menu::initializeItems()
{
    Daedalus::GEngineClasses::C_Menu& menu = getScriptData();
    std::vector<Daedalus::GameState::MenuItemHandle> items;

    for(int i=0;i<Daedalus::GEngineClasses::MenuConstants::MAX_ITEMS;i++)
    {
        if (!menu.items[i].empty())
        {
            items.push_back(m_pVM->getGameState().createMenuItem());

            LogInfo() << "Initializing item: " << menu.items[i];
            m_pVM->initializeInstance(ZMemory::toBigHandle(items.back()),
                                      m_pVM->getDATFile().getSymbolIndexByName(menu.items[i]),
                                      Daedalus::IC_MenuItem);
        }
    }

    // Create actual views
    std::map<Daedalus::GameState::MenuItemHandle, MenuItem*> outMap;
    for(Daedalus::GameState::MenuItemHandle& h : items)
    {
        Daedalus::GEngineClasses::C_Menu_Item& itemData = getItemScriptData(h);

        MenuItem* m = nullptr;
        switch(itemData.type)
        {
            case Daedalus::GEngineClasses::C_Menu_Item::MENU_ITEM_UNDEF:break;
            case Daedalus::GEngineClasses::C_Menu_Item::MENU_ITEM_INPUT:
            case Daedalus::GEngineClasses::C_Menu_Item::MENU_ITEM_TEXT: m = new MenuItemTypes::MenuItemText(m_Engine, *this, h); break;
            case Daedalus::GEngineClasses::C_Menu_Item::MENU_ITEM_SLIDER:break;
            case Daedalus::GEngineClasses::C_Menu_Item::MENU_ITEM_CURSOR:break;
            case Daedalus::GEngineClasses::C_Menu_Item::MENU_ITEM_CHOICEBOX:break;
            case Daedalus::GEngineClasses::C_Menu_Item::MENU_ITEM_BUTTON:break;
            case Daedalus::GEngineClasses::C_Menu_Item::MENU_ITEM_LISTBOX:break;
            default:
                LogWarn() << "Invalid item type: " << itemData.type;
        }

        if(m)
        {
            addChild(m);
            outMap[h] = m;

            if(m->isSelectable())
                m_SelectableItems.push_back(h);
        }
    }

    return outMap;
}


void UI::Menu::onInputAction(EInputAction action)
{
    switch(action)
    {
        case IA_Up:
            if(!m_SelectableItems.empty()) 
            {
                // Make sure we really do have a selectable item active
                unsigned cnt = 0;
                do{
                    m_SelectedItem = Utils::mod(m_SelectedItem - 1, static_cast<int>(m_SelectableItems.size()));
                    cnt++;
                }while(!m_Items[m_SelectableItems[m_SelectedItem]]->isSelectable() && cnt != m_SelectableItems.size());
            }
            break;

        case IA_Down:
            if(!m_SelectableItems.empty()) 
            {
                // Skip all items which are no longer selectable
                do{
                    m_SelectedItem = Utils::mod(m_SelectedItem + 1, static_cast<int>(m_SelectableItems.size()));
                }while(!m_Items[m_SelectableItems[m_SelectedItem]]->isSelectable());
            }
            break; 
        
        case IA_Accept:
            if(!m_SelectableItems.empty()) performSelectAction(m_SelectableItems[m_SelectedItem]);
            break;

        case IA_Close:
            setHidden(true);
            break;
        default:
            break;
    }
}


void UI::Menu::performSelectAction(Daedalus::GameState::MenuItemHandle item)
{
    MenuItem* iData = m_Items[item];

    using namespace Daedalus::GEngineClasses::MenuConstants;

    switch(iData->getSelectionEvent(0))
    {
        case SEL_ACTION_BACK:
            m_Engine.getHud().popMenu();
            break;

        default:
            break;
    } 


    std::string customFn = iData->getItemScriptData().onSelAction_S[0];
    if(!customFn.empty())
        onCustomAction(customFn);                    
}
UI::Hud& UI::Menu::getHud()
{
    return m_Engine.getHud();
}

void UI::Menu::onTextInput(const std::string& text)
{

}

