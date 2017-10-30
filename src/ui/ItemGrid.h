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
    class ItemGrid : public View
    {
    public:
        ItemGrid(Engine::BaseEngine& e);
        ~ItemGrid();

        void setItemList( const std::vector<Daedalus::GEngineClasses::C_Item> *itemList, int selectedItem);

        void probeGridSize(int &rows, int &columns, uint32_t width, uint32_t height);

        void getGridSize(int &rows, int &columns);

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config) override;

        void drawInventory(double dt, Render::RenderConfig& config, const std::vector<Daedalus::GEngineClasses::C_Item> &itemList,
                           const std::vector<int> &indices, int selected);

    private:
        // width and height of a slot (square slots hard coded)
        int m_SlotSize;
        int m_Rows;
        int m_Columns;

        void calculateAlignedOrigin(int &xOrigin, int &yOrigin, int numRows, int numColumns);

        Handle::TextureHandle m_TexSlot;
        Handle::TextureHandle m_TexSlotHighlighted;
        Handle::TextureHandle m_TexInvBack;

        Math::Matrix applyRotationsAnsScale(Math::float3 centerPos, const Daedalus::GEngineClasses::C_Item &itemData, bool selected);
    };
}
