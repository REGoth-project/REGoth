//
// Created by desktop on 08.09.17.
//

#include "Pathfinder.h"
#include <engine/World.h>
#include <debugdraw/debugdraw.h>
#include <stdlib.h>

using namespace Logic;

static const float MAX_SIDE_DIFFERENCE_TO_REACH_POSITION   = 0.5f; // Meters
static const float MAX_HEIGHT_DIFFERENCE_TO_REACH_POSITION = 2.0f; // Meters


Pathfinder::Pathfinder(World::WorldInstance& world)
    : m_World(world)
{

}


Pathfinder::~Pathfinder()
{

}


Pathfinder::MovementReport Pathfinder::checkMoveToLocation(const Math::float3& from, const Math::float3& to)
{
    MovementReport report;
    Math::float3 fromGround = from - Math::float3(0, m_UserConfiguration.height, 0);
    Math::float3 toGround = to - Math::float3(0, m_UserConfiguration.height, 0);

    report.lowerThanStepHeight = (to.y - from.y) < -m_UserConfiguration.stepHeight;
    report.higherThanStepHeight = (to.y - from.y) > m_UserConfiguration.stepHeight;

    report.ceilingTooLow = findCeilingHeightAtPosition(toGround) > m_UserConfiguration.height;

    Physics::RayTestResult hit = m_World.getPhysicsSystem().raytrace(from, to);
    report.hardCollision = hit.hasHit;

    size_t destGroundTriangle = getGroundTriangleIndexAt(to);

    if(destGroundTriangle != (size_t)-1)
    {
        Math::float3 vertices[3];
        uint8_t matGroup;
        m_World.getWorldMesh().getTriangle(destGroundTriangle, vertices, matGroup);

        Math::float3 normal = (vertices[0] - vertices[1]).cross(vertices[0] - vertices[2]).normalize();

        float slope = calculateSlopeFromNormal(normal);

        bool goingUp = (to - from).dot(normal) < 0.0f;

        if(fabs(slope) > m_UserConfiguration.maxSlopeAngle)
        {
            if(goingUp)  report.tooSteepUp = true;
            if(!goingUp) report.tooSteepDown = true;
        }
    } else
    {
        report.lowerThanStepHeight = true;
    }


    return report;
}


float Pathfinder::findCeilingHeightAtPosition(const Math::float3& position)
{
    Math::float3 worldTop = Math::float3(position.x, FLT_MAX, position.y);
    Physics::RayTestResult hit = m_World.getPhysicsSystem().raytrace(position, worldTop);

    if(!hit.hasHit)
        return FLT_MAX;

    return (position - hit.hitPosition).length();
}


size_t Pathfinder::getGroundTriangleIndexAt(const Math::float3& position)
{
    Math::float3 worldBottom = Math::float3(position.x, FLT_MIN, position.y);
    Physics::RayTestResult hit = m_World.getPhysicsSystem().raytrace(position, worldBottom, Physics::CollisionShape::CT_WorldMesh);

    if(!hit.hasHit)
        return (size_t)-1;

    return hit.hitTriangleIndex;
}

float Pathfinder::calculateSlopeFromNormal(const Math::float3& normal)
{
    return acos(normal.y);
}


World::Waynet::WaypointIndex Pathfinder::findNextVisibleWaypoint(const Math::float3& from)
{
    // TODO: Check for obstructions
    return World::Waynet::findNearestWaypointTo(m_World.getWaynet(), from);
}


bool Pathfinder::hasActiveRouteBeenCompleted(const Math::float3& positionNow)
{
    if(!m_ActiveRoute.targetEntity.isValid())
        return m_ActiveRoute.positionsToGo.empty(); // FIXME: This goes wrong if an npc ever gets stuck or the heights don't match

    return hasTargetEntityBeenReached(positionNow);
}


bool Pathfinder::isTargetReachedByPosition(const Math::float3& position, const Math::float3& target)
{
    float diffHeight = fabs(position.y - target.y);
    float diffSide   =  Math::float2(position.x - target.x, position.z - target.z).length();

    return diffSide < MAX_SIDE_DIFFERENCE_TO_REACH_POSITION && diffHeight < MAX_HEIGHT_DIFFERENCE_TO_REACH_POSITION;
}


