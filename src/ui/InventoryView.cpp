#include "InventoryView.h"
#include <engine/BaseEngine.h>
#include <logic/Inventory.h>
#include <logic/PlayerController.h>

UI::InventoryView::InventoryView(Engine::BaseEngine& e)
    : View(e),
      m_SelfItemGrid(e)
{
    m_SelectedItemID = -1;
    addChild(&m_SelfItemGrid);
}

UI::InventoryView::~InventoryView()
{
    removeChild(&m_SelfItemGrid);
}

void UI::InventoryView::update(double dt, Engine::Input::MouseState &mstate, Render::RenderConfig &config)
{
    if(m_IsHidden)
        return;

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

    //m_SelfItemGrid.setAlignment(EAlign::A_TopRight);
    //m_SelfItemGrid.setTranslation(Math::float2(0.6f * config.state.viewWidth, 0.2f * config.state.viewHeight));
    //m_SelfItemGrid.setSize(Math::float2(0.4f * config.state.viewWidth, 0.6f * config.state.viewHeight));

    m_SelfItemGrid.setAlignment(EAlign::A_BottomRight);
    m_SelfItemGrid.setTranslation(Math::float2(0.6f * config.state.viewWidth, 0.2f * config.state.viewHeight));
    m_SelfItemGrid.setSize(Math::float2(0.4f * config.state.viewWidth, 0.8f * config.state.viewHeight));

    // TODO Dangerous coding style !!!
    // We pass a pointer which is only valid in this stack frame
    // If ItemGrid class uses it outside this stack frame we will crash!
    m_SelfItemGrid.setItemList(&itemList, 0);

    View::update(dt, mstate, config);
}
