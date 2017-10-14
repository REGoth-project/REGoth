#pragma once
#include "View.h"
#include <content/Texture.h>
#include <daedalus/DaedalusGameState.h>

namespace Logic
{
    class Inventory;
}

namespace UI
{
    //class ImageView;
    class ItemGrid : public View
    {
    public:
        ItemGrid(Engine::BaseEngine& e);
        ~ItemGrid();

        void setItemList( const std::vector<Daedalus::GEngineClasses::C_Item> *itemList, int selectedItem);

        void probeGridSize(int &rows, int &columns, uint32_t width, uint32_t height);

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config) override;

    private:
        // width and height of a slot (square slots hard coded)
        int m_SlotSize;

        void calculateAlignedOrigin(int &xOrigin, int &yOrigin, int numRows, int numColumns);

        Handle::TextureHandle m_TexSlot;
        Handle::TextureHandle m_TexSlotSelected;

        const std::vector<Daedalus::GEngineClasses::C_Item> *m_pItemList;
        int m_SelectedItem;

        Math::Matrix applyGothic2Rotations(Math::float3 centerPos, const Daedalus::GEngineClasses::C_Item &itemData, bool selected);
    };
}
