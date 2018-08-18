//
// Created by desktop on 08.09.17.
//

#pragma once

#include <math/mathlib.h>
#include <handle/HandleDef.h>
#include <engine/Waynet.h>
#include <list>

namespace World
{
    class WorldInstance;
}

namespace Logic
{
    /**
     * Wrapper around the waynet. Can find paths to certain locations and give instructions onto how to get there
     * from a given point.
     */
    class Pathfinder
    {
    public:

        /**
         * The next point on the route to go to in order to reach the final destination.
         */
        struct Instruction
        {
            Math::float3 targetPosition;
        };

        struct UserConfiguration
        {
            float height;
            float radius;
            float stepHeight;
            float maxSlopeAngle;
        };

        struct Route
        {
            Math::float3 lastKnownPosition;

            std::list<Math::float3> positionsToGo;

            // If this is valid, the Creature will move to this entity, once it has been to
            // all positions it had to go to or has a direct line of sight to it
            Handle::EntityHandle targetEntity;

            // Position the target entity was at when we started the route.
            // This must be saved so we can perform a re-route if the target moves too far from the spot
            // the route goes to
            Math::float3 targetEntityPositionOnStart;
        };

        Pathfinder(World::WorldInstance& world);
        virtual ~Pathfinder() = default;

        void startNewRouteTo(const Math::float3& positionNow, const Math::float3& target);
        void startNewRouteTo(const Math::float3& positionNow, World::Waynet::WaypointIndex waypoint);
        void startNewRouteTo(const Math::float3& positionNow, Handle::EntityHandle entity);

        /**
         * @return The next target position on the way to the location set via startNewRouteTo.
         *         If the target has been reached, positionNow is returned, so you should always check
         *         whether the route has been completed first.
         */
        Instruction updateToNextInstructionToTarget(const Math::float3& positionNow);

        /**
         * @return Whether the active route has been completed.
         */
        bool hasActiveRouteBeenCompleted(const Math::float3& positionNow);

        /**
         * Information about the creature using this pathfinder
         */
        void setConfiguration(const UserConfiguration& configuration) { m_UserConfiguration = configuration; }
        const UserConfiguration& getConfiguration() { return m_UserConfiguration; }

        /**
         * @return Whether the Pathfinder thinks the given targetposition has been reached
         */
        static bool isTargetReachedByPosition(const Math::float3& position, const Math::float3& target);
    private:


        struct MovementReport
        {
            bool lowerThanStepHeight;
            bool higherThanStepHeight;

            bool tooSteepDown;
            bool tooSteepUp;
            bool ceilingTooLow;
            bool hardCollision;

            //Handle::EntityHandle hitVob; // TODO: This could be useful
        };

        /**
         * Gives information about what would happen if an NPC moved to a certain location.
         * @param from Source location. Expected to be at (Groundlevel + Height) of the creature
         * @param to   Destination. Expected to be at (Groundlevel + Height) of the creature
         */
        MovementReport checkMoveToLocation(const Math::float3& from, const Math::float3& to);

        /**
         * Performs a series of checks on whether the creature could directly walk to the given
         * location
         */
        bool canDirectlyMovetoLocation(const Math::float3& from, const Math::float3& to);

        /**
         * @return height of the ceiling at the given position
         */
        float findCeilingHeightAtPosition(const Math::float3& floorposition);

        /**
         * Traces down to find the ground poly at the given location
         * @return Triangle-index of the world-mesh. (-1 if none)
         */
        size_t getGroundTriangleIndexAt(const Math::float3& position);

        /**
         * @return Slope angle in radians
         */
        float calculateSlopeFromNormal(const Math::float3& normal);

        /**
         * Finds the next visible waypoint from the given location. The
         * @param from
         */
        World::Waynet::WaypointIndex findNextVisibleWaypoint(const Math::float3& from);

        /**
         * @return Whether the next position on the route has been reached
         */
        bool hasNextRouteTargetBeenReached(const Math::float3& positionNow);

        /**
         * @return Whether the entity given in the active route has been reached. False if none was specified there.
         */
        bool hasTargetEntityBeenReached(const Math::float3& positionNow);

        /**
         * @return Position of the entity to go to stored in the active route. Undefined if none was specified there.
         */
        Math::float3 getTargetEntityPosition();

        /**
         * @return Whether the Target specified in the active route is an entity
         */
        bool isTargetAnEntity();

        /**
         * @return Position of the route to go to right now
         *         Note: Result is undefined when the target has been reached and there is no entity-target.
         */
        Math::float3 getCurrentTargetPosition(const Math::float3& positionNow);

        /**
         * Draws lines on where to go to
         */
        void debugDrawRoute(const Math::float3& positionNow);

        /**
         * @return Whether the target entity has moved too far from the spot it was when we started the route
         */
        bool hasTargetEntityMovedTooFar();

        /**
         * Sometimes, the waynet isn't exactly detailed and NPCs take some weird looking detours instead of
         * going straight. This function uses raytraces to check which points on the route can be erased because
         * there are not obstacles on the way to them
         */
        void cleanupRoute();

        /**
         * @return Whether the route-position on the given iterator can be removed. See cleanupRoute().
         */
        bool canRoutePositionBeRemoved(std::list<Math::float3>::iterator it);

        /**
         * @return Whether the currently active route is considered not up-to-date and should be redone.
         *         This happens for example, when a target entity moved too far or something is blocking
         *         the way now.
         */
        bool shouldReRoute(const Math::float3& positionNow);

        /**
         * Information about the creature using this pathfinder
         */
        UserConfiguration m_UserConfiguration;

        /**
         * The currently ran route. The Creature will report it's poisition and get a new target
         * location, until it reaches the destination.
         */
        Route m_ActiveRoute;

        World::WorldInstance& m_World;
    };
}


