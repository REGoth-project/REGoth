//
// Created by andre on 02.06.16.
//

#include "PlayerController.h"
#include <engine/Waynet.h>
#include <components/Entities.h>
#include <engine/World.h>
#include <debugdraw/debugdraw.h>
#include <render/WorldRender.h>
#include <components/Vob.h>
#include <utils/logger.h>
#include <entry/input.h>
#include "visuals/ModelVisual.h"

/**
 * Standard node-names
 */
namespace BodyNodes
{
    const char* NPC_NODE_RIGHTHAND	= "ZS_RIGHTHAND";
    const char* NPC_NODE_LEFTHAND	= "ZS_LEFTHAND";
    const char* NPC_NODE_SWORD		= "ZS_SWORD";
    const char* NPC_NODE_LONGSWORD	= "ZS_LONGSWORD";
    const char* NPC_NODE_BOW		= "ZS_BOW";
    const char* NPC_NODE_CROSSBOW	= "ZS_CROSSBOW";
    const char* NPC_NODE_SHIELD		= "ZS_SHIELD";
    const char* NPC_NODE_HELMET		= "ZS_HELMET";
    const char* NPC_NODE_JAWS		= "ZS_JAWS";
    const char* NPC_NODE_TORSO		= "ZS_TORSO";
}


Logic::PlayerController::PlayerController(World::WorldInstance& world,
                                          Handle::EntityHandle entity,
                                          Daedalus::GameState::NpcHandle scriptInstance)
        : Controller(world, entity),
          m_Inventory(*world.getEngine(), world.getMyHandle(), scriptInstance)
{
    m_RoutineState.routineTarget = static_cast<size_t>(-1);
    m_RoutineState.routineActive = true;

    m_AIState.closestWaypoint = 0;
    m_MoveState.currentPathPerc = 0;
    m_NPCProperties.moveSpeed = 5.0f;

    m_MoveState.direction = Math::float3(1,0,0);
    m_MoveState.position = Math::float3(0,0,0);

    m_ScriptState.npcHandle = scriptInstance;
}

void Logic::PlayerController::onUpdate(float deltaTime)
{
    ModelVisual* model = getModelVisual();

    if(!m_MoveState.currentPath.empty() || !m_RoutineState.routineWaypoints.empty())
    {
        // Do waypoint-actions
        if (travelPath(deltaTime))
        {
            // Path done, stop animation
            if (model)
                model->setAnimation(ModelVisual::Idle);

            if (m_RoutineState.routineActive)
                continueRoutine();
        }
    }

    if(model)
    {
        // Make sure the idle-animation is running
        if(!model->getAnimationHandler().getActiveAnimationPtr())
        {
            model->setAnimation(ModelVisual::Idle);
        }

        // Update model for this frame
        model->onFrameUpdate(deltaTime);
    }
}

void Logic::PlayerController::continueRoutine()
{
    if(m_RoutineState.routineWaypoints.empty())
        return;

    // Start next route
    if (m_RoutineState.routineTarget == static_cast<size_t>(-1))
        m_RoutineState.routineTarget = 0; // Start routing. Could let the integer overflow do this, but this is better.
    else
        m_RoutineState.routineTarget++;

    // Start over, if done
    if (m_RoutineState.routineTarget + 1 >= m_RoutineState.routineWaypoints.size())
        m_RoutineState.routineTarget = 0;

    gotoWaypoint(m_RoutineState.routineWaypoints[m_RoutineState.routineTarget]);
}

void Logic::PlayerController::teleportToWaypoint(size_t wp)
{
    m_AIState.closestWaypoint = wp;

    Math::Matrix transform = Math::Matrix::CreateLookAt(m_World.getWaynet().waypoints[wp].position,
                                                        m_World.getWaynet().waypoints[wp].position +
                                                                m_World.getWaynet().waypoints[wp].direction,
                                                         Math::float3(0, 1, 0)).Invert();

    m_MoveState.direction = m_World.getWaynet().waypoints[wp].direction;
    m_MoveState.position = m_World.getWaynet().waypoints[wp].position;

    setEntityTransform(transform);

    placeOnGround();


}

void Logic::PlayerController::gotoWaypoint(size_t wp)
{
    if(wp == m_AIState.targetWaypoint)
        return;

    m_AIState.targetWaypoint = wp;

    // Route is most likely outdated, make a new one
    rebuildRoute();
}

