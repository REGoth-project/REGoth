//
// Created by andre on 30.05.16.
//

#include "GameEngine.h"
#include <engine/GameSession.h>
#include <engine/AsyncAction.h>
#include <engine/GameClock.h>
#include <audio/AudioEngine.h>
#include <common.h>
#include <bx/commandline.h>
#include <components/EntityActions.h>
#include <components/Vob.h>
#include <entry/input.h>
#include <logic/CameraController.h>
#include <ui/Hud.h>
#include <ui/LoadingScreen.h>
#include <ui/zFont.h>
#include <render/RenderSystem.h>
#include <render/WorldRender.h>
#include <utils/logger.h>

using namespace Engine;

const float DRAW_DISTANCE = 100.0f;

GameEngine::GameEngine()
    : m_DefaultRenderSystem(*this)
    , m_RootUIView(*this)
    , m_EngineTextureAlloc(*this)
    , m_MainThreadID(std::this_thread::get_id())
    , m_Console(*this)
{
    m_pHUD = nullptr;
    m_pFontCache = nullptr;
    resetSession();
}

GameEngine::~GameEngine()
{
    getRootUIView().removeChild(m_pHUD);
    delete m_AudioEngine;
    delete m_pHUD;
    delete m_pFontCache;
}

void GameEngine::initEngine(int argc, char** argv)
{
    BaseEngine::initEngine(argc, argv);

    std::string snd_device;
    if (Flags::sndDevice.isSet())
        snd_device = Flags::sndDevice.getParam(0);

    m_AudioEngine = new Audio::AudioEngine(snd_device);

    // Init HUD
    m_pFontCache = new UI::zFontCache(*this);
    m_pHUD = new UI::Hud(*this);
    getRootUIView().addChild(m_pHUD);

    bx::CommandLine cmdLine(argc, (const char**)argv);
    const char* value = nullptr;

    // Initialize main render system
    m_DefaultRenderSystem.init();

    // Load a test-visual
    if (cmdLine.hasArg('v'))
    {
        value = cmdLine.findOption('v');

        if (value)
        {
            m_Args.testVisual = value;

            if (m_Args.testVisual.find(".MDM") != std::string::npos)
                m_Args.testVisual += "S";
        }
    }
}

void GameEngine::frameUpdate(double dt, uint16_t width, uint16_t height)
{
    onFrameUpdate(dt * getSession().getGameClock().getGameEngineSpeedFactor(), width, height);
}

void GameEngine::onFrameUpdate(double dt, uint16_t width, uint16_t height)
{
    // Debug only
    //    static bool lastLogicDisableKeyState = false;
    //    static bool disableLogic = false;

    //    if(inputGetKeyState(entry::Key::Key2) != lastLogicDisableKeyState)
    //    {
    //        if(!lastLogicDisableKeyState)
    //            disableLogic = !disableLogic;

    //        lastLogicDisableKeyState = inputGetKeyState(entry::Key::Key2);
    //    }
    if (!getSession().getWorldInstances().empty())
    {
        if (m_Paused)
        {
            getMainWorld().get().getCameraController()->onUpdate(dt);
        }
        else
        {
            getSession().getGameClock().update(dt);
            for (auto& s : getSession().getWorldInstances())
            {
                // Update main-world after every other world, since the camera is in there
                s->onFrameUpdate(dt, DRAW_DISTANCE * DRAW_DISTANCE, s->getCameraComp<Components::PositionComponent>().m_WorldMatrix);
            }

            // Finally, update main camera
            getMainWorld().get().getCameraController()->onUpdateExplicit(dt);
        }
    }
    drawFrame(width, height);
}

