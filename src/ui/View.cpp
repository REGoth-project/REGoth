//
// Created by desktop on 11.08.16.
//

#include <assert.h>
#include <handle/HandleDef.h>
#include "View.h"
#include <imgui/imgui.h>

using namespace UI;

namespace ViewUtil
{
    struct PosUvVertex
    {
        float m_x;
        float m_y;
        float m_u;
        float m_v;

        static void init()
        {
            ms_decl
                    .begin()
                    .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
                    .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                    .end();
        }

        static bgfx::VertexDecl ms_decl;
    };

    bgfx::VertexDecl PosUvVertex::ms_decl;

    bool screenQuad(int32_t _x, int32_t _y, int32_t _width, int32_t _height, bool _originBottomLeft = false)
    {
        if (bgfx::checkAvailTransientVertexBuffer(6, PosUvVertex::ms_decl))
        {
            bgfx::TransientVertexBuffer vb;
            bgfx::allocTransientVertexBuffer(&vb, 6, PosUvVertex::ms_decl);
            PosUvVertex* vertex = (PosUvVertex*) vb.data;

            const float widthf = float(_width);
            const float heightf = float(_height);

            const float minx = float(_x);
            const float miny = float(_y);
            const float maxx = minx + widthf;
            const float maxy = miny + heightf;

            float halfTexel = (bgfx::getRendererType() == bgfx::RendererType::Direct3D9) ? 0.5f : 0.0f;

            const float texelHalfW = halfTexel / widthf;
            const float texelHalfH = halfTexel / heightf;
            const float minu = texelHalfW;
            const float maxu = 1.0f - texelHalfW;
            const float minv = _originBottomLeft ? texelHalfH + 1.0f : texelHalfH;
            const float maxv = _originBottomLeft ? texelHalfH : texelHalfH + 1.0f;

            vertex[0].m_x = minx;
            vertex[0].m_y = miny;
            vertex[0].m_u = minu;
            vertex[0].m_v = minv;

            vertex[1].m_x = maxx;
            vertex[1].m_y = miny;
            vertex[1].m_u = maxu;
            vertex[1].m_v = minv;

            vertex[2].m_x = maxx;
            vertex[2].m_y = maxy;
            vertex[2].m_u = maxu;
            vertex[2].m_v = maxv;

            vertex[3].m_x = maxx;
            vertex[3].m_y = maxy;
            vertex[3].m_u = maxu;
            vertex[3].m_v = maxv;

            vertex[4].m_x = minx;
            vertex[4].m_y = maxy;
            vertex[4].m_u = minu;
            vertex[4].m_v = maxv;

            vertex[5].m_x = minx;
            vertex[5].m_y = miny;
            vertex[5].m_u = minu;
            vertex[5].m_v = minv;

            bgfx::setVertexBuffer(&vb);

            return true;
        }

        return false;
    }
}

View::View()
{
    m_IsHidden = false;
    m_pParent = nullptr;

    m_Translation = Math::float2(0,0);
    m_Size = Math::float2(1,1);
    m_Alignment = EAlign::A_TopLeft;

    ViewUtil::PosUvVertex::init();

    bgfx::setViewSeq(BGFX_VIEW, true);
}

View::~View()
{
    // Make sure all children have been removed
    assert(m_Children.empty());
}

void View::removeChild(View *pView)
{
    // Search for the view in our vector
    for(size_t i=0;i<m_Children.size();i++)
    {
        if(m_Children[i] == pView)
        {
            // Overwrite with last element and shorten the vector by 1
            m_Children[i] = m_Children.back();
            m_Children.pop_back();
            break;
        }
    }
}

void View::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    if(m_IsHidden)
        return;

    // Update all children
    for(View* v : m_Children)
    {
        v->update(dt, mstate, config);
    }
}

void View::drawTexture(uint8_t id, int x, int y, int width, int height, int surfaceWidth, int surfaceHeight,
                       bgfx::TextureHandle texture, bgfx::ProgramHandle program, bgfx::UniformHandle texUniform)
{


    //extern bgfx::ProgramHandle imguiGetImageProgram(uint8_t _mip);
    //bgfx::ProgramHandle program = imguiGetImageProgram(0);

    //imguiBeginArea("Picking Render Target:", x, y, width, height);
    //imguiImage(texture, 0, width, height);

    bgfx::setState(BGFX_STATE_DEFAULT & ~BGFX_STATE_DEPTH_WRITE);

    float ortho[16];
    bx::mtxOrtho(ortho, 0.0f, (float)surfaceWidth, (float)surfaceHeight, 0.0f, 0.0f, 1000.0f);
    bgfx::setViewTransform(id, NULL, ortho);
    bgfx::setViewRect(id, 0, 0, surfaceWidth, surfaceHeight);

    if (ViewUtil::screenQuad(x, y, width, height, false))
    {
        bgfx::setTexture(0, texUniform, texture);
        bgfx::setState(BGFX_STATE_RGB_WRITE
                       | BGFX_STATE_ALPHA_WRITE
                       | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
        );

        bgfx::setScissor(uint16_t(std::max(0, x)), uint16_t(std::max(0, y)), width, height
        );

        //setCurrentScissor();
        bgfx::submit(id, program);
    }



    //imguiEndArea();
}

Math::float2 View::getAbsoluteTranslation()
{

    if(!m_pParent)
        return m_Translation;

    return m_Translation + m_pParent->getAbsoluteTranslation();
}

Math::float2 View::getAbsoluteSize()
{
    if(!m_pParent)
        return m_Size;

    Math::float2 abs = m_pParent->getAbsoluteSize();

    return Math::float2(m_Size.x * abs.x, m_Size.y * abs.y);
}

void View::addChild(View* pView)
{
    m_Children.push_back(pView);
    pView->setParent(this);
}

Math::float2 View::getAlignOffset(EAlign align, float width, float height)
{
    switch(align)
    {
        case A_Center:          return Math::float2(-width / 2.0f, -height / 2.0f);
        case A_LeftCenter:      return Math::float2(0, -height / 2.0f);
        case A_RightCenter:     return Math::float2(-width, -height / 2.0f);
        case A_TopCenter:       return Math::float2(-width / 2.0f, 0);
        case A_BottomCenter:    return Math::float2(-width / 2.0f, -height);
        case A_TopLeft:         return Math::float2(0, 0);
        case A_TopRight:        return Math::float2(-width, 0);
        case A_BottomLeft:      return Math::float2(0, -height);
        case A_BottomRight:     return Math::float2(-width, -height);
    }
    return Math::float2(nullptr);
}
