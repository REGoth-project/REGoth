#pragma once

#include <daedalus/DaedalusGameState.h>
#include <set>
#include "Controller.h"
#include "Inventory.h"
#include "LogicDef.h"
#include "NpcScriptState.h"

namespace Logic
{
    class ModelVisual;

    class PlayerController : public Controller
    {
    public:

        /**
         * @param world World of the underlaying entity
         * @param entity Entity owning this controller
         */
        PlayerController(World::WorldInstance& world, Handle::EntityHandle entity, Daedalus::GameState::NpcHandle scriptInstance);

        /**
         * @return The type of this class. If you are adding a new base controller, be sure to add it to ControllerTypes.h
         */
        virtual EControllerType getControllerType(){ return EControllerType::PlayerController; }

        /**
         * Sets the daily routine for this.
         * TODO: Add timing for these // TODO: Done, now remove this
         *
         * @param wps List of waypoints the NPC should visit, in order
         */
        void setDailyRoutine(const std::vector<size_t>& wps)
        { m_RoutineState.routineWaypoints = wps; }
        void addRoutineWaypoint(size_t wp)
        { m_RoutineState.routineWaypoints.push_back(wp); }

        void setFollowTarget(Handle::EntityHandle e){ m_RoutineState.entityTarget = e; }

        /**
         * Called when the models visual changed
         */
        virtual void onVisualChanged();

        /**
         * Handle NPC specific messages
         * @param message Message to handle
         * @param sourceVob Instigator vob. Can be invalid.
         */
        virtual void onMessage(EventMessages::EventMessage& message, Handle::EntityHandle sourceVob) override;

        /**
         * Called on game-tick
         */
        void onUpdate(float deltaTime) override;

        /**
         * Updates the controller using input read from the user
         */
        void onUpdateByInput(float deltaTime);

        /**
         * Called at rendertime
         */
        void onDebugDraw() override;

        /**
         * Lets the controller move the entity to the next routine target
         */
        void continueRoutine();

        /**
         * Calculates a route and begins to move the NPC to the given waypoint
         */
        void gotoWaypoint(size_t wp);

        /**
         * Recalculates the current route
         */
        void rebuildRoute();

        /**
         * Stops going along the current route
         */
        void stopRoute();

        /**
         * Teleports the entity to the given waypoint
         * @param Waypoint index to go to
         */
        void teleportToWaypoint(size_t wp);
        void teleportToPosition(const Math::float3& pos);

        /**
         * Sets the direction the NPC should be facing to (snaps, doesn't interpolate nor plays animations)
         * @param direction Direction to face
         */
        void setDirection(const Math::float3& direction);

        /**
         * @return The inventory of this player
         */
        Inventory& getInventory()
        {
            return m_Inventory;
        }

        /**
         * Equips the item with the given handle
         * Note: Be careful that this is actually inside the inventory of the player
         */
        void equipItem(Daedalus::GameState::ItemHandle item);

        /**
         * Draws the weapon currently in the 1h- or 2h-slot
         * @return Handle to the just drawn weapon
         */
        Daedalus::GameState::ItemHandle drawWeaponMelee();

        /**
         * Puts back any weapon that was currently drawn
         * @param force If true, no animation will be played
         */
        void undrawWeapon(bool force = false);

        /**
         * Uses the given Item on this NPC
         * @param item Item to use
         * @return Whether the item should be deleted from the inventory now
         */
        bool useItem(Daedalus::GameState::ItemHandle item);

        /**
         * Checks if this NPC can use the given item. If not, messages will be displayed if this is also the player
         * @param item Item to check
         * @return Whether we can equip or use this
         */
        bool canUse(Daedalus::GameState::ItemHandle item);

        /**
         * Changes one of the script attributes. Checks for godmode, immortality, maxima, etc
         * @param atr Attribute of the script-object
         * @param change Delta value of the change
         */
        void changeAttribute(Daedalus::GEngineClasses::C_Npc::EAttributes atr, int change);

        /**
         * @return The ModelVisual of the underlaying vob
         */
        ModelVisual* getModelVisual();

        /**
         * Places the playercontroller on the ground again. TODO: TESTING-ONLY!
         */
        void placeOnGround();

        /**
         * @return The script-side handle to this NPC
         */
        Daedalus::GameState::NpcHandle getScriptHandle()
        {
            return m_ScriptState.npcHandle;
        }

        /**
         * @return Looks up the script instance to our handle
         */
        Daedalus::GEngineClasses::C_Npc& getScriptInstance();

        /**
         * Front/Right/Left-Attack with the current weapon
         */
        void attackFront();
        void attackLeft();
        void attackRight();

        /**
         * @return True, if this is the currently controlled charakter
         */
        bool isPlayerControlled();

        /**
         * Starts a random dialog-gesture
         */
        void startDialogAnimation();

        /**
         * Lets this NPC stop everything that it is currently doing and goes to the usual "just standing"-mode
         */
        void interrupt();

        /**
         * Traces from this position to the position of the entity and checks if something is between them
         * or if the entity is out of range
         * @param ignoreAngles Usually, the direction the NPC is facing to is checked as well, this turns off the check
         * @return whether this NPC can see the given entity
         */
        bool canSee(Handle::EntityHandle entity, bool ignoreAngles = false);

        /**
         * Checks the angle from the facing direction of the NPC to the given point
         * @param pos Point to get the angle to from
         * @return Angle between the forward direction of the npc and pos as radians, [0, pi]
         */
        float getAngleTo(const Math::float3& pos);

        /**
         * Lets the NPC stop all it's actions
         * @param walkingAllowed Whether walking should be allowed and not be stopped
         * @param startAniTransition Starts a transition animation from the current state to standing
         */
        void standUp(bool walkingAllowed = false, bool startAniTransition = false);

