#include "InventoryView.h"
#include <engine/BaseEngine.h>
#include <logic/Inventory.h>
#include <logic/PlayerController.h>
#include <logic/ScriptEngine.h>

UI::InventoryView::InventoryView(Engine::BaseEngine& e)
    : View(e),
      m_SelfItemGrid(e),
      m_ItemInfobox(e)
{
    m_SelectedItemID = -1;
    addChild(&m_SelfItemGrid);

    m_SelectedSlot = 0;
    m_DeltaRows = 0;
    m_DeltaColumns = 0;
    m_DisplayedRowsStart = 0;

    m_InputUp = Engine::Input::RegisterAction(Engine::ActionType::Inventory_Up, [this](bool triggered, float)
    {
        if (triggered)
            m_DeltaRows -= 1;
    });
    m_InputUp.getAction().setEnabled(false);
    m_InputDown = Engine::Input::RegisterAction(Engine::ActionType::Inventory_Down, [this](bool triggered, float)
    {
        if (triggered)
            m_DeltaRows += 1;
    });
    m_InputDown.getAction().setEnabled(false);
    m_InputLeft = Engine::Input::RegisterAction(Engine::ActionType::Inventory_Left, [this](bool triggered, float)
    {
        if (triggered)
            m_DeltaColumns -= 1;
    });
    m_InputLeft.getAction().setEnabled(false);
    m_InputRight = Engine::Input::RegisterAction(Engine::ActionType::Inventory_Right, [this](bool triggered, float)
    {
        if (triggered)
            m_DeltaColumns += 1;
    });
    m_InputRight.getAction().setEnabled(false);
}

UI::InventoryView::~InventoryView()
{
    removeChild(&m_SelfItemGrid);
}

