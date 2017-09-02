#pragma once
#include "View.h"
#include <handle/HandleDef.h>

namespace UI
{
    class ImageView : public View
    {
    public:
        ImageView(Engine::BaseEngine& e);
        ~ImageView();

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config) override;

        /**
         * Sets the text to be displayed inside the box
         */
        void setImage(Handle::TextureHandle image, int32_t width = 0, int32_t height = 0);

        /**
         * @param rel If set to true, images actual width and height will be ignored and it will be stretched to fit
         *            with the given m_Size-value of the View-class.
         */
        void setRelativeSize(bool rel) { m_RelativeSize = rel; }

    protected:
        /**
         * Image to display
         */
        Handle::TextureHandle m_Image;

        /**
         * Width and height of the given image
         */
        int32_t m_ImageWidth;
        int32_t m_ImageHeight;

        /**
         * If set to true, images actual width and height will be ignored and it will be stretched to fit
         * with the given m_Size-value of the View-class.
         */
        bool m_RelativeSize;
    };
}
