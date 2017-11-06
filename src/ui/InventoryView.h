#pragma once
#include "View.h"
#include <content/Texture.h>
#include <daedalus/DaedalusGameState.h>

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
    size_t m_SelectedItemID;
    int m_LastTimeSelectedItemSlot;
    int m_DisplayedRowsStart;
    int m_DeltaRows;
    int m_DeltaColumns;

    //int m_SlotSize;

    // Applies the rotations and scale typical for Gothic 1 & 2 inventory view. centerPos is
    // an offset relative to the item's mesh origin, it is the center positions for rotations
    // and scale. If selected is true the item will be scaled up a bit.
    Math::Matrix applyRotationsAndScale(Math::float3 centerPos,
                                        const Daedalus::GEngineClasses::C_Item &itemData,
                                        bool selected);

    // Renders the item into a square with the dimensions of size x size. The item's largest
    // bounding box is scaled to fit that size. Position denotes the upper left corner of that
    // square.
    void drawItem(Render::RenderConfig &config,
                  const Daedalus::GEngineClasses::C_Item &itemData,
                  Math::float2 position, float size, bool selected);

    void drawItemGrid(Render::RenderConfig& config, Math::float2 position,
                      Math::float2 size, UI::EAlign alignment, float slotSize,
                      const std::vector<Daedalus::GEngineClasses::C_Item> &itemList,
                      const std::vector<int> &indices, int selected);

    void drawItemInfobox(Render::RenderConfig& config, const Daedalus::GEngineClasses::C_Item &item,
                         Math::float2 position, Math::float2 size);

    Handle::TextureHandle m_TexSlot;
    Handle::TextureHandle m_TexSlotHighlighted;
    Handle::TextureHandle m_TexInvBack;

    Engine::ManagedActionBinding m_InputUp;
    Engine::ManagedActionBinding m_InputDown;
    Engine::ManagedActionBinding m_InputLeft;
    Engine::ManagedActionBinding m_InputRight;
    Engine::ManagedActionBinding m_InputUse;
    Engine::ManagedActionBinding m_InputDrop;
    Engine::ManagedActionBinding m_InputAlternate;
};

} // namespace UI
