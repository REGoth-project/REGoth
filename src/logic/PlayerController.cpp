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
#include <components/VobClasses.h>
#include "visuals/ModelVisual.h"
#include <engine/BaseEngine.h>
#include <stdlib.h>
#include <content/AudioEngine.h>
#include <engine/Input.h>

using namespace Logic;

// TODO: HACK, remove!
static bool s_action_triggered = false;

/**
 * Standard node-names
 */
namespace BodyNodes
{
    const char* NPC_NODE_RIGHTHAND = "ZS_RIGHTHAND";
    const char* NPC_NODE_LEFTHAND = "ZS_LEFTHAND";
    const char* NPC_NODE_SWORD = "ZS_SWORD";
    const char* NPC_NODE_LONGSWORD = "ZS_LONGSWORD";
    const char* NPC_NODE_BOW = "ZS_BOW";
    const char* NPC_NODE_CROSSBOW = "ZS_CROSSBOW";
    const char* NPC_NODE_SHIELD = "ZS_SHIELD";
    const char* NPC_NODE_HELMET = "ZS_HELMET";
    const char* NPC_NODE_JAWS = "ZS_JAWS";
    const char* NPC_NODE_TORSO = "ZS_TORSO";
}

#define SINGLE_ACTION_KEY(key, fn) { \
    static bool last = false; \
    if(inputGetKeyState(key) && !last)\
        last = true; \
    else if(!inputGetKeyState(key) && last){\
        last = false;\
        fn();\
    } }

PlayerController::PlayerController(World::WorldInstance& world,
                                   Handle::EntityHandle entity,
                                   Daedalus::GameState::NpcHandle scriptInstance)
        : Controller(world, entity),
          m_Inventory(*world.getEngine(), world.getMyHandle(), scriptInstance),
          m_AIStateMachine(world, entity)
{
    m_RoutineState.routineTarget = static_cast<size_t>(-1);
    m_RoutineState.routineActive = true;

    m_AIState.closestWaypoint = 0;
    m_MoveState.currentPathPerc = 0;
    m_NPCProperties.moveSpeed = 7.0f;

    m_MoveState.direction = Math::float3(1, 0, 0);
    m_MoveState.position = Math::float3(0, 0, 0);
    m_AIState.targetWaypoint = World::Waynet::INVALID_WAYPOINT;
    m_AIState.closestWaypoint = World::Waynet::INVALID_WAYPOINT;

    m_ScriptState.npcHandle = scriptInstance;

    m_EquipmentState.weaponMode = EWeaponMode::WeaponNone;
    m_EquipmentState.activeWeapon.invalidate();

    m_isDrawWeaponMelee = false;
    m_isForward = false;
    m_isBackward = false;
    m_isTurnLeft = false;
    m_isTurnRight = false;
    m_isStrafeLeft = false;
    m_isStrafeRight = false;


}

void PlayerController::onUpdate(float deltaTime)
{
    // This vob should react to messages
    getEM().processMessageQueue();

    m_AIStateMachine.doAIState(deltaTime);

    ModelVisual* model = getModelVisual();

    // Build the route to follow this entity
    if (m_RoutineState.entityTarget.isValid())
    {
        Math::float3 targetPos = m_World.getEntity<Components::PositionComponent>(m_RoutineState.entityTarget)
                .m_WorldMatrix.Translation();

        // FIXME: Doing this every frame is too often
        size_t targetWP = World::Waynet::findNearestWaypointTo(m_World.getWaynet(), targetPos);

        setDailyRoutine({});
        gotoWaypoint(targetWP);
    }

    if (!m_MoveState.currentPath.empty() || !m_RoutineState.routineWaypoints.empty())
    {
        // Do waypoint-actions
        if (travelPath(deltaTime))
        {
            // Path done, stop animation
            //if (model)
            //    model->setAnimation(ModelVisual::Idle);

            if (m_RoutineState.routineActive)
                continueRoutine();
        }
    }

    if (model)
    {
        // Make sure the idle-animation is running
        if (!model->getAnimationHandler().getActiveAnimationPtr())
        {
            model->setAnimation(ModelVisual::Idle);
        }

        // Update model for this frame
        model->onFrameUpdate(deltaTime);
    }

    // TODO: HACK, take this out!
    // Make following NPCs a bit faster...
    /*if(m_RoutineState.entityTarget.isValid())
    {
        float defSpeed = 7.0f;
        if (inputGetKeyState(entry::Key::Space))
            m_NPCProperties.moveSpeed = defSpeed * 4;
        else if (inputGetKeyState(entry::Key::KeyB))
            m_NPCProperties.moveSpeed = defSpeed * 8;
        else
            m_NPCProperties.moveSpeed = defSpeed;
    }*/
}

