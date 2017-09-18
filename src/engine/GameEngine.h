#pragma once
#include "BaseEngine.h"
#include "GameSession.h"
#include "GameClock.h"
#include <future>
#include "Input.h"
#include <logic/CameraController.h>
#include <logic/Console.h>
#include <render/RenderSystem.h>

namespace Engine
{
    class GameEngine : public BaseEngine
    {
    public:
        GameEngine();
        ~GameEngine();

        /**
         * @brief Initializes the Engine
         */
        virtual void initEngine(int argc, char** argv) override;

        /**
         * @return Default set of shaders and constants
         */
        Render::RenderSystem& getDefaultRenderSystem()
        {
            return m_DefaultRenderSystem;
        }

        /**
         * executes all jobs in the queue and removes the ones, that return true
         */
        void processMessageQueue();

        /**
         * Guarantees execution of the given function in the main thread
         * @param job function to execute in the main thread
         * @param forceQueueing if false AND if called from main thread: executes the job right away
         * 		  instead of queueing and does not acquire the lock.
         */
        void executeInMainThread(const std::function<void(GameEngine* engine)>& job, bool forceQueueing = false);

        /**
         * Execute the given job on the main thread one time per frame update until it returns true
         */
        void executeInMainThreadUntilTrue(const std::function<bool(GameEngine* engine)>& job,
                                          bool forceQueueing = false);

        /**
         * @return Console
         */
        Logic::Console& getConsole() { return m_Console; }

        /**
         * @return Base-level UI-View. Parent of all other views.
         */
        UI::View& getRootUIView() { return m_RootUIView; }

        /**
         * @return Allocator for always present textures
         */
        Textures::TextureAllocator& getEngineTextureAlloc() { return m_EngineTextureAlloc; }

        /**
         * information stored in a session is cleared when loading a savegame or starting a new game
         * @return GameSession
         */
        GameSession& getSession() { return *m_Session; }

        /**
         * @return Gameclock
         */
        GameClock& getGameClock();

        /**
         * drop all information bound to the current session
         */
        void resetSession();

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
         * @return data-access to the main world
         */
        Handle::WorldHandle getMainWorld();

        /**
         * @return HUD
         */
        UI::Hud& getHud() { return *m_pHUD; }
        UI::zFontCache& getFontCache() { return *m_pFontCache; }
        Audio::AudioEngine& getAudioEngine() { return *m_AudioEngine; }

        /**
         * @return true if the calling thread is the main thread
         */
        bool isMainThread();

        /**
         * @brief Frame update // TODO: Remove width and height
         */
        void frameUpdate(double dt, uint16_t width, uint16_t m_height);

        /**
         * Called when a world was added
         */
        virtual void onWorldCreated(Handle::WorldHandle world) override;
        virtual void onWorldRemoved(Handle::WorldHandle world) override;

    protected:

        /**
         * Update-method for subclasses
         */
        void onFrameUpdate(double dt, uint16_t width, uint16_t height) override;

        /**
         * Draws the worlds and presents the frame
         */
        void drawFrame(uint16_t width, uint16_t height);

        /**
         * Default rendering system
         */
        Render::RenderSystem m_DefaultRenderSystem;

        Audio::AudioEngine* m_AudioEngine = nullptr;

        /**
         * ingame console
         */
        Logic::Console m_Console;

        /**
         * Game session, stores information that should be reset on starting a new game/loading
         * unique_ptr is used, because we can't overwrite the session itself,
         * because it has a member variable reference
         */
        std::unique_ptr<GameSession> m_Session;

        /**
         * Gameclock
         */
        GameClock m_GameClock;

        /**
         * Base UI-View
         */
        UI::View m_RootUIView;
        UI::Hud* m_pHUD;
        UI::zFontCache* m_pFontCache;

        /**
         * ID of the main thread (bgfx thread)
         */
        std::thread::id m_MainThreadID;
        std::list<AsyncAction> m_MessageQueue;
        std::mutex m_MessageQueueMutex;

        /**
         * Allocator for always present textures
         */
        Textures::TextureAllocator m_EngineTextureAlloc;
    };
}