Pathfinder::Instruction Pathfinder::updateToNextInstructionToTarget(const Math::float3& positionNow)
{
    Instruction inst;

    if(hasActiveRouteBeenCompleted(positionNow))
    {
        inst.targetPosition = positionNow;
        return inst;
    }

    if(hasNextRouteTargetBeenReached(positionNow))
    {
        if(!m_ActiveRoute.positionsToGo.empty())
        {
            m_ActiveRoute.positionsToGo.pop_front();
        }
    }

    inst.targetPosition = getCurrentTargetPosition();

    return inst;
}


bool Pathfinder::hasNextRouteTargetBeenReached(const Math::float3& positionNow)
{
    if(m_ActiveRoute.positionsToGo.empty())
    {
        return isTargetAnEntity() && hasTargetEntityBeenReached(positionNow);
    }

    return isTargetReachedByPosition(positionNow, m_ActiveRoute.positionsToGo.front());
}


bool Pathfinder::hasTargetEntityBeenReached(const Math::float3& positionNow)
{
    if(!isTargetAnEntity())
        return false;

    Math::float3 targetEntityPosition = getTargetEntityPosition();

    return isTargetReachedByPosition(positionNow, targetEntityPosition);
}


Math::float3 Pathfinder::getTargetEntityPosition()
{
    if(!m_ActiveRoute.targetEntity.isValid())
        return Math::float3(0,0,0);

    Vob::VobInformation vob = Vob::asVob(m_World, m_ActiveRoute.targetEntity);
    assert(vob.isValid());

    return vob.position->m_WorldMatrix.Translation();
}


bool Pathfinder::isTargetAnEntity()
{
    return m_ActiveRoute.targetEntity.isValid();
}


Math::float3 Pathfinder::getCurrentTargetPosition()
{
    if(m_ActiveRoute.positionsToGo.empty())
    {
        return getTargetEntityPosition();
    }else
    {
        return m_ActiveRoute.positionsToGo.front();
    }
}


void Pathfinder::startNewRouteTo(const Math::float3& positionNow, Handle::EntityHandle entity)
{
    using namespace World;

    // TODO: Should be re-evaluated every once in a while, since the entity could be moving
    startNewRouteTo(positionNow, getTargetEntityPosition());

    // startNewRouteTo appends the position to go to if it's off the waynet, we can't have that
    // when the target could be moving
    if(!m_ActiveRoute.positionsToGo.empty())
        m_ActiveRoute.positionsToGo.pop_back();

    m_ActiveRoute.targetEntity = entity;
}


void Pathfinder::startNewRouteTo(const Math::float3& positionNow, World::Waynet::WaypointIndex waypoint)
{
    Math::float3 waypointPosition = m_World.getWaynet().waypoints[waypoint].position;
    startNewRouteTo(positionNow, waypointPosition);
}


void Pathfinder::startNewRouteTo(const Math::float3& positionNow, const Math::float3& position)
{
    using namespace World;

    m_ActiveRoute.positionsToGo.clear();
    m_ActiveRoute.lastKnownPosition = positionNow;
    m_ActiveRoute.targetEntity.invalidate();

    if(isTargetReachedByPosition(positionNow, position))
        return;

    if(canDirectlyMovetoLocation(positionNow, position))
    {
        m_ActiveRoute.positionsToGo.push_back(position);
        return;
    }

    Waynet::WaypointIndex nearestWpToTarget = Waynet::findNearestWaypointTo(m_World.getWaynet(), getTargetEntityPosition());
    Waynet::WaypointIndex nearestWpToStart = Waynet::findNearestWaypointTo(m_World.getWaynet(), positionNow);

    std::vector<Waynet::WaypointIndex> path = Waynet::findWay(m_World.getWaynet(), nearestWpToStart, nearestWpToTarget);

    for(Waynet::WaypointIndex i : path)
    {
        const Math::float3 wpPosition = m_World.getWaynet().waypoints[i].position;
        m_ActiveRoute.positionsToGo.push_back(wpPosition);
    }

    // If the last position is off the waynet, add it as well
    if(m_ActiveRoute.positionsToGo.empty() || !isTargetReachedByPosition(m_ActiveRoute.positionsToGo.back(), position))
        m_ActiveRoute.positionsToGo.push_back(position);
}

bool Pathfinder::canDirectlyMovetoLocation(const Math::float3& from, const Math::float3& to)
{
    Physics::RayTestResult hit = m_World.getPhysicsSystem().raytrace(from, to);

    return !hit.hasHit; // FIXME: This breaks when the creature should go down a slope but is standing on the top of it right now
}
