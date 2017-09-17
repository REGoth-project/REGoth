#pragma once
#include <vector>
#include <handle/HandleDef.h>
#include <engine/Input.h>

namespace World
{
    class WorldInstance;
}

namespace Engine
{
    class Action;
}

namespace Logic
{
    class NpcAnimationHandler;
    class PlayerController;

    /**
     * Possible movement-states for the player.
     * Turning is a modifier that can be applied to all of them.
     */
    enum class EMovementState
    {
        None,
        Forward,
        Backward,
        StrafeLeft,
        StrafeRight,
        DrawWeapon,
        UndrawWeapon,
        FightForward,
    };

    /**
     * This is not only for NPCs and AI, but also handles the players inputs
     */
    class NpcAIHandler
    {
    public:
        NpcAIHandler(World::WorldInstance& world, Handle::EntityHandle hostVob);
        ~NpcAIHandler();

        /**
         * Frame update
         * @param deltaTime Time since last frame
         */
        void update(float deltaTime);

        /**
         * Frame update for the player-character
         * @param deltaTime Time since last frame
         */
        void playerUpdate(float deltaTime);

        /**
         * Frame update for non-player-characters
         * @param deltaTime Time since last frame
         */
        void npcUpdate(float deltaTime);

        /**
         * Moves the player back to the usual-standing state
         */
        void standup();

        /**
         * Handles key event
         */
        void onAction(Engine::ActionType actionType, bool triggered);
        void unbindKeys();

        /**
         * @param state Movementstate to set for AI
         */
        void setTargetMovementState(EMovementState state);

        /**
         * @return Controller this is attached to
         */
        PlayerController& getController() const;

        /**
         * @return This controllers animation handler
         */
        NpcAnimationHandler& getNpcAnimationHandler() const;

    private:
        /**
         * Resets the players input. Used after input was processed...
         */
        void resetKeyStates();

        /**
         * Movement-directions
         */
        struct
        {
            bool isForward = false;
            bool isBackward = false;
            bool isTurnLeft = false;
            bool isTurnRight = false;
            bool isStrafeLeft = false;
            bool isStrafeRight = false;
            bool isLastWeaponKey = false;
            bool isAction = false;
        } m_MovementState;

        /**
         * World this resides in
         */
        World::WorldInstance& m_World;

        /**
         * Vob (PlayerController) this belongs to
         */
        Handle::EntityHandle m_HostVob;

        /**
         * Current movment state
         */
        EMovementState m_ActiveMovementState;
        EMovementState m_TargetMovementState;
    };
}
