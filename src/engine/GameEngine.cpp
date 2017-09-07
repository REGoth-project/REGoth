//
// Created by andre on 30.05.16.
//

#include "GameEngine.h"
#include <common.h>
#include <bx/commandline.h>
#include <components/EntityActions.h>
#include <components/Vob.h>
#include <entry/input.h>
#include <logic/CameraController.h>
#include <render/RenderSystem.h>
#include <render/WorldRender.h>
#include <utils/logger.h>
#include <glfw/include/GLFW/glfw3.h>

using namespace Engine;

const float DRAW_DISTANCE = 100.0f;

GameEngine::GameEngine()
    : m_DefaultRenderSystem(*this)
{
}

GameEngine::~GameEngine()
{
}

void GameEngine::initEngine(int argc, char** argv)
{
    BaseEngine::initEngine(argc, argv);

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
            getGameClock().update(dt);
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
        view = Components::Actions::Position::makeViewMatrixFrom(getMainWorld().get().getComponentAllocator(),
                                                                 getMainWorld().get().getCamera());
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
    m_DefaultRenderSystem.getConfig().state.drawDistanceSquared =
            DRAW_DISTANCE * DRAW_DISTANCE;  // TODO: Config for these kind of variables
    m_DefaultRenderSystem.getConfig().state.farPlane = farPlane;
    m_DefaultRenderSystem.getConfig().state.viewWidth = width;
    m_DefaultRenderSystem.getConfig().state.viewHeight = height;
    m_DefaultRenderSystem.getConfig().state.cameraProj = proj;

    bgfx::touch(0);

    // Draw only main world
    if (getMainWorld().isValid())
    {
        static unsigned int frame = 0;
        frame++;

        static Math::Matrix viewProj;
        bool assign = !Input::getKeysState()[GLFW_KEY_G];

        if(assign)
            viewProj = Math::Matrix(proj) * view;

        getMainWorld().get().getBspTree().markNodesVisibleNow(viewProj, frame);
        Render::drawWorld(getMainWorld().get(), m_DefaultRenderSystem.getConfig(), m_DefaultRenderSystem);
    }
    //bgfx::frame();
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
