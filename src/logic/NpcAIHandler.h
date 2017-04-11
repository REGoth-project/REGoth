#pragma once
#include <handle/HandleDef.h>
#include <vector>

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
         * Sets up keybindings for this handler
         */
        void bindKeys();
        void unbindKeys();

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
            /**
             * All bound actions. Stored so we can unbind them later
             */
            Engine::Action* actionForward = nullptr;
            Engine::Action* actionBackward = nullptr;
            Engine::Action* actionTurnLeft = nullptr;
            Engine::Action* actionTurnRight = nullptr;
            Engine::Action* actionStrafeLeft = nullptr;
            Engine::Action* actionStrafeRight = nullptr;
            Engine::Action* actionLastWeapon = nullptr;
            Engine::Action* actionAction = nullptr;
        }m_MovementState;

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
    };
}


