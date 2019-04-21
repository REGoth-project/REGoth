#pragma once

#include <set>
#include "CharacterEquipment.h"
#include "Controller.h"
#include "Inventory.h"
#include "LogicDef.h"
#include "NpcAIHandler.h"
#include "NpcAnimationHandler.h"
#include "NpcScriptState.h"
#include "Pathfinder.h"
#include <daedalus/DaedalusGameState.h>

namespace UI
{
    class Menu_Status;
}

namespace Physics
{
    struct RayTestResult;
}

namespace World
{
    class AudioWorld;
}

namespace Logic
{
    class ModelVisual;

    /**
     * Useful script functions for reporting
     */
    namespace PlayerScriptInfo
    {
        const char* const PLAYER_MOB_MISSING_KEY = "PLAYER_MOB_MISSING_KEY";
        const char* const PLAYER_MOB_MISSING_LOCKPICK = "PLAYER_MOB_MISSING_LOCKPICK";
        const char* const PLAYER_MOB_MISSING_KEY_OR_LOCKPICK = "PLAYER_MOB_MISSING_KEY_OR_LOCKPICK";
        const char* const PLAYER_MOB_NEVER_OPEN = "PLAYER_MOB_NEVER_OPEN";
        const char* const PLAYER_MOB_TOO_FAR_AWAY = "PLAYER_MOB_TOO_FAR_AWAY";
        const char* const PLAYER_MOB_WRONG_SIDE = "PLAYER_MOB_WRONG_SIDE";
        const char* const PLAYER_MOB_MISSING_ITEM = "PLAYER_MOB_MISSING_ITEM";
        const char* const PLAYER_MOB_ANOTHER_IS_USING = "PLAYER_MOB_ANOTHER_IS_USING";
        const char* const PLAYER_PLUNDER_IS_EMPTY = "PLAYER_PLUNDER_IS_EMPTY";
        const char* const PLAYER_RANGED_NO_AMMO = "PLAYER_RANGED_NO_AMMO";
    }  // namespace PlayerScriptInfo

    /**
     * All kinds of things an NPC can do
     */
    enum EBodyState
    {
        BS_STAND = 0,
        BS_WALK = 1,
        BS_SNEAK = 2,
        BS_RUN = 3,
        BS_SPRINT = 4,
        BS_SWIM = 5,
        BS_CRAWL = 6,
        BS_DIVE = 7,
        BS_JUMP = 8,
        BS_CLIMB = 9,
        BS_FALL = 10,
        BS_SIT = 11,
        BS_LIE = 12,
        BS_INVENTORY = 13,
        BS_ITEMINTERACT = 14,
        BS_MOBINTERACT = 15,
        BS_MOBINTERACT_INTERRUPT = 16,
        BS_TAKEITEM = 17,
        BS_DROPITEM = 18,
        BS_THROWITEM = 19,
        BS_PICKPOCKET = 20,
        BS_STUMBLE = 21,
        BS_UNCONSCIOUS = 22,
        BS_DEAD = 23,
        BS_AIMNEAR = 24,
        BS_AIMFAR = 25,
        BS_HIT = 26,
        BS_PARADE = 27,
        BS_CASTING = 28,
        BS_PETRIFIED = 29,
        BS_CONTROLLING = 30,
        BS_MAX = 31,
    };

    class PlayerController : public Controller
    {
    public:
        using WalkMode = EventMessages::MovementMessage::WalkMode;

        /**
         * @param world World of the underlaying entity
         * @param entity Entity owning this controller
         */
        PlayerController(World::WorldInstance& world, Handle::EntityHandle entity, Daedalus::GameState::NpcHandle scriptInstance);

        /**
         * @return The type of this class. If you are adding a new base controller, be sure to add it to ControllerTypes.h
         */
        EControllerType getControllerType() override { return EControllerType::PlayerController; }

        /**
         * Called when the models visual changed
         */
        void onVisualChanged() override;

