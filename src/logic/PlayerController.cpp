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
#include "MobController.h"
#include <engine/BaseEngine.h>
#include <stdlib.h>
#include <content/AudioEngine.h>
#include <engine/Input.h>
#include "ItemController.h"
#include <json.hpp>
#include <ui/Hud.h>
#include <ui/Menu_Status.h>

using json = nlohmann::json;
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
          m_Inventory(world, scriptInstance),
          m_AIStateMachine(world, entity)
{
    m_RoutineState.routineTarget = static_cast<size_t>(-1);
    m_RoutineState.routineActive = true;

    m_AIState.closestWaypoint = 0;
    m_MoveState.currentPathPerc = 0;
    m_NPCProperties.moveSpeed = 7.0f;
    m_NPCProperties.enablePhysics = true;

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

    m_LastAniRootPosUpdatedAniHash = 0;
    m_NoAniRootPosHack = false;

    setBodyState(BS_STAND);
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

        // Needs to be done here to account for changes of feet-height
        placeOnGround();

        if(!m_NoAniRootPosHack
           && m_LastAniRootPosUpdatedAniHash != getModelVisual()->getAnimationHandler().getAnimationStateHash())
        {
            // Apply model root-velcoity
            Math::float3 position = getEntityTransform().Translation();
            position += getEntityTransform().Rotate(getModelVisual()->getAnimationHandler().getRootNodeVelocity());

            Math::Matrix t = getEntityTransform();
            t.Translation(position);
            setEntityTransform(t);

            m_LastAniRootPosUpdatedAniHash = getModelVisual()->getAnimationHandler().getAnimationStateHash();
        }
    }

    // Run extra stuff if this is the controlled character
    if(isPlayerControlled())
    {
        onUpdateForPlayer(deltaTime);
    }
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

    if (isPlayerControlled())
    {
        VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(m_World, m_Entity);
        Daedalus::GEngineClasses::C_Npc& scriptnpc = VobTypes::getScriptObject(npc);

        // Print inventory
        const std::list<Daedalus::GameState::ItemHandle>& items = m_Inventory.getItems();

        uint16_t idx=20;
        bgfx::dbgTextPrintf(0, idx++, 0x0f, "Inventory:");
        for(Daedalus::GameState::ItemHandle i : items)
        {
            Daedalus::GEngineClasses::C_Item idata = m_World.getScriptEngine().getGameState().getItem(i);

            if(idata.count[0] > 1)
                bgfx::dbgTextPrintf(0, idx++, 0x0f, " %s [%d]" , idata.name.c_str(), idata.count[0]);
            else
                bgfx::dbgTextPrintf(0, idx++, 0x0f, " %s", idata.name.c_str());

        }
    }
}

void PlayerController::unequipItem(Daedalus::GameState::ItemHandle item)
{
    // Get item
    Daedalus::GEngineClasses::C_Item& itemData = m_World.getScriptEngine().getGameState().getItem(item);
    ModelVisual* model = getModelVisual();

    EModelNode node = EModelNode::None;

    if((itemData.flags & Daedalus::GEngineClasses::C_Item::ITM_CAT_EQUIPABLE) != Daedalus::GEngineClasses::C_Item::ITM_CAT_EQUIPABLE)
        return; // Can't equip

    // TODO: Don't forget if an item is already unequipped before executing stat changing script-code!

    // Put into set of all equipped items first, then differentiate between the item-types
    m_EquipmentState.equippedItemsAll.erase(item);

    if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_2HD_AXE) != 0)
    {
        node = EModelNode::Longsword;

        // Take off 2h weapon
        m_EquipmentState.equippedItems.equippedWeapon2h.invalidate();
    } else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_2HD_SWD) != 0)
    {
        node = EModelNode::Longsword;

        // Take off 2h weapon
        m_EquipmentState.equippedItems.equippedWeapon2h.invalidate();
    } else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_CROSSBOW) != 0)
    {
        node = EModelNode::Crossbow;

        // Take off crossbow
        m_EquipmentState.equippedItems.equippedCrossBow.invalidate();
    } else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_BOW) != 0)
    {
        node = EModelNode::Bow;

        // Take off bow
        m_EquipmentState.equippedItems.equippedBow.invalidate();
    } else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_SWD) != 0)
    {
        node = EModelNode::Sword;

        // Take off 1h weapon
        m_EquipmentState.equippedItems.equippedWeapon1h.invalidate();
    } else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_AXE) != 0)
    {
        node = EModelNode::Sword;

        // Take off 1h weapon
        m_EquipmentState.equippedItems.equippedWeapon1h.invalidate();
    } else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_AMULET) != 0)
    {
        node = EModelNode::None;

        // Take off amulet
        m_EquipmentState.equippedItems.equippedAmulet.invalidate();
    } else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_RING) != 0)
    {
        node = EModelNode::None;

        // Take off ring
        m_EquipmentState.equippedItems.equippedRings.erase(item);
    } else if ((itemData.mainflag & Daedalus::GEngineClasses::C_Item::ITM_CAT_RUNE) != 0
               || (itemData.mainflag & Daedalus::GEngineClasses::C_Item::ITM_CAT_MAGIC) != 0)
    {
        node = EModelNode::None;

        // Take off our rune/scroll
        m_EquipmentState.equippedItems.equippedRunes.erase(item);
    }

    // Show visual on the npc-model
    if (node != EModelNode::None)
        model->setNodeVisual("", node);
}


