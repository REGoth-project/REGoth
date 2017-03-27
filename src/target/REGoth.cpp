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
#include "rgconfig.h"
#include <imgui/imgui.h>
#include <ui/DialogBox.h>
#include <ZenLib/utils/logger.h>
#include <json.hpp>
#include <fstream>
#include <ui/Console.h>
#include <components/VobClasses.h>
#include <logic/NpcScriptState.h>
#include <logic/PlayerController.h>
#include <ui/ImageView.h>
#include <ui/BarView.h>
#include <ui/Hud.h>
#include <ui/Menu.h>
#include <logic/SavegameManager.h>
#include <utils/cli.h>
#include <utils/zTools.h>

using json = nlohmann::json;

#if BX_PLATFORM_ANDROID
#include "engine/PlatformAndroid.h"
#define PLATFORM_CLASS Engine::PlatformAndroid
#elif BX_PLATFORM_LINUX || BX_PLATFORM_OSX || BX_PLATFORM_WINDOWS || BX_PLATFORM_EMSCRIPTEN
#include "engine/PlatformGLFW.h"
#define PLATFORM_CLASS Engine::PlatformGLFW
#else
#error Unknown platform
#endif

namespace Flags
{
    Cli::Flag help("h", "help", 0, "Prints this message");
    Cli::Flag vsync("vsync", "vertical-sync", 0, "Enables vertical sync", {"0"}, "Rendering");
}

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



class ExampleCubes : public /*entry::AppI*/ PLATFORM_CLASS
{
public:

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