        /**
         * Sets the current routine-state function
         * @param symRoutine Routine to use now
         */
        void setRoutineFunc(size_t symRoutine);

        /**
         * Changes the routine for this NPC. Possible values: GUIDE, FOLLOW, START, more?
         * @param routineName Routine-type to start
         */
        void changeRoutine(const std::string& routineName);

        /**
         * @return True, if the NPC is not dead, unconscious, etc
         */
        bool isNpcReady();

        /**
         * @return The ai-state machine of this NPC
         */
        NpcScriptState& getAIStateMachine(){ return m_AIStateMachine; }

        /**
         * @return Waypoint closest to the NPC
         */
        size_t getClosestWaypoint() { return m_AIState.closestWaypoint; }

        /**
         * @return Waypoint the NPC is currently going to
         */
        size_t getTargetWaypoint(){ return m_AIState.targetWaypoint; }

        /**
         * Sets up the bindings for this playercontroller // TODO: REMOVE THIS AND DO IT OUTSIDE OF THE PLAYERCONTROLLER
         */
        void setupKeyBindings();

    protected:

        /**
         * Events
         */
        bool EV_Event(Logic::EventMessages::EventMessage& message, Handle::EntityHandle sourceVob);
        bool EV_Npc(Logic::EventMessages::NpcMessage& message, Handle::EntityHandle sourceVob);
        bool EV_Damage(Logic::EventMessages::DamageMessage& message, Handle::EntityHandle sourceVob);
        bool EV_Weapon(Logic::EventMessages::WeaponMessage& message, Handle::EntityHandle sourceVob);
        bool EV_Movement(Logic::EventMessages::MovementMessage& message, Handle::EntityHandle sourceVob);
        bool EV_Attack(Logic::EventMessages::AttackMessage& message, Handle::EntityHandle sourceVob);
        bool EV_UseItem(Logic::EventMessages::UseItemMessage& message, Handle::EntityHandle sourceVob);
        bool EV_State(Logic::EventMessages::StateMessage& message, Handle::EntityHandle sourceVob);
        bool EV_Manipulate(Logic::EventMessages::ManipulateMessage& message, Handle::EntityHandle sourceVob);
        bool EV_Conversation(Logic::EventMessages::ConversationMessage& message, Handle::EntityHandle sourceVob);
        bool EV_Magic(Logic::EventMessages::MagicMessage& message, Handle::EntityHandle sourceVob);

        /**
         * Moves the NPC to the next waypoint of the current path
         * @return true, if the current paths destination was reached
         */
        bool travelPath(float deltaTime);

        /**
         * Current routine state
         */
        struct
        {
            // Route-Waypoint index to got to
            size_t routineTarget;

            // If this is false, the NPC won't do it's routine after finishing moving to the current target
            bool routineActive;

            /**
            * List of waypoints to move to on this AIs daily routine
            * TODO: Implement timing for these
            */
            std::vector<size_t> routineWaypoints;

            /**
             * Target of where the NPC should keep trying to go to
             */
            Handle::EntityHandle entityTarget;
        }m_RoutineState;

        struct
        {
            // Waypoint this NPC is closest to/was last positioned at
            size_t closestWaypoint;

            // Waypoint the NPC is going to
            size_t targetWaypoint;
        }m_AIState;

        struct
        {
            // Path the NPC is currently trying to take (To m_AIState.targetWaypoint)
            std::vector<size_t> currentPath;

            // Node the NPC is currently going to on the path
            size_t targetNode;

            // Percentage of how far the NPC has gotten so far on the current path
            float currentPathPerc;

            // Length of the current route
            float currentRouteLength;

            // Where the npc currently is
            Math::float3 position;

            // Where the npc currently is looking at
            Math::float3 direction;

        }m_MoveState;

        struct
        {
            // Move speed in m/s
            float moveSpeed;

            // BBox for collision
            Math::float3 collisionBBox[2];

            // Root offset of the model
            Math::float3 modelRoot;
        }m_NPCProperties;

        struct
        {
            // Handle to the npc data on script-side
            Daedalus::GameState::NpcHandle npcHandle;
        }m_ScriptState;

        struct
        {
            // Style of weapon the NPC is holding right now
            EWeaponMode weaponMode;
            Daedalus::GameState::ItemHandle activeWeapon;

            // All equiped items. Contains weapons, rings, armor...
            std::set<Daedalus::GameState::ItemHandle> equippedItemsAll;

            // All possible equipped items on an NPC
            struct
            {
                std::set<Daedalus::GameState::ItemHandle> equippedRings;
                std::set<Daedalus::GameState::ItemHandle> equippedRunes;
                Daedalus::GameState::ItemHandle equippedWeapon1h;
                Daedalus::GameState::ItemHandle equippedWeapon2h;
                Daedalus::GameState::ItemHandle equippedBow;
                Daedalus::GameState::ItemHandle equippedCrossBow;
                Daedalus::GameState::ItemHandle equippedBelt;
                Daedalus::GameState::ItemHandle equippedAmulet;
            }equippedItems;
        }m_EquipmentState;

        /**
         * This players inventory
         */
        Inventory m_Inventory;

        /**
         * State manager
         */
        NpcScriptState m_AIStateMachine;

        /**
         * Key states
         */
        bool m_isDrawWeaponMelee;
        bool m_isForward;
        bool m_isBackward;
        bool m_isTurnLeft;
        bool m_isTurnRight;
        bool m_isStrafeLeft;
        bool m_isStrafeRight;
        bool m_MoveSpeed1, m_MoveSpeed2;
    };
}
