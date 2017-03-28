#include <imgui/imgui.h>
#include <render/RenderSystem.h>
#include <utils/logger.h>
#include "ConsoleBox.h"
#include <engine/BaseEngine.h>
#include "zFont.h"
#include <ui/Console.h>

UI::ConsoleBox::ConsoleBox(Engine::BaseEngine& e, UI::Console& console) :
    View(e),
    m_Console(console)
{
    m_Config.height = 10;
    m_BackgroundTexture = e.getEngineTextureAlloc().loadTextureVDF("CONSOLE.TGA");
    m_SuggestionsBackgroundTexture = e.getEngineTextureAlloc().loadTextureVDF("DLG_CHOICE.TGA");
}

UI::ConsoleBox::~ConsoleBox()
{
}

void UI::ConsoleBox::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    m_IsHidden = !m_Console.isOpen();
    if (m_IsHidden)
        return;

    auto font = DEFAULT_FONT;
    const UI::zFont* fnt = m_Engine.getFontCache().getFont(font);
    if(!fnt)
        return;

    View::update(dt, mstate, config);

    int consoleSizeX = config.state.viewWidth;
    int consoleSizeY = Math::iround(config.state.viewHeight * 0.25f);
    // Draw console
    {
        // Draw console background image
        Textures::Texture& background = m_Engine.getEngineTextureAlloc().getTexture(m_BackgroundTexture);

        bgfx::ProgramHandle program = config.programs.imageProgram;
        drawTexture(BGFX_VIEW,
                    0, 0,
                    consoleSizeX, consoleSizeY,
                    config.state.viewWidth, config.state.viewHeight, background.m_TextureHandle, program,
                    config.uniforms.diffuseTexture);

        // draw console output + current line
        std::stringstream ss;
        auto& outputList = m_Console.getOutputLines();
        std::size_t maxDisplayLines = 10;
        std::size_t numLines = std::min(outputList.size(), maxDisplayLines);
        std::vector<std::string> outputLines(numLines);
        std::copy_n(outputList.begin(), numLines, outputLines.rbegin());
        outputLines.push_back(m_Console.getTypedLine());
        auto joined = Utils::join(outputLines.begin(), outputLines.end(), "\n");
        drawText(joined, 0, consoleSizeY, A_BottomLeft, config, font);
    }
    // Draw suggestions
    {
        const auto& suggestionsList = m_Console.getSuggestions();
        if (suggestionsList.empty() || suggestionsList.back().empty())
            return;

        const auto& suggestions = suggestionsList.back();
        std::size_t maxSuggestions = 10;
        std::vector<std::string> lines;
        std::size_t i = 0;
        std::size_t fillWidth = 40;
        for (auto& suggestionEntry : suggestions)
        {
            if (i >= maxSuggestions)
            {
                lines.back() = "...";
                break;
            }
            std::stringstream ss;
            for (auto& alias : suggestionEntry)
            {
                ss << std::left << std::setw(fillWidth) << alias;
            }
            lines.push_back(ss.str());
            i++;
        }
        // FIXME: glyphs are not monospace. print each column separately?
        int glyphWidth = 10;
        int fontHeight = fnt->getFontHeight();
        int suggestionBoxSizeX = fillWidth * suggestions.front().size() * glyphWidth;
        int suggestionBoxSizeY = fontHeight * lines.size();
        auto joined = Utils::join(lines.begin(), lines.end(), "\n");
        // Get background image
        Textures::Texture& background = m_Engine.getEngineTextureAlloc().getTexture(m_SuggestionsBackgroundTexture);

        bgfx::ProgramHandle program = config.programs.imageProgram;
        drawTexture(BGFX_VIEW,
                    0, consoleSizeY,
                    suggestionBoxSizeX, suggestionBoxSizeY,
                    config.state.viewWidth, config.state.viewHeight, background.m_TextureHandle, program,
                    config.uniforms.diffuseTexture);
        drawText(joined, 0, consoleSizeY + suggestionBoxSizeY, A_BottomLeft, config, font);
    }
}