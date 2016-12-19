//
// Created by desktop on 17.12.16.
//

#include "Hud.h"
#include "BarView.h"
#include <engine/BaseEngine.h>

UI::Hud::Hud(Engine::BaseEngine& engine) : m_Engine(engine)
{
    Textures::TextureAllocator& alloc = engine.getEngineTextureAlloc();

    m_pHealthBar = new BarView();
    m_pManaBar = new BarView();
    m_pEnemyHealthBar = new BarView();

    addChild(m_pHealthBar);
    addChild(m_pManaBar);
    addChild(m_pEnemyHealthBar);

    // Initialize status bars
    {
        Handle::TextureHandle hBarBackground = alloc.loadTextureVDF("BAR_BACK.TGA");
        Handle::TextureHandle hBarHealth = alloc.loadTextureVDF("BAR_HEALTH.TGA");
        Handle::TextureHandle hBarMana = alloc.loadTextureVDF("BAR_MANA.TGA");

        // Images
        m_pHealthBar->setBackgroundImage(alloc.getTexture(hBarBackground));
        m_pManaBar->setBackgroundImage(alloc.getTexture(hBarBackground));
        m_pEnemyHealthBar->setBackgroundImage(alloc.getTexture(hBarBackground));

        m_pHealthBar->setBarImage(alloc.getTexture(hBarHealth));
        m_pManaBar->setBarImage(alloc.getTexture(hBarMana));
        m_pEnemyHealthBar->setBarImage(alloc.getTexture(hBarHealth));

        // Alignment
        m_pHealthBar->setAlignment(A_BottomLeft);
        m_pManaBar->setAlignment(A_BottomRight);
        m_pEnemyHealthBar->setAlignment(A_TopCenter);

        // Size
        Math::float2 barSize = Math::float2(0.6f, 0.6f);
        m_pHealthBar->setSize(barSize);
        m_pManaBar->setSize(barSize);
        m_pEnemyHealthBar->setSize(barSize);

        // Position
        m_pHealthBar->setTranslation(Math::float2(0.01f, 0.99f));
        m_pManaBar->setTranslation(Math::float2(0.99f, 0.99f));
        m_pEnemyHealthBar->setTranslation(Math::float2(0.5f, 0.01f));
    }
}

UI::Hud::~Hud()
{
    removeChild(m_pHealthBar);
    removeChild(m_pManaBar);
    removeChild(m_pEnemyHealthBar);

    delete m_pManaBar;
    delete m_pHealthBar;
    delete m_pEnemyHealthBar;
}

void UI::Hud::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    View::update(dt, mstate, config);
}

void UI::Hud::setHealth(float value)
{
    m_pHealthBar->setValue(value);
}

void UI::Hud::setMana(float value)
{
    m_pManaBar->setValue(value);
}

void UI::Hud::setEnemyHealth(float value)
{
    m_pEnemyHealthBar->setValue(value);
}
