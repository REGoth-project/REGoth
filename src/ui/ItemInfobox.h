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
    class ItemInfobox : public View
    {
    public:
        ItemInfobox(Engine::BaseEngine& e);
        ~ItemInfobox();

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config) override;

        void drawInfobox(Render::RenderConfig& config, const Daedalus::GEngineClasses::C_Item &item);

    private:
        Handle::TextureHandle m_TexInvBack;

    };
}
