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

    /**
     * @brief enabled Enabled is synonymous with not being hidden
     * @return Whether the inventory is enabled
     */
    bool enabled() { return !m_IsHidden;}

    /**
     * @brief setEnabled Acticates / deactivates input bindings and hides / unhides the view
     * @param enabled Desired enable - state
     */
    void setEnabled(bool enabled);

protected:
    size_t m_SelectedItemID;
    int m_LastTimeSelectedItemSlot;
    int m_DisplayedRowsStart;
    int m_DeltaRows;
    int m_DeltaColumns;

    enum ItemDrawFlags
    {
        Selected = 1 << 0,
        Equipped = 1 << 1
    };

    struct ItemDrawState
    {
        // Index into an array of items
        int index;
        // Additional information for rendering the item or it's slot
        int flags; // ItemDrawFlags
    };

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
                      const std::vector<ItemDrawState> &itemsToDraw);
                      //const std::vector<int> &indices, int selected,
                      //const std::vector<int> &equipped);

    void drawItemInfobox(Render::RenderConfig& config, const Daedalus::GEngineClasses::C_Item &item,
                         Math::float2 position, Math::float2 size);

    static int32_t getPrimarySortValue(const Daedalus::GEngineClasses::C_Item &item);
    bool compareItems(const Daedalus::GEngineClasses::C_Item &l, const Daedalus::GEngineClasses::C_Item &r);

    Handle::TextureHandle m_TexSlot;
    Handle::TextureHandle m_TexSlotHighlighted;
    Handle::TextureHandle m_TexSlotEquipped;
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