void Logic::PlayerController::rebuildRoute()
{
    m_MoveState.currentPath = World::Waynet::findWay(m_World.getWaynet(),
                                                     m_AIState.closestWaypoint,
                                                     m_AIState.targetWaypoint);

    m_MoveState.currentPathPerc = 0.0f;
    m_MoveState.currentRouteLength = World::Waynet::getPathLength(m_World.getWaynet(), m_MoveState.currentPath);
    m_MoveState.targetNode = 0;
}

bool Logic::PlayerController::travelPath(float deltaTime)
{
    if (m_MoveState.currentPath.empty())
        return true;

    Math::float3 targetPosition = m_World.getWaynet().waypoints[m_MoveState.currentPath[m_MoveState.targetNode]].position;

    Math::float3 currentPosition = getEntityTransform().Translation();
    Math::float3 differenceXZ = targetPosition - currentPosition;

    // Remove angle
    differenceXZ.y = 0.0f;
    if(differenceXZ.lengthSquared() > 0.0f)
    {
        Math::float3 direction = differenceXZ;
        direction.normalize();

        m_MoveState.direction = direction;

        direction *= deltaTime * m_NPCProperties.moveSpeed;
        m_MoveState.position = currentPosition + direction;

        // Move
        setEntityTransform(Math::Matrix::CreateLookAt(currentPosition + direction,
                                                      currentPosition + direction * 2,
                                                      Math::float3(0, 1, 0)).Invert());

    }

    placeOnGround();

    // Set run animation
    getModelVisual()->setAnimation(ModelVisual::Run);

    if(differenceXZ.lengthSquared() < 0.5f) // TODO: Find a nice setting for this
    {
        if(abs(currentPosition.y - targetPosition.y) < 2.0f)
        {
            m_MoveState.targetNode++;

            if(m_MoveState.targetNode >= m_MoveState.currentPath.size())
            {
                m_MoveState.currentPath.clear();
                return true;
            }
        }
    }

    return false;

    // Frame update...
    /*m_MoveState.currentPathPerc = std::min(1.0f, m_MoveState.currentPathPerc
                                                 + deltaTime
                                                   * (m_NPCProperties.moveSpeed / m_MoveState.currentRouteLength));

    // Get new position
    Math::float3 lastPosition = getEntityTransform().Translation();

    Math::float3 position = World::Waynet::interpolatePositionOnPath(m_World.getWaynet(),
                                                                     m_MoveState.currentPath,
                                                                     m_MoveState.currentPathPerc);

    // TODO: Merge this with the interpolate-call!
    size_t cwp = World::Waynet::getWaypointOnPath(m_World.getWaynet(),
                                                  m_MoveState.currentPath,
                                                  m_MoveState.currentPathPerc);

    if(cwp != static_cast<size_t>(-1))
        m_AIState.closestWaypoint = m_MoveState.currentPath[cwp];

    // FIXME: This should be taken care of by the physics engine
    Math::float3 direction = position - lastPosition;

    // Remove angle
    direction.y = 0.0f;
    direction = direction.normalize();

    setEntityTransform(Math::Matrix::CreateLookAt(position, position + direction, Math::float3(0,1,0)).Invert());

    placeOnGround();



    if (m_MoveState.currentPathPerc >= 1.0f) // == would be sufficient here, but why not?
        return true;

    return false;*/
}

void Logic::PlayerController::onDebugDraw()
{
    /*if (!m_MoveState.currentPath.empty())
    {
        Render::debugDrawPath(m_World.getWaynet(), m_MoveState.currentPath);
    }*/


    /*Math::float3 to = getEntityTransform().Translation() + Math::float3(0.0f, -100.0f, 0.0f);
    Math::float3 from = getEntityTransform().Translation() + Math::float3(0.0f, 0.0f, 0.0f);

    Physics::RayTestResult hit = m_World.getPhysicsSystem().raytrace(from, to, Physics::CollisionShape::CT_WorldMesh);

    if (hit.hasHit)
    {
        ddDrawAxis(hit.hitPosition.x, hit.hitPosition.y, hit.hitPosition.z);

        float shadow = m_World.getWorldMesh().interpolateTriangleShadowValue(hit.hitTriangleIndex, hit.hitPosition);

        if(getModelVisual())
            getModelVisual()->setShadowValue(shadow);

        Math::float3 v3[3];
        m_World.getWorldMesh().getTriangle(hit.hitTriangleIndex, v3);

        for(int i=0;i<3;i++)
        {
            ddDrawAxis(v3[i].x, v3[i].y, v3[i].z);
        }
    }*/
}

