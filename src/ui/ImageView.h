#pragma once
#include "View.h"

namespace UI
{
    class ImageView : public View
    {
    public:
        ImageView();
        ~ImageView();

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, Engine::Input::MouseState &mstate, Render::RenderConfig &config) override;

        /**
         * Sets the text to be displayed inside the box
         */
        void setImage(bgfx::TextureHandle image);

    protected:

        /**
         * Image to display
         */
        bgfx::TextureHandle m_Image;

    };
}