        /**
         * Handle NPC specific messages
         * @param message Message to handle
         * @param sourceVob Instigator vob. Can be invalid.
         */
        void onMessage(std::shared_ptr<EventMessages::EventMessage> message, Handle::EntityHandle sourceVob) override;

        /**
         * Called on game-tick
         */
        void onUpdate(float deltaTime) override;

        /**
         * Updates the controller using input read from the user
         */
        void onUpdateByInput(float deltaTime);

        /**
         * Update routine for the NPC currently controlled by the player
         * @param deltaTime Time since last frame
         */
        void onUpdateForPlayer(float deltaTime);

        /**
         * Called at rendertime
         */
        void onDebugDraw() override;

        /**
         * Calculates a route and begins to move the NPC to the given waypoint
         */
        void gotoWaypoint(World::Waynet::WaypointIndex wp);
        void gotoVob(Handle::EntityHandle vob);
        void gotoPosition(const Math::float3& position);

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
         * Applies a rotation to the NPCs Y-Axis
         * @param rad Rotation to apply in radians
         */
        void applyRotationY(float rad);

        /**
         * @return The direction the player is facing
         */
        Math::float3 getDirection()
        {
            return m_MoveState.direction;
        }

        /**
         * @return The inventory of this player
         */
        Inventory& getInventory()
        {
            return m_Inventory;
        }

        /**
         * (Un)Equips the item with the given handle
         * Note: Be careful that this is actually inside the inventory of the player
         */
        void equipItem(Daedalus::GameState::ItemHandle item);
        void unequipItem(Daedalus::GameState::ItemHandle item);

        /**
         * Draws the weapon currently in the 1h- or 2h-slot
         * @param fist [optional] if true, draw fists regardless of the equipped weapon
         * @return Handle to the just drawn weapon
         */
        Daedalus::GameState::ItemHandle drawWeaponMelee(bool forceFist = false);

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
         * Puts <count> instances of the given item into ths players inventory
         * @param instanceName Instance-name of the item to put
         * @param count How many instances to create (ie. 500 Gold)
         */
        void giveItem(const std::string& instanceName, unsigned int count = 1);

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
        void setAttribute(Daedalus::GEngineClasses::C_Npc::EAttributes atr, int value);

        /**
         * @return The ModelVisual of the underlaying vob
         */
        ModelVisual* getModelVisual();

        /**
         * @return Animation handler of this NPC
         */
        NpcAnimationHandler& getNpcAnimationHandler() { return m_NPCAnimationHandler; }
        /**
         * Places the playercontroller on the ground again. TODO: TESTING-ONLY!
         */
        void placeOnGround();
        /**
         * Places the playercontroller on the surface hit by provided rayresult. TODO: TESTING-ONLY!
         */
        void placeOnSurface(const Physics::RayTestResult& result);

        /**
         * Saves data of the ground on which the NPC is standing
         */
        void traceDownNPCGround();

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
         * @return True, if this is the currently controlled character
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
         * Lets this character die
         * @param attackingNPC Character which was the reason that this npc died
         */
        void die(Handle::EntityHandle attackingNPC);

        /**
         * If the character is not already unconscious, this method will play the animations and put him on the ground
         */
        void checkUnconscious();

        /**
         * @return Name of the guild this NPC is currently in
         */
        std::string getGuildName();

        /**
         * Sets the body-state of this character
         * @param state State to set
         */
        void setBodyState(EBodyState state) { m_AIState.bodyState = state; }
        EBodyState getBodyState() { return m_AIState.bodyState; }
        /**
         * Traces from this position to the position of the entity and checks if something is between them
         * or if the entity is out of range
         * @param ignoreAngles Usually, the direction the NPC is facing to is checked as well, this turns off the check
         * @return whether this NPC can see the given entity
         */
        bool canSee(Handle::EntityHandle entity, bool ignoreAngles = false);

