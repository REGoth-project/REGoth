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
        : m_World(world), m_HostVob(hostVob)
{
    unbindKeys();
}

void NpcAIHandler::update(float deltaTime)
{

}

void NpcAIHandler::playerUpdate(float deltaTime)
{
    if(!getController().getEM().isEmpty())
        return; // Messages are being played, don't handle inputs

    bool actionTriggered = false;

    /**
     * TODO: There is still a problem when you switch directions frame-perfect from forward to backward
     * You will then walk forward while holding the back-key.
     */

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
}

void NpcAIHandler::unbindKeys()
{
    Engine::Input::RemoveAction(Engine::ActionType::PlayerForward,     m_MovementState.actionForward);
    Engine::Input::RemoveAction(Engine::ActionType::PlayerBackward,    m_MovementState.actionBackward);
    Engine::Input::RemoveAction(Engine::ActionType::PlayerStrafeLeft,  m_MovementState.actionStrafeLeft);
    Engine::Input::RemoveAction(Engine::ActionType::PlayerStrafeRight, m_MovementState.actionStrafeRight);
    Engine::Input::RemoveAction(Engine::ActionType::PlayerTurnLeft,    m_MovementState.actionTurnLeft);
    Engine::Input::RemoveAction(Engine::ActionType::PlayerTurnRight,   m_MovementState.actionTurnRight);

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
    //MoveSpeed1 = false;
    //MoveSpeed2 = false;
}


PlayerController& NpcAIHandler::getController() const
{
    return *reinterpret_cast<Logic::PlayerController*>(m_World.getEntity<Components::LogicComponent>(m_HostVob).m_pLogicController);;
}

NpcAnimationHandler& NpcAIHandler::getNpcAnimationHandler() const
{
    return getController().getNpcAnimationHandler();
}



