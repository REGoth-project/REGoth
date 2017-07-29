#include <engine/Input.h>
#include <components/Entities.h>
#include "NpcAIHandler.h"
#include <engine/World.h>
#include <logic/PlayerController.h>

using namespace Logic;

NpcAIHandler::~NpcAIHandler()
{
}

NpcAIHandler::NpcAIHandler(World::WorldInstance& world, Handle::EntityHandle hostVob)
    : m_World(world)
    , m_HostVob(hostVob)
{
    unbindKeys();
    m_ActiveMovementState = EMovementState::None;
}

void NpcAIHandler::update(float deltaTime)
{
}

void NpcAIHandler::standup()
{
    m_ActiveMovementState = EMovementState::None;
    getNpcAnimationHandler().Action_Stand(true);  // Force stand
}

void NpcAIHandler::playerUpdate(float deltaTime)
{
    if (!getController().getEM().isEmpty())
    {
        // While something else is playing, don't allow any movement state to be active
        m_ActiveMovementState = EMovementState::None;

        // Messages are being played, don't handle inputs
        return;
    }

    if (m_MovementState.isAction)
        bgfx::dbgTextPrintf(0, 6, 0x4, "Action");

    switch (m_ActiveMovementState)
    {
        case EMovementState::None:

            // Check for turns
            if (m_MovementState.isTurnLeft)
            {
                getNpcAnimationHandler().Action_TurnLeft();
            }
            else if (m_MovementState.isTurnRight)
            {
                getNpcAnimationHandler().Action_TurnRight();
            }

            getNpcAnimationHandler().Action_Stand(false, m_MovementState.isTurnLeft || m_MovementState.isTurnRight);

            // Only allow changing states when the character is standing (ie. not playing transition animations)
            if (getNpcAnimationHandler().isStanding(true))
            {
                if (m_MovementState.isLastWeaponKey)
                {
                    if (getController().getWeaponMode() == EWeaponMode::WeaponNone)
                    {
                        getController().drawWeaponMelee();
                        m_ActiveMovementState = EMovementState::DrawWeapon;
                    }
                    else
                    {
                        getController().undrawWeapon();
                        m_ActiveMovementState = EMovementState::UndrawWeapon;
                    }
                }
                else if (m_MovementState.isForward)
                {
                    if (m_MovementState.isAction && getController().getWeaponMode() != EWeaponMode::WeaponNone)
                    {
                        m_ActiveMovementState = EMovementState::FightForward;
                    }
                    else
                    {
                        m_ActiveMovementState = EMovementState::Forward;
                    }
                }
                else if (m_MovementState.isBackward)
                {
                    m_ActiveMovementState = EMovementState::Backward;
                }
                else if (m_MovementState.isStrafeLeft)
                {
                    m_ActiveMovementState = EMovementState::StrafeLeft;
                }
                else if (m_MovementState.isStrafeRight)
                {
                    m_ActiveMovementState = EMovementState::StrafeRight;
                }
            }
            break;

        case EMovementState::Forward:
            bgfx::dbgTextPrintf(0, 5, 0x4, "Forward");
            if (m_MovementState.isForward)
            {
                // Forward-key still pressed, keep going
                if (!getNpcAnimationHandler().Action_GoForward())
                {
                    // No Space, go back to "standing"
                    m_ActiveMovementState = EMovementState::None;
                    getNpcAnimationHandler().Action_Stand();
                }
                else
                {
                    // And check for turns
                    if (m_MovementState.isTurnLeft)
                    {
                        getNpcAnimationHandler().Action_TurnLeft();
                    }
                    else if (m_MovementState.isTurnRight)
                    {
                        getNpcAnimationHandler().Action_TurnRight();
                    }
                }
            }
            else
            {
                // Forward-key not pressed anymore, go back to "standing"
                m_ActiveMovementState = EMovementState::None;
                getNpcAnimationHandler().Action_Stand();  // FIXME: Shouldn't need to force stand here (and in the other states)
            }
            break;

        case EMovementState::Backward:
            bgfx::dbgTextPrintf(0, 5, 0x4, "Backward");
            if (m_MovementState.isBackward)
            {
                // Backward-key still pressed, keep going
                getNpcAnimationHandler().Action_GoBackward();

                // And check for turns
                if (m_MovementState.isTurnLeft)
                {
                    getNpcAnimationHandler().Action_TurnLeft();
                }
                else if (m_MovementState.isTurnRight)
                {
                    getNpcAnimationHandler().Action_TurnRight();
                }
            }
            else
            {
                // Backward-key not pressed anymore, go back to "standing"
                m_ActiveMovementState = EMovementState::None;
                getNpcAnimationHandler().Action_Stand();
            }
            break;

        case EMovementState::StrafeLeft:
            if (m_MovementState.isStrafeLeft)
            {
                // Strafe-key still pressed, keep going
                getNpcAnimationHandler().Action_StrafeLeft();

                // And check for turns
                if (m_MovementState.isTurnLeft)
                {
                    getNpcAnimationHandler().Action_TurnLeft();
                }
                else if (m_MovementState.isTurnRight)
                {
                    getNpcAnimationHandler().Action_TurnRight();
                }
            }
            else
            {
                // Strafe-key not pressed anymore, go back to "standing"
                m_ActiveMovementState = EMovementState::None;
                getNpcAnimationHandler().Action_Stand();
            }
            break;

        case EMovementState::StrafeRight:
            if (m_MovementState.isStrafeRight)
            {
                // Strafe-key still pressed, keep going
                getNpcAnimationHandler().Action_StrafeRight();

                // And check for turns
                if (m_MovementState.isTurnLeft)
                {
                    getNpcAnimationHandler().Action_TurnLeft();
                }
                else if (m_MovementState.isTurnRight)
                {
                    getNpcAnimationHandler().Action_TurnRight();
                }
            }
            else
            {
                // Strafe-key not pressed anymore, go back to "standing"
                m_ActiveMovementState = EMovementState::None;
                getNpcAnimationHandler().Action_Stand();
            }
            break;

        case EMovementState::DrawWeapon:
            getNpcAnimationHandler().Action_DrawWeapon(0);

            if (getNpcAnimationHandler().isStanding(true) && !m_MovementState.isLastWeaponKey)
            {
                // Wait until the drawing-animation is over, then go to standing state
                m_ActiveMovementState = EMovementState::None;
            }
            break;

        case EMovementState::UndrawWeapon:
            getNpcAnimationHandler().Action_UndrawWeapon();

            if (getNpcAnimationHandler().isStanding(true) && !m_MovementState.isLastWeaponKey)
            {
                // Wait until the undrawing-animation is over, then go to standing state
                m_ActiveMovementState = EMovementState::None;

                // Actually undraw the weapon and go back to normal state
                getController().undrawWeapon();
            }
            break;

        case EMovementState::FightForward:
            if (m_MovementState.isForward && m_MovementState.isAction)
            {
                getNpcAnimationHandler().Action_FightForward();
            }
            else
            {
                // FIXME: Find out when the animation actually ended
                getNpcAnimationHandler().Action_Stand(true);
            }

            if (getNpcAnimationHandler().isStanding(true))
            {
                m_ActiveMovementState = EMovementState::None;
            }
            break;
    }

    bool actionTriggered = false;

    /**
     * TODO: There is still a problem when you switch directions frame-perfect from forward to backward
     * You will then walk forward while holding the back-key.
     */
    /*
    if(m_MovementState.isForward && !m_MovementState.isBackward)
    {
        getNpcAnimationHandler().Action_GoForward();
        actionTriggered = true;
    }

    if(m_MovementState.isBackward && !m_MovementState.isForward)
    {
        getNpcAnimationHandler().Action_GoBackward();
        actionTriggered = true;
    }

    if(m_MovementState.isStrafeLeft && !m_MovementState.isStrafeRight)
    {
        getNpcAnimationHandler().Action_StrafeLeft();
        actionTriggered = true;
    }

    if(m_MovementState.isStrafeRight && !m_MovementState.isStrafeLeft)
    {
        getNpcAnimationHandler().Action_StrafeRight();
        actionTriggered = true;
    }

    // Turning should always work
    if(m_MovementState.isTurnRight)
    {
        getNpcAnimationHandler().Action_TurnRight();
        actionTriggered = true;
    }else if(m_MovementState.isTurnLeft)
    {
        getNpcAnimationHandler().Action_TurnLeft();
        actionTriggered = true;
    }

    if(!actionTriggered)
    {
        // No key pressed, just stand
        getNpcAnimationHandler().Action_Stand();
    }

*/

    resetKeyStates();
}

