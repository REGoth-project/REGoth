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
        void update(double dt, Engine::Input::MouseState &mstate, Render::RenderConfig &config) override;

        /**
         * @param v Value in range of 0..1
         */
        void setValue(float v);

        /**
         * @param value current value
         * @param maxValue current max Value, if 0 then m_Value will be set to 0
         */
        void setValue(int32_t value, int32_t maxValue);

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
