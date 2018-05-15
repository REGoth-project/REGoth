#pragma once
#include <future>
#include "World.h"
#include "JobManager.h"
#include <bx/commandline.h>
#include <engine/GameClock.h>
#include <engine/GameSession.h>
#include <engine/World.h>
#include <logic/Console.h>
#include <logic/SavegameManager.h>
#include <memory/StaticReferencedAllocator.h>
#include <ui/View.h>
#include <vdfs/fileIndex.h>

namespace UI
{
    class Hud;
    class zFont;
    class zFontCache;
}

namespace Audio
{
    class AudioEngine;
}

namespace Engine
{
    class GameSession;
    const int MAX_NUM_WORLDS = 4;

    class BaseEngine
    {
    public:
        struct EngineArgs
        {
            EngineArgs()
                : cmdline(0, NULL)
            {
            }

            std::string gameBaseDirectory;
            std::string startupZEN;
            std::string playerScriptname;
            bool startNewGame;
            std::string testVisual;
            std::string modfile;
            bool noTextureFiltering;
            bx::CommandLine cmdline;
        };

        BaseEngine();
        virtual ~BaseEngine();

        /**
         * @brief Initializes the Engine
         */
        virtual void initEngine(int argc, char** argv);

        /**
         * @return Basic gametype this is. Needed for sky configuration, for example
         */
        Daedalus::GameType getBasicGameType() { return m_BasicGameType; };
        /**
         * @return Basic gametype this is. Needed for sky configuration, for example
         */
        void setBasicGameType(Daedalus::GameType type) { m_BasicGameType = type; }
        /**
         * @brief Frame update // TODO: Remove width and height
         */
        void frameUpdate(double dt, uint16_t width, uint16_t m_height);

        /**
         * @return Main VDF-Archive
         */
        VDFS::FileIndex& getVDFSIndex() { return m_FileIndex; }
        /**
         * Returns the world-instance of the given handle.
         * Note: Do not save this pointer somewhere! It may change!
         * @param h Handle to look up
         * @return Data of the world-instance
         */
        World::WorldInstance& getWorldInstance(Handle::WorldHandle& h);

        /**
         * @return Gameclock
         */
        GameClock& getGameClock();

        /**
         * information stored in a session is cleared when loading a savegame or starting a new game
         * @return GameSession
         */
        GameSession& getSession() { return *m_Session; }

        /**
         * drop all information bound to the current session
         */
        void resetSession();

        /**
         * Access for executing/queueing tasks that should be done synchronously or asynchronously
         */
        JobManager& getJobManager();

        /**
         * @return Console
         */
        Logic::Console& getConsole() { return m_Console; }
        /**
         * @return Arguments passed to the engine
         */
        EngineArgs getEngineArgs();

        /**
         * @return Base-level UI-View. Parent of all other views.
         */
        UI::View& getRootUIView() { return m_RootUIView; }
        /**
         * // TODO: Move to GameEngine, or pass GameEngine to world!
         * @return HUD
         */
        UI::Hud& getHud() { return *m_pHUD; }
        UI::zFontCache& getFontCache() { return *m_pFontCache; }
        Audio::AudioEngine& getAudioEngine() { return *m_AudioEngine; }
        /**
         * Sets the path the engine is looking for files
         * @param path New path
         */
        void setContentBasePath(const std::string& path) { m_ContentBasePath = path; }
        /*+
         * @return The path where the engine is looking for content files
         */
        const std::string& getContentBasePath() { return m_ContentBasePath; }
        /**
         * @return Allocator for always present textures
         */
        Textures::TextureAllocator& getEngineTextureAlloc() { return m_EngineTextureAlloc; }
        /**
         * @return data-access to the main world
         */
        Handle::WorldHandle getMainWorld();

        /**
         * Saves the given world to a file, as savegame
         * @param world World to save
         * @param file File to save to
         * @return Whether the operation was successful
         */
        bool saveWorld(Handle::WorldHandle world, const std::string& file);

        /**
         * Pauses or continues the game.
         * @param paused
         */
        void setPaused(bool paused);

        /**
         * Pauses or continues the game. Depending on the current state
         */
        void togglePaused() { setPaused(!m_Paused); }

        /**
         * Called when a world was added
         * TODO onWorld... should be protected, but currently need to call this function from GameSession
         */
        virtual void onWorldCreated(Handle::WorldHandle world);
        virtual void onWorldRemoved(Handle::WorldHandle world){};

        /**
         * amount of time for the next frame that should not be considered as elapsed
         */
        Utils::TimeSpan m_ExcludedFrameTime;

    protected:

        /**
         * Handles delegation of function calls to main thread and starting of asynchronous operations
         * contains ID of the main thread (bgfx thread), MUST BE initialized first
         */
        JobManager m_JobManager;

        /**
         * Update-method for subclasses
         */
        virtual void onFrameUpdate(double dt, uint16_t width, uint16_t height) = 0;

        /**
         * @brief Called to load archives into the main VDFS-Index
         *		  Overwrite to load your own default archives
         */
        virtual void loadArchives();

        /**
         * Enum with values for Gothic I and Gothic II
         */
        Daedalus::GameType m_BasicGameType;

        /**
         * Main VDFS-Index
         */
        VDFS::FileIndex m_FileIndex;

        /**
         * Game session, stores information that should be reset on starting a new game/loading
         * unique_ptr is used, because we can't overwrite the session itself,
         * because it has a member variable reference
         */
        std::unique_ptr<GameSession> m_Session;

        /**
         * ingame console
         */
        Logic::Console m_Console;

        /**
         * Arguments
         */
        EngineArgs m_Args;

        Audio::AudioEngine* m_AudioEngine = nullptr;

        /**
         * Base UI-View
         */
        UI::View m_RootUIView;
        UI::Hud* m_pHUD;
        UI::zFontCache* m_pFontCache;

        /**
         * Allocator for always present textures
         */
        Textures::TextureAllocator m_EngineTextureAlloc;

        /**
         * Folder where the content is
         */
        std::string m_ContentBasePath;

        /**
         * if the engine is paused. When it is paused the world doesn't receive the delta time updates
         */
        bool m_Paused;
    };
}