void NpcAIHandler::bindKeys()
{
    m_MovementState.actionForward = Engine::Input::RegisterAction(Engine::ActionType::PlayerForward, [this](bool triggered, float) {
        m_MovementState.isForward = m_MovementState.isForward || triggered;
    });

    m_MovementState.actionBackward = Engine::Input::RegisterAction(Engine::ActionType::PlayerBackward, [this](bool triggered, float) {
        m_MovementState.isBackward = m_MovementState.isBackward || triggered;
    });

    m_MovementState.actionStrafeLeft = Engine::Input::RegisterAction(Engine::ActionType::PlayerStrafeLeft, [this](bool triggered, float) {
        m_MovementState.isStrafeLeft = m_MovementState.isStrafeLeft || triggered;
    });

    m_MovementState.actionStrafeRight = Engine::Input::RegisterAction(Engine::ActionType::PlayerStrafeRight, [this](bool triggered, float) {
        m_MovementState.isStrafeRight = m_MovementState.isStrafeRight || triggered;
    });

    m_MovementState.actionTurnRight = Engine::Input::RegisterAction(Engine::ActionType::PlayerTurnRight, [this](bool triggered, float) {
        m_MovementState.isTurnRight = m_MovementState.isTurnRight || triggered;
    });

    m_MovementState.actionTurnRight = Engine::Input::RegisterAction(Engine::ActionType::PlayerTurnLeft, [this](bool triggered, float) {
        m_MovementState.isTurnLeft = m_MovementState.isTurnLeft || triggered;
    });

    m_MovementState.actionTurnRight = Engine::Input::RegisterAction(Engine::ActionType::PlayerDrawWeaponMelee, [this](bool triggered, float) {
        m_MovementState.isLastWeaponKey = m_MovementState.isLastWeaponKey || triggered;
    });

    m_MovementState.actionAction = Engine::Input::RegisterAction(Engine::ActionType::PlayerAction, [this](bool triggered, float) {
        m_MovementState.isAction = m_MovementState.isAction || triggered;
    });
}

