#pragma once
#include <future>
#include "World.h"
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

    enum class ExecutionPolicy
    {
        MainThread,
        NewThread
    };

    template <class ReturnType>
    using JobType = std::function<ReturnType(BaseEngine* engine)>;

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
         * increase the time, which the current frame should not treat as elapsed
         */
        void addToExludedFrameTime(int64_t milliseconds) { m_ExcludedFrameTime += milliseconds; };
        int64_t getExludedFrameTime() { return m_ExcludedFrameTime; };
        void resetExludedFrameTime() { m_ExcludedFrameTime = 0; };
        /**
         * @return true if the calling thread is the main thread
         */
        bool isMainThread();

        /**
         * Guarantees execution of the given function in the main thread at frame-end
         * May be called from any thread
         * @param job function to execute in the main thread
         */
        void queueMainThreadJob(std::function<void(BaseEngine*)> job);

        /**
         * executes all jobs in the queue
         */
        void processMessageQueue();

        /**
         * Executes the job in the specified thread
         * May be called from any thread
         * @param job the job to be executed
         * @param executionPolicy defines which thread should execute the job
         * @return future, which contains the result
         */
        template <class ReturnType=void>
        std::future<ReturnType> executeInThread(JobType<ReturnType> job, ExecutionPolicy policy);

        template <class ReturnType=void>
        std::future<ReturnType> executeInMainThread(JobType<ReturnType> job)
        {
            return executeInThread(std::move(job), ExecutionPolicy::MainThread);
        }

        /**
         * Called when a world was added
         * TODO onWorld... should be protected, but currently need to call this function from GameSession
         */
        virtual void onWorldCreated(Handle::WorldHandle world);
        virtual void onWorldRemoved(Handle::WorldHandle world){};

    protected:
        /**
         * wraps the job into a new functor, that puts the job's return value into the promise
         */
        template <class ReturnType>
        JobType<void> wrapJob(JobType<ReturnType> job, std::shared_ptr<std::promise<ReturnType>> promise);

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
         * ID of the main thread (bgfx thread), must be initialized first
         */
        std::thread::id m_MainThreadID;

        /**
         * Whether multi-threading is enabled
         */
        bool m_EnableMultiThreading;

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

        /**
         * list of jobs to be executed at frame-end
         */
        std::list<std::function<void(BaseEngine*)>> m_MessageQueue;
        std::mutex m_MessageQueueMutex;

        /**
         * futures created by std::async block in their destructors
         * that's why they are kept alive in this list
         */
        std::list<std::shared_future<void>> m_AsyncJobs;
        std::mutex m_AsyncJobsMutex;

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

namespace Engine
{
    template <class ReturnType>
    inline JobType<void> BaseEngine::wrapJob(JobType<ReturnType> job, std::shared_ptr<std::promise<ReturnType>> promise)
    {
        return [promise, job = std::move(job)](BaseEngine* engine){
            promise->set_value(job(engine));
        };
    }

    // template specialization needed, because of promise<void>::set_value() specialization
    template <>
    inline JobType<void> BaseEngine::wrapJob(JobType<void> job, std::shared_ptr<std::promise<void>> promise)
    {
        return [promise, job = std::move(job)](BaseEngine* engine){
            job(engine);
            promise->set_value();
        };
    }

    template <class ReturnType>
    inline std::future<ReturnType> BaseEngine::executeInThread(JobType<ReturnType> job, ExecutionPolicy policy)
    {
        if (!m_EnableMultiThreading && policy == ExecutionPolicy::NewThread)
            policy = ExecutionPolicy::MainThread;

        std::shared_ptr<std::promise<ReturnType>> promise = std::make_shared<std::promise<ReturnType>>();
        std::future<ReturnType> waitableFuture = promise->get_future();

        auto wrappedJob = wrapJob(std::move(job), promise);

        switch (policy)
        {
            case ExecutionPolicy::MainThread:
                if (isMainThread())
                    wrappedJob(this); // if caller is main thread, execute right away
                else
                    queueMainThreadJob(std::move(wrappedJob));
                break;
            case ExecutionPolicy::NewThread:
            {
                std::future<void> keepAliveFuture = std::async(std::launch::async, std::move(wrappedJob), this);
                std::lock_guard<std::mutex> guard(m_AsyncJobsMutex);
                m_AsyncJobs.push_back(std::move(keepAliveFuture));
            }
                break;
        }
        return waitableFuture;
    }
}