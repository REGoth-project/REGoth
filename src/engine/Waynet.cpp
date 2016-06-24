//
// Created by andre on 31.05.16.
//

#include <utils/logger.h>
#include <set>
#include <numeric>
#include <algorithm>
#include "Waynet.h"

using namespace World;

Waynet::WaynetInstance Waynet::makeWaynetFromZen(const ZenLoad::oCWorldData& zenWorld)
{
    WaynetInstance w;

    // Copy waypoint-information
    for(const ZenLoad::zCWaypointData& zwp : zenWorld.waynet.waypoints)
    {
        Waypoint wp;
        wp.name = zwp.wpName;

        // FIXME: Only temporary, to make NPCs walk on the ground rather than IN the ground while there is no physics engine
        const float heightOffset = 0.5f;

        // FIXME: They only seem right with the world in zyx-order. Find out why!
        wp.direction = Math::float3(zwp.direction.x, zwp.direction.y, zwp.direction.z);
        wp.position = Math::float3(zwp.position.x, zwp.position.y, zwp.position.z) * (1.0f / 100.0f);
        wp.position.y += heightOffset;

        wp.waterDepth = static_cast<float>(zwp.waterDepth);
        wp.underWater = zwp.underWater;

        // Note: Edges are emplaced later

        w.waypoints.push_back(wp);
        w.waypointsByName[wp.name] = w.waypoints.size() - 1;
    }

    // Copy edges to waypoints
    for(std::pair<size_t, size_t> e : zenWorld.waynet.edges)
    {
        // Waypoints are layed out as a pair of "left" and "right" waypoints.
        // FIXME: I'm not sure whether this means that this is meant to be a directed graph or not.
        // FIXME: Going for undirected...

        w.waypoints[e.first].edges.push_back(e.second);
        w.waypoints[e.second].edges.push_back(e.first);

    }

    return w;
}

std::vector<size_t> Waynet::findWay(const WaynetInstance& waynet, size_t start, size_t end)
{
    // FIXME: This is not a very fast implementation. Improve!
    // Simple Dijkstra-Implementation. Totally non-optimized.


    //LogInfo() << "Entered function: " << start;

    // Give all other nodes a distance of infinity
    std::vector<float> distances(waynet.waypoints.size(), FLT_MAX);
    std::vector<size_t> prev(waynet.waypoints.size(), static_cast<size_t>(-1));
    std::set<size_t> unvisitedSet;

    for(size_t i=0;i<waynet.waypoints.size();i++)
        unvisitedSet.insert(i);

    // Init startnode with a distance of 0
    distances[start] = 0.0f;
    size_t cn = start;

    //LogInfo() << "Starting: " << cn;

    do
    {

        for (size_t e : waynet.waypoints[cn].edges)
        {
            if (unvisitedSet.find(e) != unvisitedSet.end())
            {
                // Check if this actually was a shorter path
                float tentativeDist =
                        distances[cn] + (waynet.waypoints[cn].position - waynet.waypoints[e].position).lengthSquared();
                if (distances[e] > tentativeDist)
                {
                    distances[e] = tentativeDist;
                    prev[e] = cn;
                }
            }
        }

        //LogInfo() << "Visited: " << cn;

        unvisitedSet.erase(cn);

        if(!unvisitedSet.empty())
        {
            size_t smallest = *unvisitedSet.begin();
            for (size_t n : unvisitedSet)
            {
                if (distances[smallest] > distances[n])
                {
                    smallest = n;
                }
            }

            cn = smallest;
        }


    }while(unvisitedSet.find(end) != unvisitedSet.end() && cn != static_cast<size_t>(-1) && cn != end);

    // No path found
    if(cn == static_cast<size_t>(-1))
        return std::vector<size_t>();

    // Put path together
    std::vector<size_t> path;
    cn = end;

    while(prev[cn] != static_cast<size_t>(-1))
    {
        path.push_back(cn);
        cn = prev[cn];
    }

    // Happens on short paths
    if(!path.empty() && path.back() != start)
        path.push_back(start);

    std::reverse(path.begin(), path.end());

    /*LogInfo() << " ---------- PATH ----------";
    for(size_t p : path)
    {
        LogInfo() << " - " << waynet.waypoints[p].name;
    }*/

    return path;
}

Math::float3 World::Waynet::interpolatePositionOnPath(const WaynetInstance& waynet, const std::vector<size_t>& path, float p)
{
    if(path.size() == 1)
        return waynet.waypoints[path[0]].position;

    if(p >= 1.0f)
        return waynet.waypoints[path[path.size()-1]].position;

    float pathLength = getPathLength(waynet, path);
    float traveled = pathLength * p;

    float d = 0.0f;
    for(size_t i=0;i<path.size()-1;i++)
    {
        // Check if the percentage-value is inside the current region
        float len = (waynet.waypoints[path[i]].position - waynet.waypoints[path[i+1]].position).length();

        if(d <= traveled && traveled < d + len)
        {
            //LogInfo() << "Segment: " << i << "/" << path.size() << " at " << d << ", need " << traveled << "(Total: " << pathLength << ")";
            // Found our segment!
            // Upscale our percentage to this single segment
            p = (traveled - d) / len;
            //p = (p - d) / lenNorm;
            return Math::float3::lerp(waynet.waypoints[path[i]].position,
                                      waynet.waypoints[path[i+1]].position, p);
        }

        d += len;
    }

    // FIXME: Throw an exception or something?
    return waynet.waypoints[path[0]].position;
}

float World::Waynet::getPathLength(const WaynetInstance& waynet, const std::vector<size_t>& path)
{
    if(path.size() <= 1)
        return 0.0f;

    float d = 0.0f;
    for(size_t i=0;i<path.size()-1;i++)
    {
        d += (waynet.waypoints[path[i]].position - waynet.waypoints[path[i+1]].position).length();
    }

    return d;
}

size_t World::Waynet::getWaypointOnPath(const WaynetInstance& waynet, const std::vector<size_t>& path, float p)
{
    if(path.size() == 1)
        return 0;

    if(p >= 1.0f)
        return path.size()-1;

    float pathLength = getPathLength(waynet, path);
    float traveled = pathLength * p;

    float d = 0.0f;
    for(size_t i=0;i<path.size()-1;i++)
    {
        // Check if the percentage-value is inside the current region
        float len = (waynet.waypoints[path[i]].position - waynet.waypoints[path[i+1]].position).length();

        if(d <= traveled && traveled < d + len)
        {
            float mid = (d + d + len) * 0.5f;
            return traveled < mid ? i : i+1;
        }

        d += len;
    }

    return static_cast<size_t>(-1);
}

size_t World::Waynet::findNearestWaypointTo(const WaynetInstance& waynet, const Math::float3& position)
{
    size_t nearest= static_cast<size_t>(-1);
    float nearestLen = FLT_MAX;

    for(size_t i=0;i<waynet.waypoints.size();i++)
    {
        float chk = (position - waynet.waypoints[i].position).lengthSquared();
        if(chk < nearestLen)
        {
            nearestLen = chk;
            nearest = i;
        }
    }

    return nearest;
}







