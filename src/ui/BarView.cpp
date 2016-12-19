//
// Created by desktop on 16.12.16.
//

#include <content/Texture.h>
#include "BarView.h"
#include <imgui/imgui.h>
#include "ImageView.h"

const float INNER_OFFSET_X = (350.0f / 8192.0f);
const float INNER_OFFSET_Y = (1100.0f / 8192.0f);

extern bgfx::ProgramHandle imguiGetImageProgram(uint8_t _mip);

UI::BarView::BarView()
{
    m_Value = 1.0f;
}

UI::BarView::~BarView()
{

}

void UI::BarView::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    bgfx::ProgramHandle program = config.programs.imageProgram;

    // Un-normalize transforms
    Math::float2 absTranslation = getAbsoluteTranslation();
    Math::float2 absSize = getAbsoluteSize();
    Math::float2 offset = getAlignOffset(m_Alignment, m_Background.m_Width * absSize.x, m_Background.m_Height * absSize.y);
    int px = (int) (absTranslation.x * config.state.viewWidth + 0.5f);
    int py = (int) (absTranslation.y * config.state.viewHeight + 0.5f);

    int sx = (int) (m_Background.m_Width * absSize.x + 0.5f);
    int sy = (int) (m_Background.m_Height * absSize.y + 0.5f);

    px += offset.x;
    py += offset.y;

    //absSize.x *= m_Background.m_Width / (float) config.state.viewWidth;
    //absSize.y *= m_Background.m_Height / (float) config.state.viewHeight;

    // Draw background
    drawTexture(BGFX_VIEW, px, py,
                (int)(m_Background.m_Width * absSize.x + 0.5f), (int)(m_Background.m_Height * absSize.y + 0.5f),
                config.state.viewWidth, config.state.viewHeight, m_Background.m_TextureHandle,
                program, config.uniforms.diffuseTexture);

    // Draw bar
    // The bar has a little border around itself, since its texture the same size as the background texture
    int pxBorderX = (int)((INNER_OFFSET_X * m_Background.m_Width) + 0.5f);
    int pxBorderY = (int)((INNER_OFFSET_Y * m_Background.m_Height) + 0.5f);

    drawTexture(BGFX_VIEW, px + pxBorderX * absSize.x, py + pxBorderY * absSize.y,
                (int)((sx - pxBorderX * 2 * absSize.x) * m_Value + 0.5f),
                (int)((sy - pxBorderY * 2 * absSize.y) + 0.5f),
                config.state.viewWidth, config.state.viewHeight, m_Bar.m_TextureHandle,
                program, config.uniforms.diffuseTexture);

    View::update(dt, mstate, config);
}

void UI::BarView::setValue(float v)
{
    m_Value = v;
}

void UI::BarView::setBackgroundImage(const Textures::Texture& texture)
{
    m_Background = texture;
}

void UI::BarView::setBarImage(const Textures::Texture& texture)
{
    m_Bar = texture;
}
