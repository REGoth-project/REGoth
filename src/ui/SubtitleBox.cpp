//
// Created by desktop on 11.08.16.
//

#include <imgui/imgui.h>
#include <render/RenderSystem.h>
#include <entry/input.h>
#include <utils/logger.h>
#include "SubtitleBox.h"
#include <engine/BaseEngine.h>
#include "zFont.h"

UI::SubtitleBox::SubtitleBox(Engine::BaseEngine& e) : View(e)
{
    m_ScrollArea = 0;
    m_BackgroundTexture = e.getEngineTextureAlloc().loadTextureVDF("DLG_CHOICE.TGA");

    setTranslation(Math::float2(0.25f, 0.0025f));
}

UI::SubtitleBox::~SubtitleBox()
{

}

void UI::SubtitleBox::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    if (m_IsHidden)
        return;

    const UI::zFont* fnt = m_Engine.getFontCache().getFont(DEFAULT_FONT);

    if(!fnt)
        return;

    View::update(dt, mstate, config);


    // Draw our choices
    /*const int height = config.state.viewHeight / 10;
    const int halfwidth = config.state.viewHeight / 2;
    imguiBeginScrollArea(m_Text.speaker.c_str(), (config.state.viewWidth / 2) - halfwidth, 10, halfwidth * 2, height,
                         &m_ScrollArea);

    imguiLabel(m_Text.text.c_str());

    imguiEndScrollArea();*/

    // Un-normalize transforms
    Math::float2 absTranslation = getAbsoluteTranslation();
    Math::float2 absSize = getAbsoluteSize();
    int px = (int) (absTranslation.x * config.state.viewWidth + 0.5f);
    int py = (int) (absTranslation.y * config.state.viewHeight + 0.5f);

    int sx = (int) (0.50f * config.state.viewWidth + 0.5f);
    int sy = 13 * 6; // 6 lines of dialog

    // Draw background image
    {
        // Get background image
        Textures::Texture& background = m_Engine.getEngineTextureAlloc().getTexture(m_BackgroundTexture);

        bgfx::ProgramHandle program = config.programs.imageProgram;
        drawTexture(BGFX_VIEW, px, py, sx, sy,
                    config.state.viewWidth, config.state.viewHeight, background.m_TextureHandle, program,
                    config.uniforms.diffuseTexture);

    }


    // Draw text
    {
        // Insert linebreaks if the text is too long to fit
        std::string layouted = fnt->layoutText(m_Text.text, (int)(sx * 0.95f));

        // Draw dialog text
        // First newline is for the speaker, later
        drawText("\n" + layouted, px + (sx / 2), py + (sy / 2), A_Center, config);

        // Count number of newlines in text
        int ncnt = 0;
        for(int i=0;i<layouted.size();i++)
            ncnt += layouted[i] == '\n' ? 1 : 0;

        // Put the speaker, followed by ncnt newlines, to align it right above the actual text
        std::string speaker = m_Text.speaker + "\n";
        for(int i=0;i<ncnt;i++)
            speaker.push_back('\n');

        drawText(speaker, px + (sx / 2), py + (sy / 2), A_Center, config, DEFAULT_FONT_HI);
    }
}

void UI::SubtitleBox::setText(const std::string& speaker, const std::string& text)
{
    m_Text.text = text;
    m_Text.speaker = speaker;
}
