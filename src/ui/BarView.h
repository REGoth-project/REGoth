#pragma once
#include "View.h"

namespace UI
{
    class ImageView;
    class BarView : public View
    {
    public:
        BarView();
        ~BarView();

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, Engine::Input::MouseState &mstate, Render::RenderConfig &config) override;

        /**
         * @param v Value in range of 0..1
         */
        void setValue(float v);

        void setBackgroundImage(bgfx::TextureHandle tx);
        void setBarImage(bgfx::TextureHandle tx);

    protected:

        /**
         * Progress-value of this bar (range 0..1)
         */
        float m_Value;

        /**
         * Views for rendering
         */
        UI::ImageView* m_pBackground;
        UI::ImageView* m_pBar;
    };
}
