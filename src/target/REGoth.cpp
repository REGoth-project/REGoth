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
#include "entry/input.h"
#include <debugdraw/debugdraw.h>
#include "config.h"

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

class ExampleCubes : public entry::AppI
{
	void init(int _argc, char** _argv) BX_OVERRIDE
	{
		Args args(_argc, _argv);

		axis = 0;
		m_width = 1280;
		m_height = 720;
		m_debug = BGFX_DEBUG_TEXT;
		m_reset = BGFX_RESET_MAXANISOTROPY | BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X8;

		bgfx::init(args.m_type, args.m_pciId);
		bgfx::reset(m_width, m_height, m_reset);

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

		m_Engine.initEngine(_argc, _argv);
		m_Engine.addWorld(m_Engine.getEngineArgs().startupZEN);

		m_timeOffset = bx::getHPCounter();

		ddInit();
	}

	virtual int shutdown() BX_OVERRIDE
	{
		// Cleanup.

		ddShutdown();

		// Shutdown bgfx.
		bgfx::shutdown();

		return 0;
	}

	bool update() BX_OVERRIDE
	{
		if (!entry::processEvents(m_width, m_height, m_debug, m_reset))
		{
			int64_t now = bx::getHPCounter();
			static int64_t last = now;
			const int64_t frameTime = now - last;
			last = now;
			const double freq = double(bx::getHPFrequency());
			const double toMs = 1000.0 / freq;

			float time = (float)((now - m_timeOffset) / double(bx::getHPFrequency()));
			const float dt = float(frameTime/freq);


			// Use debug font to print information about this example.
			bgfx::dbgTextClear();
			bgfx::dbgTextPrintf(0, 1, 0x4f, "REGoth-Engine (%s)", m_Engine.getEngineArgs().startupZEN.c_str());
			bgfx::dbgTextPrintf(0, 2, 0x0f, "Frame: % 7.3f[ms] %.1f[fps]", 1000.0 * dt, 1.0f / (double(dt)));


			// This dummy draw call is here to make sure that view 0 is cleared
			// if no other draw callvm.getDATFile().getSymbolByIndex(self)s are submitted to view 0.
			//bgfx::touch(0);

			

			// Set render states.
			//bgfx::setState(BGFX_STATE_DEFAULT);
			//bgfx::setTexture(0, m_texUniform, m_texHandle);

			ddBegin(0);

			m_Engine.frameUpdate(dt);

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

			// Advance to next frame. Rendering thread will be kicked to
			// process submitted rendering primitives.
			bgfx::frame();

			return true;
		}

		return false;
	}

	Handle::MeshHandle m_LevelMesh;
	Engine::GameEngine m_Engine;
	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_debug;
	uint32_t m_reset;
	int64_t m_timeOffset;
	float axis;
};

ENTRY_IMPLEMENT_MAIN(ExampleCubes);