void Logic::PlayerController::equipItem(Daedalus::GameState::ItemHandle item)
{
    // Get item
    Daedalus::GEngineClasses::C_Item& itemData = m_World.getScriptEngine().getGameState().getItem(item);

    if(!itemData.visual_change.empty())
    {
        Vob::VobInformation vob = Vob::asVob(m_World, m_Entity);

        // TODO: Only replace the mesh loaded in the lib to keep attachments, etc?
        Vob::setVisual(vob, itemData.visual_change);
    }
}

Logic::ModelVisual* Logic::PlayerController::getModelVisual()
{
    Vob::VobInformation vob = Vob::asVob(m_World, m_Entity);

    // TODO: Bring in some type-checking here
    return reinterpret_cast<ModelVisual*>(vob.visual);
}

void Logic::PlayerController::placeOnGround()
{
    // Fix position
    Math::float3 to = getEntityTransform().Translation() + Math::float3(0.0f, -100.0f, 0.0f);
    Math::float3 from = getEntityTransform().Translation() + Math::float3(0.0f, 0.0f, 0.0f);

    Physics::RayTestResult hit = m_World.getPhysicsSystem().raytrace(from, to);


    if (hit.hasHit)
    {
        Math::Matrix m = getEntityTransform();

        float feet = m_NPCProperties.modelRoot.y;

        // FIXME: Actually read the flying-flag of the MDS
        if (feet == 0.0f)
        {
            feet = 0.8f;
        }

        m_MoveState.position = hit.hitPosition + Math::float3(0.0f, feet, 0.0f);
        m.Translation(m_MoveState.position);
        setEntityTransform(m);
    }

    // FIXME: Get rid of the second cast here or at least only do it on the worldmesh!
    Physics::RayTestResult hitwm = m_World.getPhysicsSystem().raytrace(from, to, Physics::CollisionShape::CT_WorldMesh);
    if(hitwm.hasHit)
    {
        // Update color
        float shadow = m_World.getWorldMesh().interpolateTriangleShadowValue(hitwm.hitTriangleIndex, hitwm.hitPosition);

        if(getModelVisual())
            getModelVisual()->setShadowValue(shadow);
    }
}

void Logic::PlayerController::onVisualChanged()
{
    getModelVisual()->getCollisionBBox(m_NPCProperties.collisionBBox);
    m_NPCProperties.modelRoot = getModelVisual()->getModelRoot();
}

void Logic::PlayerController::onUpdateByInput(float deltaTime)
{
    ModelVisual* model = getModelVisual();

    if(!model)
        return;

    if (inputGetKeyState(entry::Key::KeyA))
    {
        model->setAnimation(ModelVisual::EModelAnimType::StrafeLeft);
    } else if (inputGetKeyState(entry::Key::KeyD))
    {
        model->setAnimation(ModelVisual::EModelAnimType::StrafeRight);
    } else if (inputGetKeyState(entry::Key::KeyW))
    {
        model->setAnimation(ModelVisual::EModelAnimType::Run);
    } else if (inputGetKeyState(entry::Key::KeyS))
    {
        model->setAnimation(ModelVisual::EModelAnimType::Backpedal);
    } else if (inputGetKeyState(entry::Key::KeyQ))
    {
        model->setAnimation(ModelVisual::EModelAnimType::AttackFist);
    } else {
        model->setAnimation(ModelVisual::Idle);
    }

    float yaw = 0.0f;
    const float turnSpeed = 2.5f;
    if (inputGetKeyState(entry::Key::Left))
    {
        yaw -= turnSpeed * deltaTime;
    } else if (inputGetKeyState(entry::Key::Right))
    {
        yaw += turnSpeed * deltaTime;
    }

    // TODO: HACK, take this out!
    if(inputGetKeyState(entry::Key::Space))
        deltaTime *= 4.0f;

    // Apply animation-velocity
    Math::float3 rootNodeVel = model->getAnimationHandler().getRootNodeVelocity() * deltaTime;



    float angle = atan2(m_MoveState.direction.z, m_MoveState.direction.x);
    m_MoveState.direction = Math::float3(cos(angle + yaw), 0, sin(angle + yaw));
    angle = atan2(m_MoveState.direction.z, m_MoveState.direction.x);


    m_MoveState.position += Math::Matrix::CreateRotationY(angle + yaw) * rootNodeVel;

    Math::Matrix newTransform = Math::Matrix::CreateTranslation(m_MoveState.position) * Math::Matrix::CreateRotationY(angle + yaw);
    setEntityTransform(newTransform);

    placeOnGround();
}
