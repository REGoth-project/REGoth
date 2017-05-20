#pragma once
#include <memory/StaticReferencedAllocator.h>
#include "World.h"
#include <vdfs/fileIndex.h>
#include <ui/View.h>
#include <bx/commandline.h>
#include <logic/SavegameManager.h>
#include <future>
#include "GameClock.h"

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
	const int MAX_NUM_WORLDS = 4;

	class BaseEngine
	{
	public:

        struct EngineArgs
        {
			EngineArgs() : cmdline(0, NULL) {}

            std::string gameBaseDirectory;
            std::string startupZEN;
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
		Daedalus::GameType getBasicGameType() { return m_BasicGameType; };

		/**
         * @return Basic gametype this is. Needed for sky configuration, for example
         */
		void setBasicGameType(Daedalus::GameType type) { m_BasicGameType = type; }

		/**
		 * @brief Adds a world based of the given file
		 * @param worldfile Path to look for the worldfile. Can be inside a VDF-Archive
		 *		  or on disk (TODO)
		 */
		Handle::WorldHandle addWorld(const std::string& worldFile, const std::string& savegame = "");

        /**
         * Removes the current world and loads a new one
         * @param worldFile Path to look for the worldfile. Can be inside a VDF-Archive
         * @return Handle to the world
         */
        Handle::WorldHandle loadWorld(const std::string& worldFile, const std::string& savegame = "");

        /**
         * Removes a world and everything inside
         * @param world World to remove
         */
        void removeWorld(Handle::WorldHandle world);

        /**
         * Removes all worlds and everything inside
         */
        void removeAllWorlds();

		/**
		 * @brief Frame update // TODO: Remove width and height
		 */
		void frameUpdate(double dt, uint16_t width, uint16_t m_height);

		/**
		 * @return Main VDF-Archive
		 */
		VDFS::FileIndex& getVDFSIndex() { return m_FileIndex;  }

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
        GameClock& getGameClock() { return m_GameClock; }

        /**
         * @return Console
         */
        UI::Console& getConsole() { return m_Console; }

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
		UI::Hud& getHud(){ return *m_pHUD; }

        UI::zFontCache& getFontCache(){ return *m_pFontCache; }

        Audio::AudioEngine &getAudioEngine() { return *m_AudioEngine; }

		/**
		 * Sets the path the engine is looking for files
		 * @param path New path
		 */
		void setContentBasePath(const std::string& path){ m_ContentBasePath = path; }

		/*+
         * @return The path where the engine is looking for content files
         */
		const std::string& getContentBasePath(){ return m_ContentBasePath; }

		/**
		 * @return Allocator for always present textures
		 */
		Textures::TextureAllocator& getEngineTextureAlloc(){ return m_EngineTextureAlloc; }

		/**
         * Sets the currently active world. Player and camera will be taken from this world.
         * @param world
         */
		void setMainWorld(Handle::WorldHandle world);

        /**
         * @return data-access to the main world
         */
        Handle::WorldHandle getMainWorld()
        {
            return m_MainWorld;
        }

        /**
         * Saves the given world to a file, as savegame
         * @param world World to save
         * @param file File to save to
         * @return Whether the operation was successful
         */
        bool saveWorld(Handle::WorldHandle world, const std::string& file);

		/**
         * sets the speed factor for EVERYTHING the game for updating world instances
         * i.e. world (ergo animations), in game clock (ergo sky) even the camera
         * only for fun / debug purpose (default = 1.0)
         * @param factor
         */
		void setGameEngineSpeedFactor(float factor)
		{
			m_GameEngineSpeedFactor = factor;
		}

		/**
         * @return m_GameEngineSpeedFactor
         */
		float getGameEngineSpeedFactor() const
		{
			return m_GameEngineSpeedFactor;
		}

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
		 * increase the time, which the current frame should not treat as elapsed
		 */
		void addToExludedFrameTime(int64_t milliseconds) { m_ExcludedFrameTime += milliseconds; };

		int64_t getExludedFrameTime() { return m_ExcludedFrameTime; };
		void resetExludedFrameTime() { m_ExcludedFrameTime = 0; };

		/**
		 * Insert the given action at the end of the queue
		 */
		void queueSaveGameAction(SavegameManager::SaveGameAction saveGameAction);

		/**
		 * process all queued actions by FIFO
		 */
        void processSaveGameActionQueue();

        void processAsyncActionQueue();

	protected:

		/**
		 * Called when a world was added
		 */
		virtual void onWorldCreated(Handle::WorldHandle world);
		virtual void onWorldRemoved(Handle::WorldHandle world){};

		/**
		 * Update-method for subclasses
		 */
		virtual void onFrameUpdate(double dt, uint16_t width, uint16_t height)=0;

		/**
		 * @brief Called to load archives into the main VDFS-Index
		 *		  Overwrite to load your own default archives
		 */
		virtual void loadArchives();

        /**
         *
         */
        Daedalus::GameType m_BasicGameType;

		/**
		 * Currently active world instances
		 */
		std::list<std::unique_ptr<World::WorldInstance>> m_WorldInstances;

        /**
         * Main world of this engine-instance
         */
        Handle::WorldHandle m_MainWorld;

		/**
		 * Main VDFS-Index
		 */
		VDFS::FileIndex m_FileIndex;

		/**
		 * Registered worlds
		 */
		std::vector<Handle::WorldHandle> m_Worlds;

        /**
         * ingame clock
         */
        GameClock m_GameClock;

        /**
         * ingame clock
         */
        UI::Console m_Console;

        /**
         * Arguments
         */
        EngineArgs m_Args;

        Audio::AudioEngine *m_AudioEngine = nullptr;

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
         * Global speed factor. affects all instances (world (ergo animations), ingame clock (ergo sky))
         */
		float m_GameEngineSpeedFactor;

		/**
         * if the engine is paused. When it is paused the world doesn't receive the delta time updates
         */
		bool m_Paused;

        /**
         * save/load action queue
         */
		std::queue<Engine::SavegameManager::SaveGameAction> m_SaveGameActionQueue;
        struct AsyncAction
        {
            AsyncAction(std::shared_future<void> job) :
                job(job)
            {
            }
            std::function<void(BaseEngine& engine)> prolog;
            std::shared_future<void> job;
            std::function<void(BaseEngine& engine)> epilog;
        };
		std::queue<AsyncAction> m_AsyncActionQueue;

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
		ExcludeFrameTime(BaseEngine& baseEngine, bool enabled = true) :
			m_Engine(baseEngine),
            m_Enabled(enabled),
			m_Start(bx::getHPCounter())
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
