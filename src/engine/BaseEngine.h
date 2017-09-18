#pragma once
#include "World.h"
#include <bx/commandline.h>
#include <engine/World.h>
#include <logic/SavegameManager.h>
#include <memory/StaticReferencedAllocator.h>
#include <ui/View.h>
#include <vdfs/fileIndex.h>
#include <utils/cli.h>

namespace Flags
{
    static Cli::Flag gameDirectory("g", "game-dir", 1, "Root-folder of your Gothic installation", {"."}, "Data");
    static Cli::Flag g1Directory("", "g1-path", 1, "Game data to use when the -g1 flag is specified on the commandline", {"."}, "Data");
    static Cli::Flag g2Directory("", "g2-path", 1, "Game data to use when the -g2 flag is specified on the commandline", {"."}, "Data");
    static Cli::Flag startG1("g1", "start-g1", 0, "Uses the path stored in the 'g1-path' config setting as game data path");
    static Cli::Flag startG2("g2", "start-g2", 0, "Uses the path stored in the 'g2-path' config setting as game data path");

    static Cli::Flag modFile("m", "mod-file", 1, "Additional .mod-file to load", {""}, "Data");
    static Cli::Flag world("w", "world", 1, ".ZEN-file to load out of one of the vdf-archives", {""}, "Data");
    static Cli::Flag emptyWorld("", "empty-world", 0, "Will load no .ZEN-file at all.");
    static Cli::Flag playerScriptname("p", "player", 1, "When starting a new game, the player will be inserted as the given NPC", {"PC_HERO"});
    static Cli::Flag startNewGame("", "skipmenu", 0, "Skips the menu and starts a new game directly on game startup");
    static Cli::Flag sndDevice("snd", "sound-device", 1, "OpenAL sound device", {""}, "Sound");
}

namespace Engine
{
    class GameSession;
    class AsyncAction;
    const int MAX_NUM_WORLDS = 4;

    enum class ExecutionPolicy
    {
        MainThread,
        NewThread
    };

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
        Daedalus::GameType getBasicGameType() { return m_BasicGameType; }
        /**
         * @return Basic gametype this is. Needed for sky configuration, for example
         */
        void setBasicGameType(Daedalus::GameType type) { m_BasicGameType = type; }

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
         * @return Arguments passed to the engine
         */
        EngineArgs getEngineArgs();

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
         * Saves the given world to a file, as savegame
         * @param world World to save
         * @param file File to save to
         * @return Whether the operation was successful
         */
        bool saveWorld(Handle::WorldHandle world, const std::string& file);

        /**
         * increase the time, which the current frame should not treat as elapsed
         */
        void addToExludedFrameTime(int64_t milliseconds) { m_ExcludedFrameTime += milliseconds; }
        int64_t getExludedFrameTime() { return m_ExcludedFrameTime; }
        void resetExludedFrameTime() { m_ExcludedFrameTime = 0; }

        /**
         * Called when a world was added
         * TODO onWorld... should be protected, but currently need to call this function from GameSession
         */
        virtual void onWorldCreated(Handle::WorldHandle world);
        virtual void onWorldRemoved(Handle::WorldHandle world){}

    protected:
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
         * Arguments
         */
        EngineArgs m_Args;

        /**
         * Folder where the content is
         */
        std::string m_ContentBasePath;

        /**
         * if the engine is paused. When it is paused the world doesn't receive the delta time updates
         */
        bool m_Paused;

        /**
         * amount of time for the next frame that should not be considered as elapsed
         */
        int64_t m_ExcludedFrameTime;
    };

    /**
     * instances of this class shall only be created/destroyed in one thread
     */
    class ExcludeFrameTime
    {
    public:
        ExcludeFrameTime(BaseEngine& baseEngine, bool enabled = true)
            : m_Engine(baseEngine)
            , m_Enabled(enabled)
            , m_Start(bx::getHPCounter())
        {
            if (m_Enabled)
                m_ReferenceCounter++;
        }

        ~ExcludeFrameTime()
        {
            if (m_Enabled)
            {
                m_ReferenceCounter--;
                if (m_ReferenceCounter == 0)
                {
                    auto elapsed = bx::getHPCounter() - m_Start;
                    m_Engine.addToExludedFrameTime(elapsed);
                }
            }
        }

    private:
        BaseEngine& m_Engine;
        // used to dynamically enable/disable this excluder at runtime
        int64_t m_Enabled;
        int64_t m_Start;
        // number of excluders currently alive
        // for handling overlapping or nested excluders
        static size_t m_ReferenceCounter;
    };
}
