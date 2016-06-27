//
// Created by andre on 02.06.16.
//

#include "CameraController.h"
#include <entry/input.h>

Logic::CameraController::CameraController(World::WorldInstance& world, Handle::EntityHandle entity)
    : Controller(world, entity),
      m_Active(true),
      m_CameraMode(ECameraMode::Free)
{
    memset(&m_CameraSettings, 0, sizeof(m_CameraSettings));

    m_CameraSettings.freeCameraSettings.moveSpeed = 1.0f;
    m_CameraSettings.freeCameraSettings.turnSpeed = 0.7f;

	m_CameraSettings.freeCameraSettings.position = Math::float3(0.0f, 2.0f, -4.0f);
}

void Logic::CameraController::onUpdate(float deltaTime)
{
    if(!m_Active)
        return; // TODO: Should do automatic movement anyways!

    switch(m_CameraMode)
    {
        case ECameraMode::Free:
        {
            // Direction
            if (inputGetKeyState(entry::Key::Left))
            {
                m_CameraSettings.freeCameraSettings.yaw -= deltaTime * m_CameraSettings.freeCameraSettings.turnSpeed;
            } else if (inputGetKeyState(entry::Key::Right))
            {
                m_CameraSettings.freeCameraSettings.yaw += deltaTime * m_CameraSettings.freeCameraSettings.turnSpeed;
            }

            if (inputGetKeyState(entry::Key::Up))
            {
                m_CameraSettings.freeCameraSettings.pitch += deltaTime * m_CameraSettings.freeCameraSettings.turnSpeed;
            } else if (inputGetKeyState(entry::Key::Down))
            {
                m_CameraSettings.freeCameraSettings.pitch -= deltaTime * m_CameraSettings.freeCameraSettings.turnSpeed;
            }

            // Get forward/right vector
            std::pair<Math::float3, Math::float3> fr = getDirectionVectors(m_CameraSettings.freeCameraSettings.yaw,
                                                                           m_CameraSettings.freeCameraSettings.pitch );

            if (inputGetKeyState(entry::Key::KeyA))
            {
                m_CameraSettings.freeCameraSettings.position += deltaTime * fr.second * m_CameraSettings.freeCameraSettings.moveSpeed;
            } else if (inputGetKeyState(entry::Key::KeyD))
            {
                m_CameraSettings.freeCameraSettings.position -= deltaTime * fr.second * m_CameraSettings.freeCameraSettings.moveSpeed;
            }
            if (inputGetKeyState(entry::Key::KeyW))
            {
                m_CameraSettings.freeCameraSettings.position += deltaTime * fr.first * m_CameraSettings.freeCameraSettings.moveSpeed;
            } else if (inputGetKeyState(entry::Key::KeyS))
            {
                m_CameraSettings.freeCameraSettings.position -= deltaTime * fr.first * m_CameraSettings.freeCameraSettings.moveSpeed;
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