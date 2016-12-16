//
// Created by desktop on 16.12.16.
//

#include "BarView.h"
#include "ImageView.h"

UI::BarView::BarView()
{
    m_pBackground = new UI::ImageView();
    m_pBar = new UI::ImageView();

    addChild(m_pBackground);
    addChild(m_pBar);
}

UI::BarView::~BarView()
{
    removeChild(m_pBackground);
    removeChild(m_pBar);

    delete m_pBar;
    delete m_pBackground;
}

void UI::BarView::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    m_pBar->setSize(Math::float2(m_Value, 1.0f));

    View::update(dt, mstate, config);
}

void UI::BarView::setValue(float v)
{
    m_Value = v;
}

void UI::BarView::setBackgroundImage(bgfx::TextureHandle tx)
{
    m_pBackground->setImage(tx);
}

void UI::BarView::setBarImage(bgfx::TextureHandle tx)
{
    m_pBar->setImage(tx);
}