void UI::InventoryView::update(double dt, Engine::Input::MouseState &mstate, Render::RenderConfig &config)
{
    if(m_IsHidden)
        return;

    // Just some debug things
    static bool isFirstTime = true;
    if(isFirstTime)
    {
        isFirstTime = false;

        if( Daedalus::GameType::GT_Gothic1 == m_Engine.getBasicGameType())
        {
            m_Engine.getConsole().submitCommand("giveitem ALTESSCHWERT");
            m_Engine.getConsole().submitCommand("giveitem HEILTRANK");
            m_Engine.getConsole().submitCommand("giveitem CRONOS_BRIEF");
            m_Engine.getConsole().submitCommand("giveitem CORDS_SPALTER");
            m_Engine.getConsole().submitCommand("giveitem ITAMARROW");
            m_Engine.getConsole().submitCommand("giveitem HEALTHWATER");
            m_Engine.getConsole().submitCommand("giveitem ASTRONOMIE");
            m_Engine.getConsole().submitCommand("giveitem ITARRUNEFIREBOLT");
        }
        else if( Daedalus::GameType::GT_Gothic2 == m_Engine.getBasicGameType())
        {
            m_Engine.getConsole().submitCommand("giveitem ITFP_POTION_MANA_02");
            m_Engine.getConsole().submitCommand("giveitem ITRI_ADDON_HEALTH_01");
            m_Engine.getConsole().submitCommand("giveitem ITAM_ADDON_HEALTH");
            m_Engine.getConsole().submitCommand("giveitem ITWR_STONEPLATECOMMON_ADDON");
            m_Engine.getConsole().submitCommand("giveitem ITMW_ADDON_PIR1HAXE");
            m_Engine.getConsole().submitCommand("giveitem ITFO_BEER");
            m_Engine.getConsole().submitCommand("giveitem ITRU_ARMYOFDARKNESS");
            m_Engine.getConsole().submitCommand("giveitem ITPO_HEALTH_01");
            m_Engine.getConsole().submitCommand("giveitem ITFO_APPLE");
            m_Engine.getConsole().submitCommand("giveitem ITKE_BROMOR");
            m_Engine.getConsole().submitCommand("giveitem ITPL_MUSHROOM_01");
            m_Engine.getConsole().submitCommand("giveitem ITPL_MUSHROOM_02");
            m_Engine.getConsole().submitCommand("giveitem ITFO_CHEESE");
            m_Engine.getConsole().submitCommand("giveitem ITKE_LOCKPICK");
            m_Engine.getConsole().submitCommand("giveitem ITWR_MAP_NEWWORLD_CITY");
            m_Engine.getConsole().submitCommand("giveitem ITMI_PANFULL");
            m_Engine.getConsole().submitCommand("giveitem ITAT_WARANFIRETONGUE");
            m_Engine.getConsole().submitCommand("giveitem ITLSTORCH");
            m_Engine.getConsole().submitCommand("giveitem ITAR_PAL_H");
            m_Engine.getConsole().submitCommand("giveitem ITPL_STRENGTH_HERB_01");
            m_Engine.getConsole().submitCommand("giveitem ITMI_GOLD");
            m_Engine.getConsole().submitCommand("giveitem ITRI_DEX_01");
        }
    }

    World::WorldInstance &world = m_Engine.getMainWorld().get();
    Logic::PlayerController* playerController = reinterpret_cast<Logic::PlayerController*>(
                world.getEntity<Components::LogicComponent>(world.getScriptEngine().getPlayerEntity()).m_pLogicController);

    const std::list<Daedalus::GameState::ItemHandle> &itemHandleList = playerController->getInventory().getItems();
    std::vector<Daedalus::GEngineClasses::C_Item> itemList(itemHandleList.size());
    std::list<Daedalus::GameState::ItemHandle>::const_iterator itItem = itemHandleList.begin();
    for(uint32_t index = 0; index < itemList.size(); ++index)
    {
        itemList[index] = world.getScriptEngine().getGameState().getItem(*itItem);
        ++itItem;
    }

    m_SelfItemGrid.setAlignment(EAlign::A_TopRight);
    m_SelfItemGrid.setTranslation(Math::float2(0.7f * config.state.viewWidth, 0.1f * config.state.viewHeight));
    m_SelfItemGrid.setSize(Math::float2(0.3f * config.state.viewWidth, 0.8f * config.state.viewHeight));

    m_ItemInfobox.setSize(Math::float2(600, 140));
    m_ItemInfobox.setTranslation(Math::float2(0.5f * config.state.viewWidth - 300, config.state.viewHeight - 140 - 10));

    int numRows, numColumns, numSlots;
    m_SelfItemGrid.probeGridSize(numRows, numColumns, config.state.viewWidth, config.state.viewHeight);
    numSlots = numRows * numColumns;

    int maxRowsByItemCount = itemList.size() / numColumns;
    // Get old row and column
    int currentRow = m_SelectedSlot / numColumns;
    int currentColumn = m_SelectedSlot % numColumns;
    // Adjust row by input
    currentRow = std::max(0, std::min(currentRow + m_DeltaRows, maxRowsByItemCount));
    // and adjusts column by input
    // This is the actual index for the item in itemList which we want to select
    int itemIndex = std::max(0, std::min(currentRow * numColumns + currentColumn + m_DeltaColumns, int(itemList.size()) - 1));
    m_SelectedSlot = itemIndex;
    // Reset for next input
    m_DeltaRows = 0;
    m_DeltaColumns = 0;

    int itemRow = itemIndex / numColumns;
    // The item is above the rows we are currently showing
    if (itemRow < m_DisplayedRowsStart)
        m_DisplayedRowsStart = itemRow;
    // The item is below the rows we are currently showing
    if (itemRow >= m_DisplayedRowsStart + numRows)
        m_DisplayedRowsStart = itemRow - numRows + 1;
    // If there is enough space anyway, simply display all items
    if (itemList.size() <= numSlots)
        m_DisplayedRowsStart = 0;

    int offsetSlots = m_DisplayedRowsStart * numColumns;
    int numVisibleItems = std::min(numSlots, int(itemList.size() - offsetSlots));

    // Provide the render function a list of indices into the itemList array
    std::vector<int> indices(itemList.size());
    std::iota(indices.begin(), indices.end(), 0);

    // Sort the indices
    const auto &constItemList = itemList;
    std::sort(indices.begin(), indices.end(), [&constItemList](const int &a, const int &b)
    {
        return constItemList[a].value > constItemList[b].value;
    });

    // only display visible items
    std::vector<int> displayIndices(indices.begin() + offsetSlots, indices.begin() + offsetSlots + numVisibleItems);

    m_SelfItemGrid.drawInventory(dt, config, itemList, displayIndices, m_SelectedSlot - offsetSlots);
    m_ItemInfobox.drawInfobox(config, itemList[indices[m_SelectedSlot]]);

    View::update(dt, mstate, config);
}

void UI::InventoryView::setEnabled(bool enabled)
{
    m_IsHidden = !enabled;
    if(enabled)
    {
        m_InputUp.getAction().setEnabled(true);
        m_InputDown.getAction().setEnabled(true);
        m_InputLeft.getAction().setEnabled(true);
        m_InputRight.getAction().setEnabled(true);
    }
    else
    {
        m_InputUp.getAction().setEnabled(false);
        m_InputDown.getAction().setEnabled(false);
        m_InputLeft.getAction().setEnabled(false);
        m_InputRight.getAction().setEnabled(false);
    }
}