void GameEngine::drawFrame(uint16_t width, uint16_t height)
{
    Math::Matrix view;
    if (getMainWorld().isValid())
        view = Components::Actions::Position::makeViewMatrixFrom(getMainWorld().get().getComponentAllocator(), getMainWorld().get().getCamera());
    else
        view = Math::Matrix::CreateIdentity();

    // Set view and projection matrix for view 0.
    float farPlane = 1000.0f;

    float proj[16];
    bx::mtxProj(proj, 60.0f, float(width) / float(height), 0.1f, farPlane);

    // Set for every view
    for (uint8_t i = 0; i < 255; i++)
    {
        bgfx::setViewTransform(i, view.mv, proj);

        // Set view default viewport.
        bgfx::setViewRect(i, 0, 0, uint16_t(width), uint16_t(height));
    }

    // Update the frame-config with the cameras world-matrix
    if (getMainWorld().isValid())
        m_DefaultRenderSystem.getConfig().state.cameraWorld = getMainWorld().get().getCameraComp<Components::PositionComponent>().m_WorldMatrix;
    m_DefaultRenderSystem.getConfig().state.drawDistanceSquared = DRAW_DISTANCE * DRAW_DISTANCE;  // TODO: Config for these kind of variables
    m_DefaultRenderSystem.getConfig().state.farPlane = farPlane;
    m_DefaultRenderSystem.getConfig().state.viewWidth = width;
    m_DefaultRenderSystem.getConfig().state.viewHeight = height;

    bgfx::touch(0);

    // Draw only main world
    if (getMainWorld().isValid())
        Render::drawWorld(getMainWorld().get(), m_DefaultRenderSystem.getConfig(), m_DefaultRenderSystem);

    //bgfx::frame();
}

void GameEngine::executeInMainThread(const AsyncAction::JobType<void>& job, bool forceQueue)
{
    auto wrappedJob = [job](Engine::GameEngine* engine) -> bool {
        job(engine);
        return true;
    };
    executeInMainThreadUntilTrue(wrappedJob, forceQueue);
}

void GameEngine::executeInMainThreadUntilTrue(const AsyncAction::JobType<bool>& job, bool forceQueue)
{
    if (!forceQueue && isMainThread())
    {
        // execute right away
        bool success = job(this);
        if (success)
            return;
        // else job returned false -> queue the job
    }
    std::lock_guard<std::mutex> guard(m_MessageQueueMutex);
    m_MessageQueue.emplace_back(AsyncAction{job});
}

void GameEngine::processMessageQueue()
{
    m_MessageQueueMutex.lock();
    auto current = m_MessageQueue.begin();
    while (current != m_MessageQueue.end())
    {
        auto& action = *current;
        // if the job queues a new job it would create a deadlock, so we need to release the mutex before
        m_MessageQueueMutex.unlock();
        bool finished = action.run(*this);
        m_MessageQueueMutex.lock();
        if (finished)
            current = m_MessageQueue.erase(current);  // erase returns next iterator
        else
            std::advance(current, 1);
    }
    m_MessageQueueMutex.unlock();
}

bool GameEngine::isMainThread()
{
    return std::this_thread::get_id() == m_MainThreadID;
}

void GameEngine::resetSession()
{
    // GameSession's destructor will clean up worlds
    m_Session = std::make_unique<GameSession>(*this);
}

Handle::WorldHandle GameEngine::getMainWorld()
{
    return getSession().getMainWorld();
}

void GameEngine::setPaused(bool paused)
{
    if (paused != m_Paused)
    {
        if (getMainWorld().isValid())
        {
            if (paused)
                getMainWorld().get().getAudioWorld().pauseSounds();
            else
                getMainWorld().get().getAudioWorld().continueSounds();
        }
        m_Paused = paused;
    }
}

GameClock& GameEngine::getGameClock()
{
    return getSession().getGameClock();
}

void GameEngine::onWorldCreated(Handle::WorldHandle world)
{
    BaseEngine::onWorldCreated(world);

    if (world.isValid())
    {
        world.get().createCamera();
    }
}

void GameEngine::onWorldRemoved(Handle::WorldHandle world)
{
    BaseEngine::onWorldRemoved(world);
}
