//
// Created by andre on 02.06.16.
//

#include "CameraController.h"
#include <entry/input.h>
#include <engine/World.h>

Logic::CameraController::CameraController(World::WorldInstance& world, Handle::EntityHandle entity)
    : Controller(world, entity),
      m_Active(true),
      m_CameraMode(ECameraMode::FirstPerson)
{
    memset(&m_CameraSettings, 0, sizeof(m_CameraSettings));

    m_CameraSettings.freeCameraSettings.moveSpeed = 0.01f;
    m_CameraSettings.freeCameraSettings.turnSpeed = 0.2f;

	m_CameraSettings.freeCameraSettings.position = Math::float3(0.0f, 2.0f, -4.0f);

    // init viewer
    m_CameraSettings.viewerCameraSettings.lookAt = Math::float3(0,0,0);
    m_CameraSettings.viewerCameraSettings.yaw = 0.0f;
    m_CameraSettings.viewerCameraSettings.pitch = 0.0f;
    m_CameraSettings.viewerCameraSettings.zoom = 30.0f;

    float mouseState[3];
    inputGetMouse(mouseState);
    m_LastMousePosition = Math::float2(mouseState[0], mouseState[1]);

    //inputSetMouseLock(true);
}

void Logic::CameraController::onUpdate(float deltaTime)
{
    if(!m_Active)
        return; // TODO: Should do automatic movement anyways!

    float turnSpeed = m_CameraSettings.freeCameraSettings.turnSpeed;
    float moveSpeed = m_CameraSettings.freeCameraSettings.moveSpeed;

    if(inputGetKeyState(entry::Key::Key0))
        m_CameraMode = ECameraMode::FirstPerson;
    else if(inputGetKeyState(entry::Key::Key1))
        m_CameraMode = ECameraMode::Free;
    else if(inputGetKeyState(entry::Key::Key2))
        m_CameraMode = ECameraMode::Viewer;

    if(inputGetKeyState(entry::Key::Space))
        moveSpeed *= 2.0f;

    switch(m_CameraMode)
    {
        case ECameraMode::FirstPerson:
        {
            /*float mouseState[3];
            inputGetMouse(mouseState);
            m_CameraSettings.freeCameraSettings.yaw += (m_LastMousePosition.x - mouseState[0]) * 0.5f;
            m_CameraSettings.freeCameraSettings.pitch += (m_LastMousePosition.y - mouseState[1]) * 0.5f;

            m_LastMousePosition = Math::float2(mouseState[0], mouseState[1]);*/

            // Direction
            if (inputGetKeyState(entry::Key::Left))
            {
                m_CameraSettings.freeCameraSettings.yaw -= deltaTime * turnSpeed;
            } else if (inputGetKeyState(entry::Key::Right))
            {
                m_CameraSettings.freeCameraSettings.yaw += deltaTime * turnSpeed;
            }

            if (inputGetKeyState(entry::Key::Up))
            {
                m_CameraSettings.freeCameraSettings.pitch += deltaTime * turnSpeed;
            } else if (inputGetKeyState(entry::Key::Down))
            {
                m_CameraSettings.freeCameraSettings.pitch -= deltaTime * turnSpeed;
            }

            // Get forward/right vector
            std::pair<Math::float3, Math::float3> fr = getDirectionVectors(m_CameraSettings.freeCameraSettings.yaw,
                                                                           m_CameraSettings.freeCameraSettings.pitch);

            if (inputGetKeyState(entry::Key::KeyA))
            {
                m_CameraSettings.freeCameraSettings.position +=
                        deltaTime * fr.second * moveSpeed;
            } else if (inputGetKeyState(entry::Key::KeyD))
            {
                m_CameraSettings.freeCameraSettings.position -=
                        deltaTime * fr.second * moveSpeed;
            }
            if (inputGetKeyState(entry::Key::KeyW))
            {
                m_CameraSettings.freeCameraSettings.position +=
                        deltaTime * fr.first * moveSpeed;
            } else if (inputGetKeyState(entry::Key::KeyS))
            {
                m_CameraSettings.freeCameraSettings.position -=
                        deltaTime * fr.first * moveSpeed;
            }

            // Fix position
            Math::float3 to = m_CameraSettings.freeCameraSettings.position + Math::float3(0.0f, -100.0f, 0.0f);
            Math::float3 hit = m_World.getPhysicsSystem().raytrace(m_CameraSettings.freeCameraSettings.position, to);

            if (hit != to)
            {
                m_CameraSettings.freeCameraSettings.position = hit + Math::float3(0.0f, 1.8f, 0.0f);
            }

            m_ViewMatrix = Math::Matrix::CreateView(m_CameraSettings.freeCameraSettings.position,
                                                    m_CameraSettings.freeCameraSettings.yaw,
                                                    m_CameraSettings.freeCameraSettings.pitch);

            setEntityTransform(m_ViewMatrix.Invert());
        }
            break;
        case ECameraMode::Free:
        {
            // Direction
            if (inputGetKeyState(entry::Key::Left))
            {
                m_CameraSettings.freeCameraSettings.yaw -= deltaTime * turnSpeed;
            } else if (inputGetKeyState(entry::Key::Right))
            {
                m_CameraSettings.freeCameraSettings.yaw += deltaTime * turnSpeed;
            }

            if (inputGetKeyState(entry::Key::Up))
            {
                m_CameraSettings.freeCameraSettings.pitch += deltaTime * turnSpeed;
            } else if (inputGetKeyState(entry::Key::Down))
            {
                m_CameraSettings.freeCameraSettings.pitch -= deltaTime * turnSpeed;
            }

            // Get forward/right vector
            std::pair<Math::float3, Math::float3> fr = getDirectionVectors(m_CameraSettings.freeCameraSettings.yaw,
                                                                           m_CameraSettings.freeCameraSettings.pitch );

            if (inputGetKeyState(entry::Key::KeyA))
            {
                m_CameraSettings.freeCameraSettings.position += deltaTime * fr.second * moveSpeed;
            } else if (inputGetKeyState(entry::Key::KeyD))
            {
                m_CameraSettings.freeCameraSettings.position -= deltaTime * fr.second * moveSpeed;
            }
            if (inputGetKeyState(entry::Key::KeyW))
            {
                m_CameraSettings.freeCameraSettings.position += deltaTime * fr.first * moveSpeed;
            } else if (inputGetKeyState(entry::Key::KeyS))
            {
                m_CameraSettings.freeCameraSettings.position -= deltaTime * fr.first * moveSpeed;
            }

            m_ViewMatrix = Math::Matrix::CreateView(m_CameraSettings.freeCameraSettings.position,
                                                    m_CameraSettings.freeCameraSettings.yaw,
                                                    m_CameraSettings.freeCameraSettings.pitch);

            setEntityTransform(m_ViewMatrix.Invert());
        }
        break;
        case ECameraMode::Viewer:
        {
            auto &settings = m_CameraSettings.viewerCameraSettings;

            // I do not know how to properly retrieve that input. change at will.
            //bool shift = inputGetModifiersState() & entry::Modifier::LeftShift;
            //bool ctrl = inputGetModifiersState() & entry::Modifier::LeftCtrl;
            bool shift  = inputGetKeyState(entry::Key::KeyQ); // should be left shift
            bool ctrl   = inputGetKeyState(entry::Key::KeyE); // should be left ctrl
            bool mmb    = inputGetKeyState(entry::Key::KeyX); // should be middle mouse button
            bool lmb    = inputGetKeyState(entry::Key::KeyY); // should be left mouse button


            float mouseState[3];
            inputGetMouse(mouseState);
            if(mouseState[0] != 0.0f && mouseState[1] != 0.0f)
                settings.mousePosition = Math::float2(mouseState[0], mouseState[1]);

            if(mmb)
            {
                Math::float2 translationVector = settings.mousePosition - settings.oldMousePosition;

                if( shift == ctrl ) // neither pan nor zoom activated, or both -> rotate
                {
                    settings.yaw   = settings.oldYaw    + 5.0f * translationVector.x;
                    settings.pitch = settings.oldPitch  + 5.0f * translationVector.y;
                }
                else if(shift) // pan view
                {
                    settings.lookAt = settings.oldLookAt
                            - 50.0f * settings.right * translationVector.x + 50.0f * settings.up * translationVector.y;
                }
                else if(ctrl) // zoom
                {
                    settings.zoom = settings.oldZoom + 100.0f * translationVector.y;
                    if(settings.zoom < 1.0f)
                        settings.zoom = 1.0f;
                }
            }
            else
            {
                settings.oldYaw             = settings.yaw;
                settings.oldPitch           = settings.pitch;
                settings.oldZoom            = settings.zoom;
                settings.oldLookAt          = settings.lookAt;
                settings.oldMousePosition   = settings.mousePosition;
            }

            static uint32_t lmbFrames = 30;
            // execute this function only once in 30 frames for better usability of the tool
            // this is a very bad solution. better set the mouse in the center of the frame
            if(lmb && lmbFrames >= 30)
            {
                lmbFrames = 0;

                const float maxRayLength = 1000.0f;

                // these relations are found by trial and error
                Math::float3 cameraSpaceRayEndpoint = maxRayLength * Math::float3( (2.0f * settings.mousePosition.x - 1.0f), (0.5f - settings.mousePosition.y), 1.0f );
                Math::float3 to = m_ViewMatrix.Invert() * cameraSpaceRayEndpoint;
                Math::float3 from = settings.lookAt + settings.zoom * settings.in;
                Math::float3 hit = m_World.getPhysicsSystem().raytrace(from, to);

                if(hit != to)
                {
                    settings.lookAt = hit;
                    Math::float3 distance = from - hit;
                    settings.zoom = distance.length();
                    settings.pitch = asin(distance.y/settings.zoom);
                    settings.yaw = atan2(distance.x, distance.z);
                }
            }
            lmbFrames++;

            // getDirectionVectors only returns 2 of 3 direction vectors
            std::tie(settings.in, settings.right) = getDirectionVectors(settings.yaw, settings.pitch);
            settings.up = settings.right.cross(settings.in);

            m_ViewMatrix = Math::Matrix::CreateLookAt(
                        settings.lookAt + settings.zoom * settings.in, settings.lookAt, settings.up);
            setEntityTransform(m_ViewMatrix.Invert());
        }
        break;
    }
}

std::pair<Math::float3, Math::float3> Logic::CameraController::getDirectionVectors(float yaw, float pitch)
{
    Math::float3 direction(
            cos(pitch) * sin(yaw),
            sin(pitch),
            cos(pitch) * cos(yaw)
    );

    // Right vector
    Math::float3 right(
            sin(yaw - 3.14f / 2.0f),
            0,
            cos(yaw - 3.14f / 2.0f)
    );

    return std::make_pair(direction.normalize(), right.normalize());
}