void PlayerController::continueRoutine()
{
    if (m_RoutineState.routineWaypoints.empty())
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

void PlayerController::teleportToWaypoint(size_t wp)
{

    m_AIState.closestWaypoint = wp;

    teleportToPosition(m_World.getWaynet().waypoints[wp].position);

    setDirection(m_World.getWaynet().waypoints[wp].direction);
}

void PlayerController::teleportToPosition(const Math::float3& pos)
{
    m_MoveState.position = pos;

    // This also update the entity transform from m_MoveState.position
    setDirection(m_MoveState.direction);

    placeOnGround();

    // Start with idle-animation
    getModelVisual()->setAnimation(ModelVisual::Idle);
}

void PlayerController::gotoWaypoint(size_t wp)
{
    if (wp == m_AIState.targetWaypoint)
        return;

    // If this is the first ever waypoint this NPC has to go to, just teleport him there
    // FIXME: This is a hack to get around daily routines not doing that yet
    if (m_AIState.closestWaypoint == World::Waynet::INVALID_WAYPOINT)
    {
        teleportToWaypoint(wp);
        return;
    }

    // Set our current target as closest so we don't move back if we are following an entity
    if (m_AIState.targetWaypoint != World::Waynet::INVALID_WAYPOINT)
        m_AIState.closestWaypoint = m_AIState.targetWaypoint;
    else
        m_AIState.closestWaypoint = World::Waynet::findNearestWaypointTo(m_World.getWaynet(),
                                                                         getEntityTransform().Translation());

    m_AIState.targetWaypoint = wp;

    // Route is most likely outdated, make a new one
    rebuildRoute();
}

void PlayerController::rebuildRoute()
{

    m_MoveState.currentPath = World::Waynet::findWay(m_World.getWaynet(),
                                                     m_AIState.closestWaypoint,
                                                     m_AIState.targetWaypoint);

    m_MoveState.currentPathPerc = 0.0f;
    m_MoveState.currentRouteLength = World::Waynet::getPathLength(m_World.getWaynet(), m_MoveState.currentPath);
    m_MoveState.targetNode = 0;

    // Update script-instance with target waypoint
    getScriptInstance().wp = m_World.getWaynet().waypoints[m_AIState.targetWaypoint].name;
}

bool PlayerController::travelPath(float deltaTime)
{
    if (m_MoveState.currentPath.empty())
        return true;

    Math::float3 targetPosition = m_World.getWaynet().waypoints[m_MoveState.currentPath[m_MoveState.targetNode]].position;

    Math::float3 currentPosition = getEntityTransform().Translation();
    Math::float3 differenceXZ = targetPosition - currentPosition;

    // Remove angle
    differenceXZ.y = 0.0f;
    if (differenceXZ.lengthSquared() > 0.0f)
    {
        Math::float3 direction = differenceXZ;
        direction.normalize();

        m_MoveState.direction = direction;

        // FIXME: This is right, but somehow NPCs don't appear where they belong
        //m_NPCProperties.moveSpeed = getModelVisual()->getAnimationHandler().getRootNodeVelocity().length();

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

    if (differenceXZ.lengthSquared() < 0.5f) // TODO: Find a nice setting for this
    {
        //if(abs(currentPosition.y - targetPosition.y) < 2.0f)
        {
            m_AIState.closestWaypoint = m_MoveState.currentPath[m_MoveState.targetNode];
            m_MoveState.targetNode++;

            if (m_MoveState.targetNode >= m_MoveState.currentPath.size())
            {
                m_MoveState.currentPath.clear();
                return true;
            }
        }
    }

    return false;
}

void PlayerController::onDebugDraw()
{
    if (!m_MoveState.currentPath.empty())
    {
        Render::debugDrawPath(m_World.getWaynet(), m_MoveState.currentPath);
    }


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

    if (isPlayerControlled())
    {
        VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(m_World, m_Entity);
        Daedalus::GEngineClasses::C_Npc& scriptnpc = VobTypes::getScriptObject(npc);

        bgfx::dbgTextPrintf(0, 6, 0x0f, "Level: %d", scriptnpc.level);
        bgfx::dbgTextPrintf(0, 7, 0x0f, "XP   : %d", scriptnpc.exp);
    }
}

void PlayerController::equipItem(Daedalus::GameState::ItemHandle item)
{
    // Get item
    Daedalus::GEngineClasses::C_Item& itemData = m_World.getScriptEngine().getGameState().getItem(item);
    ModelVisual* model = getModelVisual();

    EModelNode node = EModelNode::None;

    // TODO: Don't forget if an item is already equipped before executing stat changing script-code!

    // Put into set of all equipped items first, then differentiate between the item-types
    m_EquipmentState.equippedItemsAll.insert(item);

    if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_2HD_AXE) != 0)
    {
        node = EModelNode::Longsword;

        // Take of any 1h weapon
        m_EquipmentState.equippedItems.equippedWeapon1h.invalidate();

        // Put on our 2h weapon
        m_EquipmentState.equippedItems.equippedWeapon2h = item;
    } else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_2HD_SWD) != 0)
    {
        node = EModelNode::Longsword;

        // Take of any 1h weapon
        m_EquipmentState.equippedItems.equippedWeapon1h.invalidate();

        // Put on our 2h weapon
        m_EquipmentState.equippedItems.equippedWeapon2h = item;
    } else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_CROSSBOW) != 0)
    {
        node = EModelNode::Crossbow;

        // Take off a bow
        m_EquipmentState.equippedItems.equippedBow.invalidate();

        // Put on our crossbow
        m_EquipmentState.equippedItems.equippedCrossBow = item;
    } else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_BOW) != 0)
    {
        node = EModelNode::Bow;

        // Take off a crossbow
        m_EquipmentState.equippedItems.equippedCrossBow.invalidate();

        // Put on our bow
        m_EquipmentState.equippedItems.equippedBow = item;
    } else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_SWD) != 0)
    {
        node = EModelNode::Sword;

        // Take of any 2h weapon
        m_EquipmentState.equippedItems.equippedWeapon2h.invalidate();

        // Put on our 1h weapon
        m_EquipmentState.equippedItems.equippedWeapon1h = item;
    } else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_AXE) != 0)
    {
        node = EModelNode::Sword;

        // Take of any 2h weapon
        m_EquipmentState.equippedItems.equippedWeapon2h.invalidate();

        // Put on our 1h weapon
        m_EquipmentState.equippedItems.equippedWeapon1h = item;
    } else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_AMULET) != 0)
    {
        node = EModelNode::None;

        // Put on our amulet
        m_EquipmentState.equippedItems.equippedAmulet = item;
    } else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_RING) != 0)
    {
        node = EModelNode::None;

        // Put on our ring
        m_EquipmentState.equippedItems.equippedRings.insert(item);
    } else if ((itemData.mainflag & Daedalus::GEngineClasses::C_Item::ITM_CAT_RUNE) != 0
               || (itemData.mainflag & Daedalus::GEngineClasses::C_Item::ITM_CAT_MAGIC) != 0)
    {
        node = EModelNode::None;

        // Put on our ring
        m_EquipmentState.equippedItems.equippedRunes.insert(item);
    }

    // Show visual on the npc-model
    if (node != EModelNode::None)
        model->setNodeVisual(itemData.visual, node);
}

