#pragma once
#include <map>
#include <string>
#include <vector>
#include <math/mathlib.h>
#include <zenload/zTypes.h>

namespace World
{
    namespace Waynet
    {
        /**
         * Note: These generally don't change, so it's okay to not use handles, but indices for them
         */

        typedef size_t WaypointIndex;
        enum : WaypointIndex
        {
            INVALID_WAYPOINT = static_cast<WaypointIndex>(-1)
        };

        struct Waypoint
        {
            std::string name;
            std::string classname;
            Math::float3 position;
            Math::float3 direction;
            float waterDepth;
            bool underWater;

            /**
             * Indices to all edges of this waypoint
             */
            std::vector<WaypointIndex> edges;
        };

        struct WaynetInstance
        {
            /**
             * Memory of all known waypoints
             */
            std::vector<Waypoint> waypoints;

            /**
             * Map of waypoint names to their indices in the waypoints-vector
             */
            std::map<std::string, WaypointIndex> waypointsByName;
        };

        /**
         * @brief Adds a named waypoint to the given waynet instance
         */
        void addWaypoint(WaynetInstance& waynet, const Waypoint& wp);

        /**
         * @brief Creates a waynet from the given loaded zen-world
         */
        WaynetInstance makeWaynetFromZen(const ZenLoad::oCWorldData& zenWorld);

        /**
         * @brief Finds a way between two waypoints in the given waypoint instance
         * @return list of all waypoints that need to be visited. Will be empty if none was found.
         */
        std::vector<size_t> findWay(const WaynetInstance& waynet, WaypointIndex start, WaypointIndex end);

        /**
         * @brief Gets the interpolated position of the given percentage on the input-path
         */
        Math::float3 interpolatePositionOnPath(const WaynetInstance& waynet, const std::vector<size_t>& path, float p);

        /**
         * @brief Calculates the closest waypoint-index on the given path and percentage
         * @return The index of the waypoint in the path-vector closest to the given percentage
         */
        size_t getWaypointOnPath(const WaynetInstance& waynet, const std::vector<size_t>& path, float p);

        /**
         * @brief Returns the total length of the given path
         */
        float getPathLength(const WaynetInstance& waynet, const std::vector<size_t>& path);

        /**
         * @brief Finds the nearest waypoint to the given position
         */
        size_t findNearestWaypointTo(const WaynetInstance& waynet, const Math::float3& position);

        /**
         * @return True, if the given waypoint exists inside the waynet
         */
        inline bool waypointExists(const WaynetInstance& waynet, const std::string& wp)
        {
            return waynet.waypointsByName.find(wp) != waynet.waypointsByName.end();
        }

        /**
         * @return Index of the waypoint named like the input string
         */
        inline size_t getWaypointIndex(const WaynetInstance& waynet, const std::string& wp)
        {
            auto it = waynet.waypointsByName.find(wp);

            if (it == waynet.waypointsByName.end())
                return INVALID_WAYPOINT;

            return (*it).second;
        }
    }
}
