#pragma once
#include "View.h"
#include "ItemGrid.h"

namespace UI
{
class InventoryView : public View
{
public:
    InventoryView(Engine::BaseEngine& e);
    ~InventoryView();

    /**
     * Updates/draws the UI-Views
     * @param dt time since last frame
     * @param mstate mouse-state
     */
    void update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config) override;

protected:
    int m_SelectedItemID;
    ItemGrid m_SelfItemGrid;
    //ItemGrid m_TradeItemGrid;
    //ItemGrid m_LootItemGrid;
};

} // namespace UI
