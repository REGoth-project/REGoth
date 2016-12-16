//
// Created by desktop on 16.12.16.
//

#include <render/RenderSystem.h>
#include "ImageView.h"

extern bgfx::ProgramHandle imguiGetImageProgram(uint8_t _mip);

UI::ImageView::ImageView()
{


}

UI::ImageView::~ImageView()
{

}

void UI::ImageView::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    // Un-normalize transforms
    Math::float2 absTranslation = getAbsoluteTranslation();
    Math::float2 absSize = getAbsoluteSize();
    int px = (int)(absTranslation.x * config.state.viewWidth + 0.5f);
    int py = (int)(absTranslation.y * config.state.viewHeight + 0.5f);
    int width = (int)(absSize.x * config.state.viewWidth + 0.5f);
    int height = (int)(absSize.y * config.state.viewHeight+ 0.5f);

    bgfx::ProgramHandle program = ::imguiGetImageProgram(1);
    drawTexture(BGFX_VIEW, px, py, width, height,
                config.state.viewWidth, config.state.viewHeight, m_Image, program, config.uniforms.diffuseTexture);

    View::update(dt, mstate, config);
}

void UI::ImageView::setImage(bgfx::TextureHandle image)
{
    m_Image = image;
}