Daedalus::GameState::ItemHandle PlayerController::drawWeaponMelee()
{
    // Check if we already have a weapon in our hands
    if (m_EquipmentState.weaponMode != EWeaponMode::WeaponNone)
        return m_EquipmentState.activeWeapon;

    ModelVisual* model = getModelVisual();
    ModelVisual::EModelAnimType drawingAnimation = ModelVisual::EModelAnimType::Idle;

    // Remove anything that was active before putting something new there
    m_EquipmentState.activeWeapon.invalidate();

    // Check what kind of weapon we got here
    if (m_EquipmentState.equippedItems.equippedWeapon1h.isValid())
    {
        drawingAnimation = ModelVisual::EModelAnimType::Draw1h;
        m_EquipmentState.activeWeapon = m_EquipmentState.equippedItems.equippedWeapon1h;
        m_EquipmentState.weaponMode = EWeaponMode::Weapon1h;
    } else if (m_EquipmentState.equippedItems.equippedWeapon2h.isValid())
    {
        drawingAnimation = ModelVisual::EModelAnimType::Draw2h;
        m_EquipmentState.activeWeapon = m_EquipmentState.equippedItems.equippedWeapon2h;
        m_EquipmentState.weaponMode = EWeaponMode::Weapon2h;
    } else
    {
        drawingAnimation = ModelVisual::EModelAnimType::DrawFist;
        m_EquipmentState.weaponMode = EWeaponMode::WeaponFist;
    }

    // Move the visual
    if (m_EquipmentState.activeWeapon.isValid())
    {
        // Get actual data of the weapon we are going to draw
        Daedalus::GEngineClasses::C_Item& itemData = m_World.getScriptEngine().getGameState().getItem(
                m_EquipmentState.activeWeapon);

        // Clear the possible on-body-visuals first
        model->setNodeVisual("", EModelNode::Lefthand);
        model->setNodeVisual("", EModelNode::Righthand);
        model->setNodeVisual("", EModelNode::Sword);
        model->setNodeVisual("", EModelNode::Longsword);

        // Put visual into hand
        // TODO: Listen to ani-events for this!
        model->setNodeVisual(itemData.visual, EModelNode::Righthand);
    }

    // Play drawing animation
    model->playAnimation(drawingAnimation);

    // Couldn't draw anything
    return m_EquipmentState.activeWeapon;
}

void PlayerController::undrawWeapon(bool force)
{
    ModelVisual* model = getModelVisual();

    // TODO: Listen to ani-events for this!
    // TODO: Even do an animation for this!
    // TODO: Implement force-flag

    // Clear hands
    model->setNodeVisual("", EModelNode::Lefthand);
    model->setNodeVisual("", EModelNode::Righthand);
    m_EquipmentState.weaponMode = EWeaponMode::WeaponNone;

    // activeWeapon should be only invalid when using fists
    if (m_EquipmentState.activeWeapon.isValid())
    {
        // reequip the currently active item
        equipItem(m_EquipmentState.activeWeapon);

        // Remove active weapon
        m_EquipmentState.activeWeapon.invalidate();
    }

}

ModelVisual* PlayerController::getModelVisual()
{
    Vob::VobInformation vob = Vob::asVob(m_World, m_Entity);

    // TODO: Bring in some type-checking here
    return reinterpret_cast<ModelVisual*>(vob.visual);
}

void PlayerController::placeOnGround()
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
    if (hitwm.hasHit)
    {
        // Update color
        float shadow = m_World.getWorldMesh().interpolateTriangleShadowValue(hitwm.hitTriangleIndex, hitwm.hitPosition);

        if (getModelVisual())
            getModelVisual()->setShadowValue(shadow);
    }
}

void PlayerController::onVisualChanged()
{
    getModelVisual()->getCollisionBBox(m_NPCProperties.collisionBBox);
    m_NPCProperties.modelRoot = getModelVisual()->getModelRoot();
}

