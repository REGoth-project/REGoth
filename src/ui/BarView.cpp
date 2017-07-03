//
// Created by desktop on 16.12.16.
//

#include "BarView.h"
#include <content/Texture.h>
#include <engine/BaseEngine.h>
#include <imgui/imgui.h>
#include "ImageView.h"

extern bgfx::ProgramHandle imguiGetImageProgram(uint8_t _mip);

UI::BarView::BarView(Engine::BaseEngine& e) : View(e)
{
    m_Value = 1.0f;
    setInnerOffset(INNER_OFFSET_DEFAULT);
}

UI::BarView::~BarView()
{
}

void UI::BarView::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    if (m_IsHidden)
        return;

    Textures::Texture& background = m_Engine.getEngineTextureAlloc().getTexture(m_Background);
    Textures::Texture& bar = m_Engine.getEngineTextureAlloc().getTexture(m_Bar);

    bgfx::ProgramHandle program = config.programs.imageProgram;

    // Un-normalize transforms
    Math::float2 absTranslation = getAbsoluteTranslation();
    Math::float2 absSize = getAbsoluteSize();
    Math::float2 offset = getAlignOffset(m_Alignment, background.m_Width * absSize.x, background.m_Height * absSize.y);
    int px = (int)(absTranslation.x * config.state.viewWidth + 0.5f);
    int py = (int)(absTranslation.y * config.state.viewHeight + 0.5f);

    int sx = (int)(background.m_Width * absSize.x + 0.5f);
    int sy = (int)(background.m_Height * absSize.y + 0.5f);

    px += offset.x;
    py += offset.y;

    //absSize.x *= m_Background.m_Width / (float) config.state.viewWidth;
    //absSize.y *= m_Background.m_Height / (float) config.state.viewHeight;

    // Draw background
    drawTexture(BGFX_VIEW, px, py,
                (int)(background.m_Width * absSize.x + 0.5f), (int)(background.m_Height * absSize.y + 0.5f),
                config.state.viewWidth, config.state.viewHeight, background.m_TextureHandle,
                program, config.uniforms.diffuseTexture);

    // Draw bar
    // The bar has a little border around itself, since its texture the same size as the background texture
    int pxBorderX = (int)((m_InnerOffset.x * background.m_Width) + 0.5f);
    int pxBorderY = (int)((m_InnerOffset.y * background.m_Height) + 0.5f);

    drawTexture(BGFX_VIEW, px + pxBorderX * absSize.x, py + pxBorderY * absSize.y,
                (int)((sx - pxBorderX * 2 * absSize.x) * m_Value + 0.5f),
                (int)((sy - pxBorderY * 2 * absSize.y) + 0.5f),
                config.state.viewWidth, config.state.viewHeight, bar.m_TextureHandle,
                program, config.uniforms.diffuseTexture);

    View::update(dt, mstate, config);
}

void UI::BarView::setValue(float v)
{
    m_Value = v;
}

void UI::BarView::setValue(int32_t value, int32_t maxValue)
{
    if (maxValue != 0)
    {
        m_Value = value / static_cast<float>(maxValue);
    }
    else
    {
        m_Value = 0;
    }
}

void UI::BarView::setBackgroundImage(Handle::TextureHandle texture)
{
    m_Background = texture;
}

void UI::BarView::setBarImage(Handle::TextureHandle texture)
{
    m_Bar = texture;
}

void UI::BarView::setInnerOffset(const Math::float2& offset)
{
    m_InnerOffset = offset;
}
