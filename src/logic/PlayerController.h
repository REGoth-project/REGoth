#pragma once

#include <daedalus/DaedalusGameState.h>
#include <set>
#include "Controller.h"
#include "Inventory.h"
namespace Logic
{
    class ModelVisual;

    enum class EWeaponMode
    {
        WeaponNone,
        Weapon1h,
        Weapon2h,
        WeaponBow,
        WeaponCrossBow,
        WeaponMagic,
        WeaponFist
    };

    enum class EWeaponDrawType
    {
        Weapon_NF,
        Weapon_FF,
        Weapon_Magic
    };

    class PlayerController : public Controller
    {
    public:
        /**
         * @param world World of the underlaying entity
         * @param entity Entity owning this controller
         */
        PlayerController(World::WorldInstance& world, Handle::EntityHandle entity, Daedalus::GameState::NpcHandle scriptInstance);

        /**
         * Sets the daily routine for this.
         * TODO: Add timing for these
         *
         * @param wps List of waypoints the NPC should visit, in order
         */
        void setDailyRoutine(const std::vector<size_t>& wps)
        { m_RoutineState.routineWaypoints = wps; }
        void addRoutineWaypoint(size_t wp)
        { m_RoutineState.routineWaypoints.push_back(wp); }

        /**
         * Called when the models visual changed
         */
        virtual void onVisualChanged();

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
         * Teleports the entity to the given waypoint
         * @param Waypoint index to go to
         */
        void teleportToWaypoint(size_t wp);

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
         */
        void undrawWeapon();

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
         * Front/Right/Left-Attack with the current weapon
         */
        void attackFront();
        void attackLeft();
        void attackRight();

    protected:

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

    public:
        bool m_isDrawWeaponMelee;
        bool m_isForward;
        bool m_isBackward;
        bool m_isTurnLeft;
        bool m_isTurnRight;
        bool m_isStrafeLeft;
        bool m_isStrafeRight;
    };
}