void PlayerController::onUpdateByInput(float deltaTime)
{

    ModelVisual* model = getModelVisual();

    if (!model)
        return;



    // FIXME: Temporary test-code
    static bool lastDraw = false;

    /*
#define SINGLE_ACTION_KEY(key, fn) { \
    static bool last = false; \
    if(inputGetKeyState(key) && !last)\
        last = true; \
    else if(!inputGetKeyState(key) && last){\
        last = false;\
        fn();\
    } }

    SINGLE_ACTION_KEY(entry::Key::KeyK, [&](){
        // Let all near NPCs draw their weapon
        std::set<Handle::EntityHandle> nearNPCs = m_World.getScriptEngine().getNPCsInRadius(getEntityTransform().Translation(), 10.0f);

        for(const Handle::EntityHandle& h : nearNPCs)
        {
            VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(m_World, h);
            VobTypes::NPC_DrawMeleeWeapon(npc);

            npc.playerController->setDailyRoutine({}); // FIXME: Idle-animation from routine finish overwriting other animations!
        }
    });

    SINGLE_ACTION_KEY(entry::Key::KeyJ, [&](){
        // Let all near NPCs draw their weapon
        std::set<Handle::EntityHandle> nearNPCs = m_World.getScriptEngine().getNPCsInRadius(getEntityTransform().Translation(), 10.0f);

        for(const Handle::EntityHandle& h : nearNPCs)
        {
            VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(m_World, h);
            VobTypes::NPC_UndrawWeapon(npc);
        }
    });

    SINGLE_ACTION_KEY(entry::Key::KeyH, [&](){
        // Let all near NPCs draw their weapon
        std::set<Handle::EntityHandle> nearNPCs = m_World.getScriptEngine().getNPCsInRadius(getEntityTransform().Translation(), 10.0f);

        for(const Handle::EntityHandle& h : nearNPCs)
        {
            VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(m_World, h);
            npc.playerController->attackFront();
        }
    });
    */
    if (m_isDrawWeaponMelee)
    {
        if (!lastDraw)
        {
            lastDraw = true;

            if (m_EquipmentState.activeWeapon.isValid())
                undrawWeapon();
            else
                drawWeaponMelee();
        }

        // Don't overwrite the drawing animation
        return;
    } else if (!m_isDrawWeaponMelee && lastDraw)
    {
        lastDraw = false;
    }


    if (m_EquipmentState.weaponMode == EWeaponMode::WeaponNone)
    {
        static std::string lastMovementAni = "";
        if (m_isStrafeLeft)
        {
            model->setAnimation(ModelVisual::EModelAnimType::StrafeLeft);
            lastMovementAni = getModelVisual()->getAnimationHandler().getActiveAnimationPtr()->getModelAniHeader().aniName;
        } else if (m_isStrafeRight)
        {
            model->setAnimation(ModelVisual::EModelAnimType::StrafeRight);
            lastMovementAni = getModelVisual()->getAnimationHandler().getActiveAnimationPtr()->getModelAniHeader().aniName;
        } else if (m_isForward)
        {
            model->setAnimation(ModelVisual::EModelAnimType::Run);
            lastMovementAni = getModelVisual()->getAnimationHandler().getActiveAnimationPtr()->getModelAniHeader().aniName;
        } else if (m_isBackward)
        {
            model->setAnimation(ModelVisual::EModelAnimType::Backpedal);
            lastMovementAni = getModelVisual()->getAnimationHandler().getActiveAnimationPtr()->getModelAniHeader().aniName;
        }
//		else if(inputGetKeyState(entry::Key::KeyQ))
//		{
//			model->setAnimation(ModelVisual::EModelAnimType::AttackFist);
//		}
        else if (getModelVisual()->getAnimationHandler().getActiveAnimationPtr()
                 && getModelVisual()->getAnimationHandler().getActiveAnimationPtr()->getModelAniHeader().aniName ==
                    lastMovementAni)
        {
            model->setAnimation(ModelVisual::Idle);
        }
    }
//	else
//	{
//        std::map<EWeaponMode, std::vector<ModelVisual::EModelAnimType>> aniMap =
//                {
//                        {EWeaponMode::Weapon1h, {       ModelVisual::EModelAnimType::Attack1h_L,
//                                                        ModelVisual::EModelAnimType::Attack1h_R,
//                                                        ModelVisual::EModelAnimType::Run1h,
//                                                        ModelVisual::EModelAnimType::Backpedal1h,
//                                                        ModelVisual::EModelAnimType::Attack1h,
//                                                        ModelVisual::EModelAnimType::Idle1h}},

//                        {EWeaponMode::Weapon2h, {       ModelVisual::EModelAnimType::Attack2h_L,
//                                                        ModelVisual::EModelAnimType::Attack2h_R,
//                                                        ModelVisual::EModelAnimType::Run2h,
//                                                        ModelVisual::EModelAnimType::Backpedal2h,
//                                                        ModelVisual::EModelAnimType::Attack2h,
//                                                        ModelVisual::EModelAnimType::Idle2h}},

//                        {EWeaponMode::WeaponBow, {      ModelVisual::EModelAnimType::IdleBow,
//                                                        ModelVisual::EModelAnimType::IdleBow,
//                                                        ModelVisual::EModelAnimType::RunBow,
//                                                        ModelVisual::EModelAnimType::BackpedalBow,
//                                                        ModelVisual::EModelAnimType::AttackBow,
//                                                        ModelVisual::EModelAnimType::IdleBow}},

//                        {EWeaponMode::WeaponCrossBow, { ModelVisual::EModelAnimType::IdleCBow,
//                                                        ModelVisual::EModelAnimType::IdleCBow,
//                                                        ModelVisual::EModelAnimType::RunCBow,
//                                                        ModelVisual::EModelAnimType::BackpedalCBow,
//                                                        ModelVisual::EModelAnimType::AttackCBow,
//                                                        ModelVisual::EModelAnimType::IdleCBow}}
//                };

//		if(inputGetKeyState(entry::Key::KeyA))
//		{
//			model->setAnimation(aniMap[m_EquipmentState.weaponMode][0]);
//		}
//		else if(inputGetKeyState(entry::Key::KeyD))
//		{
//            model->setAnimation(aniMap[m_EquipmentState.weaponMode][1]);
//		}
//		else if(inputGetKeyState(entry::Key::KeyW))
//		{
//            model->setAnimation(aniMap[m_EquipmentState.weaponMode][2]);
//		}
//		else if(inputGetKeyState(entry::Key::KeyS))
//		{
//            model->setAnimation(aniMap[m_EquipmentState.weaponMode][3]);
//		}
//		else if(inputGetKeyState(entry::Key::KeyQ))
//		{
//            model->setAnimation(aniMap[m_EquipmentState.weaponMode][4]);
//		}
//		else {
//            model->setAnimation(aniMap[m_EquipmentState.weaponMode][5]);
//		}
//	}

    float yaw = 0.0f;
    const float turnSpeed = 2.5f;
    if (m_isTurnLeft)
    {
        yaw += turnSpeed * deltaTime;
    } else if (m_isTurnRight)
    {
        yaw -= turnSpeed * deltaTime;
    }

    // TODO: HACK, take this out!
    if (m_MoveSpeed1)
        deltaTime *= 4.0f;

    if (m_MoveSpeed2)
        deltaTime *= 16.0f;



    // Apply animation-velocity
    Math::float3 rootNodeVel = model->getAnimationHandler().getRootNodeVelocity() * deltaTime;

    float angle = atan2(m_MoveState.direction.z, m_MoveState.direction.x);
    m_MoveState.direction = Math::float3(cos(angle + yaw), 0, sin(angle + yaw));
    angle = atan2(m_MoveState.direction.z, m_MoveState.direction.x);

    m_MoveState.position += Math::Matrix::CreateRotationY(-angle + Math::PI * 0.5f) * rootNodeVel;

    setDirection(m_MoveState.direction);

    //Math::Matrix newTransform = Math::Matrix::CreateTranslation(m_MoveState.position) * Math::Matrix::CreateRotationY(angle + yaw);
    //setEntityTransform(newTransform);

    placeOnGround();

}

