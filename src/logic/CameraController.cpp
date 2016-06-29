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