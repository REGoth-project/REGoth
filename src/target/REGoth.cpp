/*
* Copyright 2011-2016 Branimir Karadzic. All rights reserved.
* License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
*/

#include <common.h>
#include "bgfx_utils.h"
#include <content/StaticLevelMesh.h>
#include <vdfs/fileIndex.h>
#include <zenload/zenParser.h>
#include <zenload/zCMesh.h>
#include <engine/World.h>
#include <engine/GameEngine.h>
#include <utils/bgfx_lib.h>
#include <content/VertexTypes.h>
#include <render/WorldRender.h>
#include <debugdraw/debugdraw.h>
#include <bx/uint32_t.h>
#include <zenload/ztex2dds.h>
#include <render/RenderSystem.h>
#include "config.h"
#include "engine/Platform.h"
#include <imgui/imgui.h>
#include <ui/DialogBox.h>



struct PosColorVertex
{
	float m_x;
	float m_y;
	float m_z;
	uint32_t m_abgr;

	static void init()
	{
		ms_decl
			.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
			.end();
	};

	static bgfx::VertexDecl ms_decl;
};

bgfx::VertexDecl PosColorVertex::ms_decl;

static PosColorVertex s_cubeVertices[8] =
{
	{ -1.0f,  1.0f,  1.0f, 0xff000000 },
	{ 1.0f,  1.0f,  1.0f, 0xff0000ff },
	{ -1.0f, -1.0f,  1.0f, 0xff00ff00 },
	{ 1.0f, -1.0f,  1.0f, 0xff00ffff },
	{ -1.0f,  1.0f, -1.0f, 0xffff0000 },
	{ 1.0f,  1.0f, -1.0f, 0xffff00ff },
	{ -1.0f, -1.0f, -1.0f, 0xffffff00 },
	{ 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t s_cubeIndices[36] =
{
	0, 1, 2, // 0
	1, 3, 2,
	4, 6, 5, // 2
	5, 6, 7,
	0, 2, 4, // 4
	4, 2, 6,
	1, 5, 3, // 6
	5, 7, 3,
	0, 4, 1, // 8
	4, 5, 1,
	2, 3, 6, // 10
	6, 3, 7,
};


struct PosColorTexCoord0Vertex
{
    float m_x;
    float m_y;
    float m_z;
    uint32_t m_abgr;
    float m_u;
    float m_v;

    static void init()
    {
        ms_decl
                .begin()
                .add(bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0,    4, bgfx::AttribType::Uint8, true)
                .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                .end();
    }

    static bgfx::VertexDecl ms_decl;
};

bgfx::VertexDecl PosColorTexCoord0Vertex::ms_decl;



class ExampleCubes : public /*entry::AppI*/ Engine::Platform
{
    void renderScreenSpaceQuad(uint32_t _view, bgfx::ProgramHandle _program, float _x, float _y, float _width, float _height)
    {
        bgfx::TransientVertexBuffer tvb;
        bgfx::TransientIndexBuffer tib;

        if (bgfx::allocTransientBuffers(&tvb, PosColorTexCoord0Vertex::ms_decl, 4, &tib, 6) )
        {
            PosColorTexCoord0Vertex* vertex = (PosColorTexCoord0Vertex*)tvb.data;

            float zz = 1.0f;

            const float minx = _x;
            const float maxx = _x + _width;
            const float miny = _y;
            const float maxy = _y + _height;

            float minu = 0.0f;
            float minv = 0.0f;
            float maxu =  _width;
            float maxv =  _height;

            vertex[0].m_x = minx;
            vertex[0].m_y = miny;
            vertex[0].m_z = zz;
            vertex[0].m_abgr = 0xffffffff;
            vertex[0].m_u = minu;
            vertex[0].m_v = minv;

            vertex[1].m_x = maxx;
            vertex[1].m_y = miny;
            vertex[1].m_z = zz;
            vertex[1].m_abgr = 0xffffffff;
            vertex[1].m_u = maxu;
            vertex[1].m_v = minv;

            vertex[2].m_x = maxx;
            vertex[2].m_y = maxy;
            vertex[2].m_z = zz;
            vertex[2].m_abgr = 0xffffffff;
            vertex[2].m_u = maxu;
            vertex[2].m_v = maxv;

            vertex[3].m_x = minx;
            vertex[3].m_y = maxy;
            vertex[3].m_z = zz;
            vertex[3].m_abgr = 0xffffffff;
            vertex[3].m_u = minu;
            vertex[3].m_v = maxv;

            uint16_t* indices = (uint16_t*)tib.data;

            indices[0] = 0;
            indices[1] = 2;
            indices[2] = 1;
            indices[3] = 0;
            indices[4] = 3;
            indices[5] = 2;


            bgfx::setIndexBuffer(&tib);
            bgfx::setVertexBuffer(&tvb);
            bgfx::submit(_view, _program);
        }
    }

    void showSplash()
    {
        float view[16];
        float proj[16];

        bx::mtxIdentity(view);
        bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);



        bgfx::setViewRect(1, 0, 0, (uint16_t)getWindowWidth(), (uint16_t)getWindowHeight());

        // Set view and projection matrix for view 0.
        bgfx::setViewTransform(1, NULL, proj);

        Textures::TextureAllocator alloc(&m_pEngine->getVDFSIndex());
        Handle::TextureHandle txh = alloc.loadTextureVDF("STARTSCREEN.TGA");

        Textures::Texture& texture = alloc.getTexture(txh);
        bgfx::frame();

        bgfx::touch(0);

        bgfx::setState(BGFX_STATE_DEFAULT);
        const Render::RenderConfig& cfg = m_pEngine->getDefaultRenderSystem().getConfig();
        bgfx::setTexture(0, cfg.uniforms.diffuseTexture, texture.m_TextureHandle);
        renderScreenSpaceQuad(1, cfg.programs.fullscreenQuadProgram, 0.0f, 0.0f, 1280.0f, 720.0f);

        bgfx::dbgTextPrintf(0, 1, 0x4f, "Loading...");
        bgfx::frame();
    }

	void init(int _argc, char** _argv) BX_OVERRIDE
	{
        std::cout << "Running REGoth Engine" << std::endl;

//		Args args(_argc, _argv);

		axis = 0;
		m_debug = BGFX_DEBUG_TEXT;
		m_reset = BGFX_RESET_MAXANISOTROPY | BGFX_RESET_MSAA_X8;

        m_Width = getWindowWidth();
        m_Height = getWindowHeight();

//		bgfx::init(args.m_type, args.m_pciId);
        bgfx::init();
		bgfx::reset((uint32_t)m_Width, (uint32_t)m_Height, m_reset);

		// Enable debug text.
		bgfx::setDebug(m_debug);

		// Set view 0 clear state.
		bgfx::setViewClear(0
			, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
			, 0x7EC0EEff
			, 1.0f
			, 0
			);

		// Create vertex stream declaration.
		PosColorVertex::init();
		Meshes::UVNormColorVertex::init();
		Meshes::UVNormVertex::init();
		Meshes::UVVertex::init();
		Meshes::PositionVertex::init();
		Meshes::PositionColorVertex::init();
		Meshes::PositionUVVertex::init();
		Meshes::SkeletalVertex::init();

        PosColorTexCoord0Vertex::init();

		m_pEngine = new Engine::GameEngine;
		m_pEngine->initEngine(_argc, _argv);


        showSplash();

        // Add startworld
		m_pEngine->addWorld(m_pEngine->getEngineArgs().startupZEN);

		m_timeOffset = bx::getHPCounter();

		ddInit();


        // Imgui.
        imguiCreate();
        m_scrollArea = 0;
	}

	virtual int shutdown() BX_OVERRIDE
	{
		// Cleanup.

		delete m_pEngine;

		ddShutdown();

        imguiDestroy();

		// Shutdown bgfx.
		bgfx::shutdown();

		return 0;
	}

	bool update() BX_OVERRIDE
	{
        Engine::Input::fireBindings();


        // Check for resize
        if(m_Width != getWindowWidth() || m_Height != getWindowHeight())
        {
            m_Width = getWindowWidth();
            m_Height = getWindowHeight();

            // Notify bgfx about framebuffer resize
            bgfx::reset((uint32_t)m_Width, (uint32_t)m_Height);
        }

        int64_t now = bx::getHPCounter();
        static int64_t last = now;
        const int64_t frameTime = now - last;
        last = now;
        const double freq = double(bx::getHPFrequency());
        const double toMs = 1000.0 / freq;

        float time = (float)((now - m_timeOffset) / double(bx::getHPFrequency()));
        const float dt = float(frameTime/freq);

        Engine::Input::MouseState ms; Engine::Input::getMouseState(ms);
        // Prepare rendering of debug-textboxes, etc
        imguiBeginFrame(ms.m_mx
                ,  ms.m_my
                , (ms.m_buttons[0] ? IMGUI_MBUT_LEFT   : 0)
                  | (ms.m_buttons[1] ? IMGUI_MBUT_RIGHT  : 0)
                  | (ms.m_buttons[2] ? IMGUI_MBUT_MIDDLE : 0)
                ,  ms.m_mz
                , (uint16_t)getWindowWidth()
                , (uint16_t)getWindowHeight()
        );

        // Use debug font to print information about this example.
        bgfx::dbgTextClear();
        bgfx::dbgTextPrintf(0, 1, 0x4f, "REGoth-Engine (%s)", m_pEngine->getEngineArgs().startupZEN.c_str());
        bgfx::dbgTextPrintf(0, 2, 0x0f, "Frame: % 7.3f[ms] %.1f[fps]", 1000.0 * dt, 1.0f / (double(dt)));


        // This dummy draw call is here to make sure that view 0 is cleared
        // if no other draw callvm.getDATFile().getSymbolByIndex(self)s are submitted to view 0.
        //bgfx::touch(0);



        // Set render states.
        //bgfx::setState(BGFX_STATE_DEFAULT);
        //bgfx::setTexture(0, m_texUniform, m_texHandle);

        ddBegin(0);

        m_pEngine->frameUpdate(dt, (uint16_t)getWindowWidth(), (uint16_t)getWindowHeight());
        // Draw and process all UI-Views
        // Set render states.


        m_pEngine->getRootUIView().update(dt, ms, m_pEngine->getDefaultRenderSystem().getConfig());

        ddSetTransform(nullptr);
        ddDrawAxis(0.0f, 0.0f, 0.0f);

        ddPush();
        ddSetColor(0xff00ff00);
        ddSetTransform(Math::Matrix::CreateTranslation(10,0,0).mv);
        Aabb aabb =
                {
                        {  -1.0f, -1.0f, -1.0f },
                        { 1.0f, 1.0f, 1.0f },
                };
        ddDraw(aabb);

        ddSetTransform(nullptr);
        ddSetColor(0xff0000ff);
        ddMoveTo(0,0,0);
        ddLineTo(10,0,0);
        ddPop();

        ddEnd();

        imguiEndFrame();

        // Advance to next frame. Rendering thread will be kicked to
        // process submitted rendering primitives.
        bgfx::frame();

        return true;
	}

	Engine::GameEngine* m_pEngine;
	uint32_t m_debug;
	uint32_t m_reset;
    int m_Width, m_Height;
	int64_t m_timeOffset;
	float axis;
    int32_t m_scrollArea;
};

//ENTRY_IMPLEMENT_MAIN(ExampleCubes);

//Usage of Platform:

int main(int argc, char** argv)
{
    ExampleCubes app;
    return app.run(argc, argv);
}