void PlayerController::attackFront()
{
    if (m_EquipmentState.weaponMode == EWeaponMode::WeaponNone)
        return;

    ModelVisual::EModelAnimType type = ModelVisual::EModelAnimType::NUM_ANIMATIONS;
    switch (m_EquipmentState.weaponMode)
    {
        case EWeaponMode::Weapon1h:
            type = ModelVisual::EModelAnimType::Attack1h;
            break;
        case EWeaponMode::Weapon2h:
            type = ModelVisual::EModelAnimType::Attack2h;
            break;
        case EWeaponMode::WeaponBow:
            type = ModelVisual::EModelAnimType::AttackBow;
            break;
        case EWeaponMode::WeaponCrossBow:
            type = ModelVisual::EModelAnimType::AttackCBow;
            break;
        case EWeaponMode::WeaponFist:
            type = ModelVisual::EModelAnimType::AttackFist;
            break;
            //case EWeaponMode::WeaponMagic: type = ModelVisual::EModelAnimType::AttackMagic; break; // TODO: Magic
        default:
            break;
    }

    if (type != ModelVisual::EModelAnimType::NUM_ANIMATIONS)
        getModelVisual()->playAnimation(type);
}

void PlayerController::attackLeft()
{
    if (m_EquipmentState.weaponMode == EWeaponMode::WeaponNone)
        return;

    ModelVisual::EModelAnimType type = ModelVisual::EModelAnimType::NUM_ANIMATIONS;
    switch (m_EquipmentState.weaponMode)
    {
        case EWeaponMode::Weapon1h:
            type = ModelVisual::EModelAnimType::Attack1h_L;
            break;
        case EWeaponMode::Weapon2h:
            type = ModelVisual::EModelAnimType::Attack2h_L;
            break;
        default:
            break;
    }

    if (type != ModelVisual::EModelAnimType::NUM_ANIMATIONS)
        getModelVisual()->playAnimation(type);
}

void PlayerController::attackRight()
{
    if (m_EquipmentState.weaponMode == EWeaponMode::WeaponNone)
        return;

    ModelVisual::EModelAnimType type = ModelVisual::EModelAnimType::NUM_ANIMATIONS;
    switch (m_EquipmentState.weaponMode)
    {
        case EWeaponMode::Weapon1h:
            type = ModelVisual::EModelAnimType::Attack1h_R;
            break;
        case EWeaponMode::Weapon2h:
            type = ModelVisual::EModelAnimType::Attack2h_L;
            break;
        default:
            break;
    }

    if (type != ModelVisual::EModelAnimType::NUM_ANIMATIONS)
        getModelVisual()->playAnimation(type);
}

void PlayerController::onMessage(EventMessages::EventMessage& message, Handle::EntityHandle sourceVob)
{
    Controller::onMessage(message, sourceVob);

    bool done = false;
    switch (message.messageType)
    {
        case EventMessages::EventMessageType::Event:
            done = EV_Event(message, sourceVob);
            break;
        case EventMessages::EventMessageType::Npc:
            done = EV_Npc(reinterpret_cast<EventMessages::NpcMessage&>(message), sourceVob);
            break;
        case EventMessages::EventMessageType::Damage:
            done = EV_Damage(reinterpret_cast<EventMessages::DamageMessage&>(message), sourceVob);
            break;
        case EventMessages::EventMessageType::Weapon:
            done = EV_Weapon(reinterpret_cast<EventMessages::WeaponMessage&>(message), sourceVob);
            break;
        case EventMessages::EventMessageType::Movement:
            done = EV_Movement(reinterpret_cast<EventMessages::MovementMessage&>(message), sourceVob);
            break;
        case EventMessages::EventMessageType::Attack:
            done = EV_Attack(reinterpret_cast<EventMessages::AttackMessage&>(message), sourceVob);
            break;
        case EventMessages::EventMessageType::UseItem:
            done = EV_UseItem(reinterpret_cast<EventMessages::UseItemMessage&>(message), sourceVob);
            break;
        case EventMessages::EventMessageType::State:
            done = EV_State(reinterpret_cast<EventMessages::StateMessage&>(message), sourceVob);
            break;
        case EventMessages::EventMessageType::Manipulate:
            done = EV_Manipulate(reinterpret_cast<EventMessages::ManipulateMessage&>(message), sourceVob);
            break;
        case EventMessages::EventMessageType::Conversation:
            done = EV_Conversation(reinterpret_cast<EventMessages::ConversationMessage&>(message), sourceVob);
            break;
        case EventMessages::EventMessageType::Magic:
            done = EV_Magic(reinterpret_cast<EventMessages::MagicMessage&>(message), sourceVob);
            break;
    }

    // Flag as deleted if this is done
    message.deleted = done;
}

bool PlayerController::EV_Event(EventMessages::EventMessage& message, Handle::EntityHandle sourceVob)
{
    return false;
}

