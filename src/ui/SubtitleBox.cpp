//
// Created by desktop on 11.08.16.
//

#include "SubtitleBox.h"
#include <engine/BaseEngine.h>
#include <entry/input.h>
#include <imgui/imgui.h>
#include <render/RenderSystem.h>
#include <utils/logger.h>
#include <algorithm>
#include "zFont.h"

UI::SubtitleBox::SubtitleBox(Engine::BaseEngine& e) : View(e)
{
    m_Scaling = 1;
    m_growDirection = 0;
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

    if (!fnt)
        return;

    View::update(dt, mstate, config);

    // Draw our choices
    /*const int height = config.state.viewHeight / 10;
    const int halfwidth = config.state.viewHeight / 2;
    imguiBeginScrollArea(m_Text.speaker.c_str(), (config.state.viewWidth / 2) - halfwidth, 10, halfwidth * 2, height,
                         &m_ScrollArea);

    imguiLabel(m_Text.text.c_str());

    imguiEndScrollArea();*/

    m_Scaling += static_cast<float>(dt) / GROW_SHRINK_TIME * m_growDirection;
    m_Scaling = Math::clamp(m_Scaling, 0.0f, 1.0f);

    // Un-normalize transforms
    Math::float2 absTranslation = getAbsoluteTranslation();

    float pxMax = absTranslation.x * config.state.viewWidth;
    float pyMax = (absTranslation.y + 0.02f) * config.state.viewHeight;

    float sxMax = 0.5f * config.state.viewWidth;
    int wrapAroundWidth = Math::iround(0.95f * sxMax);
    std::vector<std::string> lines = fnt->layoutText(m_Text.text, wrapAroundWidth);
    auto fontHeight = fnt->getFontHeight();
    int linesOfText = static_cast<int>(lines.size()) + 1;  // +1 for speaker
    // render the Box as if there were at least 4 lines of text
    // so that the size won't change as often, but is still adjusted for very long texts (mods?)
    linesOfText = std::max(linesOfText, 4);
    float syMax = fontHeight * (linesOfText + 2);  // +2 for some extra space

    Math::float2 maxSize = {sxMax, syMax};
    Math::float2 posMax = {pxMax, pyMax};
    Math::float2 center = posMax + maxSize / 2;
    // Draw background image
    {
        Math::float2 pos = center - maxSize / 2 * m_Scaling;
        Math::float2 size = maxSize * m_Scaling;

        // Get background image
        Textures::Texture& background = m_Engine.getEngineTextureAlloc().getTexture(m_BackgroundTexture);

        bgfx::ProgramHandle program = config.programs.imageProgram;
        drawTexture(BGFX_VIEW,
                    Math::iround(pos.x), Math::iround(pos.y),
                    Math::iround(size.x), Math::iround(size.y),
                    config.state.viewWidth, config.state.viewHeight, background.m_TextureHandle, program,
                    config.uniforms.diffuseTexture);
    }
    if (m_Scaling == 1.0f)
    {
        int centerx = Math::iround(center.x);
        int centery = Math::iround(center.y);
        // split so that each line is not longer than wrapAroundWidth pixel
        const char* speakerFont = DEFAULT_FONT_HI;
        const char* dialogTextFont = DEFAULT_FONT;
        // TODO read alignment from config
        SubtitleBox::TextAlignment alignment = SubtitleBox::TextAlignment::center;
        switch (alignment)
        {
            case SubtitleBox::TextAlignment::center:
                lines.insert(lines.begin(), m_Text.speaker);
                for (unsigned i = 0; i < lines.size(); ++i)
                {
                    const char* font = i == 0 ? speakerFont : dialogTextFont;
                    unsigned before = i;
                    unsigned long after = lines.size() - i - 1;
                    std::string line = std::string(before, '\n') + lines[i] + std::string(after, '\n');
                    drawText(line, centerx, centery, A_Center, config, font);
                }
                break;
            case SubtitleBox::TextAlignment::left:
                std::stringstream ss;
                for (const auto& line : lines)
                {
                    ss << '\n'
                       << line;
                }
                drawText(m_Text.speaker + std::string(lines.size(), '\n'), centerx, centery, A_Center, config, speakerFont);
                drawText(ss.str(), centerx, centery, A_Center, config, dialogTextFont);
                break;
        }
    }
}

void UI::SubtitleBox::setText(const std::string& speaker, const std::string& text)
{
    m_Text.text = text;
    m_Text.speaker = speaker;
}