#pragma once
#include "View.h"

namespace Engine
{
    class BaseEngine;
}

namespace UI
{
    class BarView;
    class Hud : public View
    {
    public:
        Hud(Engine::BaseEngine& engine);
        ~Hud();

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, Engine::Input::MouseState &mstate, Render::RenderConfig &config) override;

        /**
         * @param value Value for health-bar in 0..1 range
         */
        void setHealth(float value);

        /**
         * @param value Value for mana-bar in 0..1 range
         */
        void setMana(float value);

        /**
         * @param value Value for enemy health-bar in 0..1 range
         */
        void setEnemyHealth(float value);
    protected:

        BarView* m_pHealthBar;
        BarView* m_pManaBar;
        BarView* m_pEnemyHealthBar;

        /**
         * Engine access, ie. for main texture allocator and VDFS-index
         */
        Engine::BaseEngine& m_Engine;
    };
}