bool PlayerController::EV_Npc(EventMessages::NpcMessage& message, Handle::EntityHandle sourceVob)
{
    return false;
}

bool PlayerController::EV_Damage(EventMessages::DamageMessage& message, Handle::EntityHandle sourceVob)
{
    return false;
}

bool PlayerController::EV_Weapon(EventMessages::WeaponMessage& message, Handle::EntityHandle sourceVob)
{
    return false;
}

bool
PlayerController::EV_Movement(EventMessages::MovementMessage& message, Handle::EntityHandle sourceVob)
{
    switch (static_cast<EventMessages::MovementMessage::MovementSubType>(message.subType))
    {
        case EventMessages::MovementMessage::ST_RobustTrace:
            break;
        case EventMessages::MovementMessage::ST_GotoFP:
        {
            if (!message.targetVob.isValid())
            {
                std::vector<Handle::EntityHandle> fp = m_World.getFreepointsInRange(getEntityTransform().Translation(),
                                                                                    100.0f, message.targetVobName, true,
                                                                                    m_Entity);

                if (fp.empty())
                    return true; // End message on invalid freepoint

                Components::PositionComponent& pos = m_World.getEntity<Components::PositionComponent>(fp.front());
                message.targetPosition = pos.m_WorldMatrix.Translation();

                // Find closest position from the waynet
                World::Waynet::WaypointIndex wp = World::Waynet::findNearestWaypointTo(m_World.getWaynet(),
                                                                                       message.targetPosition);

                if (wp != World::Waynet::INVALID_WAYPOINT)
                    gotoWaypoint(wp);
            }

            if (m_MoveState.currentPath.empty())
            {
                // Just teleport to position // TODO: Implement properly
                teleportToPosition(message.targetPosition);

                // Back to idle-animation when done
                getModelVisual()->setAnimation(ModelVisual::Idle);
                return true;
            }

            return false;
        }
            break;

        case EventMessages::MovementMessage::ST_GotoVob:
        {
            Math::float3 pos;

            // Find a waypoint with that name
            World::Waynet::WaypointIndex wp = World::Waynet::getWaypointIndex(m_World.getWaynet(),
                                                                              message.targetVobName);

            if (wp != World::Waynet::INVALID_WAYPOINT)
            {
                gotoWaypoint(wp);

                if (m_MoveState.currentPath.empty())
                {
                    // Back to idle-animation when done
                    getModelVisual()->setAnimation(ModelVisual::Idle);
                    return true;
                }

                return false;
            } else
            {
                // This must be an actual vob
                Handle::EntityHandle v = m_World.getVobEntityByName(message.targetVobName);

                if (v.isValid())
                {

                    Vob::VobInformation vob = Vob::asVob(m_World, v);
                    message.targetPosition = Vob::getTransform(vob).Translation();

                    // Fall through to ST_GotoPos
                }
            }
        }

        case EventMessages::MovementMessage::ST_GotoPos:
        {
            // Move to nearest waypoint for now
            // TODO: Implement moving to arbitrary positions

            World::Waynet::WaypointIndex wp = World::Waynet::findNearestWaypointTo(m_World.getWaynet(),
                                                                                   message.targetPosition);

            if (wp != World::Waynet::INVALID_WAYPOINT)
                gotoWaypoint(wp);

            if (m_MoveState.currentPath.empty())
            {
                // Back to idle-animation when done
                getModelVisual()->setAnimation(ModelVisual::Idle);
                return true;
            }

            return false;
        }
            break;

        case EventMessages::MovementMessage::ST_GoRoute:
            break;
        case EventMessages::MovementMessage::ST_Turn:
            break;

        case EventMessages::MovementMessage::ST_TurnToVob:
        {
            Vob::VobInformation vob = Vob::asVob(m_World, message.targetVob);

            // Fill position-field
            message.targetPosition = Vob::getTransform(vob).Translation();

            // Fall through to ST_TurnToPos now
        }

        case EventMessages::MovementMessage::ST_TurnToPos:
        {
            Math::float3 dir = (message.targetPosition - getEntityTransform().Translation());
            dir.y = 0.0f; // Don't let the NPC face upwards/downwards
            dir.normalize();

            // Just snap to the direction for now
            // FIXME: Play animation
            setDirection(dir);
            return true;
        }
            break;

        case EventMessages::MovementMessage::ST_TurnAway:
            break;
        case EventMessages::MovementMessage::ST_Jump:
            break;
        case EventMessages::MovementMessage::ST_SetWalkMode:
            break;
        case EventMessages::MovementMessage::ST_WhirlAround:
            break;
        case EventMessages::MovementMessage::ST_Standup:
            standUp();
            return true;
            break;

        case EventMessages::MovementMessage::ST_CanSeeNpc:
            break; // Unused
        case EventMessages::MovementMessage::ST_Strafe:
            break;
        case EventMessages::MovementMessage::ST_Dodge:
            break;
        case EventMessages::MovementMessage::ST_BeamTo:
            break;
        case EventMessages::MovementMessage::ST_AlignToFP:
            break;
        case EventMessages::MovementMessage::ST_MoveMax:
            break;
    }

    return false;
}

bool PlayerController::EV_Attack(EventMessages::AttackMessage& message, Handle::EntityHandle sourceVob)
{
    return false;
}

bool PlayerController::EV_UseItem(EventMessages::UseItemMessage& message, Handle::EntityHandle sourceVob)
{
    return false;
}

