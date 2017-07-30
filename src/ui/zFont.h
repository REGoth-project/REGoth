//
// Created by desktop on 21.10.16.
//

#pragma once
#include <content/VertexTypes.h>
#include <handle/HandleDef.h>
#include <math/mathlib.h>
#include <zenload/zCFont.h>

namespace Engine
{
    class BaseEngine;
}

namespace UI
{
    class zFont
    {
    public:
        zFont(Engine::BaseEngine& e, const ZenLoad::zCFont::FontInfo& fontInfo);
        ~zFont();

        struct Glyph
        {
            int width;
            Math::float2 uvTopLeft;
            Math::float2 uvBottomRight;
        };

        struct GlyphStream
        {
            GlyphStream()
                : xPos(0)
                , yPos(0)
            {
            }

            /**
             * Sets the starting-position of this stream
             * @param x Startring x
             * @param y Starting y
             */
            void setPosition(int x, int y)
            {
                xReturn = x;
                xPos = x;
                yPos = y;
            }

            std::vector<Meshes::PositionUVVertex2D> vxStream;
            int xPos;     // Next glyphs x position
            int yPos;     // Next glyphs y position
            int xReturn;  // x-position to return after a newline
        };

        /**
         * Grabs the texture-coords and width of the given character
         * @param c Character-code. Gothic-Fonts only support byte-sized fonts (Max characters 255).
         * @param g Information about the glyph, if found
         * @return True, if the glyph was found in the font
         */
        bool getGlyphOf(unsigned char c, Glyph& g) const;

        /**
         * @return Height of the font in pixels
         */
        int getFontHeight() const { return m_Font.fontHeight; };
        /**
         * Returns lines where each is no wider than maxWidth
         * @param text Text to fit
         * @param maxWidth Max width of the text before a linebreak
         * @return vector of lines
         */
        std::vector<std::string> layoutText(const std::string& text, int maxWidth) const;

        /**
         * Calculates the width and height of the given piece of text
         * @param txt Text to analyze
         * @param width Width of txt in pixels
         * @param height Height of txt in pixels
         */
        void calcTextMetrics(const std::string& txt, int& width, int& height) const;

        /**
         * @return Texture handle (Engines texture allocator)
         */
        Handle::TextureHandle getFontTexture() const { return m_FontTexture; }
        /**
         * Appends a single character to a glyph-stream
         * @param glyphStream Stream to append to (Simply create an object of that yourself)
         * @param c Charactercode to add
         */
        void appendGlyph(GlyphStream& glyphStream, unsigned char c) const;

        /**
         * Binds the given glpyhStream to bgfx as a transient buffer
         * @param glyphStream Stream to bind
         */
        static bool bindGlyphStream(const GlyphStream& glyphStream);

    private:
        /**
         * Loaded font
         */
        ZenLoad::zCFont::FontInfo m_Font;

        /**
         * Texture of the loaded font
         */
        Handle::TextureHandle m_FontTexture;

        /**
         * Engine
         */
        Engine::BaseEngine& m_Engine;
    };

    class zFontCache
    {
    public:
        zFontCache(Engine::BaseEngine& e);

        /**
         * Tries to get a font from cache. If not already loaded, loads it.
         * @param font Font to look up
         * @return Pointer to font. Nullptr if load failed.
         */
        const zFont* getFont(const std::string& font);

    private:
        /**
         * Engine
         */
        Engine::BaseEngine& m_Engine;

        /**
         * Map of already loaded fonts
         */
        std::map<std::string, zFont> m_Fonts;
    };
}