void PlayerController::equipItem(Daedalus::GameState::ItemHandle item)
{
    // Get item
    Daedalus::GEngineClasses::C_Item& itemData = m_World.getScriptEngine().getGameState().getItem(item);
    ModelVisual* model = getModelVisual();

    if((itemData.flags & Daedalus::GEngineClasses::C_Item::ITM_CAT_EQUIPABLE) != Daedalus::GEngineClasses::C_Item::ITM_CAT_EQUIPABLE)
        return; // Can't equip

    EModelNode node = EModelNode::None;

    // TODO: Don't forget if an item is already equipped before executing stat changing script-code!

    // Put into set of all equipped items first, then differentiate between the item-types
    m_EquipmentState.equippedItemsAll.insert(item);

    if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_2HD_AXE) != 0)
    {
        node = EModelNode::Longsword;

        // Take of any 1h weapon
        m_EquipmentState.equippedItems.equippedWeapon1h.invalidate();
        model->setNodeVisual("", EModelNode::Sword);

        // Put on our 2h weapon
        m_EquipmentState.equippedItems.equippedWeapon2h = item;
    } else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_2HD_SWD) != 0)
    {
        node = EModelNode::Longsword;

        // Take of any 1h weapon
        m_EquipmentState.equippedItems.equippedWeapon1h.invalidate();
        model->setNodeVisual("", EModelNode::Sword);

        // Put on our 2h weapon
        m_EquipmentState.equippedItems.equippedWeapon2h = item;
    } else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_CROSSBOW) != 0)
    {
        node = EModelNode::Crossbow;

        // Take off a bow
        m_EquipmentState.equippedItems.equippedBow.invalidate();
        model->setNodeVisual("", EModelNode::Bow);

        // Put on our crossbow
        m_EquipmentState.equippedItems.equippedCrossBow = item;
    } else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_BOW) != 0)
    {
        node = EModelNode::Bow;

        // Take off a crossbow
        m_EquipmentState.equippedItems.equippedCrossBow.invalidate();
        model->setNodeVisual("", EModelNode::Crossbow);

        // Put on our bow
        m_EquipmentState.equippedItems.equippedBow = item;
    } else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_SWD) != 0)
    {
        node = EModelNode::Sword;

        // Take of any 2h weapon
        m_EquipmentState.equippedItems.equippedWeapon2h.invalidate();
        model->setNodeVisual("", EModelNode::Longsword);

        // Put on our 1h weapon
        m_EquipmentState.equippedItems.equippedWeapon1h = item;
    } else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_AXE) != 0)
    {
        node = EModelNode::Sword;

        // Take of any 2h weapon
        m_EquipmentState.equippedItems.equippedWeapon2h.invalidate();
        model->setNodeVisual("", EModelNode::Longsword);

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

        // Put on our rune/scroll
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
    if(!m_NPCProperties.enablePhysics)
        return;

    // Check for states
    /*switch(getBodyState())
    {
        case BS_STAND:break;
        case BS_WALK:break;
        case BS_SNEAK:break;
        case BS_RUN:break;
        case BS_SPRINT:break;
        case BS_SWIM:break;
        case BS_CRAWL:break;
        case BS_DIVE:break;
        case BS_JUMP:break;
        case BS_CLIMB:break;
        case BS_FALL:break;
        case BS_SIT:break;
        case BS_LIE:break;
        case BS_INVENTORY:break;
        case BS_ITEMINTERACT:break;
        case BS_MOBINTERACT:break;
        case BS_MOBINTERACT_INTERRUPT:break;
        case BS_TAKEITEM:break;
        case BS_DROPITEM:break;
        case BS_THROWITEM:break;
        case BS_PICKPOCKET:break;
        case BS_STUMBLE:break;
        case BS_UNCONSCIOUS:return; // Animation takes care of that. Would fall through the ground for some reason otherwise.
        case BS_DEAD:break;
        case BS_AIMNEAR:break;
        case BS_AIMFAR:break;
        case BS_HIT:break;
        case BS_PARADE:break;
        case BS_CASTING:break;
        case BS_PETRIFIED:break;
        case BS_CONTROLLING:break;
        case BS_MAX:break;
    }*/

    // Fix position
    Math::float3 to = getEntityTransform().Translation() + Math::float3(0.0f, -100.0f, 0.0f);
    Math::float3 from = getEntityTransform().Translation() + Math::float3(0.0f, 1.0f, 0.0f);

    if(to == from)
        return; // FIXME: This happens if an NPC falls out of the world

    Physics::RayTestResult hit = m_World.getPhysicsSystem().raytrace(from, to);

    if(!hit.hasHit)
    {
        // Try again, but from further above
        from += Math::float3(0.0f, 10000.0f, 0.0f);
        hit = m_World.getPhysicsSystem().raytrace(from, to); 
    }

    if (hit.hasHit)
    {
        Math::Matrix m = getEntityTransform();

        float feet = getModelVisual()->getModelRoot().y;

        // FIXME: Actually read the flying-flag of the MDS
        if (feet == 0.0f)
        {
            feet = 0.8f;
        }

        m_MoveState.position = hit.hitPosition + Math::float3(0.0f, feet, 0.0f);
        m.Translation(m_MoveState.position);
        setEntityTransform(m);
        setDirection(m_MoveState.direction);
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

    getModelVisual()->setTransient(true); // Don't export this from here. Will be rebuilt after loading anyways.
}

void PlayerController::onUpdateByInput(float deltaTime)
{

    ModelVisual* model = getModelVisual();

    if (!model)
        return;

    if(m_World.getDialogManager().isDialogActive())
        return;

    // Stand up if wounded and forward is pressed
    if(getModelVisual()->isAnimPlaying("S_WOUNDEDB") && getBodyState() == EBodyState::BS_UNCONSCIOUS)
    {
        // Only stand up if the unconscious-state has ended (aka. is not valid anymore)
        // Otherwise, the player would fall down immediately
        if(m_isForward && m_AIStateMachine.isStateActive())
        {
            // FIXME: End UNCONSCIOUS-state here

            getEM().onMessage(EventMessages::ConversationMessage::playAnimation("T_WOUNDEDB_2_STAND"));
            setBodyState(EBodyState::BS_STAND);
        }
    }

    if(!getEM().isEmpty() || getUsedMob().isValid())
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

    m_NoAniRootPosHack = false;
    if (m_EquipmentState.weaponMode == EWeaponMode::WeaponNone)
    {
        static std::string lastMovementAni = "";
        if (m_isStrafeLeft)
        {
            model->setAnimation(ModelVisual::EModelAnimType::StrafeLeft);
            lastMovementAni = getModelVisual()->getAnimationHandler().getActiveAnimationPtr()->getModelAniHeader().aniName;
            m_NoAniRootPosHack = true;
        } else if (m_isStrafeRight)
        {
            model->setAnimation(ModelVisual::EModelAnimType::StrafeRight);
            lastMovementAni = getModelVisual()->getAnimationHandler().getActiveAnimationPtr()->getModelAniHeader().aniName;
            m_NoAniRootPosHack = true;
        } else if (m_isForward)
        {
            model->setAnimation(ModelVisual::EModelAnimType::Run);
            lastMovementAni = getModelVisual()->getAnimationHandler().getActiveAnimationPtr()->getModelAniHeader().aniName;
            m_NoAniRootPosHack = true;
        } else if (m_isBackward)
        {
            model->setAnimation(ModelVisual::EModelAnimType::Backpedal);
            lastMovementAni = getModelVisual()->getAnimationHandler().getActiveAnimationPtr()->getModelAniHeader().aniName;
            m_NoAniRootPosHack = true;
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
            m_NoAniRootPosHack = true;
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

    if(!m_AIState.usedMob.isValid())
    {
        if (m_isTurnLeft)
        {
            yaw += turnSpeed * deltaTime;
            m_NoAniRootPosHack = true;
        } else if (m_isTurnRight)
        {
            yaw -= turnSpeed * deltaTime;
            m_NoAniRootPosHack = true;
        }
    }

    // No direction key pressed
    if(!m_NoAniRootPosHack)
        return;


    // TODO: HACK, take this out!
    float moveMod = 1.0f;
    if (m_MoveSpeed1)
        moveMod *= 4.0f;

    if (m_MoveSpeed2)
        moveMod *= 16.0f;

    getModelVisual()->getAnimationHandler().setSpeedMultiplier(moveMod);

    // Apply animation-velocity
    Math::float3 rootNodeVel = model->getAnimationHandler().getRootNodeVelocityAvg() * deltaTime;

    float angle = atan2(m_MoveState.direction.z, m_MoveState.direction.x);
    m_MoveState.direction = Math::float3(cos(angle + yaw), 0, sin(angle + yaw));
    angle = atan2(m_MoveState.direction.z, m_MoveState.direction.x);

    m_MoveState.position += Math::Matrix::CreateRotationY(-angle + Math::PI * 0.5f) * rootNodeVel;

    setDirection(m_MoveState.direction);

    //Math::Matrix newTransform = Math::Matrix::CreateTranslation(m_MoveState.position) * Math::Matrix::CreateRotationY(angle + yaw);
    //setEntityTransform(newTransform);

    placeOnGround();

    // Reset key-states
    m_isStrafeLeft = false;
    m_isStrafeRight = false;
    m_isForward = false;
    m_isBackward = false;
    m_isTurnLeft = false;
    m_isTurnRight = false;
    m_MoveSpeed1 = false;
    m_MoveSpeed2 = false;
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
                Handle::EntityHandle v = message.targetVob;

                if(!v.isValid())
                {
                    v = m_World.getVobEntityByName(message.targetVobName);
                }

                if (v.isValid())
                {
                    Vob::VobInformation vob = Vob::asVob(m_World, v);
                    message.targetPosition = Vob::getTransform(vob).Translation();

                    // Fall through to ST_GotoPos
                }else{
                    return true;
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
            // Start standing up when we first see this message
            if(!message.inUse)
            {
                standUp(false, message.targetMode != 0);
                message.inUse = true;
            }

            // Go as long as the standup-animation is playing
            return getModelVisual()->isAnimPlaying("S_RUN"); // Fixme: This needs to be set according to walkmode/weapon!
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
                m_AIStateMachine.startAIState(message.functionSymbol, true, message.isRoutineState, message.isPrgState);
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
    switch (static_cast<EventMessages::ManipulateMessage::ManipulateSubType>(message.subType))
    {
        case EventMessages::ManipulateMessage::ST_TakeVob:break;
        case EventMessages::ManipulateMessage::ST_DropVob:break;
        case EventMessages::ManipulateMessage::ST_ThrowVob:break;
        case EventMessages::ManipulateMessage::ST_Exchange:break;
        case EventMessages::ManipulateMessage::ST_UseMob:break;

        case EventMessages::ManipulateMessage::ST_UseItem:
        {
            if(!message.targetItem.isValid())
            {
                // We need a name now
                if(message.symIdx == static_cast<size_t>(-1))
                    return true;

                // Try to find this in our inventory
                Daedalus::GameState::ItemHandle h = m_Inventory.getItem(message.symIdx);

                if(h.isValid())
                    message.targetItem = h;
                else
                    return true; // Could not find that item
            }

            // Get item data

            // Execute the items script-function

            return true;
        }
            break;

        case EventMessages::ManipulateMessage::ST_InsertInteractItem:break;
        case EventMessages::ManipulateMessage::ST_RemoveInteractItem:break;
        case EventMessages::ManipulateMessage::ST_CreateInteractItem:break;
        case EventMessages::ManipulateMessage::ST_DestroyInteractItem:break;
        case EventMessages::ManipulateMessage::ST_PlaceInteractItem:break;
        case EventMessages::ManipulateMessage::ST_ExchangeInteractItem:break;
        case EventMessages::ManipulateMessage::ST_UseMobWithItem:break;

        case EventMessages::ManipulateMessage::ST_CallScript:

            if(message.symIdx != static_cast<size_t>(-1))
            {
                m_World.getScriptEngine().prepareRunFunction();
                m_World.getScriptEngine().runFunctionBySymIndex(message.symIdx);
            }
            return false;

        case EventMessages::ManipulateMessage::ST_EquipItem:break;
        case EventMessages::ManipulateMessage::ST_UseItemToState:break;
        case EventMessages::ManipulateMessage::ST_TakeMob:break;
        case EventMessages::ManipulateMessage::ST_DropMob:break;
        case EventMessages::ManipulateMessage::ST_ManipMax:break;
    }
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
                if(isPlayerControlled())
                {
                    LogInfo() << "PLAYER: New animation started: " << message.animation;
                }

                getModelVisual()->setAnimation(message.animation, false);
                active = getModelVisual()->getAnimationHandler().getActiveAnimationPtr();
                message.internInProgress = true;
            }

            // Go as long as this animation is playing
            bool done = !active
                        || active->getModelAniHeader().aniName != message.animation;

            if(done && isPlayerControlled())
                LogInfo() << "PLAYER: Done with animation: " << message.animation;

            return done;
        }
            break;

        case EventMessages::ConversationMessage::ST_PlaySound:
            break;

        case EventMessages::ConversationMessage::ST_LookAt:
            break;

        case EventMessages::ConversationMessage::ST_Output:
        {
            m_World.getDialogManager().displaySubtitle(message.text, getScriptInstance().name[0]);

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
                m_World.getAudioEngine().stopSounds();
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

    // FIXME: Read align-to-ground-flag
    m_MoveState.direction.y = 0;
    m_MoveState.direction.normalize();

    // This is a hack present in the original game. If the charakter is sitting and one of the following animations
    // are played, the direction should be reversed
    Math::float3 d = m_MoveState.direction;
    if(getModelVisual()->isAnimPlaying("S_BENCH_S1") || getModelVisual()->isAnimPlaying("S_THRONE_S1"))
        d *= -1.0f;

    // Set direction
    setEntityTransform(Math::Matrix::CreateLookAt(m_MoveState.position,
                                                  m_MoveState.position + d,
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

    getEM().clear();
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

bool PlayerController::freeLineOfSight(const Math::float3& target)
{
    // Trace from the top of our BBox (eyes)
    Math::float3 start = getEntityTransform().Translation()
                         + Math::float3(0.0f, m_NPCProperties.collisionBBox[1].y, 0.0f);


    // Do the raytest to the other object
    Physics::RayTestResult res = m_World.getPhysicsSystem().raytrace(
            start,
            target,
            Physics::CollisionShape::CT_WorldMesh); // FIXME: Should trace everything except the two objects in question!

    return !res.hasHit;
}


float PlayerController::getAngleTo(const Math::float3& pos)
{
    Math::float3 dir = (getEntityTransform().Translation() - pos).normalize();

    return atan(m_MoveState.direction.dot(dir));
}

void PlayerController::standUp(bool walkingAllowed, bool startAniTransition)
{
    stopRoute();

    setBodyState(BS_STAND);

    // TODO: Check if the Character is already standing

    if(startAniTransition
       && getModelVisual()->getAnimationHandler().getActiveAnimationPtr())
    {
        std::string playingAni = getModelVisual()->getAnimationHandler().getActiveAnimationPtr()->getModelAniHeader().aniName;

        // State animation?
        if(playingAni.substr(0, 2) == "S_")
        {
            // Build transition to standing
            std::string transition = "T_" + playingAni.substr(2) + "_2_STAND";

            getModelVisual()->setAnimation(transition, false);
        }
    }else
    {
        // Just jump to the idle-animation
        getModelVisual()->stopAnimations();
    }
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
    if(routineName.empty())
    {
        setRoutineFunc(0);
        return;
    }

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


bool PlayerController::useItem(Daedalus::GameState::ItemHandle item)
{
    if(!canUse(item))
        return false;

    Daedalus::GEngineClasses::C_Item& data = m_World.getScriptEngine().getGameState().getItem(item);

    // Food?
    if((data.flags & Daedalus::GEngineClasses::C_Item::ITM_CAT_FOOD) != 0)
    {
        // Nutrition doesn't seem to be used anywhere...
        changeAttribute(Daedalus::GEngineClasses::C_Npc::EATR_HITPOINTS, data.nutrition);
    }

    // Weapon?
    if((data.mainflag & Daedalus::GEngineClasses::C_Item::ITM_CAT_NF) != 0
       || (data.mainflag & Daedalus::GEngineClasses::C_Item::ITM_CAT_FF) != 0
       || (data.mainflag & Daedalus::GEngineClasses::C_Item::ITM_CAT_ARMOR) != 0
       || (data.mainflag & Daedalus::GEngineClasses::C_Item::ITM_CAT_MAGIC) != 0)
    {
        equipItem(item);
        return false;
    }

    // Call script function to be executed on use
    if(data.on_state[0])
    {
        m_World.getScriptEngine().setInstanceNPC("self", getScriptHandle());
        m_World.getScriptEngine().prepareRunFunction();
        m_World.getScriptEngine().runFunctionBySymIndex(data.on_state[0]);

        return true;
    }

    return false;
}

bool PlayerController::canUse(Daedalus::GameState::ItemHandle item)
{
    if(!item.isValid())
        return false;

    // TODO: Check talents

    // Check attributes
    Daedalus::GEngineClasses::C_Item& data = m_World.getScriptEngine().getGameState().getItem(item);
    Daedalus::GEngineClasses::C_Npc& npc = getScriptInstance();
    ScriptEngine& s = m_World.getScriptEngine();

    for(int i=0; i < Daedalus::GEngineClasses::C_Item::COND_ATR_MAX; i++)
    {
        // Why is 0 not allowed? That's how gothic is doing it though, as it seems...
        if(data.cond_atr[i] > 0)
        {
            assert(data.cond_atr[i] < Daedalus::GEngineClasses::C_Npc::EATR_MAX);

            // Check for enough strength, etc.
            if(npc.attribute[data.cond_atr[i]] < data.cond_value[i])
            {
                // Display messages, if this is the player and do debug-output
                s.setInstanceNPC("self", getScriptHandle());
                s.setInstanceItem("item", item);

                s.prepareRunFunction();

                s.pushInt(data.cond_value[i]);
                s.pushInt(data.cond_atr[i]);
                s.pushInt(isPlayerControlled() ? 1 : 0);
                s.runFunction("G_CANNOTUSE");

                return false;
            }
        }
    }

    return true;
}

void PlayerController::setAttribute(Daedalus::GEngineClasses::C_Npc::EAttributes atr, int value)
{
    Daedalus::GEngineClasses::C_Npc& npc = getScriptInstance();

    if((uint32_t)atr > Daedalus::GEngineClasses::C_Npc::EATR_MAX)
        return;

    // Apply change
    npc.attribute[atr] = value;

    // Clamp to 0
    if(npc.attribute[atr] < 0)
        npc.attribute[atr] = 0;

    // Clamp to max
    if(atr == Daedalus::GEngineClasses::C_Npc::EATR_HITPOINTS)
        npc.attribute[atr] = std::min(npc.attribute[atr],
                                      npc.attribute[Daedalus::GEngineClasses::C_Npc::EATR_HITPOINTSMAX]);

    if(atr == Daedalus::GEngineClasses::C_Npc::EATR_MANA)
        npc.attribute[atr] = std::min(npc.attribute[atr],
                                      npc.attribute[Daedalus::GEngineClasses::C_Npc::EATR_MANAMAX]);
}

void PlayerController::changeAttribute(Daedalus::GEngineClasses::C_Npc::EAttributes atr, int change)
{
    Daedalus::GEngineClasses::C_Npc& npc = getScriptInstance();
    // TODO: Godmode right here

    if(!change)
        return; // Nothing to do here...

    if((uint32_t)atr > Daedalus::GEngineClasses::C_Npc::EATR_MAX)
        return; // Invalid value set from script?

    // Check for immortality
    if( atr == Daedalus::GEngineClasses::C_Npc::EATR_HITPOINTS
        && npc.flags & Daedalus::GEngineClasses::C_Npc::EFLAG_IMMORTAL
        && change != -999) // Yep, this is in the game!
    {
        return;
    }

    // Apply change
    npc.attribute[atr] += change;

    // Clamp to 0
    if(npc.attribute[atr] < 0)
        npc.attribute[atr] = 0;

    // Clamp to max
    if(atr == Daedalus::GEngineClasses::C_Npc::EATR_HITPOINTS)
        npc.attribute[atr] = std::min(npc.attribute[atr],
                                      npc.attribute[Daedalus::GEngineClasses::C_Npc::EATR_HITPOINTSMAX]);

    if(atr == Daedalus::GEngineClasses::C_Npc::EATR_MANA)
        npc.attribute[atr] = std::min(npc.attribute[atr],
                                      npc.attribute[Daedalus::GEngineClasses::C_Npc::EATR_MANAMAX]);

    // TODO: Switch animation overlay (wounded, etc)

    // Check for death
    if(atr == Daedalus::GEngineClasses::C_Npc::EATR_HITPOINTS)
    {
        if(npc.attribute[atr] == 0)
            die(m_Entity);
        //else if(npc.attribute[atr] == 1)
        // TODO: Drop unconscious
    }
}


void PlayerController::setupKeyBindings()
{
    Engine::Input::clearActions();

    Engine::Input::RegisterAction(Engine::ActionType::OpenStatusMenu, [this](bool triggered, float) {
        if(triggered)
        {
            UI::Menu_Status& statsScreen = m_World.getEngine()->getHud().pushMenu<UI::Menu_Status>();
        
            // Update the players status menu once
            updateStatusScreen(statsScreen); 
        }
    });

    Engine::Input::RegisterAction(Engine::ActionType::OpenConsole, [this](bool triggered, float) {
        if(triggered)
            m_World.getEngine()->getHud().getConsole().setOpen(true);
    });

    Engine::Input::RegisterAction(Engine::ActionType::PlayerDrawWeaponMelee, [this](bool triggered, float) {
        m_isDrawWeaponMelee = triggered;
    });
    Engine::Input::RegisterAction(Engine::ActionType::PlayerForward, [this](bool triggered, float) {


        // Increment state, if currently using a mob
        if(getUsedMob().isValid() && triggered)
        {
            if (getEM().isEmpty())
            {
                VobTypes::MobVobInformation mob = VobTypes::asMobVob(m_World, getUsedMob());
                if (mob.isValid())
                {
                    LogInfo() << getScriptInstance().name[0] << ": Incrementing state on mob: "
                              << mob.mobController->getFocusName();
                    mob.mobController->useMobIncState(m_Entity, MobController::D_Forward);
                }
            }
        }else
        {
            m_isForward = m_isForward || triggered;
        }
    });
    Engine::Input::RegisterAction(Engine::ActionType::PlayerBackward, [this](bool triggered, float) {


        // Increment state, if currently using a mob
        if(getUsedMob().isValid() && triggered)
        {
            if (getEM().isEmpty())
            {
                VobTypes::MobVobInformation mob = VobTypes::asMobVob(m_World, getUsedMob());
                if (mob.isValid())
                {
                    LogInfo() << getScriptInstance().name[0] << ": Decrementing state on mob: "
                              << mob.mobController->getFocusName();
                    mob.mobController->useMobIncState(m_Entity, MobController::D_Backward);
                }
            }
        }else
        {
            m_isBackward = m_isBackward || triggered;
        }
    });
    Engine::Input::RegisterAction(Engine::ActionType::PlayerTurnLeft, [this](bool triggered, float) {
        m_isTurnLeft = m_isTurnLeft || triggered;
    });
    Engine::Input::RegisterAction(Engine::ActionType::PlayerTurnRight, [this](bool triggered, float) {
        m_isTurnRight = m_isTurnRight || triggered;
    });
    Engine::Input::RegisterAction(Engine::ActionType::PlayerStrafeLeft, [this](bool triggered, float) {
        m_isStrafeLeft = m_isStrafeLeft || triggered;
    });
    Engine::Input::RegisterAction(Engine::ActionType::PlayerStrafeRight, [this](bool triggered, float) {
        m_isStrafeRight = m_isStrafeRight || triggered;
    });

    Engine::Input::RegisterAction(Engine::ActionType::DebugMoveSpeed, [this](bool triggered, float intensity) {
        m_MoveSpeed1 = m_MoveSpeed1 || triggered;
    });

    Engine::Input::RegisterAction(Engine::ActionType::DebugMoveSpeed2, [this](bool triggered, float intensity) {
        m_MoveSpeed2 = m_MoveSpeed2 || triggered;
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

            // ----- ITEMS -----
            Handle::EntityHandle nearestItem;
            float shortestDistItem = 5.0f;
            const std::set<Handle::EntityHandle>& items = m_World.getScriptEngine().getWorldItems();
            for(Handle::EntityHandle h : items)
            {
                Math::Matrix& m = m_World.getEntity<Components::PositionComponent>(h).m_WorldMatrix;

                float dist = (m.Translation() -
                              getEntityTransform().Translation()).lengthSquared();
                if (dist < shortestDistItem && dist < 10.0f * 10.0f)
                {
                    nearestItem = h;
                    shortestDistItem = dist;
                }
            }

            std::set<Handle::EntityHandle> nearNPCs = m_World.getScriptEngine().getNPCsInRadius(
                    getEntityTransform().Translation(), 5.0f);

            // Talk to the nearest NPC other than the current player, of course
            Handle::EntityHandle nearestNPC;
            float shortestDistNPC = 5.0f;
            for (const Handle::EntityHandle& h : nearNPCs)
            {
                if (h != m_World.getScriptEngine().getPlayerEntity())
                {
                    VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(m_World, h);

                    float dist = (Vob::getTransform(npc).Translation() -
                                  getEntityTransform().Translation()).lengthSquared();
                    if (dist < shortestDistNPC)
                    {
                        nearestNPC = h;
                        shortestDistNPC = dist;
                    }
                }
            }

            std::set<Handle::EntityHandle> mobs = m_World.getScriptEngine().getWorldMobs();

            // Use the nearest mob
            Handle::EntityHandle nearestMob;
            float shortestDistMob = 5.0f;
            for (const Handle::EntityHandle& h : mobs)
            {
                VobTypes::MobVobInformation vob = VobTypes::asMobVob(m_World, h);

                float dist = (Vob::getTransform(vob).Translation() -
                              getEntityTransform().Translation()).lengthSquared();

                if (dist < shortestDistMob)
                {
                    nearestMob = h;
                    shortestDistMob = dist;
                }
            }

            int nearest = 0;

            if(shortestDistItem < shortestDistMob && shortestDistItem < shortestDistNPC)
                nearest = 1;

            if(shortestDistMob < shortestDistItem && shortestDistMob < shortestDistNPC)
                nearest = 3;

            if(shortestDistNPC < shortestDistItem && shortestDistNPC < shortestDistMob)
                nearest = 2;

            if(nearest == 1 && nearestItem.isValid())
            {
                // Pick it up
                VobTypes::ItemVobInformation item = VobTypes::asItemVob(m_World, nearestItem);
                item.itemController->pickUp(m_Entity);

                return;
            }

            // ----- TALKING ----



            if (nearest == 2 && nearestNPC.isValid())
            {
                VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(m_World, nearestNPC);

                if(npc.playerController->getBodyState() == BS_STAND)
                {
                    Daedalus::GameState::NpcHandle shnpc = VobTypes::getScriptHandle(npc);
                    m_World.getDialogManager().startDialog(shnpc);
                }else if(npc.playerController->getBodyState() == BS_UNCONSCIOUS
                        || npc.playerController->getBodyState() == BS_DEAD)
                {
                    // Take all his items
                    auto& inv = npc.playerController->getInventory();
                    for(auto h : inv.getItems())
                    {
                        unsigned int cnt = inv.getItemCount(h);
                        size_t itm = inv.getItem(h).instanceSymbol;

                        getInventory().addItem(itm, cnt);
                    }

                    inv.clear();
                }

                return;
            }

            if (nearest == 3 && nearestMob.isValid())
            {
                VobTypes::MobVobInformation vob = VobTypes::asMobVob(m_World, nearestMob);

                LogInfo() << "Using mob: " << vob.mobController->getFocusName();

                vob.mobController->useMobToState(m_Entity, 1);
                return;
            }

            /*size_t targetWP = World::Waynet::findNearestWaypointTo(m_World.getWaynet(), getEntityTransform().Translation());
                Handle::EntityHandle h = VobTypes::Wld_InsertNpc(m_World, "VLK_574_Mud", m_World.getWaynet().waypoints[targetWP].name);
                VobTypes::NpcVobInformation vob = VobTypes::asNpcVob(m_World, h);

                vob.playerController->changeRoutine("");
                vob.playerController->teleportToWaypoint(targetWP);*/

            // Use item last picked up
            if(!getInventory().getItems().empty())
            {
                Daedalus::GameState::ItemHandle lastItem = getInventory().getItems().back();
                if (lastItem.isValid())
                {
                    if (useItem(lastItem))
                        getInventory().removeItem(lastItem);
                }
            }


        }
    });


}

void PlayerController::giveItem(const std::string& instanceName, unsigned int count)
{
    size_t symIdx = m_World.getScriptEngine().getSymbolIndexByName(instanceName);

    if(symIdx == static_cast<size_t>(-1))
    {
        LogWarn() << "Unknown item instance: " << instanceName;
        return;
    }

    LogInfo() << "Giving " << count << "x " << instanceName << " to " << getScriptInstance().name[0];

    // Add our script-instance to the npcs inventory
    m_World.getScriptEngine().getGameState().createInventoryItem(symIdx, getScriptHandle(), count);
}

void PlayerController::exportPart(json& j)
{
    size_t sym = getScriptInstance().instanceSymbol;

    Controller::exportPart(j);

    j["type"] = "PlayerController";

    // Export script-values
    auto& scriptObj = getScriptInstance();
    j["scriptObj"]["instanceSymbol"] = scriptObj.instanceSymbol;
    j["scriptObj"]["id"] = scriptObj.id;
    j["scriptObj"]["name"] = Utils::putArray(scriptObj.name);
    j["scriptObj"]["slot"] = scriptObj.slot;
    j["scriptObj"]["npcType"] = scriptObj.npcType;
    j["scriptObj"]["flags"] = scriptObj.flags;
    j["scriptObj"]["attribute"] = Utils::putArray(scriptObj.attribute);
    j["scriptObj"]["protection"] = Utils::putArray(scriptObj.protection);
    j["scriptObj"]["damage"] = Utils::putArray(scriptObj.damage);
    j["scriptObj"]["damagetype"] = scriptObj.damagetype;
    j["scriptObj"]["guild"] = scriptObj.guild;
    j["scriptObj"]["level"] = scriptObj.level;
    j["scriptObj"]["mission"] = Utils::putArray(scriptObj.mission);
    j["scriptObj"]["fight_tactic"] = scriptObj.fight_tactic;
    j["scriptObj"]["weapon"] = scriptObj.weapon;
    j["scriptObj"]["voice"] = scriptObj.voice;
    j["scriptObj"]["voicePitch"] = scriptObj.voicePitch;
    j["scriptObj"]["bodymass"] = scriptObj.bodymass;
    j["scriptObj"]["daily_routine"] = scriptObj.daily_routine;
    j["scriptObj"]["start_aistate"] = scriptObj.start_aistate;
    j["scriptObj"]["spawnPoint"] = scriptObj.spawnPoint;
    j["scriptObj"]["spawnDelay"] = scriptObj.spawnDelay;
    j["scriptObj"]["senses"] = scriptObj.senses;
    j["scriptObj"]["senses_range"] = scriptObj.senses_range;
    j["scriptObj"]["ai"] = Utils::putArray(scriptObj.ai);
    j["scriptObj"]["wp"] = scriptObj.wp;
    j["scriptObj"]["exp"] = scriptObj.exp;
    j["scriptObj"]["exp_next"] = scriptObj.exp_next;
    j["scriptObj"]["lp"] = scriptObj.lp;

    // Export inventory
    m_Inventory.exportInventory(j["inventory"]);

    // Export equipped items
    {
        j["equipped"] = json::array();
        for(auto item : m_EquipmentState.equippedItemsAll)
        {
            // Write instance of the equipped item
            Daedalus::GEngineClasses::C_Item& data = m_World.getScriptEngine().getGameState().getItem(item);
            std::string instanceName = m_World.getScriptEngine().getVM()
                    .getDATFile().getSymbolByIndex(data.instanceSymbol).name;

            j["equipped"].push_back(instanceName);
        }
    }

    // Import state
    m_AIStateMachine.exportScriptState(j["AIState"]);
}

void PlayerController::importObject(const json& j, bool noTransform)
{
    if(!noTransform)
    {
        Controller::importObject(j);

        // Teleport to position
        {
            teleportToPosition(getEntityTransform().Translation());
            setDirection(-1.0f * getEntityTransform().Forward());
        }
    }

    // Set script values
    {
        auto& scriptObj = getScriptInstance();

        scriptObj.instanceSymbol = j["scriptObj"]["instanceSymbol"];
        scriptObj.id = j["scriptObj"]["id"];

        Utils::putArray(scriptObj.name, j["scriptObj"]["name"]);

        // Need this in iso8859-1 again
        //for(std::string& s : scriptObj.name)
        //    s = Utils::utf8_to_iso8859_1(s.c_str());

        scriptObj.slot = j["scriptObj"]["slot"];
        scriptObj.npcType = j["scriptObj"]["npcType"];
        scriptObj.flags = (Daedalus::GEngineClasses::C_Npc::ENPCFlag)((int)j["scriptObj"]["flags"]);

        Utils::putArray(scriptObj.attribute, j["scriptObj"]["attribute"]);
        Utils::putArray(scriptObj.protection, j["scriptObj"]["protection"]);
        Utils::putArray(scriptObj.damage, j["scriptObj"]["damage"]);

        scriptObj.damagetype = j["scriptObj"]["damagetype"];
        scriptObj.guild = j["scriptObj"]["guild"];
        scriptObj.level = j["scriptObj"]["level"];
        Utils::putArray(scriptObj.mission, j["scriptObj"]["mission"]);
        scriptObj.fight_tactic = j["scriptObj"]["fight_tactic"];
        scriptObj.weapon = j["scriptObj"]["weapon"];
        scriptObj.voice = j["scriptObj"]["voice"];
        scriptObj.voicePitch = j["scriptObj"]["voicePitch"];
        scriptObj.bodymass = j["scriptObj"]["bodymass"];
        scriptObj.daily_routine = j["scriptObj"]["daily_routine"];
        scriptObj.start_aistate = j["scriptObj"]["start_aistate"];
        scriptObj.spawnPoint = j["scriptObj"]["spawnPoint"];
        scriptObj.spawnDelay = j["scriptObj"]["spawnDelay"];
        scriptObj.senses = j["scriptObj"]["senses"];
        scriptObj.senses_range = j["scriptObj"]["senses_range"];
        Utils::putArray(scriptObj.ai, j["scriptObj"]["ai"]);
        scriptObj.wp = j["scriptObj"]["wp"];
        scriptObj.exp = j["scriptObj"]["exp"];
        scriptObj.exp_next = j["scriptObj"]["exp_next"];
        scriptObj.lp = j["scriptObj"]["lp"];
    }

    // Import inventory
    m_Inventory.importInventory(j["inventory"]);

    // Import equipments
    {
        Inventory& inv = m_Inventory;

        for(const std::string& sym : j["equipped"])
        {
            Daedalus::GameState::ItemHandle h = inv.getItem(sym);

            assert(h.isValid()); // Item to equip MUST be inside the inventory

            equipItem(h);
        }
    }

    // Import state
    m_AIStateMachine.importScriptState(j["AIState"]);

    // Check for death
    if(getScriptInstance().attribute[Daedalus::GEngineClasses::C_Npc::EATR_HITPOINTS] == 0)
        die(Handle::EntityHandle::makeInvalidHandle());
}


void PlayerController::importObject(const json& j)
{
    importObject(j, false);

    // Teleport to last waypoint
    //teleportToWaypoint(j["scriptObj"]["wp"]);

}


Handle::EntityHandle PlayerController::importPlayerController(World::WorldInstance& world, const json& j)
{
    unsigned int instanceSymbol = j["scriptObj"]["instanceSymbol"];

    /*std::string name = j["scriptObj"]["name"][0];
    if(name != "Diego" && name != "ich")
        return Handle::EntityHandle();*/

    // Create npc
    Handle::EntityHandle e = VobTypes::Wld_InsertNpc(world, instanceSymbol);

    if(!e.isValid())
        return Handle::EntityHandle::makeInvalidHandle();

    VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(world, e);

    // Load controller-values
    npc.playerController->importObject(j);


    return e;
}

void PlayerController::die(Handle::EntityHandle attackingNPC)
{
    interrupt();

    // TODO: Drop weapons held in hand

    setBodyState(EBodyState::BS_DEAD);

    if(!m_AIStateMachine.isInState(NPC_PRGAISTATE_DEAD))
    {
        Daedalus::GameState::NpcHandle oldOther = m_World.getScriptEngine().getNPCFromSymbol("other");

        if(attackingNPC.isValid())
        {
            VobTypes::NpcVobInformation attacker = VobTypes::asNpcVob(m_World, attackingNPC);
            m_World.getScriptEngine().setInstanceNPC("other", VobTypes::getScriptHandle(attacker));
        }else
        {
            m_World.getScriptEngine().setInstanceNPC("other",Daedalus::GameState::NpcHandle());
        }

        m_AIStateMachine.startAIState(Logic::NPC_PRGAISTATE_DEAD, false, false, true);

        // Restore old other
        m_World.getScriptEngine().setInstanceNPC("other", oldOther);
    }

    setAttribute(Daedalus::GEngineClasses::C_Npc::EAttributes::EATR_HITPOINTS, 0);

    // TODO: Move this into onDamage_Anim if that function exists!
    getEM().onMessage(EventMessages::ConversationMessage::playAnimation("T_DEAD"));

    m_AIStateMachine.clearRoutine();
}

void PlayerController::checkUnconscious()
{
    // Gothic is just brute-force checking here if any of these animations are being played
    // If so, we already put the character onto the ground
    const std::string woundedAnims[] = {
            "T_STAND_2_WOUNDEDB",
            "T_STAND_2_WOUNDED",
            "S_WOUNDEDB",
            "S_WOUNDED",
            "T_WOUNDEDB_2_DEADB",
            "T_WOUNDED_2_DEAD",
            "T_WOUNDED_TRY",
            "T_WOUNDEDB_TRY"
    };

    for(size_t i=0; i < Utils::arraySize(woundedAnims);i++)
    {
        if(getModelVisual()->isAnimPlaying(woundedAnims[i]))
            return;
    }

    // Not yet unconscious, we can change that...
    getEM().onMessage(EventMessages::ConversationMessage::playAnimation("T_STAND_2_WOUNDEDB"));
    setBodyState(EBodyState::BS_UNCONSCIOUS);
}

void PlayerController::onUpdateForPlayer(float deltaTime)
{
    // Nofity hud about current stats
    UI::Hud& hud = m_World.getEngine()->getHud();
    auto& stats = getScriptInstance();

    hud.setHealth(stats.attribute[Daedalus::GEngineClasses::C_Npc::EATR_HITPOINTS] /
                  (float)stats.attribute[Daedalus::GEngineClasses::C_Npc::EATR_HITPOINTSMAX]);

    hud.setMana(stats.attribute[Daedalus::GEngineClasses::C_Npc::EATR_MANA] /
                  (float)stats.attribute[Daedalus::GEngineClasses::C_Npc::EATR_MANAMAX]);

}

std::string PlayerController::getGuildName()
{
    // Guilds are stored as an array in a symbol called "TXT_GUILDS"
    auto& sym = m_World.getScriptEngine().getVM().getDATFile().getSymbolByName("TXT_GUILDS");
    std::string* adr = sym.getStrAddr((unsigned int)getScriptInstance().guild);

    return *adr;
}

void PlayerController::updateStatusScreen(UI::Menu_Status& statsScreen)
{
    auto& stats = getScriptInstance();

    statsScreen.setAttribute(UI::Menu_Status::A_STR, stats.attribute[Daedalus::GEngineClasses::C_Npc::EATR_STRENGTH]);
    statsScreen.setAttribute(UI::Menu_Status::A_DEX, stats.attribute[Daedalus::GEngineClasses::C_Npc::EATR_STRENGTH]);

    statsScreen.setAttribute(UI::Menu_Status::A_MANA, stats.attribute[Daedalus::GEngineClasses::C_Npc::EATR_MANA],
                             stats.attribute[Daedalus::GEngineClasses::C_Npc::EATR_MANAMAX]);
    statsScreen.setAttribute(UI::Menu_Status::A_HEALTH, stats.attribute[Daedalus::GEngineClasses::C_Npc::EATR_HITPOINTS],
                             stats.attribute[Daedalus::GEngineClasses::C_Npc::EATR_HITPOINTSMAX]);

    statsScreen.setGuild(getGuildName());
    statsScreen.setLevel(stats.level);
    statsScreen.setExperience(stats.exp);
    statsScreen.setExperienceNext(stats.exp_next);
    statsScreen.setLearnPoints(stats.lp);
}
