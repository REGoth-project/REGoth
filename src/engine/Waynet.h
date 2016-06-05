#pragma once
#include <string>
#include <map>
#include <vector>
#include <zenload/zTypes.h>
#include <math/mathlib.h>

namespace World
{
    namespace Waynet
    {
        /**
         * Note: These generally don't change, so it's okay to not use handles, but indices for them
         */

        struct Waypoint
        {
            std::string name;
            Math::float3 position;
            Math::float3 direction;
            float waterDepth;
            bool underWater;

            /**
             * Indices to all edges of this waypoint
             */
            std::vector <size_t> edges;
        };

        struct WaynetInstance
        {
            /**
             * Memory of all known waypoints
             */
            std::vector <Waypoint> waypoints;

            /**
             * Map of waypoint names to their indices in the waypoints-vector
             */
            std::map <std::string, size_t> waypointsByName;
        };

        /**
         * @brief Creates a waynet from the given loaded zen-world
         */
        WaynetInstance makeWaynetFromZen(const ZenLoad::oCWorldData& zenWorld);

        /**
         * @brief Finds a way between two waypoints in the given waypoint instance
         * @return list of all waypoints that need to be visited. Will be empty if none was found.
         */
        std::vector<size_t> findWay(const WaynetInstance& waynet, size_t start, size_t end);

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
            return (*it).second;
        }
    }
}
