/*
 * Copyright 2011-2016 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#include <fstream>
#include "rgconfig.h"
#include <common.h>
#include <json.hpp>
#include <ZenLib/utils/logger.h>
#include <bx/uint32_t.h>
#include <components/VobClasses.h>
#include <content/StaticLevelMesh.h>
#include <content/VertexTypes.h>
#include <debugdraw/debugdraw.h>
#include <imgui/imgui.h>
#include <logic/Console.h>
#include <logic/NpcScriptState.h>
#include <logic/PlayerController.h>
#include <logic/SavegameManager.h>
#include <render/RenderSystem.h>
#include <render/WorldRender.h>
#include <target/REGoth.h>
#include <ui/BarView.h>
#include <ui/DialogBox.h>
#include <ui/Hud.h>
#include <ui/ImageView.h>
#include <ui/Menu.h>
#include <ui/Menu_Main.h>
#include <utils/cli.h>
#include <utils/zTools.h>
#include <vdfs/fileIndex.h>
#include <zenload/zCMesh.h>
#include <zenload/zenParser.h>
#include <zenload/ztex2dds.h>
#include <utils/GLFW_Keys.h>
#include <logic/ScriptEngine.h>
#include <logic/DialogManager.h>
#include <content/StaticMeshAllocator.h>
#include <content/SkeletalMeshAllocator.h>

using json = nlohmann::json;

namespace Flags
{
    Cli::Flag help("h", "help", 0, "Prints this message");
    Cli::Flag vsync("vsync", "vertical-sync", 0, "Enables vertical sync", {"0"}, "Rendering");
}

// TODO: Keymap should be loaded from config
std::map<int, UI::EInputAction> keyMap = {
    {GLFW_KEY_UP, UI::IA_Up},
    {GLFW_KEY_DOWN, UI::IA_Down},
    {GLFW_KEY_LEFT, UI::IA_Left},
    {GLFW_KEY_RIGHT, UI::IA_Right},
    {GLFW_KEY_ENTER, UI::IA_Accept},
    {GLFW_KEY_ESCAPE, UI::IA_Close},
    {GLFW_KEY_BACKSPACE, UI::IA_Backspace},
    {GLFW_KEY_0, UI::IA_0},
    {GLFW_KEY_1, UI::IA_1},
    {GLFW_KEY_2, UI::IA_2},
    {GLFW_KEY_3, UI::IA_3},
    {GLFW_KEY_4, UI::IA_4},
    {GLFW_KEY_5, UI::IA_5},
    {GLFW_KEY_6, UI::IA_6},
    {GLFW_KEY_7, UI::IA_7},
    {GLFW_KEY_8, UI::IA_8},
    {GLFW_KEY_9, UI::IA_9},
    {GLFW_KEY_HOME, UI::IA_HOME},
    {GLFW_KEY_END, UI::IA_END},
    {GLFW_KEY_PAGE_UP, UI::IA_Up},
    {GLFW_KEY_PAGE_DOWN, UI::IA_Down},
    {GLFW_KEY_B, UI::IA_ToggleStatusMenu},
    {GLFW_KEY_F10, UI::IA_ToggleConsole},
    {GLFW_KEY_BACKSPACE, UI::IA_ToggleStatusMenu},
    {GLFW_KEY_N, UI::IA_ToggleLogMenu},
    {GLFW_KEY_L, UI::IA_ToggleLogMenu}};

void REGoth::init(int _argc, char** _argv)
{
    std::cout << "Running REGoth Engine" << std::endl;

    m_debug = BGFX_DEBUG_TEXT;
#if BX_PLATFORM_ANDROID
    m_reset = 0;
#else
    m_reset = BGFX_RESET_MAXANISOTROPY | BGFX_RESET_MSAA_X8;
    if (Flags::vsync.isSet())
        m_reset |= BGFX_RESET_VSYNC;
#endif

    m_Width = getWindowWidth();
    m_Height = getWindowHeight();
    m_HUDMode = 2;

    bgfx::init();
    bgfx::reset(m_Width, m_Height, m_reset);

    // Enable debug text.
    bgfx::setDebug(m_debug);

    // Set view 0 clear state.
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x7EC0EEff, 1.0f, 0);

    // Create vertex stream declaration.
    Meshes::UVNormColorVertex::init();
    Meshes::UVNormVertex::init();
    Meshes::UVVertex::init();
    Meshes::PositionVertex::init();
    Meshes::PositionColorVertex::init();
    Meshes::PositionUVVertex::init();
    Meshes::PositionUVVertex2D::init();
    Meshes::SkeletalVertex::init();

    m_pEngine = new Engine::GameEngine;

#if BX_PLATFORM_ANDROID
    // Content is somewhere else on android
    m_pEngine->setContentBasePath("/sdcard/REGoth/");
#endif

    m_pEngine->initEngine(_argc, _argv);

    showSplash();

    auto& menuMain = m_pEngine->getHud().pushMenu<UI::Menu_Main>();
    auto gameType = menuMain.determineGameType();
    m_pEngine->setBasicGameType(gameType);
    // Init SavegameManager
    Engine::SavegameManager::init(*m_pEngine);

    if (m_pEngine->getEngineArgs().startNewGame)
    {
        menuMain.onCustomAction("NEW_GAME");
    }

    m_timeOffset = bx::getHPCounter();

    ddInit();

    // Imgui.
    auto fontSize = 18.0f;

#ifdef ANDROID
    fontSize = 23.0f;
#endif

    initConsole();

    imguiCreate(fontSize);
    m_ImgUiCreated = true;
    m_scrollArea = 0;
}

void REGoth::initConsole()
{
    auto& console = m_pEngine->getConsole();
    using SuggestionBase = Logic::Console::SuggestionBase;
    using Suggestion = Logic::Console::Suggestion;
    using CandidateListGenerator = Logic::Console::CandidateListGenerator;

    // suggestion generator for the integer range [start, stop[. stop is not included
    auto rangeGen = [this](int start, int stop, int step = 1) -> std::vector<Suggestion> {
        std::vector<Suggestion> suggestions;
        for (int i = start; i < stop; i += step)
            suggestions.push_back(std::make_shared<SuggestionBase>(SuggestionBase{{std::to_string(i)}}));
        return suggestions;
    };

    // creates simple suggestion generator from iterables of string
    auto simpleStringGenGen = [](const auto& stringContainer){
        return [stringContainer](){
            std::vector<Suggestion> suggestions;
            for (auto& token : stringContainer)
                suggestions.push_back(std::make_shared<SuggestionBase>(SuggestionBase{{std::move(token)}}));
            return suggestions;
        };
    };

    console.registerCommand("estimatedGPUMem", [this](const std::vector<std::string>& args) -> std::string {
        World::WorldInstance& world = m_pEngine->getMainWorld().get();

        std::stringstream ss;
        ss << "Current world GPU Memory Consumption (Rough estimate!):" << std::endl
           << "   - Textures: " << world.getTextureAllocator().getEstimatedGPUMemoryConsumption() / 1024 / 1024 << " mb" << std::endl
           << "   - SkeletalMeshes: " << world.getSkeletalMeshAllocator().getEstimatedGPUMemoryConsumption() / 1024 / 1024 << " mb" << std::endl
           << "   - StaticMeshes: " << world.getStaticMeshAllocator().getEstimatedGPUMemoryConsumption() / 1024 / 1024 << " mb" << std::endl;

        size_t sizeLargestSkel, sizeLargestStatic;
        std::string nameLargestSkel, nameLargestStatic;
        world.getSkeletalMeshAllocator().getLargestContentInformation(sizeLargestSkel, nameLargestSkel);
        world.getStaticMeshAllocator().getLargestContentInformation(sizeLargestStatic, nameLargestStatic);

        ss << std::endl
           << "Largest SkeletalMesh: " << nameLargestSkel << " (" << sizeLargestSkel / 1024 << " kb)" << std::endl
           << "Largest StaticMesh:   " << nameLargestStatic << " (" << sizeLargestStatic / 1024 << " kb)" << std::endl;

            LogInfo() << ss.str();
            return ss.str();
        });

        console.registerCommand("kf", [&](const std::vector<std::string>& args) -> std::string {
            if(args.size() < 2)
                return "Missing argument. Usage: kf <idx>";

            m_pEngine->getMainWorld().get().getCameraController()->storeKeyframe(atoi(args[1].c_str()));
            return "Saved keyframe";
        });

    console.registerCommand("ckf", [&](const std::vector<std::string>& args) -> std::string {
            m_pEngine->getMainWorld().get().getCameraController()->clearKeyframes();
            return "Cleared keyframe";
        });

    console.registerCommand("pkf", [&](const std::vector<std::string>& args) -> std::string {

            if(args.size() < 2)
                return "Missing argument. Usage: pkf <duration>";

            m_pEngine->getMainWorld().get().getCameraController()->playKeyframes(atof(args[1].c_str()));
            return "Playing keyed animation";
        });


    console.registerCommand("stats", [](const std::vector<std::string>& args) -> std::string {
        static bool s_Stats = false;
        s_Stats = !s_Stats;

        bgfx::setDebug(s_Stats ? BGFX_DEBUG_STATS : 0);
        return "Toggled stats";
        });

    console.registerCommand("hud", [this](const std::vector<std::string>& args) -> std::string {

            if(args.size() < 2)
                return "Missing argument. Usage: hud <mode> | (0=None, 1=Gameplay, 2=Full)";

            m_HUDMode = std::min(2, std::stoi(args[1]));

        return "Toggled hud";
    });

    std::map<std::string, Logic::CameraController::ECameraMode> camModes = {
        {"ThirdPerson", Logic::CameraController::ECameraMode::ThirdPerson},
        {"FirstPerson", Logic::CameraController::ECameraMode::FirstPerson, },
        {"Free", Logic::CameraController::ECameraMode::Free},
        {"Viewer", Logic::CameraController::ECameraMode::Viewer},
        {"Static", Logic::CameraController::ECameraMode::Static},
        {"KeyedAnimation", Logic::CameraController::ECameraMode::KeyedAnimation},
    };
    auto& commandCamera = console.registerCommand("set camera", [this, camModes](const std::vector<std::string>& args) -> std::string {

        if (args.size() < 3)
            return "Missing argument. Usage: camera <mode>";

        const std::string& modeStr = args[2];
        auto it = camModes.find(modeStr);
        if (it == camModes.end())
            return "invalid camera mode: " + modeStr;
        const auto& mode = it->second;
        m_pEngine->getMainWorld().get().getCameraController()->setCameraMode(mode);
        return "Cameramode changed to " + modeStr;
    });
    std::vector<std::string> camModeNames;
    for (const auto& pair : camModes)
        camModeNames.push_back(pair.first);
    commandCamera.registerAutoComplete(simpleStringGenGen(camModeNames));

    console.registerCommand("test", [this](const std::vector<std::string>& args) -> std::string {
        auto& worldInstance = m_pEngine->getMainWorld().get();
        auto& scriptEngine = worldInstance.getScriptEngine();
        auto& datFile = scriptEngine.getVM().getDATFile();
        return "Hello World!";
    });

    console.registerCommand("reloadshaders", [this](const std::vector<std::string>& args) -> std::string {
        m_pEngine->getDefaultRenderSystem().loadShaders();

        return "Reloaded shaders";
    });

    console.registerCommand("set day", [this](const std::vector<std::string>& args) -> std::string {
        // modifies the day
        if (args.size() < 3)
            return "Missing argument. Usage: set day <day>";

        int day = std::stoi(args[2]);
        auto& clock = m_pEngine->getGameClock();
        clock.setDay(day);

        return "Set day to " + std::to_string(clock.getDay());
    });

    console.registerCommand("set clock", [this](const std::vector<std::string>& args) -> std::string {
               // modifies the world time
               if (args.size() < 3)
                   return "Invalid arguments. Usage: set clock <hh> [<mm>]";

               const int hh = std::stoi(args[2]);
               int mm = 0;
               if (args.size() >= 4)
                   mm = std::stoi(args[3]);

               if (hh < 0 || hh > 23)
                   return "Invalid argument. Hours must be in range from 0 to 23";
               if (mm < 0 || mm > 59)
                   return "Invalid argument. Minutes must be in range from 0 to 59";

               auto& clock = m_pEngine->getGameClock();
               clock.setTimeOfDay(hh, mm);

               return "Set clock to " + clock.getTimeOfDayFormatted();
           })
        .registerAutoComplete(std::bind(rangeGen, 0, 24))
        .registerAutoComplete(std::bind(rangeGen, 0, 60));

    console.registerCommand("set clockspeed", [this](const std::vector<std::string>& args) -> std::string {
        // adds an additional speed factor for the time of day
        if (args.size() < 3)
            return "Missing argument. Usage: set clockspeed <factor>";

        float factor = std::stof(args[2]);
        m_pEngine->getGameClock().setClockSpeedFactor(factor);

        return "Set clockspeed to " + std::to_string(factor);
    });

    console.registerCommand("set gamespeed", [this](const std::vector<std::string>& args) -> std::string {
        // adds an additional speed factor for the game time
        if (args.size() < 3)
            return "Missing argument. Usage: set gamespeed <factor:default=1>";

        float factor = std::stof(args[2]);
        m_pEngine->getGameClock().setGameEngineSpeedFactor(factor);

        return "Set gamespeed to " + std::to_string(factor);
    });

    const std::map<std::string, Logic::EventMessages::MovementMessage::WalkMode> walkModes = {
            {"RUN", Logic::EventMessages::MovementMessage::WalkMode::Run},
            {"WALK", Logic::EventMessages::MovementMessage::WalkMode::Walk},
            {"SNEAK", Logic::EventMessages::MovementMessage::WalkMode::Sneak},
            {"WATER", Logic::EventMessages::MovementMessage::WalkMode::Water},
            {"SWIM", Logic::EventMessages::MovementMessage::WalkMode::Swim},
            {"DIVE", Logic::EventMessages::MovementMessage::WalkMode::Dive},
    };

    auto& setWalkmode = console.registerCommand("set walkmode", [this, walkModes](const std::vector<std::string>& args) -> std::string {

        using WalkMode = Logic::EventMessages::MovementMessage::WalkMode;
        using MovementMessage = Logic::EventMessages::MovementMessage;

        if (args.size() < 3)
            return "Missing argument. Usage: set walkmode <mode>";

        std::string modestr = Utils::uppered(args[2]);

        WalkMode mode = WalkMode::Run;

        if(walkModes.find(modestr) != walkModes.end())
            mode = walkModes.find(modestr)->second;
        else
            return "Invalid walkmode: " + modestr;

        auto& s = m_pEngine->getMainWorld().get().getScriptEngine();
        VobTypes::NpcVobInformation player = VobTypes::asNpcVob(m_pEngine->getMainWorld().get(), s.getPlayerEntity());

        if(!player.isValid())
            return "No valid player found!";

        MovementMessage msg;
        msg.subType = MovementMessage::ST_SetWalkMode;
        msg.walkMode = mode;

        player.playerController->getEM().onMessage(msg);

        return "Set Walkmode to " + modestr + " (" + std::to_string((int)mode) + ")";
    });


    std::vector<std::string> walkModeNames;
    for (const auto& pair : walkModes)
        walkModeNames.push_back(pair.first);

    setWalkmode.registerAutoComplete(simpleStringGenGen(walkModeNames));

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

    auto waypointNamesGen = [this]() -> std::vector<Suggestion> {
        std::vector<Suggestion> suggestions;
        auto& waypoints = m_pEngine->getMainWorld().get().getWaynet().waypoints;
        for (auto& waypoint : waypoints)
        {
            Suggestion suggestion = std::make_shared<SuggestionBase>(SuggestionBase{{waypoint.name}});
            suggestions.push_back(suggestion);
        }
        return suggestions;
    };

    auto freepointNamesGen = [this]() -> std::vector<Suggestion> {
        std::vector<Suggestion> suggestions;
        auto& freepoints = m_pEngine->getMainWorld().get().getFreepoints();
        for (auto& freepoint : freepoints)
        {
            Suggestion suggestion = std::make_shared<SuggestionBase>(SuggestionBase{{freepoint.first}});
            suggestions.push_back(suggestion);
        }
        return suggestions;
    };

    auto spawnpointNamesGen = [waypointNamesGen, freepointNamesGen]() -> std::vector<Suggestion> {
        auto spawnpoints = waypointNamesGen();
        auto freepoints = freepointNamesGen();
        spawnpoints.insert(spawnpoints.end(), freepoints.begin(), freepoints.end() );
        return spawnpoints;
    };

    auto& gotoWaypoint = console.registerCommand("goto waypoint", [this](const std::vector<std::string>& args) -> std::string {
        if (args.size() != 3)
            return "Invalid argument. Usage: goto waypoint [waypoint]";

        const std::string& waypoint = args[2];

        auto& world = m_pEngine->getMainWorld().get();
        Logic::ScriptEngine& scriptEngine = world.getScriptEngine();

        VobTypes::NpcVobInformation player = VobTypes::asNpcVob(world, scriptEngine.getPlayerEntity());
        if (!player.isValid())
            return "Error: There is no valid player in the world";

        if (World::Waynet::waypointExists(world.getWaynet(), waypoint))
        {
            player.playerController->teleportToWaypoint(World::Waynet::getWaypointIndex(world.getWaynet(), waypoint));
            return "Player moved to waypoint " + waypoint;
        }
        else
        {
            return "Error: waypoint " + waypoint + " not found";
        }
    });
    gotoWaypoint.registerAutoComplete(waypointNamesGen);

    auto& gotoFreepoint = console.registerCommand("goto freepoint", [this](const std::vector<std::string>& args) -> std::string {
        if (args.size() != 3)
            return "Invalid argument. Usage: goto waypoint [waypoint]";

        const std::string& freepoint = args[2];

        auto& world = m_pEngine->getMainWorld().get();
        Logic::ScriptEngine& scriptEngine = world.getScriptEngine();

        VobTypes::NpcVobInformation player = VobTypes::asNpcVob(world, scriptEngine.getPlayerEntity());
        if (!player.isValid())
            return "Error: There is no valid player in the world";

        if(world.doesFreepointExist(freepoint))
        {
            player.playerController->teleportToPosition(world.getFreepointPosition(freepoint));
            return "Player moved to freepoint " + freepoint;
        } else
        {
            return "Error: freepoint " + freepoint + " not found";
        }
    });
    gotoFreepoint.registerAutoComplete(freepointNamesGen);

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

    auto zenLevelNamesGen = [this]() -> std::vector<Suggestion> {

        std::vector<std::string> g1WorldNames = {"WORLD.ZEN", "FREEMINE.ZEN", "OLDMINE.ZEN", "ORCGRAVEYARD.ZEN", "ORCTEMPEL.ZEN"};
        std::vector<std::string> g2WorldNames = {"DRAGONISLAND.ZEN", "NEWWORLD.ZEN", "OLDWORLD.ZEN", "ADDONWORLD.ZEN"};
        auto& worldNames = m_pEngine->getBasicGameType() == Daedalus::GameType::GT_Gothic1 ? g1WorldNames : g2WorldNames;
        std::vector<Suggestion> suggestions;
        for (const auto& worldName : worldNames)
            suggestions.push_back(std::make_shared<SuggestionBase>(SuggestionBase{{worldName}}));
        return suggestions;

        // also works, but ugly
        /*
           static std::vector<Suggestion> suggestions;
           if (!suggestions.empty())
           return suggestions;

           for (auto& fileInfo : m_pEngine->getVDFSIndex().getKnownFiles())
           {
           if (Utils::endsWith(Utils::lowered(fileInfo.fileName), ".zen"))
           {
        // ugly but works
        ZenLoad::ZenParser parser(fileInfo.fileName, m_pEngine->getVDFSIndex());
        parser.readHeader();
        ZenLoad::oCWorldData world;

        ZenLoad::ZenParser::ChunkHeader header;
        parser.readChunkStart(header);
        LogInfo() << fileInfo.fileName;
        assert(header.classname == "oCWorld:zCWorld");

        try
        {
        ZenLoad::oCWorld::readObjectData(world, parser, header.version);
        if (parser.getWorldMesh())
        suggestions.push_back(std::make_shared<SuggestionBase>(SuggestionBase{{fileInfo.fileName}}));
        }
        catch (const std::runtime_error&)
        {
        }
        }
        }
        return suggestions;
         */
    };

    console.registerCommand("switchlevel", [this](const std::vector<std::string>& args) -> std::string {
               const std::string& zenFilename = args.size() >= 2 ? args[1] : m_pEngine->getEngineArgs().startupZEN;
               if (!m_pEngine->getVDFSIndex().hasFile(zenFilename))
                   return "File '" + zenFilename + "' not found.";

               m_pEngine->getSession().switchToWorld(zenFilename);
               return "Switching world to: " + zenFilename;
           })
        .registerAutoComplete(zenLevelNamesGen);

    console.registerCommand("load", [this](const std::vector<std::string>& args) -> std::string {

               if (args.size() != 2)
                   return "Missing argument. Usage: load <savegame>";

               using namespace Engine;

               int index = std::stoi(args[1]);
               int maxSlots = SavegameManager::maxSlots();
               if (!(index >= 0 && index < maxSlots))
               {
                   return "invalid slot index " + std::to_string(index) + ". allowed range: 0.." + std::to_string(maxSlots - 1);
               }
               auto error = Engine::SavegameManager::loadSaveGameSlot(index);
               if (!error.empty())
                   return error;
               return "loading savegame...";
           })
        .setRequiresWorld(false);

    console.registerCommand("save", [this](const std::vector<std::string>& args) -> std::string {

        if (args.size() < 2)
            return "Missing argument. Usage: save <slotindex> [<savegamename>]";

        using namespace Engine;

        int index = std::stoi(args[1]);
        int maxSlots = Engine::SavegameManager::maxSlots();
        if (!(index >= 0 && index < maxSlots))
        {
            return "invalid slot index " + std::to_string(index) + ". allowed range: 0.." + std::to_string(maxSlots - 1);
        }

        std::string saveGameName;
        if (args.size() >= 3)
        {
            saveGameName = args[2];
        }
        else if (Engine::SavegameManager::isSavegameAvailable(index))
        {
            saveGameName = Engine::SavegameManager::readSavegameInfo(index).name;
        }

        // better do saving at frame end and not between entity updates
        this->m_pEngine->queueMainThreadJob([index, saveGameName](Engine::BaseEngine* engine){
            Engine::SavegameManager::saveToSlot(index, saveGameName);
        });

        return "Saving world to slot: " + std::to_string(index) + "...";
    });

    CandidateListGenerator worlddNpcNamesGen = [this]() {
        auto& worldInstance = m_pEngine->getMainWorld().get();
        auto& scriptEngine = worldInstance.getScriptEngine();
        auto& datFile = scriptEngine.getVM().getDATFile();

        std::vector<Suggestion> suggestions;
        for (const Handle::EntityHandle& npc : scriptEngine.getWorldNPCs())
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
            Suggestion suggestion = std::make_shared<Logic::Console::NPCSuggestion>(group, npc);
            suggestions.push_back(suggestion);
        }
        return suggestions;
    };

    CandidateListGenerator npcNamesGen = [this]() {
        auto& worldInstance = m_pEngine->getMainWorld().get();
        auto& scriptEngine = worldInstance.getScriptEngine();
        auto& datFile = scriptEngine.getVM().getDATFile();

        std::vector<Suggestion> suggestions;
        datFile.iterateSymbolsOfClass("C_NPC", [&](size_t i, Daedalus::PARSymbol& parSymbol) {
            Suggestion suggestion = std::make_shared<SuggestionBase>(SuggestionBase{{parSymbol.name}});
            suggestions.push_back(suggestion);
        });
        return suggestions;
    };

    console.registerCommand("control", [this, worlddNpcNamesGen](const std::vector<std::string>& args) -> std::string {
               if (args.size() < 2)
                   return "Missing argument. Usage: control <npc>";

               std::string requested = args[1];

               auto& scriptEngine = m_pEngine->getMainWorld().get().getScriptEngine();
               auto& worldInstance = m_pEngine->getMainWorld().get();

               auto suggestions = worlddNpcNamesGen();
               auto baseSuggestion = Logic::Console::findSuggestion(suggestions, requested);
               auto suggestion = std::dynamic_pointer_cast<Logic::Console::NPCSuggestion>(baseSuggestion);
               if (suggestion != nullptr)
               {
                   worldInstance.takeControlOver(suggestion->npcHandle);
                   VobTypes::NpcVobInformation npcVobInfo = VobTypes::asNpcVob(worldInstance, suggestion->npcHandle);
                   Daedalus::GEngineClasses::C_Npc& npcScriptObject = npcVobInfo.playerController->getScriptInstance();
                   const std::string& npcDisplayName = npcScriptObject.name[0];
                   const std::string& npcDatFileName = scriptEngine.getVM().getDATFile().getSymbolByIndex(npcScriptObject.instanceSymbol).name;
                   std::string npnNameFull = npcDisplayName + " (" + npcDatFileName + ")";
                   return "took control over " + npnNameFull;
               }
               return "NPC not found in this world: " + requested;

           })
        .registerAutoComplete(worlddNpcNamesGen);

    console.registerCommand("givexp", [this](const std::vector<std::string>& args) -> std::string {
        auto& s1 = m_pEngine->getMainWorld().get().getScriptEngine();

        if (args.size() != 2)
            return "Missing argument. Usage: givexp <experience points>";

        std::string scriptName;
        // B_GiveXP is called B_GivePlayerXP in Gothic II
        for (auto scriptFunctionName : {"B_GiveXP", "B_GivePlayerXP"})
        {
            if (s1.getVM().getDATFile().hasSymbolName(scriptFunctionName))
            {
                scriptName = scriptFunctionName;
                break;
            }
        }
        if (scriptName.empty())
            return "error: could not find script functions";

        int exp = std::stoi(args[1]);
        s1.prepareRunFunction();
        s1.pushInt(exp);
        s1.runFunction(scriptName, false);

        return "Experience points successfully given";
    });

    auto tpCallback = [this, worlddNpcNamesGen](const std::vector<std::string>& args) -> std::string {
        if (args.size() < 2)
            return "Missing argument(s). Usage: tp [<npc:default=player>] <targetnpc>";

        auto& worldInstance = m_pEngine->getMainWorld().get();
        auto& scriptEngine = worldInstance.getScriptEngine();
        auto& datFile = scriptEngine.getVM().getDATFile();

        std::string teleporterName;
        std::string targetName;
        if (args.size() >= 3)
        {
            teleporterName = args[1];
            targetName = args[2];
        }
        else
        {
            targetName = args[1];
        }

        auto suggestions = worlddNpcNamesGen();

        std::vector<VobTypes::NpcVobInformation> vobInfos;
        std::vector<std::string> fullNames;
        for (auto& requestedName : {teleporterName, targetName})
        {
            bool success = false;
            Handle::EntityHandle entityHandle = Handle::EntityHandle::makeInvalidHandle();
            if (!requestedName.empty())
            {
                auto baseSuggestion = Logic::Console::findSuggestion(suggestions, requestedName);
                auto suggestion = std::dynamic_pointer_cast<Logic::Console::NPCSuggestion>(baseSuggestion);
                if (suggestion != nullptr)
                    entityHandle = suggestion->npcHandle;
            }
            else
            {
                // case: no argument given
                entityHandle = scriptEngine.getPlayerEntity();
            }
            if (entityHandle.isValid())
            {
                VobTypes::NpcVobInformation npcVobInfo = VobTypes::asNpcVob(worldInstance, entityHandle);
                if (npcVobInfo.isValid())
                {
                    success = true;
                    Daedalus::GEngineClasses::C_Npc& npcScriptObject = npcVobInfo.playerController->getScriptInstance();
                    const std::string& npcDisplayName = npcScriptObject.name[0];
                    const std::string& npcDatFileName = scriptEngine.getVM().getDATFile().getSymbolByIndex(npcScriptObject.instanceSymbol).name;
                    vobInfos.push_back(npcVobInfo);
                    fullNames.push_back(npcDisplayName + " (" + npcDatFileName + ")");
                }
            }
            if (!success)
                return "Could not find NPC " + requestedName;
        }

        VobTypes::NpcVobInformation teleporter = vobInfos.at(0);
        VobTypes::NpcVobInformation target = vobInfos.at(1);
        Math::float3 targetPosition = target.position->m_WorldMatrix.Translation();
        Math::float3 targetDirection = target.playerController->getDirection();
        // keep a respectful distance of 1 to the NPC
        float respectfulDistance = 1;
        Math::float3 newPos = targetPosition + respectfulDistance * targetDirection;
        teleporter.playerController->teleportToPosition(newPos);
        // look towards NPC
        teleporter.playerController->setDirection((-1) * targetDirection);
        return "Teleported " + fullNames[0] + " to " + fullNames[1];

    };

    console.registerCommand("tp", tpCallback)
        .registerAutoComplete(worlddNpcNamesGen)
        .registerAutoComplete(worlddNpcNamesGen);

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

            if (nearNPCs.empty())
                return "No NPCs in range!";
            // Chose one at random
            npcVobInfo = VobTypes::asNpcVob(worldInstance, *nearNPCs.begin());
        }
        else
        {
            const std::string& requested = args.at(1);
            auto suggestions = worlddNpcNamesGen();
            auto baseSuggestion = Logic::Console::findSuggestion(suggestions, requested);
            auto suggestion = std::dynamic_pointer_cast<Logic::Console::NPCSuggestion>(baseSuggestion);
            if (suggestion == nullptr)
                return "NPC not found in this world: " + requested;
            npcVobInfo = VobTypes::asNpcVob(worldInstance, suggestion->npcHandle);
        }

        if (!npcVobInfo.isValid())
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
        }
        else if (Utils::stringEqualIngoreCase(args.at(0), "kill"))
        {
            Logic::EventMessages::StateMessage sm;
            sm.subType = Logic::EventMessages::StateMessage::EV_StartState;
            sm.functionSymbol = Logic::NPC_PRGAISTATE_DEAD;
            sm.isPrgState = true;
            npcVobInfo.playerController->die(scriptEngine.getPlayerEntity());
            stateName = "DEAD";
        }
        else
        {
            return "internal error";
        }
        Daedalus::GEngineClasses::C_Npc& npcScriptObject = npcVobInfo.playerController->getScriptInstance();
        const std::string& npcDisplayName = npcScriptObject.name[0];
        const std::string& npcDatFileName = scriptEngine.getVM().getDATFile().getSymbolByIndex(npcScriptObject.instanceSymbol).name;
        std::string npnNameFull = npcDisplayName + " (" + npcDatFileName + ")";

        return npnNameFull + " is now in " + stateName + " state";
    };

    auto& insertNPC = console.registerCommand("insertnpc", [this](const std::vector<std::string>& args) -> std::string {
        if (args.size() < 3)
            return "Missing argument. Usage: insertnpc <npc> <spawnpoint>";

        auto& worldInstance = m_pEngine->getMainWorld().get();
        auto& se = worldInstance.getScriptEngine();
        auto& datFile = se.getVM().getDATFile();

        auto name = args[1];
        auto spawnpoint = args[2];
        if (!datFile.hasSymbolName(name))
            return "NPC not found: " + name;

        if (!World::Waynet::waypointExists(worldInstance.getWaynet(), spawnpoint) && !worldInstance.doesFreepointExist(spawnpoint))
            return "Invalid spawnpoint: " + spawnpoint;

        se.getVM().getGameState().insertNPC(datFile.getSymbolIndexByName(name), spawnpoint);
        return "Inserting NPC " + name + " at spawnpoint " + spawnpoint;
    });
    insertNPC.registerAutoComplete(npcNamesGen).registerAutoComplete(spawnpointNamesGen);

    console.registerCommand("knockout", killOrKnockoutCallback).registerAutoComplete(worlddNpcNamesGen);
    console.registerCommand("kill", killOrKnockoutCallback).registerAutoComplete(worlddNpcNamesGen);

    console.registerCommand("interrupt", [this](const std::vector<std::string>& args) -> std::string {
        auto& world = m_pEngine->getMainWorld().get();
        VobTypes::NpcVobInformation player = VobTypes::asNpcVob(world, world.getScriptEngine().getPlayerEntity());
        player.playerController->interrupt();
        return "Interrupted player, cleared EM";
    });

    console.registerCommand("playsound", [this](const std::vector<std::string>& args) -> std::string {
        if (args.size() < 2)
            return "Missing argument. Usage: playsound <soundfile>";

        auto& world = m_pEngine->getMainWorld().get();
        world.getAudioWorld().playSound(args[1]);

        return "Played sound " + args[1];
    });

    console.registerCommand("volume", [this](const std::vector<std::string>& args) -> std::string {
        if (args.size() < 2)
            return "Missing argument. Usage: volume <value>";

        auto& world = m_pEngine->getMainWorld().get();
        world.getAudioWorld().setListenerGain(std::stof(args[1]));

        return "Set volume to " + args[1];
    });

    console.registerCommand("hurtself", [this](const std::vector<std::string>& args) -> std::string {
        if (args.size() < 2)
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

        if (args.size() < 2)
            return "Missing argument. Usage: usemana <mana>";

        int dmg = std::stoi(args[1]);
        player.playerController->changeAttribute(Daedalus::GEngineClasses::C_Npc::EATR_MANA, -dmg);

        return "Used " + std::to_string(dmg) + " mana";
    });

    CandidateListGenerator itemNamesGen = [this]() {
        auto& se = m_pEngine->getMainWorld().get().getScriptEngine();
        auto& datFile = se.getVM().getDATFile();
        std::vector<Suggestion> suggestions;
        {
            Daedalus::GameState::ItemHandle dummyHandle = se.getVM().getGameState().createItem();
            Daedalus::GEngineClasses::C_Item& cItem = se.getVM().getGameState().getItem(dummyHandle);
            datFile.iterateSymbolsOfClass("C_ITEM", [&](size_t i, Daedalus::PARSymbol& parSymbol) {
                // reset to default values. especially name and description
                cItem = Daedalus::GEngineClasses::C_Item();
                // Run the script-constructor
                se.getVM().initializeInstance(ZMemory::toBigHandle(dummyHandle), i, Daedalus::IC_Item);

                std::vector<std::string> aliasList;
                for (auto name : {parSymbol.name, cItem.description, cItem.name})
                {
                    std::replace(name.begin(), name.end(), ' ', '_');
                    aliasList.push_back(name);
                }
                if (aliasList[1] == aliasList[2])
                {
                    // most of the items have description equal to name, so remove one of them
                    aliasList.pop_back();
                }
                Suggestion suggestion = std::make_shared<SuggestionBase>(SuggestionBase{aliasList});
                suggestions.push_back(suggestion);
            });
            se.getVM().getGameState().removeItem(dummyHandle);
        }
        return suggestions;
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
            return "invalid amount " + std::to_string(amount);

        auto& se = m_pEngine->getMainWorld().get().getScriptEngine();

        std::size_t index = 0;
        auto suggestions = itemNamesGen();
        auto suggestion = Logic::Console::findSuggestion(suggestions, itemName);
        if (suggestion == nullptr)
            return "Item not found!";

        auto& aliasList = suggestion->aliasList;
        auto& parScriptName = aliasList[0];
        VobTypes::NpcVobInformation player = VobTypes::asNpcVob(m_pEngine->getMainWorld().get(), se.getPlayerEntity());
        std::string description;
        if (give)
        {
            auto handle = player.playerController->getInventory().addItem(parScriptName, amount);
            Daedalus::GEngineClasses::C_Item& cItem = se.getVM().getGameState().getItem(handle);
            description = cItem.description;
        }
        else
        {
            auto handle = player.playerController->getInventory().getItem(parScriptName);
            if (!handle.isValid())
                return "error: could not remove item. item " + parScriptName + " is not in inventory";

            Daedalus::GEngineClasses::C_Item& cItem = se.getVM().getGameState().getItem(handle);
            description = cItem.description;
            amount = std::min(static_cast<uint32_t>(amount), cItem.amount);
            player.playerController->getInventory().removeItem(parScriptName, amount);
        }
        std::string action = give ? "added to" : "removed from";
        return std::string("Item(s) " + action + " the inventory: ") + std::to_string(amount) + " x " + description + " (" + parScriptName + ")";

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
}

