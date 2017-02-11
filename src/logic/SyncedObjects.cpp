#include "SyncedObjects.h"
#include <engine/World.h>
#include <components/VobClasses.h>
#include <logic/PlayerController.h>
#include <debugdraw/debugdraw.h>

const Net::PlayerState::object_type_id_type Net::PlayerState::type_id = 1001;

/**
 * Max allowed distance difference of server and client state for the players position (in meters)
 */
const float MAX_ALLOWED_POSITION_DIVERGENCE = 1.0f;

namespace Net
{
    extern bool isServer;
}

namespace sfn
{
    sfn::Message& operator<<( sfn::Message& message, const Logic::PlayerInput& input) 
    { 
        message << input.isForward;
        message << input.isBackward;
        message << input.isLeft;
        message << input.isRight;
        message << input.turnAngle;
        message << input.debugSpeedMod;

        return message; 
    }  

    sfn::Message& operator>>( sfn::Message& message, Logic::PlayerInput& output) 
    {
        message >> output.isForward;
        message >> output.isBackward;
        message >> output.isLeft;
        message >> output.isRight;
        message >> output.turnAngle;
        message >> output.debugSpeedMod;

        return message; 
    }

    sfn::Message& operator<<( sfn::Message& message, const Math::float3& input)
    {
        message << input.x;
        message << input.y;
        message << input.z;

        return message;
    }

    sfn::Message& operator>>( sfn::Message& message, Math::float3& output)
    {
        message >> output.x;
        message >> output.y;
        message >> output.z;

        return message;
    }

    sfn::Message& operator<<( sfn::Message& message, const Math::float2& input)
    {
        message << input.x;
        message << input.y;

        return message;
    }

    sfn::Message& operator>>( sfn::Message& message, Math::float2& output)
    {
        message >> output.x;
        message >> output.y;

        return message;
    }
}



void Net::PlayerState::onFrameUpdate(double deltatime, World::WorldInstance& world, bool localPlayer)
{
    if(!m_PlayerEntity.isValid())
        return;

    VobTypes::NpcVobInformation p = VobTypes::asNpcVob(world, m_PlayerEntity);

    if(p.isValid())
    {
        if(!localPlayer) // Input is smoother when not received from the server. We still set the position if it's way too off.
            p.playerController->onUpdateByInput((float)deltatime, m_Input.GetValue());

        if(!isServer)
        {
            // Put the player back to where he is supposed to be if position is way off
            Math::float3 ppos = p.playerController->getEntityTransform().Translation();
            Math::float3 pdir = p.playerController->getDirection();

            ddDrawAxis(m_Position.GetValue().x, m_Position.GetValue().y, m_Position.GetValue().z, 2.0f);

            if((ppos - m_Position).length() > MAX_ALLOWED_POSITION_DIVERGENCE)
                p.playerController->setPositionAndDirection(m_Position, m_Direction);
            else
                p.playerController->setDirection(m_Direction);
        }
    }    
}
