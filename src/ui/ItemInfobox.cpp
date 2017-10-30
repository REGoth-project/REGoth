//
// Created by desktop on 16.12.16.
//

#include "ItemInfobox.h"
#include "ImageView.h"
#include <content/Texture.h>
#include <engine/BaseEngine.h>
#include <daedalus/DaedalusStdlib.h>
#include <imgui/imgui.h>

extern bgfx::ProgramHandle imguiGetImageProgram(uint8_t _mip);

UI::ItemInfobox::ItemInfobox(Engine::BaseEngine& e)
    : View(e)
{
    //Textures::TextureAllocator& texAlloc = m_Engine.getEngineTextureAlloc();
    // load this later
    m_TexInvBack.invalidate();
    //m_TexInvBack = texAlloc.loadTextureVDF("INV_BACK.TGA");
    //m_TexInvBack = texAlloc.loadTextureVDF("DLG_AMBIENT.TGA");
}

UI::ItemInfobox::~ItemInfobox()
{
}


void UI::ItemInfobox::drawInfobox(Render::RenderConfig& config, const Daedalus::GEngineClasses::C_Item &item)
{
    if(!m_TexInvBack.isValid())
    {
        Textures::TextureAllocator& texAlloc = m_Engine.getEngineTextureAlloc();
        if(m_Engine.getBasicGameType() == Daedalus::GameType::GT_Gothic1)
            m_TexInvBack = texAlloc.loadTextureVDF("DLG_AMBIENT.TGA");
        else if(m_Engine.getBasicGameType() == Daedalus::GameType::GT_Gothic2)
            m_TexInvBack = texAlloc.loadTextureVDF("DLG_AMBIENT.TGA");
    }

    Textures::Texture &texInvBack = m_Engine.getEngineTextureAlloc().getTexture(m_TexInvBack);


    drawTexture(RenderViewList::INVENTORY_BG, Math::ifloor(m_Translation.x) - 10, Math::ifloor(m_Translation.y) - 10,
                Math::ifloor(m_Size.x) + 20, Math::ifloor(m_Size.y) + 20,
                config.state.viewWidth, config.state.viewHeight, texInvBack.m_TextureHandle,
                config.programs.imageProgram, config.uniforms.diffuseTexture);

    const std::string &descriptionString = item.description.empty() ? item.name : item.description;
    drawText(descriptionString, Math::ifloor(m_Translation.x + m_Size.x / 2), Math::ifloor(m_Translation.y),
             EAlign::A_TopCenter, config);

    for(int i = 0; i < Daedalus::GEngineClasses::ITM_TEXT_MAX; ++i)
    {
        drawText(item.text[i], Math::ifloor(m_Translation.x), Math::ifloor(32.0f + m_Translation.y + i * 16.0f),
                 EAlign::A_TopLeft, config);
        // Test if the count is unequal to zero otherwise we would print zeros next to purely informatic text
        // ITM_TEXT_MAX - 1 is an exception because this is the "Value" attribute and here zero is possible
        if(item.count[i] != 0 || i == (Daedalus::GEngineClasses::ITM_TEXT_MAX - 1))
        {
            drawText(std::to_string(item.count[i]), Math::ifloor(m_Translation.x + m_Size.x), Math::ifloor(32.0f + m_Translation.y + i * 16.0f),
                     EAlign::A_TopRight, config);
        }
    }
}

void UI::ItemInfobox::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    if(m_IsHidden)
        return;

    View::update(dt, mstate, config);
}
