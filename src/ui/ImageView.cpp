//
// Created by desktop on 16.12.16.
//

#include <render/RenderSystem.h>
#include "ImageView.h"

extern bgfx::ProgramHandle imguiGetImageProgram(uint8_t _mip);

UI::ImageView::ImageView(Engine::BaseEngine& e) : View(e)
{
    m_RelativeSize = true;
    m_ImageHeight = 0;
    m_ImageWidth = 0;

}

UI::ImageView::~ImageView()
{

}

void UI::ImageView::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    if(m_IsHidden)
        return;


    if(bgfx::isValid(m_Image))
    {
        // Un-normalize transforms
        Math::float2 offset = getAlignOffset(m_Alignment, m_ImageWidth, m_ImageHeight);
        Math::float2 absTranslation = getAbsoluteTranslation();
        Math::float2 absSize = getAbsoluteSize();
        int px = (int) (absTranslation.x * config.state.viewWidth + 0.5f);
        int py = (int) (absTranslation.y * config.state.viewHeight + 0.5f);

        px += offset.x;
        py += offset.y;

        int width;
        int height;

        if (m_RelativeSize)
        {
            absSize.x *= m_ImageWidth / (float) config.state.viewWidth;
            absSize.y *= m_ImageHeight / (float) config.state.viewHeight;
        }

        width = (int) (absSize.x * config.state.viewWidth + 0.5f);
        height = (int) (absSize.y * config.state.viewHeight + 0.5f);

        bgfx::ProgramHandle program = config.programs.imageProgram;
        drawTexture(BGFX_VIEW, px, py, width, height,
                    config.state.viewWidth, config.state.viewHeight, m_Image, program, config.uniforms.diffuseTexture);
    }

    View::update(dt, mstate, config);
}

void UI::ImageView::setImage(bgfx::TextureHandle image, int32_t width, int32_t height)
{
    m_Image = image;
    m_ImageWidth = width;
    m_ImageHeight = height;
}
