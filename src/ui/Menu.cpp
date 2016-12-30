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

UI::Menu::Menu(Engine::BaseEngine& e) : View(e), m_pVM(nullptr)
{
    m_pBackgroundImage = nullptr;
}

UI::Menu::~Menu()
{
    delete m_pVM;

    removeChild(m_pBackgroundImage);
    delete m_pBackgroundImage;

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
    Daedalus::registerDaedalusStdLib(*m_pVM);
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
            case Daedalus::GEngineClasses::C_Menu_Item::MENU_ITEM_TEXT: m = new MenuItemTypes::MenuItemText(m_Engine, *this, h); break;
            case Daedalus::GEngineClasses::C_Menu_Item::MENU_ITEM_SLIDER:break;
            case Daedalus::GEngineClasses::C_Menu_Item::MENU_ITEM_INPUT:break;
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
        }
    }

    return outMap;
}