        /**
         * Traces from the npcs eyes to the given target position and checks if something is in between them
         * @param target Target position to trace to
         * @return true, if nothing is in between the npc and the target position
         */
        bool freeLineOfSight(const Math::float3& target);

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
         * @return True, if m_RefuseTalkTime has not reached 0 yet
         */
        bool isRefusingTalk() { return m_RefuseTalkTime > 0; }
        /**
         * set timer for certain dialog options to be disabled
         * @param time in seconds
         */
        void setRefuseTalkTime(float time) { m_RefuseTalkTime = time; }
        /**
         * @return The ai-state machine of this NPC
         */
        NpcScriptState& getAIStateMachine() { return m_AIStateMachine; }
        /**
         * @return Waypoint closest to the NPC
         */
        World::Waynet::WaypointIndex getClosestWaypoint();

        /**
         * @return Waypoint second closest to the NPC
         */
        World::Waynet::WaypointIndex getSecondClosestWaypoint();

        /**
         * Handles actions, that affect this NPC
         */
        void onAction(Engine::ActionType actionType, bool triggered, float intensity);

        /**
         * Updates the given status-screen once with the current attributes
         */
        void updateStatusScreen(UI::Menu_Status& statsScreen);

        /**
         * Returns material data on which the NPC is standing
         */
        ZenLoad::MaterialGroup getSurfaceMaterial();

        /**
         * @return Item this NPC is currently interacting with
         */
        Daedalus::GameState::ItemHandle getInteractItem() { return Daedalus::GameState::ItemHandle(); /* TODO: Implement */ }
        /**
         * Sets the mob this playercontroller is currently using
         */
        void setUsedMob(Handle::EntityHandle mob) { m_AIState.usedMob = mob; }
        Handle::EntityHandle getUsedMob() { return m_AIState.usedMob; }
        /**
         * Enables/Disables physics on this NPC
         */
        void setPhysicsEnabled(bool value) { m_NPCProperties.enablePhysics = value; }
        /**
         * @return Currently held weapon type
         */
        EWeaponMode getWeaponMode() { return m_EquipmentState.weaponMode; }
        /**
         * @return Classes which want to get exported on save should return true here
         */
        bool shouldExport() override { return true; }
        /**
         * Does the logic for importing an NPC/PC
         * Actually adds the NPC to the world
         * @return Entity of the imported player
         */
        static Handle::EntityHandle importPlayerController(World::WorldInstance& world, const json& logic);

        /**
         * Imports state from a json-object
         * @param j
         */
        void importObject(const json& j) override;

        /**
         * Same as the virtual importObject, but won't apply transforms
         * @param j
         * @param noTransform
         */
        void importObject(const json& j, bool noTransform);

        /**
         * Check if this NPC has equipped any melee weapon
         */
        bool hasEquippedMeleeWeapon() const;

        /**
         * @param walkMode Whether we should be runnning, sneaking, etc
         */
        void setWalkMode(WalkMode walkMode);

    protected:
        /**
         * Callbacks registered inside the animation-handler
         */
        void AniEvent_SFX(const ZenLoad::zCModelScriptEventSfx& sfx);
        void AniEvent_SFXGround(const ZenLoad::zCModelScriptEventSfx& sfx);
        void AniEvent_Tag(const ZenLoad::zCModelScriptEventTag& tag);

        void AniEvent_PFX(const ZenLoad::zCModelScriptEventPfx& pfx);
        void AniEvent_PFXStop(const ZenLoad::zCModelScriptEventPfxStop& pfxStop);
        void exportPart(json& j) override;