    void drawLog()
    {
        const std::list<std::string>& logs = Utils::Log::getLastLogLines();
        auto it = logs.begin();

        for(int i=49; i>=0 && it != logs.end() ;i++)
        {
            bgfx::dbgTextPrintf(0, i + 1, 0x4f, (*it).c_str());

            it++;
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


        bgfx::touch(0);

#if BX_PLATFORM_ANDROID

#else
        Textures::TextureAllocator alloc(&m_pEngine->getVDFSIndex());
        Handle::TextureHandle txh = alloc.loadTextureVDF("STARTSCREEN.TGA");

        if(!txh.isValid())
            return;

        Textures::Texture& texture = alloc.getTexture(txh);
        bgfx::frame();

        bgfx::setState(BGFX_STATE_DEFAULT);
        const Render::RenderConfig& cfg = m_pEngine->getDefaultRenderSystem().getConfig();
        bgfx::setTexture(0, cfg.uniforms.diffuseTexture, texture.m_TextureHandle);
        renderScreenSpaceQuad(1, cfg.programs.fullscreenQuadProgram, 0.0f, 0.0f, 1280.0f, 720.0f);
#endif


        bgfx::dbgTextPrintf(0, 1, 0x4f, "Loading...");
        bgfx::frame();
    }

	void init(int _argc, char** _argv) BX_OVERRIDE
	{
        std::cout << "Running REGoth Engine" << std::endl;

//		Args args(_argc, _argv);

		axis = 0;
		m_debug = BGFX_DEBUG_TEXT;
#if BX_PLATFORM_ANDROID
        m_reset = 0;
#else
        m_reset = BGFX_RESET_MAXANISOTROPY | BGFX_RESET_MSAA_X8;
        if (Flags::vsync.isSet())
        {
            m_reset |= BGFX_RESET_VSYNC;
        }
#endif

        m_Width = getWindowWidth();
        m_Height = getWindowHeight();
        m_NoHUD = false;


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
        Meshes::PositionUVVertex2D::init();
		Meshes::SkeletalVertex::init();

        PosColorTexCoord0Vertex::init();

		m_pEngine = new Engine::GameEngine;

#if BX_PLATFORM_ANDROID
        // Content is somewhere else on android
        m_pEngine->setContentBasePath("/sdcard/REGoth/");
#endif

		m_pEngine->initEngine(_argc, _argv);


        showSplash();

        // Add startworld
        Handle::WorldHandle w = m_pEngine->addWorld(m_pEngine->getEngineArgs().startupZEN);
        if (!w.isValid())
        {
           LogError() << "Failed to add world, world handle is invalid!";
           Platform::setQuit(true);
           return;
        }
		m_timeOffset = bx::getHPCounter();

		ddInit();


        // Imgui.
        float fontSize = 18.0f;//18

#ifdef ANDROID
        fontSize = 23.0f;
#endif

        std::function<UI::ConsoleCommand::CandidateListGenerator(std::vector<std::vector<std::string>>)> gen =
                [](std::vector<std::vector<std::string>> candidates) {
            return [candidates]() {
                return candidates;
            };
        };

        auto& console = m_pEngine->getHud().getConsole();
        console.registerCommand("stats", [](const std::vector<std::string>& args) -> std::string {
            static bool s_Stats = false;
            s_Stats = !s_Stats;

            bgfx::setDebug(s_Stats ? BGFX_DEBUG_STATS : 0);
            return "Toggled stats";
        });

        console.registerCommand("hud", [&](const std::vector<std::string>& args) -> std::string {
            static bool s_Stats = false;
            s_Stats = !s_Stats;

            m_NoHUD = !m_NoHUD;

            return "Toggled hud";
        });

        console.registerCommand("camera", [this](const std::vector<std::string>& args) -> std::string {

            if(args.size() < 2)
                return "Missing argument. Usage: camera <mode> | (0=Free, 1=Static, 2=FirstPerson, 3=ThirdPerson)";

            int idx = std::stoi(args[1]);

            m_pEngine->getMainCameraController()->setCameraMode((Logic::CameraController::ECameraMode)idx);

            return "Cameramode changed to " + std::to_string(idx);
        });

        console.registerCommand("test", [](const std::vector<std::string>& args) -> std::string {
            return "Hello World!";
        });

        console.registerCommand("set day", [this](const std::vector<std::string>& args) -> std::string {
            // modifies the day
            if(args.size() < 3)
                return "Missing argument. Usage: set day <day>";

            int day = std::stoi(args[2]);
            auto& clock = m_pEngine->getGameClock();
            clock.setDay(day);

            return "Set day to " + std::to_string(clock.getDay());
        });

        console.registerCommand("set clock", [this](const std::vector<std::string>& args) -> std::string {
            // modifies the world time
            if(args.size() != 4)
                return "Invalid arguments. Usage: set clock [hh mm]";

            const int hh = std::stoi(args[2]);
            const int mm = std::stoi(args[3]);

            if(hh < 0 || hh > 23)
                return "Invalid argument. Hours must be in range from 0 to 23";
            if(mm < 0 || mm > 59)
                return "Invalid argument. Minutes must be in range from 0 to 59";

            auto& clock = m_pEngine->getGameClock();
            clock.setTimeOfDay(hh, mm);

            return "Set clock to " + clock.getTimeOfDayFormatted();
        });

        console.registerCommand("set clockspeed", [this](const std::vector<std::string>& args) -> std::string {
            // adds an additional speed factor for the time of day
            if(args.size() < 3)
                return "Missing argument. Usage: set clockspeed <factor>";

            float factor = std::stof(args[2]);
            m_pEngine->getGameClock().setClockSpeedFactor(factor);

            return "Set clockspeed to " + std::to_string(factor);
        });

        console.registerCommand("set gamespeed", [this](const std::vector<std::string>& args) -> std::string {
            // adds an additional speed factor for the game time
            if(args.size() < 3)
                return "Missing argument. Usage: set gamespeed <factor:default=1>";

            float factor = std::stof(args[2]);
            m_pEngine->setGameEngineSpeedFactor(factor);

            return "Set gamespeed to " + std::to_string(factor);
        });

        console.registerCommand("heroexport", [this](const std::vector<std::string>& args) -> std::string {
            auto& s = m_pEngine->getMainWorld().get().getScriptEngine();

            VobTypes::NpcVobInformation player = VobTypes::asNpcVob(m_pEngine->getMainWorld().get(), s.getPlayerEntity());

            json pex;
            player.playerController->exportObject(pex);

            std::ofstream f("hero.json");
            f << Utils::iso_8859_1_to_utf8(pex.dump(4));
            f.close();

            return "Hero successfully exported to: hero.json";
        });

        console.registerCommand("goto waypoint", [this](const std::vector<std::string>& args) -> std::string {
            if(args.size() != 3)
                return "Invalid argument. Usage: goto waypoint [waypoint]";

            const std::string &waypointArgument = args[2];
            const World::Waynet::WaynetInstance& waynet = m_pEngine->getMainWorld().get().getWaynet();
            Logic::ScriptEngine &scriptEngine = m_pEngine->getMainWorld().get().getScriptEngine();
            const auto waypointIndex = World::Waynet::getWaypointIndex(waynet, waypointArgument);
            if(waypointIndex == World::Waynet::INVALID_WAYPOINT)
                return "Error: Waypoint" + waypointArgument + "not found";

            VobTypes::NpcVobInformation player = VobTypes::asNpcVob(m_pEngine->getMainWorld().get(), scriptEngine.getPlayerEntity());
            if(!player.isValid())
                return "Error: There is no valid player in the world";
            player.playerController->teleportToWaypoint(waypointIndex);

            return "Player moved to waypoint " + waypointArgument;
        });

        console.registerCommand("heroimport", [this](const std::vector<std::string>& args) -> std::string {
            auto& s = m_pEngine->getMainWorld().get().getScriptEngine();

            std::ifstream f("hero.json");
            std::stringstream saveData;
            saveData << f.rdbuf();

            json hero = json::parse(saveData);

            VobTypes::NpcVobInformation player = VobTypes::asNpcVob(m_pEngine->getMainWorld().get(), s.getPlayerEntity());
            player.playerController->importObject(hero, true);

            return "Hero successfully imported from: hero.json";
        });

        console.registerCommand("switchlevel", [this](const std::vector<std::string>& args) -> std::string {

            auto& s1 = m_pEngine->getMainWorld().get().getScriptEngine();

            if(args.size() < 2)
                return "Missing argument. Usage: switchlevel <zenfile>";

            return "Command currently broken! Try in a later release";

            std::string file = args[1];
            if(!m_pEngine->getVDFSIndex().hasFile(file))
                return "File '" + file + "' not found.";

            // Export hero
            VobTypes::NpcVobInformation player = VobTypes::asNpcVob(m_pEngine->getMainWorld().get(), s1.getPlayerEntity());

            json pex;
            player.playerController->exportObject(pex);

            // Export script-symbols
            json scriptSymbols;
            s1.exportScriptEngine(scriptSymbols);

            json dialogMan;
            m_pEngine->getMainWorld().get().getDialogManager().exportDialogManager(dialogMan);

            // Temporary save
            m_pEngine->getHud().getConsole().submitCommand("save " + m_pEngine->getMainWorld().get().getZenFile() + ".json");

            // Check if a savegame for this world exists
            if(Utils::fileExists(file + ".json"))
            {
                m_pEngine->getHud().getConsole().submitCommand("load " + file);
            }else
            {
                clearActions();
                m_pEngine->removeWorld(m_pEngine->getMainWorld());
                m_pEngine->addWorld(args[1]);
            }

            // Import hero again
            auto& s2 = m_pEngine->getMainWorld().get().getScriptEngine();
            if(s2.getPlayerEntity().isValid())
            {
                player = VobTypes::asNpcVob(m_pEngine->getMainWorld().get(),
                                            s2.getPlayerEntity()); // World and player changed
                player.playerController->importObject(pex, true);
            }else
            {
                LogError() << "Player not inserted into new world!";
            }

            // Import script-symbols
            s2.importScriptEngine(scriptSymbols);

            // Import dialog info
            m_pEngine->getMainWorld().get().getDialogManager().importDialogManager(dialogMan);

            return "Successfully switched world to: " + file;
        });

        console.registerCommand("load", [this](const std::vector<std::string>& args) -> std::string {

            if(args.size() != 2)
                return "Missing argument. Usage: load <savegame>";

            using namespace Engine;

            int idx = std::stoi(args[1]);

            if(!SavegameManager::isSavegameAvailable(idx))
                return "Savegame in slot " + std::to_string(idx) + " no available!";

            // Read general information about the saved game. Most importantly the world the player saved in
            SavegameManager::SavegameInfo info = SavegameManager::readSavegameInfo(idx);

            std::string worldPath = SavegameManager::buildWorldPath(idx, info.world);

            // Sanity check, if we really got a safe for this world. Otherwise we would end up in the fresh version
            // if it was missing. Also, IF the player saved there, there should be a save for this.
            if(!Utils::getFileSize(worldPath))
                return "Target world invalid!";

            clearActions();
            m_pEngine->removeWorld(m_pEngine->getMainWorld());
            m_pEngine->addWorld("", worldPath);
        });

        console.registerCommand("save", [this](const std::vector<std::string>& args) -> std::string {

            if(args.size() < 2)
                return "Missing argument. Usage: save <savegame>";

            int idx = std::stoi(args[1]);

            if(idx < 1)
                return "Invalid index. Must be greater than 0!";

            // TODO: Should be writing to a temp-directory first, before messing with the save-files already existing
            // Clean data from old savegame, so we don't load into worlds we haven't been to yet
            Engine::SavegameManager::clearSavegame(idx);

            // Write information about the current game-state
            Engine::SavegameManager::SavegameInfo info;
            info.version = Engine::SavegameManager::SavegameInfo::LATEST_KNOWN_VERSION;
            info.name = "Testsave";
            info.world = Utils::stripExtension(m_pEngine->getMainWorld().get().getZenFile());
            info.timePlayed = m_pEngine->getGameClock().getTotalSeconds();
            Engine::SavegameManager::writeSavegameInfo(idx, info);

            json j;
            m_pEngine->getMainWorld().get().exportWorld(j);

            // Save
            Engine::SavegameManager::writeWorld(idx, info.world, Utils::iso_8859_1_to_utf8(j.dump(4)));

            return "World saved to slot: " + std::to_string(idx);
        });

        UI::ConsoleCommand::CandidateListGenerator worlddNpcNamesGen = [this]() {
            auto& worldInstance = m_pEngine->getMainWorld().get();
            auto& scriptEngine = worldInstance.getScriptEngine();
            auto& datFile = scriptEngine.getVM().getDATFile();
            std::vector<std::vector<std::string>> aliasGroups;
            for(const Handle::EntityHandle& npc : scriptEngine.getWorldNPCs())
            {
                VobTypes::NpcVobInformation npcVobInfo = VobTypes::asNpcVob(worldInstance, npc);
                if (!npcVobInfo.isValid())
                    continue;

                Daedalus::GEngineClasses::C_Npc& npcScriptObject = npcVobInfo.playerController->getScriptInstance();
                const std::string& npcDisplayName = npcScriptObject.name[0];
                const std::string& npcDatFileName = datFile.getSymbolByIndex(npcScriptObject.instanceSymbol).name;

                std::vector<std::string> group;
                for (auto npcName : {npcDatFileName, npcDisplayName})
                {
                    std::replace(npcName.begin(), npcName.end(), ' ', '_');
                    group.push_back(std::move(npcName));
                }
                aliasGroups.push_back(std::move(group));
            }
            return aliasGroups;
        };

        console.registerCommand("givexp", [this](const std::vector<std::string>& args) -> std::string {
            auto& s1 = m_pEngine->getMainWorld().get().getScriptEngine();

            if(args.size() != 2)
                return "Missing argument. Usage: givexp <experience points>";

            if (!s1.getVM().getDATFile().hasSymbolName("B_GiveXP"))
                return "B_GiveXP is undefined!";

            int exp = std::stoi(args[1]);
            s1.prepareRunFunction();
            s1.pushInt(exp);
            s1.runFunction("B_GiveXP");

            return "Experience points successfully given";
        });

        auto tpCallback = [this, worlddNpcNamesGen](const std::vector<std::string>& args) -> std::string {
            if (args.size() < 2)
                return "Missing argument(s). Usage: tp <name>";

            std::string requested = args[1];
            auto& worldInstance = m_pEngine->getMainWorld().get();
            auto& scriptEngine = worldInstance.getScriptEngine();
            auto& datFile = scriptEngine.getVM().getDATFile();

            auto aliasGroups = worlddNpcNamesGen();
            auto group = Utils::findNameInGroups(aliasGroups, requested);

            if (group.size() >= 2)
            {
                std::string npcDatFileName = group[0];
                std::string npcDisplayName = group[1];
                Daedalus::GameState::NpcHandle npcScriptHandle = scriptEngine.getNPCFromSymbol(group[0]);
                if (npcScriptHandle.isValid())
                {
                    VobTypes::NpcVobInformation npcVobInfo = VobTypes::getVobFromScriptHandle(worldInstance, npcScriptHandle);
                    Math::float3 npcPosition = npcVobInfo.position->m_WorldMatrix.Translation();
                    VobTypes::NpcVobInformation player = VobTypes::asNpcVob(worldInstance, scriptEngine.getPlayerEntity());
                    Math::float3 npcDirection = npcVobInfo.playerController->getDirection();
                    // player keeps a respectful distance of 1 to the NPC
                    float respectfulDistance = 1;
                    Math::float3 newPos = npcPosition + respectfulDistance * npcDirection;
                    player.playerController->teleportToPosition(newPos);
                    // player looks towards NPC
                    player.playerController->setDirection((-1) * npcDirection);
                    return "Teleported to " + npcDisplayName + " (" + npcDatFileName + ")";
                }
            }
            return "Could not find NPC " + requested;
        };

        console.registerCommand("tp", tpCallback).registerAutoComplete(worlddNpcNamesGen);

        auto killOrKnockoutCallback = [this, worlddNpcNamesGen](const std::vector<std::string>& args) -> std::string {

            auto& scriptEngine = m_pEngine->getMainWorld().get().getScriptEngine();
            auto& worldInstance = m_pEngine->getMainWorld().get();

            VobTypes::NpcVobInformation npcVobInfo;

            if (args.size() == 1)
            {
                VobTypes::NpcVobInformation player = VobTypes::asNpcVob(worldInstance, scriptEngine.getPlayerEntity());
                std::set<Handle::EntityHandle> nearNPCs = scriptEngine.getNPCsInRadius(player.position->m_WorldMatrix.Translation(), 3.0f);
                // don't kill the play
                nearNPCs.erase(scriptEngine.getPlayerEntity());

                if(nearNPCs.empty())
                {
                    return "No NPCs in range!";
                } else {
                    // Chose one at random
                    npcVobInfo = VobTypes::asNpcVob(worldInstance, *nearNPCs.begin());
                }
            }
            else
            {
                const std::string& requested = args.at(1);
                auto aliasGroups = worlddNpcNamesGen();
                auto group = Utils::findNameInGroups(aliasGroups, requested);

                if (group.size() >= 2) {
                    std::string npcDatFileName = group[0];
                    Daedalus::GameState::NpcHandle npcScriptHandle = scriptEngine.getNPCFromSymbol(npcDatFileName);
                    if (npcScriptHandle.isValid()) {
                        npcVobInfo = VobTypes::getVobFromScriptHandle(worldInstance, npcScriptHandle);
                    }
                } else {
                    return "NPC not found in this world: " + requested;
                }
            }

            if(!npcVobInfo.isValid())
                return "Invalid NPC";

            std::string stateName;
            if (Utils::stringEqualIngoreCase(args.at(0), "knockout"))
            {
                VobTypes::NpcVobInformation player = VobTypes::asNpcVob(worldInstance, scriptEngine.getPlayerEntity());

                Logic::EventMessages::StateMessage sm;
                sm.subType = Logic::EventMessages::StateMessage::EV_StartState;
                sm.functionSymbol = Logic::NPC_PRGAISTATE_UNCONSCIOUS;
                sm.isPrgState = true;
                sm.other = VobTypes::getScriptHandle(player);

                npcVobInfo.playerController->getEM().onMessage(sm);
                stateName = "UNCONSCIOUS";

            } else if (Utils::stringEqualIngoreCase(args.at(0), "kill"))
            {
                Logic::EventMessages::StateMessage sm;
                sm.subType = Logic::EventMessages::StateMessage::EV_StartState;
                sm.functionSymbol = Logic::NPC_PRGAISTATE_DEAD;
                sm.isPrgState = true;
                npcVobInfo.playerController->die(scriptEngine.getPlayerEntity());
                stateName = "DEAD";
            } else{
                return "internal error";
            }
            Daedalus::GEngineClasses::C_Npc& npcScriptObject = npcVobInfo.playerController->getScriptInstance();
            const std::string& npcDisplayName = npcScriptObject.name[0];
            const std::string& npcDatFileName = scriptEngine.getVM().getDATFile().getSymbolByIndex(npcScriptObject.instanceSymbol).name;
            std::string npnNameFull = npcDisplayName + " (" + npcDatFileName + ")";

            return npnNameFull + " is now in " + stateName + " state";
        };

        console.registerCommand("knockout", killOrKnockoutCallback).registerAutoComplete(worlddNpcNamesGen);
        console.registerCommand("kill", killOrKnockoutCallback).registerAutoComplete(worlddNpcNamesGen);

        console.registerCommand("interrupt", [this](const std::vector<std::string>& args) -> std::string {
            auto& world = m_pEngine->getMainWorld().get();
            VobTypes::NpcVobInformation player = VobTypes::asNpcVob(world, world.getScriptEngine().getPlayerEntity());
            player.playerController->interrupt();
            return "Interrupted player, cleared EM";
        });

        console.registerCommand("hurtself", [this](const std::vector<std::string>& args) -> std::string {
            if(args.size() < 2)
                return "Missing argument. Usage: hurtself <damage>";

            int dmg = std::stoi(args[1]);

            auto& world = m_pEngine->getMainWorld().get();
            VobTypes::NpcVobInformation player = VobTypes::asNpcVob(world, world.getScriptEngine().getPlayerEntity());
            player.playerController->changeAttribute(Daedalus::GEngineClasses::C_Npc::EATR_HITPOINTS, -dmg);

            return "Hurt player by " + std::to_string(dmg) + " HP";
        });

        console.registerCommand("usemana", [this](const std::vector<std::string>& args) -> std::string {

            VobTypes::NpcVobInformation player = VobTypes::asNpcVob(m_pEngine->getMainWorld().get(),
                                                                    m_pEngine->getMainWorld().get().getScriptEngine().getPlayerEntity());

            if(args.size() < 2)
                return "Missing argument. Usage: usemana <mana>";

            int dmg = std::stoi(args[1]);
            player.playerController->changeAttribute(Daedalus::GEngineClasses::C_Npc::EATR_MANA, -dmg);

            return "Used " + std::to_string(dmg) + " mana";
        });

        UI::ConsoleCommand::CandidateListGenerator itemNamesGen = [this]() {
            auto& se = m_pEngine->getMainWorld().get().getScriptEngine();
            auto& datFile = se.getVM().getDATFile();
            std::vector<std::vector<std::string>> aliasGroups;
            {
                Daedalus::GameState::ItemHandle dummyHandle = se.getVM().getGameState().createItem();
                Daedalus::GEngineClasses::C_Item& cItem = se.getVM().getGameState().getItem(dummyHandle);
                datFile.iterateSymbolsOfClass("C_ITEM", [&](size_t i, Daedalus::PARSymbol& parSymbol){
                    // reset to default values. especially name and description
                    cItem = Daedalus::GEngineClasses::C_Item();
                    // Run the script-constructor
                    se.getVM().initializeInstance(ZMemory::toBigHandle(dummyHandle), i, Daedalus::IC_Item);

                    std::vector<std::string> aliasGroup;
                    for (auto name : {parSymbol.name, cItem.description, cItem.name})
                    {
                        std::replace(name.begin(), name.end(), ' ', '_');
                        aliasGroup.push_back(name);
                    }
                    aliasGroups.push_back(aliasGroup);
                });
                se.getVM().getGameState().removeItem(dummyHandle);
            }
            return aliasGroups;
        };

        console.registerCommand("quit", [](const std::vector<std::string>& args) -> std::string {
            setQuit(true);
            return std::string("Exiting ...");
        });

        auto giveOrRemoveItemCallback = [this, itemNamesGen](const std::vector<std::string>& args, bool give) -> std::string {
            if (args.size() < 2)
                return "Missing argument. Usage: giveitem/removeitem <symbol> [<amount>]";

            std::string itemName = args[1];
            int amount = 1;
            if (args.size() > 2)
                amount = std::stoi(args[2]);

            if (amount < 1)
                return "invalid ammount " + std::to_string(amount);

            auto& se = m_pEngine->getMainWorld().get().getScriptEngine();

            std::size_t index = 0;
            auto aliasGroups = itemNamesGen();
            auto group = Utils::findNameInGroups(aliasGroups, itemName);
            if (group.size() == 3)
            {
                auto& parScriptName = group[0];
                VobTypes::NpcVobInformation player = VobTypes::asNpcVob(m_pEngine->getMainWorld().get(), se.getPlayerEntity());
                std::string description;
                if (give)
                {
                    auto handle = player.playerController->getInventory().addItem(parScriptName, amount);
                    Daedalus::GEngineClasses::C_Item& cItem = se.getVM().getGameState().getItem(handle);
                    description = cItem.description;
                } else {
                    auto handle = player.playerController->getInventory().getItem(parScriptName);
                    if (handle.isValid())
                    {
                        Daedalus::GEngineClasses::C_Item& cItem = se.getVM().getGameState().getItem(handle);
                        description =  cItem.description;
                        amount = std::min(static_cast<u_int32_t >(amount), cItem.amount);
                        player.playerController->getInventory().removeItem(parScriptName, amount);
                    } else {
                        return "error: could not remove item. item " + parScriptName + " is not in inventory";
                    }
                }
                std::string action = give ? "added to" : "removed from";
                return std::string("Item(s) " + action + " the inventory: ")
                       + std::to_string(amount) + " x " + description + " (" + parScriptName + ")";

            }
            return "Item not found!";
        };

        auto giveitemCallback = [giveOrRemoveItemCallback](const std::vector<std::string>& args) -> std::string {
            if (args.size() < 2)
                return "Missing argument. Usage: giveitem <item> [<amount>]";
            return giveOrRemoveItemCallback(args, true);
        };

        auto removeitemCallback = [giveOrRemoveItemCallback](const std::vector<std::string>& args) -> std::string {
            if (args.size() < 2)
                return "Missing argument. Usage: removeitem <item> [<amount>]";
            return giveOrRemoveItemCallback(args, false);
        };

        console.registerCommand("giveitem", giveitemCallback).registerAutoComplete(itemNamesGen);
        console.registerCommand("removeitem", removeitemCallback).registerAutoComplete(itemNamesGen);

        imguiCreate(nullptr, 0, fontSize);
        m_ImgUiCreated = true;

        /*UI::Menu* m = new UI::Menu(*m_pEngine);
        m_pEngine->getRootUIView().addChild(m);
        m->initializeInstance("MENU_STATUS");*/

        m_scrollArea = 0;
	}

    int shutdown() override
    {
		// remove (destroy) the world so that it shuts down properly
        m_pEngine->removeWorld(m_pEngine->getMainWorld());

        delete m_pEngine;

        ddShutdown();

        // Clear this explicitly or the atlas is destroyed after main (and bx cleanup / allocator destruction)
        ImFontAtlas* atlas = ImGui::GetIO().Fonts;
        atlas->Clear();

        if (m_ImgUiCreated)
            imguiDestroy();

		bgfx::shutdown();

        return PLATFORM_CLASS::shutdown();
	}

	bool update() BX_OVERRIDE
	{
        const int KEY_UP = 265;
        const int KEY_DOWN = 264;
        const int KEY_LEFT = 263;
        const int KEY_RIGHT = 262;
        const int KEY_ENTER = 257;
        const int KEY_ESCAPE = 256;
	const int KEY_BACKSPACE = 259;
        std::map<int, UI::EInputAction> keyMap = {{KEY_UP,     UI::IA_Up},
                                                  {KEY_DOWN,   UI::IA_Down},
                                                  {KEY_LEFT,   UI::IA_Left},
                                                  {KEY_RIGHT,  UI::IA_Right},
                                                  {KEY_ENTER,  UI::IA_Accept},
                                                  {KEY_ESCAPE, UI::IA_Close},
						  {KEY_BACKSPACE, UI::IA_Backspace}};

        std::string frameInputText = getFrameTextInput();
        for (int i = 0; i < NUM_KEYS; i++) {
            if (getKeysTriggered()[i]) // If key has been triggered start the stopwatch
            {
                m_stopWatch.start();

                if(m_pEngine->getHud().getConsole().isOpen())
                    m_pEngine->getHud().getConsole().onKeyDown(i);
                else if (keyMap.find(i) != keyMap.end())
                {
                    m_pEngine->getHud().onInputAction(keyMap[i]);
                }
            }
            else if (getKeysState()[i]) // If key is being held and stopwatch reached time limit, fire actions
            {
                if (m_stopWatch.getTimeDiffFromStartToNow() > 400)
                {
                    if (m_stopWatch.DelayedByArgMS(70))
                    {
                        if(m_pEngine->getHud().getConsole().isOpen())
                            m_pEngine->getHud().getConsole().onKeyDown(i);
                        else if (keyMap.find(i) != keyMap.end())
                        {
                            m_pEngine->getHud().onInputAction(keyMap[i]);
                        }
                    }
                }
            }
        }

        // Pass text input from this frame
        m_pEngine->getHud().onTextInput(frameInputText);

        if(!m_pEngine->getHud().getConsole().isOpen())
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

        if(!m_NoHUD)
        {
            bgfx::dbgTextPrintf(0, 1, 0x4f, "REGoth-Engine (%s)", m_pEngine->getEngineArgs().startupZEN.c_str());
            bgfx::dbgTextPrintf(0, 2, 0x0f, "Frame: % 7.3f[ms] %.1f[fps]", 1000.0 * dt, 1.0f / (double(dt)));
        }

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


        if(!m_NoHUD)
        {
            // TODO move this code into frameupdate? so gamespeed factor doesn't need to be applied here
            float gameSpeed = m_pEngine->getGameEngineSpeedFactor();
            m_pEngine->getRootUIView().update(dt * gameSpeed, ms, m_pEngine->getDefaultRenderSystem().getConfig());
        }



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

        if(m_pEngine->getHud().getConsole().isOpen())
        {
            m_pEngine->getHud().getConsole().update();

            Engine::Input::clearTriggered();
        }

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
    Utils::StopWatch m_stopWatch;
    bool m_NoHUD;
    // prevents imgui from crashing if we failed on startup and didn't init it
    bool m_ImgUiCreated = false;
};

//ENTRY_IMPLEMENT_MAIN(ExampleCubes);

//Usage of Platform:

int main(int argc, char** argv)
{
    int ret = 0;

    // Load config values for flags
    Cli::loadConfigFile();

    // Overwrite flags set from config using the commandline
    Cli::setCommandlineArgs(argc, argv);

    // Check if the user just wanted to see the list of commands
    if(Flags::help.isSet())
    {
        Cli::printHelp();
        return 0;
    }

    // Do some commandline-operations, if wanted
    if(zTools::tryRunTools())
        return 0;

    ExampleCubes app;
    try
    {
        ret = app.run(argc, argv);
        app.shutdown();
    }
    catch (const std::exception &e)
    {
        // might be caused by the logger, don't use it
        std::cerr << "Caught exception in main loop: " << e.what() << std::endl;
        ret = 1;
    }
    catch (...)
    {
        // might be caused by the logger, don't use it
        std::cerr << "Caught unknown exception in main loop" << std::endl;
        ret = 1;
    }

    // Write current config-values
    Cli::writeConfigFile();

    return ret;
}
