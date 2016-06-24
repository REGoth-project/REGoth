//
// Created by andre on 30.05.16.
//

#include "GameEngine.h"
#include <render/WorldRender.h>
#include <common.h>
#include <entry/input.h>
#include <components/EntityActions.h>
#include <logic/CameraController.h>
#include <render/RenderSystem.h>

using namespace Engine;

const float DRAW_DISTANCE = 100.0f;

GameEngine::GameEngine()
{

}

GameEngine::~GameEngine()
{

}

void GameEngine::initEngine(int argc, char** argv)
{
    BaseEngine::initEngine(argc, argv);

    // Initialize main render system
    m_DefaultRenderSystem.init();
}

void GameEngine::onFrameUpdate(double dt, uint16_t width, uint16_t height)
{
    for(auto s : m_Worlds)
        m_WorldInstances.getElement(s).onFrameUpdate(dt, DRAW_DISTANCE * DRAW_DISTANCE, getMainCamera<Components::PositionComponent>().m_WorldMatrix);

    drawFrame(width, height);
}

void GameEngine::drawFrame(uint16_t width, uint16_t height)
{
    Math::Matrix view = Components::Actions::Position::makeViewMatrixFrom(getMainWorld().getComponentAllocator(), m_MainCamera);

    // Set view and projection matrix for view 0.
    const bgfx::HMD* hmd = bgfx::getHMD();
    if (NULL != hmd && 0 != (hmd->flags & BGFX_HMD_RENDERING))
    {
        float view[16];
        bx::mtxQuatTranslationHMD(view, hmd->eye[0].rotation, getMainCamera<Components::PositionComponent>().m_WorldMatrix.Translation().v);
        bgfx::setViewTransform(0, view, hmd->eye[0].projection, BGFX_VIEW_STEREO, hmd->eye[1].projection);

        // Set view 0 default viewport.
        //
        // Use HMD's width/height since HMD's internal frame buffer size
        // might be much larger than window size.
        bgfx::setViewRect(0, 0, 0, hmd->width, hmd->height);
    }
    else
    {
        float proj[16];
        bx::mtxProj(proj, 60.0f, float(width) / float(height), 0.1f, 10000.0f);
        bgfx::setViewTransform(0, view.mv, proj);

        // Set view 0 default viewport.
        bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));

        // Update the frame-config with the cameras world-matrix
        m_DefaultRenderSystem.getConfig().state.cameraWorld = getMainCamera<Components::PositionComponent>().m_WorldMatrix;
        m_DefaultRenderSystem.getConfig().state.drawDistanceSquared = DRAW_DISTANCE * DRAW_DISTANCE; // TODO: Config for these kind of variables
    }


    bgfx::touch(0);

    // Draw all worlds
    for(auto s : m_Worlds)
        Render::drawWorld(m_WorldInstances.getElement(s), m_DefaultRenderSystem.getConfig());

    //bgfx::frame();
}

void GameEngine::onWorldCreated(Handle::WorldHandle world)
{
    BaseEngine::onWorldCreated(world);

    if(!m_MainWorld.isValid())
    {
        m_MainWorld = world;
    }

    if(!m_MainCamera.isValid())
    {
        createMainCameraIn(m_MainWorld);
    }
}

Handle::EntityHandle GameEngine::createMainCameraIn(Handle::WorldHandle world)
{
    World::WorldInstance& winst = m_WorldInstances.getElement(world);
    // Add player-camera
    m_MainCamera = winst.addEntity(Components::PositionComponent::MASK);

	Math::Matrix lookAt = Math::Matrix::CreateLookAt(Math::float3(0,0,0), Math::float3(1,0,0), Math::float3(0,1,0));
    getMainCamera<Components::PositionComponent>().m_WorldMatrix
            = lookAt.Invert();

    Components::LogicComponent& logic = Components::Actions::initComponent<Components::LogicComponent>(
            winst.getComponentAllocator(),
            m_MainCamera);

    Logic::CameraController* cam = new Logic::CameraController(winst, m_MainCamera);
    logic.m_pLogicController = cam;

    cam->setCameraMode(Logic::CameraController::ECameraMode::Free);
    cam->getCameraSettings().freeCameraSettings.moveSpeed = 20.0f;
    cam->getCameraSettings().freeCameraSettings.turnSpeed = 5.0f;

	return m_MainCamera;
}






