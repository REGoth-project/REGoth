#pragma once
#include "View.h"
#include "ItemGrid.h"
#include "ItemInfobox.h"

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

    // Enabled is synonymous with not being hidden
    bool enabled() { return !m_IsHidden;}

    // Acticates / deactivates input bindings and hides / unhides the view
    void setEnabled(bool enabled);

protected:
    int m_SelectedItemID;
    int m_SelectedSlot;
    int m_DisplayedRowsStart;
    int m_DeltaRows;
    int m_DeltaColumns;

    ItemGrid m_SelfItemGrid;
    //ItemGrid m_TradeItemGrid;
    //ItemGrid m_LootItemGrid;

    ItemInfobox m_ItemInfobox;

    Engine::ManagedActionBinding m_InputUp;
    Engine::ManagedActionBinding m_InputDown;
    Engine::ManagedActionBinding m_InputLeft;
    Engine::ManagedActionBinding m_InputRight;
    Engine::ManagedActionBinding m_InputUse;
    Engine::ManagedActionBinding m_InputDrop;
    Engine::ManagedActionBinding m_InputAlternate;

};

} // namespace UI
