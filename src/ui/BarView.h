#pragma once
#include "View.h"
#include <content/Texture.h>

namespace UI
{
    class ImageView;
    class BarView : public View
    {
    public:
        BarView(Engine::BaseEngine& e);
        ~BarView();

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, Engine::MouseState &mstate, Render::RenderConfig &config) override;

        /**
         * @param v Value in range of 0..1
         */
        void setValue(float v);

        void setBackgroundImage(const Textures::Texture& texture);
        void setBarImage(const Textures::Texture& texture);

    protected:

        /**
         * Progress-value of this bar (range 0..1)
         */
        float m_Value;

        /**
         * Views for rendering
         */
        Textures::Texture m_Background;
        Textures::Texture m_Bar;
    };
}