        /**
         * Events
         */
        bool EV_Event(std::shared_ptr<Logic::EventMessages::EventMessage> message, Handle::EntityHandle sourceVob);
        bool EV_Npc(std::shared_ptr<Logic::EventMessages::NpcMessage> message, Handle::EntityHandle sourceVob);
        bool EV_Damage(std::shared_ptr<Logic::EventMessages::DamageMessage> message, Handle::EntityHandle sourceVob);
        bool EV_Weapon(std::shared_ptr<Logic::EventMessages::WeaponMessage> message, Handle::EntityHandle sourceVob);
        bool EV_Movement(std::shared_ptr<Logic::EventMessages::MovementMessage> message, Handle::EntityHandle sourceVob);
        bool EV_Attack(std::shared_ptr<Logic::EventMessages::AttackMessage> message, Handle::EntityHandle sourceVob);
        bool EV_UseItem(std::shared_ptr<Logic::EventMessages::UseItemMessage> message, Handle::EntityHandle sourceVob);
        bool EV_State(std::shared_ptr<Logic::EventMessages::StateMessage> message, Handle::EntityHandle sourceVob);
        bool EV_Manipulate(std::shared_ptr<Logic::EventMessages::ManipulateMessage> message, Handle::EntityHandle sourceVob);
        bool EV_Conversation(std::shared_ptr<Logic::EventMessages::ConversationMessage> message, Handle::EntityHandle sourceVob);
        bool EV_Magic(std::shared_ptr<Logic::EventMessages::MagicMessage> message, Handle::EntityHandle sourceVob);

        /**
         * Moves the NPC to the next waypoint of the current path
         */
        void travelPath();

        struct
        {
            // Handle to the Mob currently used by this NPC, if valid
            Handle::EntityHandle usedMob;

            // Current body state
            EBodyState bodyState;

        } m_AIState;

        struct
        {
            // Where the npc currently is
            Math::float3 position;

            // Where the npc currently is looking at
            Math::float3 direction;

            // Surface data obtained from raytracing the ground below NPC
            struct
            {
                bool successful;  // false if for some reason raytrace failed to hit the ground during last run
                uint32_t triangleIndex;
                Math::float3 trianglePosition;
                float waterDepth;
            } ground;

        } m_MoveState;

        struct
        {
            // BBox for collision
            Math::float3 collisionBBox[2];

            // Root offset of the model
            Math::float3 modelRoot;

            // Whether this NPC should be put on ground every frame
            bool enablePhysics;
        } m_NPCProperties;

        struct
        {
            // Handle to the npc data on script-side
            Daedalus::GameState::NpcHandle npcHandle;
        } m_ScriptState;

        struct
        {
            // Style of weapon the NPC is holding right now
            EWeaponMode weaponMode;
            Daedalus::GameState::ItemHandle activeWeapon;

        } m_EquipmentState;

        Inventory m_Inventory;
        NpcScriptState m_AIStateMachine;
        NpcAnimationHandler m_NPCAnimationHandler;
        NpcAIHandler m_AIHandler;
        Pathfinder m_PathFinder;
        CharacterEquipment m_CharacterEquipment;

        /**
         * refuse talk countdown
         */
        float m_RefuseTalkTime;

        /**
         * Key states
         */
        bool m_MoveSpeed1, m_MoveSpeed2;

        /**
         * Struct that stores information about one shot pfxEvents  of this player
         */
        struct pfxEvent
        {
            Handle::EntityHandle entity;
            std::string bodyPosition;
            bool isAttached;
            int32_t m_Num;
        };

        /**
        * Stores active pfx handler associated with this PlayerController (one shot)
        */
        std::vector<pfxEvent> m_activePfxEvents;

        /**
        * Stores ended pfx handler (emitter don't spawn new particles)
        * After all particles are dead, updatePfx() will remove these elements from this vector
        */
        std::vector<pfxEvent> m_endedPfxEvents;

        /**
         * Updates the position of the pfxEvent
         * @param e pfxEvent
         */
        void updatePfxPosition(const pfxEvent& e);

        /**
         * Upates the pfx for this player (removes dead emitter)
         * When "ATTACH" keyword is used, the position is also changed.
         */
        void updatePfx();

        void resetKeyStates();

        // FIXME: Hack for as long as animation-flags are not implemented
        // Turns of modifying the root position from the animation
        bool m_NoAniRootPosHack;
        size_t m_LastAniRootPosUpdatedAniHash;

        // Main noise sound slot. Other sounds using it won't play if there is already a sound playing here.
        Audio::SoundPtr m_MainNoiseSoundSlot;

        World::AudioWorld& m_AudioWorld;
    };
}  // namespace Logic