void NpcAIHandler::unbindKeys()
{
    Engine::Input::RemoveAction(Engine::ActionType::PlayerForward, m_MovementState.actionForward);
    Engine::Input::RemoveAction(Engine::ActionType::PlayerBackward, m_MovementState.actionBackward);
    Engine::Input::RemoveAction(Engine::ActionType::PlayerStrafeLeft, m_MovementState.actionStrafeLeft);
    Engine::Input::RemoveAction(Engine::ActionType::PlayerStrafeRight, m_MovementState.actionStrafeRight);
    Engine::Input::RemoveAction(Engine::ActionType::PlayerTurnLeft, m_MovementState.actionTurnLeft);
    Engine::Input::RemoveAction(Engine::ActionType::PlayerTurnRight, m_MovementState.actionTurnRight);
    Engine::Input::RemoveAction(Engine::ActionType::PlayerDrawWeaponMelee, m_MovementState.actionLastWeapon);
    Engine::Input::RemoveAction(Engine::ActionType::PlayerAction, m_MovementState.actionAction);

    // Zero out everything
    m_MovementState = {};
}

void NpcAIHandler::resetKeyStates()
{
    m_MovementState.isStrafeLeft = false;
    m_MovementState.isStrafeRight = false;
    m_MovementState.isForward = false;
    m_MovementState.isBackward = false;
    m_MovementState.isTurnLeft = false;
    m_MovementState.isTurnRight = false;
    m_MovementState.isLastWeaponKey = false;
    m_MovementState.isAction = false;
    //MoveSpeed1 = false;
    //MoveSpeed2 = false;
}

PlayerController& NpcAIHandler::getController() const
{
    return *reinterpret_cast<Logic::PlayerController*>(m_World.getEntity<Components::LogicComponent>(m_HostVob).m_pLogicController);
    ;
}

NpcAnimationHandler& NpcAIHandler::getNpcAnimationHandler() const
{
    return getController().getNpcAnimationHandler();
}
