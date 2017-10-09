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

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config) override;

    private:
        // width and height of a slot (square slots hard coded)
        int m_SlotSize;

        // screen space for the inventory in normalized coordinates (so the inventory scales with window size)
        // (0,0) is the top left corner, (1,1) the bottom right corner;
        Math::float2 m_Position;
        Math::float2 m_Size;

        Handle::TextureHandle m_TexSlot;
        //Handle::TextureHandle m_SlotSelected;
    };
}
