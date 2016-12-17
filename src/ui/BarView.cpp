//
// Created by desktop on 16.12.16.
//

#include <content/Texture.h>
#include "BarView.h"
#include "ImageView.h"

UI::BarView::BarView()
{
    m_pBackground = new UI::ImageView();
    m_pBar = new UI::ImageView();

    m_pBar->setAlignment(A_Center);
    m_pBackground->setAlignment(A_Center);

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
    m_pBar->setTranslation(Math::float2(0.1,0.1));
    m_pBar->setSize(Math::float2(m_Value * 0.8f, 0.8f));
    m_pBar->setAlignment(m_Alignment);
    m_pBackground->setAlignment(m_Alignment);

    View::update(dt, mstate, config);
}

void UI::BarView::setValue(float v)
{
    m_Value = v;
}

void UI::BarView::setBackgroundImage(const Textures::Texture& texture)
{
    m_pBackground->setImage(texture.m_TextureHandle, texture.m_Width, texture.m_Height);
}

void UI::BarView::setBarImage(const Textures::Texture& texture)
{
    m_pBar->setImage(texture.m_TextureHandle, texture.m_Width, texture.m_Height);
}