bool PlayerController::EV_State(EventMessages::StateMessage& message, Handle::EntityHandle sourceVob)
{
    switch (message.subType)
    {
        case EventMessages::StateMessage::EV_StartState:
        {
            if (!message.wpname.empty())
            {
                // Set wp of script instance
                getScriptInstance().wp = message.wpname;
            }

            // Set up script instances. // TODO: Self is originally not set by gothic here! Why?
            m_World.getScriptEngine().setInstance("self", getScriptInstance().instanceSymbol);
            m_World.getScriptEngine().setInstanceNPC("other", message.other);
            m_World.getScriptEngine().setInstanceNPC("victim", message.victim);

            getEM().clear();

            if (message.functionSymbol != 0)
            {
                m_AIStateMachine.startAIState(message.functionSymbol, true, message.isRoutineState);
            } else
            {
                // Start daily routine
                m_AIStateMachine.startRoutineState();
            }
        }
            return true;
            break;

        case EventMessages::StateMessage::EV_Wait:
            message.waitTime -= static_cast<float>(m_World.getWorldInfo().lastFrameDeltaTime);
            return message.waitTime < 0.0f;
            break;

        case EventMessages::StateMessage::EV_SetNpcsToState:
            break;
        case EventMessages::StateMessage::EV_SetTime:
            break;
        case EventMessages::StateMessage::EV_ApplyTimedOverlay:
            break;
        case EventMessages::StateMessage::EV_StateMax:
            break;
        default:
            break;
    }
    return false;
}

bool
PlayerController::EV_Manipulate(EventMessages::ManipulateMessage& message, Handle::EntityHandle sourceVob)
{
    return false;
}

bool PlayerController::EV_Conversation(EventMessages::ConversationMessage& message,
                                       Handle::EntityHandle sourceVob)
{
    switch (static_cast<EventMessages::ConversationMessage::ConversationSubType>(message.subType))
    {

        case EventMessages::ConversationMessage::ST_PlayAniSound:
            break;

        case EventMessages::ConversationMessage::ST_PlayAni:
        {
            ZenLoad::zCModelAni* active = getModelVisual()->getAnimationHandler().getActiveAnimationPtr();

            if (!message.internInProgress)
            {
                getModelVisual()->setAnimation(message.animation, false);
                active = getModelVisual()->getAnimationHandler().getActiveAnimationPtr();
                message.internInProgress = true;
            }

            // Go as long as this animation is playing
            bool done = !active
                        || active->getModelAniHeader().aniName != message.animation;

            return done;
        }
            break;

        case EventMessages::ConversationMessage::ST_PlaySound:
            break;

        case EventMessages::ConversationMessage::ST_LookAt:
            break;

        case EventMessages::ConversationMessage::ST_Output:
        {
            m_World.getDialogManager().displaySubtitle(message.text, message.name);

            // TODO: Rework this, when the animation-system is nicer. Need a cutscene system!
            if (!message.internInProgress)
            {
                // Don't let the routine overwrite our animations
                setDailyRoutine({});

                // Play the random dialog gesture
                startDialogAnimation();

                // Play sound of this conv-message
                m_World.getAudioEngine().playSound(message.name);

                message.internInProgress = true;
            }


            bool done = s_action_triggered;
            /*SINGLE_ACTION_KEY(entry::Key::KeyR, [&](){
                done = true;
                m_World.getDialogManager().stopDisplaySubtitle();
            });*/

            if(done)
            {
                m_World.getDialogManager().stopDisplaySubtitle();
            }

            return done;
        }
            break;

        case EventMessages::ConversationMessage::ST_OutputSVM:
            break;

        case EventMessages::ConversationMessage::ST_Cutscene:
            break;
        case EventMessages::ConversationMessage::ST_WaitTillEnd:
            break;
        case EventMessages::ConversationMessage::ST_Ask:
            break;
        case EventMessages::ConversationMessage::ST_WaitForQuestion:
            break;
        case EventMessages::ConversationMessage::ST_StopLookAt:
            break;
        case EventMessages::ConversationMessage::ST_StopPointAt:
            break;
        case EventMessages::ConversationMessage::ST_PointAt:
            break;
        case EventMessages::ConversationMessage::ST_QuickLook:
            break;
        case EventMessages::ConversationMessage::ST_PlayAni_NoOverlay:
            break;
        case EventMessages::ConversationMessage::ST_PlayAni_Face:
            break;
        case EventMessages::ConversationMessage::ST_ProcessInfos:
            break;
        case EventMessages::ConversationMessage::ST_StopProcessInfos:
            break;
        case EventMessages::ConversationMessage::ST_OutputSVM_Overlay:
            break;
        case EventMessages::ConversationMessage::ST_SndPlay:
            break;
        case EventMessages::ConversationMessage::ST_SndPlay3d:
            break;
        case EventMessages::ConversationMessage::ST_PrintScreen:
            break;
        case EventMessages::ConversationMessage::ST_StartFx:
            break;
        case EventMessages::ConversationMessage::ST_StopFx:
            break;
        case EventMessages::ConversationMessage::ST_ConvMax:
            break;
    }

    return false;
}

bool PlayerController::EV_Magic(EventMessages::MagicMessage& message, Handle::EntityHandle sourceVob)
{
    return false;
}

void PlayerController::setDirection(const Math::float3& direction)
{
    m_MoveState.direction = direction;

    // Set direction
    setEntityTransform(Math::Matrix::CreateLookAt(m_MoveState.position,
                                                  m_MoveState.position + direction,
                                                  Math::float3(0, 1, 0)).Invert());
}

bool PlayerController::isPlayerControlled()
{
    return m_World.getScriptEngine().getPlayerEntity() == m_Entity;
}

void PlayerController::startDialogAnimation()
{
    // TODO: Check for: Empty hands, no weapon, not sitting
    if (!getModelVisual())
        return;

    const int NUM_DIALOG_ANIMATIONS = 20; // This is hardcoded in the game
    size_t num = (rand() % NUM_DIALOG_ANIMATIONS) + 1;

    std::string ns = std::to_string(num);
    if (ns.size() == 1)
        ns = "0" + ns;

    getModelVisual()->setAnimation("T_DIALOGGESTURE_" + ns, false);
}