int REGoth::shutdown()
{
    // remove all worlds so that it shuts down properly
    m_pEngine->resetSession();

    delete m_pEngine;

    ddShutdown();

    // Clear this explicitly or the atlas is destroyed after main (and bx cleanup / allocator destruction)
    ImFontAtlas* atlas = ImGui::GetIO().Fonts;
    atlas->Clear();

    if (m_ImgUiCreated)
        imguiDestroy();

    bgfx::shutdown();

    return REGothPlatform::shutdown();
}

bool REGoth::update()
{
    std::string frameInputText = getFrameTextInput();
    for (int i = 0; i < NUM_KEYS; i++)
    {
        if (!getKeysTriggered()[i])  // If key has been pushed the first time or repeatedly
            continue;

        int mods = getModsTriggered()[i];

        if (m_pEngine->getConsole().isOpen())
            m_pEngine->getConsole().onKeyDown(i, mods);
        if (keyMap.find(i) != keyMap.end())
            m_pEngine->getHud().onInputAction(keyMap[i]);
    }

    // Pass text input from this frame
    m_pEngine->getHud().onTextInput(frameInputText);

    bool disableBindings = m_pEngine->getConsole().isOpen() || m_pEngine->getHud().isMenuActive();
    if (!disableBindings)
        Engine::Input::fireBindings();

    // Check for resize
    if (m_Width != getWindowWidth() || m_Height != getWindowHeight())
    {
        m_Width = getWindowWidth();
        m_Height = getWindowHeight();

        // Notify bgfx about framebuffer resize
        bgfx::reset(m_Width, m_Height);
    }
    int64_t excludedFrameTime = m_pEngine->getExludedFrameTime();
    m_pEngine->resetExludedFrameTime();
    // TODO: toogle this in multiplayer mode or disable via ExcludeFrameTime's constructor arg
    const bool isMultiplayer = false;
    if (isMultiplayer)
        excludedFrameTime = 0;

    int64_t now = bx::getHPCounter();
    static int64_t last = now;
    const int64_t frameTime = (now - last) - excludedFrameTime;
    last = now;
    const double freq = double(bx::getHPFrequency());
    const double toMs = 1000.0 / freq;

    float time = (float)((now - m_timeOffset) / double(bx::getHPFrequency()));
    const float dt = float(frameTime / freq);

    Engine::Input::MouseState ms;
    Engine::Input::getMouseState(ms);
    // Prepare rendering of debug-textboxes, etc
    imguiBeginFrame(ms.m_mx, ms.m_my, (ms.m_buttons[0] ? IMGUI_MBUT_LEFT : 0) | (ms.m_buttons[1] ? IMGUI_MBUT_RIGHT : 0) | (ms.m_buttons[2] ? IMGUI_MBUT_MIDDLE : 0), ms.m_mz, (uint16_t)getWindowWidth(), (uint16_t)getWindowHeight());

    // Use debug font to print information about this example.
    bgfx::dbgTextClear();

        if(m_HUDMode >= 2)
        {
            uint16_t xOffset = static_cast<uint16_t>(m_pEngine->getConsole().isOpen() ? 100 : 0);
            bgfx::dbgTextPrintf(xOffset, 1, 0x4f, "REGoth-Engine (%s)", m_pEngine->getEngineArgs().startupZEN.c_str());
            bgfx::dbgTextPrintf(xOffset, 2, 0x0f, "Frame: % 7.3f[ms] %.1f[fps]", 1000.0 * dt, 1.0f / (double(dt)));
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

        {
            auto& cfg = m_pEngine->getDefaultRenderSystem().getConfig();
            float gameSpeed = m_pEngine->getGameClock().getGameEngineSpeedFactor();
            if(m_HUDMode == 0)
            {
                // draw console even if HUD is disabled
                m_pEngine->getHud().getConsoleBox().update(dt * gameSpeed, ms, cfg);
            } else if(m_HUDMode >= 1)
            {
                m_pEngine->getRootUIView().update(dt * gameSpeed, ms, cfg);
            }
        }

    // debug draw
    {
        ddSetTransform(nullptr);
        ddDrawAxis(0.0f, 0.0f, 0.0f);

        ddPush();
        ddSetColor(0xff00ff00);
        ddSetTransform(Math::Matrix::CreateTranslation(10, 0, 0).mv);
        Aabb aabb =
            {
                {-1.0f, -1.0f, -1.0f},
                {1.0f, 1.0f, 1.0f},
            };
        ddDraw(aabb);

        ddSetTransform(nullptr);
        ddSetColor(0xff0000ff);
        ddMoveTo(0, 0, 0);
        ddLineTo(10, 0, 0);
        ddPop();

        ddEnd();
    }

    imguiEndFrame();

    if (disableBindings)
    {
        Engine::Input::clearTriggered();
    }

    // Advance to next frame. Rendering thread will be kicked to
    // process submitted rendering primitives.
    bgfx::frame();
    m_pEngine->processMessageQueue();

    return true;
}



void REGoth::showSplash()
{
    float view[16];
    float proj[16];

    bx::mtxIdentity(view);
    bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f, 0.0f, bgfx::getCaps()->homogeneousDepth);

    bgfx::setViewRect(1, 0, 0, (uint16_t)getWindowWidth(), (uint16_t)getWindowHeight());

    // Set view and projection matrix for view 0.
    bgfx::setViewTransform(1, NULL, proj);

    bgfx::touch(0);

#if BX_PLATFORM_ANDROID

#else
/*Textures::TextureAllocator alloc(*m_pEngine);
      Handle::TextureHandle txh = alloc.loadTextureVDF("STARTSCREEN.TGA");
      alloc.finalizeLoad(txh);

      if(!txh.isValid())
      return;

      Textures::Texture& texture = alloc.getTexture(txh);
      bgfx::frame();

      bgfx::setState(BGFX_STATE_DEFAULT);
      const Render::RenderConfig& cfg = m_pEngine->getDefaultRenderSystem().getConfig();
      bgfx::setTexture(0, cfg.uniforms.diffuseTexture, texture.m_TextureHandle);
      renderScreenSpaceQuad(1, cfg.programs.fullscreenQuadProgram, 0.0f, 0.0f, 1280.0f, 720.0f);*/
#endif

    bgfx::dbgTextPrintf(0, 1, 0x4f, "Loading...");
    bgfx::frame();
}

//Usage of Platform:

int main(int argc, char** argv)
{
    int ret = 0;

    // Load config values for flags
    Cli::loadConfigFile();

    // Overwrite flags set from config using the commandline
    Cli::setCommandlineArgs(argc, argv);

    // Check if the user just wanted to see the list of commands
    if (Flags::help.isSet())
    {
        Cli::printHelp();
        return 0;
    }

    // Do some commandline-operations, if wanted
    if (zTools::tryRunTools())
        return 0;

    REGoth app;
#ifdef NDEBUG
    try
    {
        ret = app.run(argc, argv);
        app.shutdown();
    }
    catch (const std::exception& e)
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
#else
    ret = app.run(argc, argv);
    app.shutdown();
#endif

    // Write current config-values
    Cli::writeConfigFile();

    return ret;
}
