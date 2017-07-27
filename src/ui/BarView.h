#pragma once
#include "View.h"
#include <content/Texture.h>

namespace UI
{
    static const Math::float2 INNER_OFFSET_DEFAULT = {(350.0f / 8192.0f), (1100.0f / 8192.0f)};
    static const Math::float2 INNER_OFFSET_G1_PROGRESS = {(350.0f / 8192.0f), (1100.0f / 8192.0f)};
    static const Math::float2 INNER_OFFSET_G2_PROGRESS = {(1200.0f / 8192.0f), (1100.0f / 8192.0f)};

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
        void update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config) override;

        /**
         * @param v Value in range of 0..1
         */
        void setValue(float v);

        /**
         * @param value current value
         * @param maxValue current max Value, if 0 then m_Value will be set to 0
         */
        void setValue(int32_t value, int32_t maxValue);

        void setBackgroundImage(Handle::TextureHandle texture);
        void setBarImage(Handle::TextureHandle texture);

        /**
         * Offset to be applied to the inner part of the bar.
         * Defaults to the usual Health/Mana-Bars.
         */
        void setInnerOffset(const Math::float2& offset);

    protected:
        /**
         * Progress-value of this bar (range 0..1)
         */
        float m_Value;

        /**
         * Views for rendering
         */
        Handle::TextureHandle m_Background;
        Handle::TextureHandle m_Bar;

        /**
         * Pixel-Edge-Offset for the inner part of the bar
         */
        Math::float2 m_InnerOffset;
    };
}