Daedalus::GEngineClasses::C_Npc& PlayerController::getScriptInstance()
{
    return m_World.getScriptEngine().getGameState().getNpc(getScriptHandle());
}

void PlayerController::interrupt()
{
    // TODO: More! Cancel all animations, etc
    undrawWeapon(true);
}

bool PlayerController::canSee(Handle::EntityHandle entity, bool ignoreAngles)
{
    const float MAX_ANGLE = 0.5f * Math::PI; // 90 degrees

    Components::PositionComponent& otherPos = m_World.getEntity<Components::PositionComponent>(entity);

    // Trace from the top of our BBox (eyes)
    Math::float3 start = getEntityTransform().Translation()
                         + Math::float3(0.0f, m_NPCProperties.collisionBBox[1].y, 0.0f);

    Math::float3 end = otherPos.m_WorldMatrix.Translation();

    // Check senses_range first
    float len2 = (end - start).length();

    unsigned int sensesRange = static_cast<unsigned int >(getScriptInstance().senses_range);
    if (static_cast<uint32_t>(len2) > sensesRange * sensesRange)
        return false;

    // Do the raytest to the other object
    Physics::RayTestResult res = m_World.getPhysicsSystem().raytrace(
            start,
            end,
            Physics::CollisionShape::CT_WorldMesh); // FIXME: Should trace everything except the two objects in question!

    if (!res.hasHit)
    {
        if (ignoreAngles)
            return true;

        // Check face-direction
        return getAngleTo(end) <= MAX_ANGLE;
    }

    return false;
}

float PlayerController::getAngleTo(const Math::float3& pos)
{
    Math::float3 dir = (getEntityTransform().Translation() - pos).normalize();

    return atan(m_MoveState.direction.dot(dir));
}

void PlayerController::standUp(bool walkingAllowed, bool startAniTransition)
{
    // TODO: Implement properly
    stopRoute();

}

void PlayerController::stopRoute()
{
    m_MoveState.currentPath.clear();
    m_RoutineState.routineActive = false;
    m_RoutineState.entityTarget.invalidate();
}

void PlayerController::setRoutineFunc(size_t symRoutine)
{
    getScriptInstance().daily_routine = static_cast<uint32_t>(symRoutine);
    m_AIStateMachine.reinitRoutine();
}

void PlayerController::changeRoutine(const std::string& routineName)
{
    // Build new name for the routine
    std::string namecomp = "RTN_" + routineName + "_" + std::to_string(getScriptInstance().id);

    if (m_World.getScriptEngine().hasSymbol(namecomp))
        setRoutineFunc(m_World.getScriptEngine().getSymbolIndexByName(namecomp));
    else
        LogWarn() << "Could not find routine " << namecomp << " on NPC: " << getScriptInstance().name[0];
}

bool PlayerController::isNpcReady()
{
    // TODO: Implement
    return true;
}

void PlayerController::setupKeyBindings()
{
    Engine::Input::RegisterAction(Engine::ActionType::PlayerDrawWeaponMelee, [this](bool triggered, float) {
        m_isDrawWeaponMelee = triggered;
    });
    Engine::Input::RegisterAction(Engine::ActionType::PlayerForward, [this](bool triggered, float) {
        m_isForward = triggered;
    });
    Engine::Input::RegisterAction(Engine::ActionType::PlayerBackward, [this](bool triggered, float) {
        m_isBackward = triggered;
    });
    Engine::Input::RegisterAction(Engine::ActionType::PlayerTurnLeft, [this](bool triggered, float) {
        m_isTurnLeft = triggered;
    });
    Engine::Input::RegisterAction(Engine::ActionType::PlayerTurnRight, [this](bool triggered, float) {
        m_isTurnRight = triggered;
    });
    Engine::Input::RegisterAction(Engine::ActionType::PlayerStrafeLeft, [this](bool triggered, float) {
        m_isStrafeLeft = triggered;
    });
    Engine::Input::RegisterAction(Engine::ActionType::PlayerStrafeRight, [this](bool triggered, float) {
        m_isStrafeRight = triggered;
    });

    Engine::Input::RegisterAction(Engine::ActionType::DebugMoveSpeed, [this](bool triggered, float intensity) {
        m_MoveSpeed1 = triggered;
    });

    Engine::Input::RegisterAction(Engine::ActionType::DebugMoveSpeed2, [this](bool triggered, float intensity) {
        m_MoveSpeed2 = triggered;
    });

    Engine::Input::RegisterAction(Engine::ActionType::PlayerAction, [this](bool triggered, float intensity) {
        static bool s_triggered = false;

        s_action_triggered = false;

        if (s_triggered && !triggered)
            s_triggered = false;
        else if (!s_triggered && triggered)
        {
            s_triggered = true;
            s_action_triggered = true; // Set true for one frame

            if(m_World.getDialogManager().isDialogActive())
                return;

            std::set<Handle::EntityHandle> nearNPCs = m_World.getScriptEngine().getNPCsInRadius(
                    getEntityTransform().Translation(), 10.0f);

            // Talk to the nearest NPC other than the current player, of course
            Handle::EntityHandle nearest;
            float shortestDist = FLT_MAX;
            for (const Handle::EntityHandle& h : nearNPCs)
            {
                if (h != m_World.getScriptEngine().getPlayerEntity())
                {
                    VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(m_World, h);

                    float dist = (Vob::getTransform(npc).Translation() -
                                  getEntityTransform().Translation()).lengthSquared();
                    if (dist < shortestDist)
                    {
                        nearest = h;
                        shortestDist = dist;
                    }
                }
            }

            if (nearest.isValid())
            {
                VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(m_World, nearest);
                Daedalus::GameState::NpcHandle shnpc = VobTypes::getScriptHandle(npc);
                m_World.getDialogManager().startDialog(shnpc);
            }
        }
    });

}


