//
// Created by desktop on 21.10.16.
//

#include "zFont.h"
#include <engine/BaseEngine.h>
#include <ZenLib/utils/logger.h>

#include <content/VertexTypes.h>

namespace FontUtil
{
    void appendGlyph(std::vector<Meshes::PositionUVVertex2D>& vxStream,
                    int32_t _x, int32_t _y, int32_t _width, int32_t _height,
                    const Math::float2 uvMin = Math::float2(0.0f, 0.0f),
                    const Math::float2 uvMax = Math::float2(1.0f, 1.0f))
    {

        const float widthf = float(_width);
        const float heightf = float(_height);

        const float minx = float(_x);
        const float miny = float(_y);
        const float maxx = minx + widthf;
        const float maxy = miny + heightf;

        float halfTexel = (bgfx::getRendererType() == bgfx::RendererType::Direct3D9) ? 0.5f : 0.0f;

        const float texelHalfW = halfTexel / widthf;
        const float texelHalfH = halfTexel / heightf;

        const float minu = uvMin.x + texelHalfW;
        const float maxu = uvMax.x - texelHalfW;
        const float minv = uvMin.y + texelHalfH;
        const float maxv = texelHalfH + uvMax.y;

        // Append triangles
        vxStream.resize(vxStream.size() + 6);
        Meshes::PositionUVVertex2D* vertex = &vxStream[vxStream.size() - 6];

        vertex[0].Position.x = minx;
        vertex[0].Position.y = miny;
        vertex[0].TexCoord.x = minu;
        vertex[0].TexCoord.y = minv;

        vertex[1].Position.x = maxx;
        vertex[1].Position.y = miny;
        vertex[1].TexCoord.x = maxu;
        vertex[1].TexCoord.y = minv;

        vertex[2].Position.x = maxx;
        vertex[2].Position.y = maxy;
        vertex[2].TexCoord.x = maxu;
        vertex[2].TexCoord.y = maxv;

        vertex[3].Position.x = maxx;
        vertex[3].Position.y = maxy;
        vertex[3].TexCoord.x = maxu;
        vertex[3].TexCoord.y = maxv;

        vertex[4].Position.x = minx;
        vertex[4].Position.y = maxy;
        vertex[4].TexCoord.x = minu;
        vertex[4].TexCoord.y = maxv;

        vertex[5].Position.x = minx;
        vertex[5].Position.y = miny;
        vertex[5].TexCoord.x = minu;
        vertex[5].TexCoord.y = minv;

    }
}

UI::zFont::zFont(Engine::BaseEngine& e, const ZenLoad::zCFont::FontInfo& fontInfo) : m_Engine(e), m_Font(fontInfo)
{
    LogInfo() << "Loading font file: " << fontInfo.fontName;

    // Load the fonts texture
    m_FontTexture = m_Engine.getEngineTextureAlloc().loadTextureVDF(fontInfo.fontName);

}

UI::zFont::~zFont()
{
    // Note: No need to unload font-textures, as they are needed in all worlds anyways.
}

bool UI::zFont::getGlyphOf(unsigned char c, UI::zFont::Glyph& g) const
{
    if(c > ZenLoad::zCFont::FONT_NUM_MAX_LETTERS)
        return false;

    g.width = m_Font.glyphWidth[c];
    g.uvTopLeft = Math::float2(m_Font.fontUV1[c].v);
    g.uvBottomRight = Math::float2(m_Font.fontUV2[c].v);

    return true;
}

void UI::zFont::appendGlyph(UI::zFont::GlyphStream& glyphStream, unsigned char c) const
{
    // Look up information about that glyph
    Glyph g;
    if(!getGlyphOf(c, g))
    {
        LogWarn() << "Failed to look up glyph Nr. " << (int)c;
        return;
    }

    if(c == '\n')
    {
        // Just return to beginning of the line and move one line down
        glyphStream.xPos = glyphStream.xReturn;
        glyphStream.yPos += m_Font.fontHeight;
    } else
    {
        // Append at current xPos
        FontUtil::appendGlyph(glyphStream.vxStream, glyphStream.xPos, glyphStream.yPos, g.width, m_Font.fontHeight, g.uvTopLeft,
                              g.uvBottomRight);

        // Shift xpos for the next character
        glyphStream.xPos += g.width;
    }
}

bool UI::zFont::bindGlyphStream(const UI::zFont::GlyphStream& glyphStream)
{
    if(glyphStream.vxStream.empty())
        return false;

    if (bgfx::checkAvailTransientVertexBuffer((uint32_t)glyphStream.vxStream.size(), Meshes::PositionUVVertex2D::ms_decl))
    {
        bgfx::TransientVertexBuffer vb;
        bgfx::allocTransientVertexBuffer(&vb, (uint32_t)glyphStream.vxStream.size(), Meshes::PositionUVVertex2D::ms_decl);

        Meshes::PositionUVVertex* vertex = (Meshes::PositionUVVertex*) vb.data;

        memcpy(vertex, glyphStream.vxStream.data(), sizeof(Meshes::PositionUVVertex2D) * glyphStream.vxStream.size());

        bgfx::setVertexBuffer(&vb);

        return true;
    }

    return false;
}

void UI::zFont::calcTextMetrics(const std::string& txt, int& width, int& height) const
{
    int xPos = 0;
    int yPos = 0;
    int xMax = 0;
    int yMax = 0;
    for(int i=0;i<txt.size();i++)
    {
        if(txt[i] == '\n')
        {
            xPos = 0;
            yPos += m_Font.fontHeight;
        }else
        {
            Glyph g;
            getGlyphOf((unsigned char)txt[i], g);

            xPos += g.width;
        }

        xMax = std::max(xPos, xMax);
        yMax = std::max(yPos, yMax);
    }

    width = xMax;
    height = yMax + m_Font.fontHeight; // Dont forget the last line (yMax is only the top-line)
}

std::string UI::zFont::layoutText(const std::string& text, int maxWidth) const
{
    std::string lt;
    int w = 0;
    int lastSpace = 0;
    for(int i=0;i<text.size();i++)
    {
        Glyph g;
        getGlyphOf((unsigned char)text[i], g);

        if(text[i] == ' ')
            lastSpace = i;

        lt.push_back(text[i]);
        w += g.width;

        if(w > maxWidth)
        {
            w = 0;
            lt[lastSpace] = '\n';
        }


    }

    return lt;
}

UI::zFontCache::zFontCache(Engine::BaseEngine& e) : m_Engine(e)
{

}

const UI::zFont* UI::zFontCache::getFont(const std::string& font)
{
    std::string extLessfont = Utils::stripExtension(font);

    auto it = m_Fonts.find(extLessfont);
    if(it != m_Fonts.end())
        return &it->second;

    if(!m_Engine.getVDFSIndex().hasFile(extLessfont + ".FNT"))
    {
        LogError() << "Failed to find font: " << font << ".FNT";

        return nullptr;
    }

    // Actually load the font
    ZenLoad::zCFont f(extLessfont + ".FNT", m_Engine.getVDFSIndex());

    auto it2 = m_Fonts.emplace(std::make_pair(extLessfont, UI::zFont(m_Engine, f.getFontInfo())));

    return &it2.first->second;
}


