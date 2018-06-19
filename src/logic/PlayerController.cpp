//
// Created by andre on 02.06.16.
//

#include "PlayerController.h"
#include "ItemController.h"
#include "MobController.h"
#include <json.hpp>
#include <stdlib.h>
#include "visuals/ModelVisual.h"
#include "CameraController.h"
#include <audio/AudioEngine.h>
#include <components/Entities.h>
#include <components/Vob.h>
#include <components/VobClasses.h>
#include <debugdraw/debugdraw.h>
#include <engine/BaseEngine.h>
#include <engine/Input.h>
#include <engine/Waynet.h>
#include <engine/World.h>
#include <entry/input.h>
#include <logic/SavegameManager.h>
#include <render/WorldRender.h>
#include <ui/Hud.h>
#include <ui/Menu_Status.h>
#include <ui/SubtitleBox.h>
#include <utils/logger.h>
#include <logic/ScriptEngine.h>
#include <physics/PhysicsSystem.h>
#include <logic/ScriptEngine.h>
#include <logic/DialogManager.h>
#include <engine/WorldMesh.h>
#include <logic/PfxManager.h>
#include <logic/visuals/PfxVisual.h>


#define DEBUG_PLAYER (isPlayerControlled() && false)

using json = nlohmann::json;
using namespace Logic;
using SharedEMessage = std::shared_ptr<Logic::EventMessages::EventMessage>;

/**
 * Radius the sound of the voicelines and other sounds of the NPCs(in meters)
 */
const float NPC_SOUND_RADIUS = 14;

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

/**
 * Default soundrange for SFX which doesn't specify a range. Gothic uses a default value of 35 meters.
 */
static const float DEFAULT_CHARACTER_SOUND_RANGE = 35; // Meters

#define SINGLE_ACTION_KEY(key, fn)               \
    {                                            \
        static bool last = false;                \
        if (inputGetKeyState(key) && !last)      \
            last = true;                         \
        else if (!inputGetKeyState(key) && last) \
        {                                        \
            last = false;                        \
            fn();                                \
        }                                        \
    }

PlayerController::PlayerController(World::WorldInstance& world,
                                   Handle::EntityHandle entity,
                                   Daedalus::GameState::NpcHandle scriptInstance)
    : Controller(world, entity)
    , m_Inventory(world, scriptInstance)
    , m_AIStateMachine(world, entity)
    , m_NPCAnimationHandler(world, entity)
    , m_AIHandler(world, entity)
    , m_PathFinder(world)
{
    m_AIState.closestWaypoint = 0;
    m_MoveState.currentPathPerc = 0;
    m_NPCProperties.moveSpeed = 7.0f;
    m_NPCProperties.enablePhysics = true;

    m_MoveState.direction = Math::float3(1, 0, 0);
    m_MoveState.position = Math::float3(0, 0, 0);
    m_MoveState.ground.successful = false;
    m_MoveState.ground.triangleIndex = 0;
    m_MoveState.ground.waterDepth = 0;
    m_MoveState.ground.trianglePosition = Math::float3(0, 0, 0);
    m_AIState.targetWaypoint = World::Waynet::INVALID_WAYPOINT;
    m_AIState.closestWaypoint = World::Waynet::INVALID_WAYPOINT;

    m_ScriptState.npcHandle = scriptInstance;

    m_EquipmentState.weaponMode = EWeaponMode::WeaponNone;
    m_EquipmentState.activeWeapon.invalidate();

    m_RefuseTalkTime = 0;

    m_LastAniRootPosUpdatedAniHash = 0;
    m_NoAniRootPosHack = false;

    setBodyState(BS_STAND);
}

void PlayerController::onUpdate(float deltaTime)
{
    // If anything wants this to be modified, it as to keep it up to date
    // It is important that the update-method is called last in this update-handler
    m_AIHandler.setTargetMovementState(EMovementState::None);

    m_RefuseTalkTime -= deltaTime;

    m_AIStateMachine.doAIState(deltaTime);

    // This vob should react to messages
    getEM().processMessageQueue();

    ModelVisual* model = getModelVisual();

    // Build the route to follow this entity
    if (m_RoutineState.entityTarget.isValid())
    {
        Math::float3 targetPos = m_World.getEntity<Components::PositionComponent>(m_RoutineState.entityTarget)
                                     .m_WorldMatrix.Translation();

        // FIXME: Doing this every frame is too often
        size_t targetWP = World::Waynet::findNearestWaypointTo(m_World.getWaynet(), targetPos);

        gotoWaypoint(targetWP);
    }
    m_NoAniRootPosHack = false;

    if (model)
    {
        // Make sure the idle-animation is running
        /*if (!model->getAnimationHandler().getActiveAnimationPtr())
        {
            model->setAnimation(ModelVisual::Idle);
        }*/

        // Update model for this frame
        model->onFrameUpdate(deltaTime);

        // Retrieve data of the ground on which the NPC is standing
        traceDownNPCGround();

        // Needs to be done here to account for changes of feet-height
        placeOnGround();

        Animations::Animation* activeAnim = getModelVisual()->getAnimationHandler().getActiveAnimationPtr();
        if (!m_NoAniRootPosHack && activeAnim)
        {
            // Apply model root-velcoity
            if (activeAnim->m_Flags & Animations::Animation::MSB_FLAG_MOVE_MODEL)
            {
                // Move by translation-velocity
                m_MoveState.position += getEntityTransform().Rotate(
                    getModelVisual()->getAnimationHandler().getRootNodeVelocity());
            }

            //bgfx::dbgTextPrintf(0,5, 0x2, "Vel: %s", getModelVisual()->getAnimationHandler().getRootNodeVelocity().toString().c_str());

            Math::Matrix t = getEntityTransform();
            t.Translation(m_MoveState.position);

            if (activeAnim->m_Flags & Animations::Animation::MSB_FLAG_ROTATE_MODEL)
            {
                // Rotate by rotation-velocity
                t = t * getModelVisual()->getAnimationHandler().getRootNodeRotationVelocity();
            }

            setEntityTransform(t);

            m_LastAniRootPosUpdatedAniHash = getModelVisual()->getAnimationHandler().getAnimationStateHash();
        }
    }
    //Update pfx events
    updatePfx();
    // Run extra stuff if this is the controlled character
    if (isPlayerControlled())
    {
        onUpdateForPlayer(deltaTime);
    }else
    {
        m_AIHandler.npcUpdate(deltaTime);
    }
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
    //getModelVisual()->setAnimation(ModelVisual::Idle);
}


void PlayerController::gotoWaypoint(World::Waynet::WaypointIndex wp)
{
    m_PathFinder.startNewRouteTo(getEntityTransform().Translation(), wp);
}


void PlayerController::gotoVob(Handle::EntityHandle vob)
{
    m_PathFinder.startNewRouteTo(getEntityTransform().Translation(), vob);
}

void PlayerController::gotoPosition(const Math::float3& position)
{
    m_PathFinder.startNewRouteTo(getEntityTransform().Translation(), position);
}

void PlayerController::travelPath()
{
    Math::float3 positionNow = getEntityTransform().Translation();

    Pathfinder::Instruction inst = m_PathFinder.updateToNextInstructionToTarget(positionNow);

    if(!m_PathFinder.isTargetReachedByPosition(positionNow, inst.targetPosition))
    {
        // Turn towards target
        setDirection(inst.targetPosition - positionNow);
    }

    // Start running
    m_AIHandler.setTargetMovementState(EMovementState::Forward);
}


void PlayerController::onDebugDraw()
{
    if (isPlayerControlled())
    {
        VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(m_World, m_Entity);
        Daedalus::GEngineClasses::C_Npc& scriptnpc = VobTypes::getScriptObject(npc);

        if (!getInventory().getItems().empty())
        {
            // Print inventory
            const std::list<Daedalus::GameState::ItemHandle>& items = m_Inventory.getItems();
            Daedalus::DATFile& datFile = m_World.getScriptEngine().getVM().getDATFile();

            uint16_t idx = 27;
            bgfx::dbgTextPrintf(0, idx++, 0x0f, "Inventory:");
            for (Daedalus::GameState::ItemHandle i : items)
            {
                Daedalus::GEngineClasses::C_Item idata = m_World.getScriptEngine().getGameState().getItem(i);

                std::string displayName;
                {
                    if (!idata.description.empty())
                    {
                        displayName = idata.description;
                    }
                    else if (!idata.name.empty())
                    {
                        displayName = idata.name;
                    }
                    else
                    {
                        displayName = datFile.getSymbolByIndex(idata.instanceSymbol).name;
                    }
                }

                if (idata.amount > 1)
                    bgfx::dbgTextPrintf(0, idx++, 0x0f, " %s [%d]", displayName.c_str(), idata.amount);
                else
                    bgfx::dbgTextPrintf(0, idx++, 0x0f, " %s", displayName.c_str());
            }
        }
    }
}

void PlayerController::unequipItem(Daedalus::GameState::ItemHandle item)
{
    // Get item
    Daedalus::GEngineClasses::C_Item& itemData = m_World.getScriptEngine().getGameState().getItem(item);
    ModelVisual* model = getModelVisual();

    EModelNode node = EModelNode::None;

    if ((itemData.mainflag & Daedalus::GEngineClasses::C_Item::ITM_CAT_EQUIPABLE) == 0)
        return;  // Can't equip

    // TODO: Don't forget if an item is already unequipped before executing stat changing script-code!

    // Put into set of all equipped items first, then differentiate between the item-types
    m_EquipmentState.equippedItemsAll.erase(item);

    if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_2HD_AXE) != 0)
    {
        node = EModelNode::Longsword;

        // Take off 2h weapon
        m_EquipmentState.equippedItems.equippedWeapon2h.invalidate();
    }
    else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_2HD_SWD) != 0)
    {
        node = EModelNode::Longsword;

        // Take off 2h weapon
        m_EquipmentState.equippedItems.equippedWeapon2h.invalidate();
    }
    else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_CROSSBOW) != 0)
    {
        node = EModelNode::Crossbow;

        // Take off crossbow
        m_EquipmentState.equippedItems.equippedCrossBow.invalidate();
    }
    else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_BOW) != 0)
    {
        node = EModelNode::Bow;

        // Take off bow
        m_EquipmentState.equippedItems.equippedBow.invalidate();
    }
    else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_SWD) != 0)
    {
        node = EModelNode::Sword;

        // Take off 1h weapon
        m_EquipmentState.equippedItems.equippedWeapon1h.invalidate();
    }
    else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_AXE) != 0)
    {
        node = EModelNode::Sword;

        // Take off 1h weapon
        m_EquipmentState.equippedItems.equippedWeapon1h.invalidate();
    }
    else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_AMULET) != 0)
    {
        node = EModelNode::None;

        // Take off amulet
        m_EquipmentState.equippedItems.equippedAmulet.invalidate();
    }
    else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_RING) != 0)
    {
        node = EModelNode::None;

        // Take off ring
        m_EquipmentState.equippedItems.equippedRings.erase(item);
    }
    else if ((itemData.mainflag & Daedalus::GEngineClasses::C_Item::ITM_CAT_RUNE) != 0 || (itemData.mainflag & Daedalus::GEngineClasses::C_Item::ITM_CAT_MAGIC) != 0)
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

    if ((itemData.mainflag & Daedalus::GEngineClasses::C_Item::ITM_CAT_EQUIPABLE) == 0)
        return;  // Can't equip

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
    }
    else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_2HD_SWD) != 0)
    {
        node = EModelNode::Longsword;

        // Take of any 1h weapon
        m_EquipmentState.equippedItems.equippedWeapon1h.invalidate();
        model->setNodeVisual("", EModelNode::Sword);

        // Put on our 2h weapon
        m_EquipmentState.equippedItems.equippedWeapon2h = item;
    }
    else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_CROSSBOW) != 0)
    {
        node = EModelNode::Crossbow;

        // Take off a bow
        m_EquipmentState.equippedItems.equippedBow.invalidate();
        model->setNodeVisual("", EModelNode::Bow);

        // Put on our crossbow
        m_EquipmentState.equippedItems.equippedCrossBow = item;
    }
    else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_BOW) != 0)
    {
        node = EModelNode::Bow;

        // Take off a crossbow
        m_EquipmentState.equippedItems.equippedCrossBow.invalidate();
        model->setNodeVisual("", EModelNode::Crossbow);

        // Put on our bow
        m_EquipmentState.equippedItems.equippedBow = item;
    }
    else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_SWD) != 0)
    {
        node = EModelNode::Sword;

        // Take of any 2h weapon
        m_EquipmentState.equippedItems.equippedWeapon2h.invalidate();
        model->setNodeVisual("", EModelNode::Longsword);

        // Put on our 1h weapon
        m_EquipmentState.equippedItems.equippedWeapon1h = item;
    }
    else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_AXE) != 0)
    {
        node = EModelNode::Sword;

        // Take of any 2h weapon
        m_EquipmentState.equippedItems.equippedWeapon2h.invalidate();
        model->setNodeVisual("", EModelNode::Longsword);

        // Put on our 1h weapon
        m_EquipmentState.equippedItems.equippedWeapon1h = item;
    }
    else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_AMULET) != 0)
    {
        node = EModelNode::None;

        // Put on our amulet
        m_EquipmentState.equippedItems.equippedAmulet = item;
    }
    else if ((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_RING) != 0)
    {
        node = EModelNode::None;

        // Put on our ring
        m_EquipmentState.equippedItems.equippedRings.insert(item);
    }
    else if ((itemData.mainflag & Daedalus::GEngineClasses::C_Item::ITM_CAT_RUNE) != 0 || (itemData.mainflag & Daedalus::GEngineClasses::C_Item::ITM_CAT_MAGIC) != 0)
    {
        node = EModelNode::None;

        // Put on our rune/scroll
        m_EquipmentState.equippedItems.equippedRunes.insert(item);
    }

    // Show visual on the npc-model
    if (node != EModelNode::None)
        model->setNodeVisual(itemData.visual, node);
}

Daedalus::GameState::ItemHandle PlayerController::drawWeaponMelee(bool forceFist)
{
    // Check if we already have a weapon in our hands
    if (m_EquipmentState.weaponMode != EWeaponMode::WeaponNone)
        return m_EquipmentState.activeWeapon;

    ModelVisual* model = getModelVisual();

    // Remove anything that was active before putting something new there
    m_EquipmentState.activeWeapon.invalidate();

    // Check what kind of weapon we got here
    if (!forceFist && m_EquipmentState.equippedItems.equippedWeapon1h.isValid())
    {
        m_EquipmentState.activeWeapon = m_EquipmentState.equippedItems.equippedWeapon1h;
        m_EquipmentState.weaponMode = EWeaponMode::Weapon1h;
    }
    else if (!forceFist && m_EquipmentState.equippedItems.equippedWeapon2h.isValid())
    {
        m_EquipmentState.activeWeapon = m_EquipmentState.equippedItems.equippedWeapon2h;
        m_EquipmentState.weaponMode = EWeaponMode::Weapon2h;
    }
    else
    {
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

void PlayerController::placeOnSurface(const Physics::RayTestResult& hit)
{
    if (DEBUG_PLAYER)
    {
        LogInfo() << (int)getMaterial(hit.hitTriangleIndex) << ", Placing hero at position: " << hit.hitPosition.x << ", " << hit.hitPosition.y << ", " << hit.hitPosition.z;
    }
    Math::Matrix m = getEntityTransform();

    float feet = getModelVisual()->getModelRoot().y;

    // FIXME: Actually read the flying-flag of the MDS
    if (feet == 0.0f)
    {
        feet = 0.9762f;  // FIXME: Boundingbox of the animation or something should be used instead
    }

    m_MoveState.position = hit.hitPosition + Math::float3(0.0f, feet, 0.0f);
    m.Translation(m_MoveState.position);
    setEntityTransform(m);
    setDirection(m_MoveState.direction);
}

void PlayerController::placeOnGround()
{
    if (!m_NPCProperties.enablePhysics)
        return;

    // Check for states
    /*switch(getBodyState())
    {
        case bs_stand:break;
        case bs_walk:break;
        case bs_sneak:break;
        case bs_run:break;
        case bs_sprint:break;
        case bs_swim:break;
        case bs_crawl:break;
        case bs_dive:break;
        case bs_jump:break;
        case bs_climb:break;
        case bs_fall:break;
        case bs_sit:break;
        case bs_lie:break;
        case bs_inventory:break;
        case bs_iteminteract:break;
        case bs_mobinteract:break;
        case bs_mobinteract_interrupt:break;
        case bs_takeitem:break;
        case bs_dropitem:break;
        case bs_throwitem:break;
        case bs_pickpocket:break;
        case bs_stumble:break;
        case bs_unconscious:return; // animation takes care of that. would fall through the ground for some reason otherwise.
        case bs_dead:break;
        case bs_aimnear:break;
        case bs_aimfar:break;
        case bs_hit:break;
        case bs_parade:break;
        case bs_casting:break;
        case bs_petrified:break;
        case bs_controlling:break;
        case bs_max:break;
    }*/

    // Fix position
    Math::float3 entityPosition = getEntityTransform().Translation();
    Math::float3 to = entityPosition + Math::float3(0.0f, -100.0f, 0.0f);
    Math::float3 from = entityPosition + Math::float3(0.0f, 30.0f, 0.0f);

    std::vector<Physics::RayTestResult> hitall = m_World.getPhysicsSystem().raytraceAll(from, to);
    if (hitall.empty())
        return;
    std::sort(hitall.begin(), hitall.end(), [](const Physics::RayTestResult& a, const Physics::RayTestResult& b) {
        return a.hitPosition.y > b.hitPosition.y;
    });

    bool fellThrough = true;
    bool underWater = false;
    bool aboveGround = false;
    bool shallowWater = false;
    float fallthroughEpsilon = 0.25f;  // Give a little room for animations, etc
    Physics::RayTestResult highestHitSurface = hitall[0];
    Physics::RayTestResult waterHitSurface = hitall[0];
    Physics::RayTestResult closestHitGroundSurface = hitall[0];
    float highestHitY = std::numeric_limits<float>::min();
    float closestResult = std::numeric_limits<float>::max();
    for (const auto& result : hitall)
    {
        fellThrough = fellThrough && (result.hitPosition.y - fallthroughEpsilon > entityPosition.y);  // for all results NPC's Y coordinate lower than result position Y coordinate
        if (highestHitY < result.hitPosition.y)
        {
            highestHitY = result.hitPosition.y;
            highestHitSurface = result;
        }
        auto material = getMaterial(result.hitTriangleIndex);
        if (result.hitFlags == Physics::CollisionShape::CT_Object) material = ZenLoad::MaterialGroup::UNDEF;  // we don't want underlying worldmesh material in this case
        if (material == ZenLoad::MaterialGroup::WATER)
        {
            waterHitSurface = result;  // Doesn't matter if there are more water hits atm
        }
        float newDistance = 0.0f;
        if ((newDistance = (std::abs(entityPosition.y - result.hitPosition.y))) < closestResult && material != ZenLoad::MaterialGroup::WATER)
        {
            closestHitGroundSurface = result;
            closestResult = newDistance;
        }
    }

    auto manageState = [&]() {
        if (fellThrough)
        {
            placeOnSurface(highestHitSurface);
            return;
        }

        placeOnSurface(closestHitGroundSurface);
    };
    manageState();
    //     // FIXME: Get rid of the second cast here or at least only do it on the worldmesh!
    if (m_MoveState.ground.successful)
    {
        // Update color
        float shadow = m_World.getWorldMesh().interpolateTriangleShadowValue(m_MoveState.ground.triangleIndex, m_MoveState.ground.trianglePosition);

        if (getModelVisual())
            getModelVisual()->setShadowValue(shadow);
    }
}

void PlayerController::onVisualChanged()
{
    getModelVisual()->getCollisionBBox(m_NPCProperties.collisionBBox);
    m_NPCProperties.modelRoot = getModelVisual()->getModelRoot();

    getModelVisual()->setTransient(true);  // Don't export this from here. Will be rebuilt after loading anyways.

    // Setup callbacks
    getModelVisual()->getAnimationHandler().setCallbackEventSFX([this](const ZenLoad::zCModelScriptEventSfx& sfx) {
        AniEvent_SFX(sfx);
    });

    getModelVisual()->getAnimationHandler().setCallbackEventSFXGround([this](const ZenLoad::zCModelScriptEventSfx& sfx) {
        AniEvent_SFXGround(sfx);
    });

    getModelVisual()->getAnimationHandler().setCallbackEventTag([this](const ZenLoad::zCModelScriptEventTag& tag) {
        AniEvent_Tag(tag);
    });
    getModelVisual()->getAnimationHandler().setCallbackEventPfx([this](const ZenLoad::zCModelScriptEventPfx& pfx) {
        AniEvent_PFX(pfx);
    });
    getModelVisual()->getAnimationHandler().setCallbackEventPfxStop([this](const ZenLoad::zCModelScriptEventPfxStop& pfxStop) {
        AniEvent_PFXStop(pfxStop);
    });
}

void PlayerController::onUpdateByInput(float deltaTime)
{
    ModelVisual* model = getModelVisual();

    if (!model)
        return;

    if (m_World.getDialogManager().isDialogActive())
        return;

    if (m_World.getEngine()->getHud().isMenuActive())
        resetKeyStates();

    // Stand up if wounded
    if (getModelVisual()->isAnimPlaying("S_WOUNDEDB") && getBodyState() == EBodyState::BS_UNCONSCIOUS)
    {
        // Only stand up if the unconscious-state has ended (aka. is not valid anymore)
        // Otherwise, the player would fall down immediately
        if (m_AIStateMachine.isStateActive())
        {
            // FIXME: End UNCONSCIOUS-state here

            getEM().onMessage(EventMessages::ConversationMessage::playAnimation("T_WOUNDEDB_2_STAND"));
            setBodyState(EBodyState::BS_STAND);
        }
    }

    if (!getEM().isEmpty() || getUsedMob().isValid())
        return;

    // TODO: HACK, take this out!
    float moveMod = 1.0f;
    if (m_MoveSpeed1)
        moveMod *= 4.0f;

    if (m_MoveSpeed2)
        moveMod *= 16.0f;

    m_MoveSpeed2 = false;
    m_MoveSpeed1 = false;

    getModelVisual()->getAnimationHandler().setSpeedMultiplier(moveMod);

    m_AIHandler.playerUpdate(deltaTime);
}


void PlayerController::onMessage(SharedEMessage message, Handle::EntityHandle sourceVob)
{
    Controller::onMessage(message, sourceVob);

    bool done = false;
    switch (message->messageType)
    {
        case EventMessages::EventMessageType::Event:
            done = EV_Event(message, sourceVob);
            break;
        case EventMessages::EventMessageType::Npc:
            done = EV_Npc(std::dynamic_pointer_cast<EventMessages::NpcMessage>(message), sourceVob);
            break;
        case EventMessages::EventMessageType::Damage:
            done = EV_Damage(std::dynamic_pointer_cast<EventMessages::DamageMessage>(message), sourceVob);
            break;
        case EventMessages::EventMessageType::Weapon:
            done = EV_Weapon(std::dynamic_pointer_cast<EventMessages::WeaponMessage>(message), sourceVob);
            break;
        case EventMessages::EventMessageType::Movement:
            done = EV_Movement(std::dynamic_pointer_cast<EventMessages::MovementMessage>(message), sourceVob);
            break;
        case EventMessages::EventMessageType::Attack:
            done = EV_Attack(std::dynamic_pointer_cast<EventMessages::AttackMessage>(message), sourceVob);
            break;
        case EventMessages::EventMessageType::UseItem:
            done = EV_UseItem(std::dynamic_pointer_cast<EventMessages::UseItemMessage>(message), sourceVob);
            break;
        case EventMessages::EventMessageType::State:
            done = EV_State(std::dynamic_pointer_cast<EventMessages::StateMessage>(message), sourceVob);
            break;
        case EventMessages::EventMessageType::Manipulate:
            done = EV_Manipulate(std::dynamic_pointer_cast<EventMessages::ManipulateMessage>(message), sourceVob);
            break;
        case EventMessages::EventMessageType::Conversation:
            done = EV_Conversation(std::dynamic_pointer_cast<EventMessages::ConversationMessage>(message), sourceVob);
            break;
        case EventMessages::EventMessageType::Magic:
            done = EV_Magic(std::dynamic_pointer_cast<EventMessages::MagicMessage>(message), sourceVob);
            break;
        case EventMessages::EventMessageType::Mob:
            //TODO handle this somehow?
            break;
    }

    // Flag as deleted if this is done
    message->deleted = done;
}

bool PlayerController::EV_Event(SharedEMessage message, Handle::EntityHandle sourceVob)
{
    return false;
}

bool PlayerController::EV_Npc(std::shared_ptr<EventMessages::NpcMessage> message, Handle::EntityHandle sourceVob)
{
    return false;
}

bool PlayerController::EV_Damage(std::shared_ptr<EventMessages::DamageMessage> message, Handle::EntityHandle sourceVob)
{
    return false;
}

bool PlayerController::EV_Weapon(std::shared_ptr<EventMessages::WeaponMessage> message, Handle::EntityHandle sourceVob)
{
    return false;
}

bool PlayerController::EV_Movement(std::shared_ptr<EventMessages::MovementMessage> sharedMessage, Handle::EntityHandle sourceVob)
{
    auto& message = *sharedMessage;
    switch (static_cast<EventMessages::MovementMessage::MovementSubType>(message.subType))
    {
        case EventMessages::MovementMessage::ST_RobustTrace:
            break;
        case EventMessages::MovementMessage::ST_GotoFP:
        {
            if(message.isFirstRun)
            {
                using namespace Components;

                assert(message.targetVob.isValid());
                assert(hasComponent<SpotComponent>(m_World.getEntity<EntityComponent>(message.targetVob)));
                assert(hasComponent<PositionComponent>(m_World.getEntity<EntityComponent>(message.targetVob)));

                // How long the FP should count as occupied. Gothic uses a default value of 30 seconds
                const float secondsOccupied = 30;
                m_World.markFreepointOccupied(message.targetVob, m_Entity, secondsOccupied);

                Components::PositionComponent& pos = m_World.getEntity<Components::PositionComponent>(message.targetVob);
                message.targetPosition = pos.m_WorldMatrix.Translation();

                gotoPosition(pos.m_WorldMatrix.Translation());
            }
            else
            {
                if(!m_PathFinder.hasActiveRouteBeenCompleted(getEntityTransform().Translation()))
                {
                    travelPath();
                }
            }

            return m_PathFinder.hasActiveRouteBeenCompleted(getEntityTransform().Translation());
        }
        break;

        case EventMessages::MovementMessage::ST_GotoVob:
        {
            if(message.isFirstRun)
            {
                Math::float3 pos;

                // Find a waypoint with that name
                World::Waynet::WaypointIndex wp = World::Waynet::getWaypointIndex(m_World.getWaynet(),
                                                                                  message.targetVobName);

                if (wp != World::Waynet::INVALID_WAYPOINT)
                {
                    gotoWaypoint(wp);
                } else
                {
                    // This must be an actual vob
                    Handle::EntityHandle v = message.targetVob;

                    if (!v.isValid())
                    {
                        v = m_World.getVobEntityByName(message.targetVobName);
                    }

                    // isEntityValid can be false if the npc entity was removed from this world while this message was queued
                    if (v.isValid() && m_World.isEntityValid(v))
                    {
                        gotoVob(v);
                    }
                }
            }
            else
            {
                if(!m_PathFinder.hasActiveRouteBeenCompleted(getEntityTransform().Translation()))
                {
                    travelPath();
                }
            }

            return m_PathFinder.hasActiveRouteBeenCompleted(getEntityTransform().Translation());
        }
        break;

        case EventMessages::MovementMessage::ST_GotoPos:
        {
            if(message.isFirstRun)
            {
                gotoPosition(message.targetPosition);
            }
            else
            {
                if(!m_PathFinder.hasActiveRouteBeenCompleted(getEntityTransform().Translation()))
                {
                    travelPath();
                }
            }

            return m_PathFinder.hasActiveRouteBeenCompleted(getEntityTransform().Translation());
        }
        break;

        case EventMessages::MovementMessage::ST_GoRoute:
            break;
        case EventMessages::MovementMessage::ST_Turn:
            break;

        case EventMessages::MovementMessage::ST_TurnToVob:
        {
            if (!m_World.isEntityValid(message.targetVob))
                return true;  // case: player entity was removed from this world while this message was queued

            Vob::VobInformation vob = Vob::asVob(m_World, message.targetVob);

            // Fill position-field
            message.targetPosition = Vob::getTransform(vob).Translation();

            // Fall through to ST_TurnToPos now
        }

        case EventMessages::MovementMessage::ST_TurnToPos:
        {
            Math::float3 dir = (message.targetPosition - getEntityTransform().Translation());
            dir.y = 0.0f;  // Don't let the NPC face upwards/downwards
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
            setWalkMode(message.walkMode);
            return true;
            break;

        case EventMessages::MovementMessage::ST_WhirlAround:
            break;
        case EventMessages::MovementMessage::ST_Standup:
            // Start standing up when we first see this message or nothing is playing yet
            if (!message.inUse || !getModelVisual()->getAnimationHandler().getActiveAnimationPtr())
            {
                standUp(false, message.targetMode != 0);
                message.inUse = true;
            }

            // Go as long as the standup-animation is playing
            return m_NPCAnimationHandler.isStanding();
            break;

        case EventMessages::MovementMessage::ST_CanSeeNpc:
            break;  // Unused
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

bool PlayerController::EV_Attack(std::shared_ptr<EventMessages::AttackMessage> message, Handle::EntityHandle sourceVob)
{
    return false;
}

bool PlayerController::EV_UseItem(std::shared_ptr<EventMessages::UseItemMessage> message, Handle::EntityHandle sourceVob)
{
    return false;
}

bool PlayerController::EV_State(std::shared_ptr<EventMessages::StateMessage> sharedMessage, Handle::EntityHandle sourceVob)
{
    auto& message = *sharedMessage;
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
            }
            else
            {
                // Start daily routine
                m_AIStateMachine.startRoutineState();
            }
        }
            return true;
            break;

        case EventMessages::StateMessage::EV_Wait:
            message.waitTime -= static_cast<float>(m_World.getEngine()->getGameClock().getLastDt());
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

bool PlayerController::EV_Manipulate(std::shared_ptr<EventMessages::ManipulateMessage> sharedMessage, Handle::EntityHandle sourceVob)
{
    auto& message = *sharedMessage;
    switch (static_cast<EventMessages::ManipulateMessage::ManipulateSubType>(message.subType))
    {
        case EventMessages::ManipulateMessage::ST_TakeVob:
            break;
        case EventMessages::ManipulateMessage::ST_DropVob:
            break;
        case EventMessages::ManipulateMessage::ST_ThrowVob:
            break;
        case EventMessages::ManipulateMessage::ST_Exchange:
            break;
        case EventMessages::ManipulateMessage::ST_UseMob:
            break;

        case EventMessages::ManipulateMessage::ST_UseItem:
        {
            if (!message.targetItem.isValid())
            {
                // We need a name now
                if (message.symIdx == static_cast<size_t>(-1))
                    return true;

                // Try to find this in our inventory
                Daedalus::GameState::ItemHandle h = m_Inventory.getItem(message.symIdx);

                if (h.isValid())
                    message.targetItem = h;
                else
                    return true;  // Could not find that item
            }

            // Get item data

            // Execute the items script-function

            return true;
        }
        break;

        case EventMessages::ManipulateMessage::ST_InsertInteractItem:
            break;
        case EventMessages::ManipulateMessage::ST_RemoveInteractItem:
            break;
        case EventMessages::ManipulateMessage::ST_CreateInteractItem:
            break;
        case EventMessages::ManipulateMessage::ST_DestroyInteractItem:
            break;
        case EventMessages::ManipulateMessage::ST_PlaceInteractItem:
            break;
        case EventMessages::ManipulateMessage::ST_ExchangeInteractItem:
            break;
        case EventMessages::ManipulateMessage::ST_UseMobWithItem:
            break;

        case EventMessages::ManipulateMessage::ST_CallScript:

            if (message.symIdx != static_cast<size_t>(-1))
            {
                m_World.getScriptEngine().prepareRunFunction();
                m_World.getScriptEngine().runFunctionBySymIndex(message.symIdx);
            }
            return false;

        case EventMessages::ManipulateMessage::ST_EquipItem:
            break;
        case EventMessages::ManipulateMessage::ST_UseItemToState:
            break;
        case EventMessages::ManipulateMessage::ST_TakeMob:
            break;
        case EventMessages::ManipulateMessage::ST_DropMob:
            break;
        case EventMessages::ManipulateMessage::ST_ManipMax:
            break;
    }
    return false;
}

bool PlayerController::EV_Conversation(std::shared_ptr<EventMessages::ConversationMessage> sharedMessage,
                                       Handle::EntityHandle sourceVob)
{
    using EventMessages::ConversationMessage;
    auto& message = *sharedMessage;
    bool isMonolog = false;
    switch (static_cast<ConversationMessage::ConversationSubType>(message.subType))
    {
        case ConversationMessage::ST_PlayAniSound:
            break;

        case ConversationMessage::ST_PlayAni:
        {
            Animations::Animation* active = getModelVisual()->getAnimationHandler().getActiveAnimationPtr();

            if (message.status == ConversationMessage::Status::INIT)
            {
                if (isPlayerControlled())
                {
                    LogInfo() << "PLAYER: New animation started: " << message.animation;
                }

                // In case the passed animation doesn't exist, we want to be in a defined state
                getModelVisual()->stopAnimations();
                getModelVisual()->setAnimation(message.animation, false);
                active = getModelVisual()->getAnimationHandler().getActiveAnimationPtr();
                message.status = ConversationMessage::Status::PLAYING;
            }

            // Go as long as this animation is playing
            bool done = !active || active->m_Name != message.animation;

            if (done && isPlayerControlled())
                LogInfo() << "PLAYER: Done with animation: " << message.animation;

            return done;
        }
        break;

        case ConversationMessage::ST_PlaySound:
            break;

        case ConversationMessage::ST_LookAt:
            break;

        case ConversationMessage::ST_OutputMonolog:
            // case: AI_OutputSVM which are not scheduled in Dialog
            // Vatras' preach or Herold's announcement, smalltalk, ALARM...
            isMonolog = true;
        // fall through
        case ConversationMessage::ST_Output:
        {
            auto& subtitleBox = m_World.getDialogManager().getSubtitleBox();
            // TODO: Rework this, when the animation-system is nicer. Need a cutscene system!
            if (message.status == ConversationMessage::Status::INIT)
            {
                message.status = ConversationMessage::Status::PLAYING;

                if (!isMonolog)
                {
                    m_World.getDialogManager().setCurrentMessage(sharedMessage);
                    std::string characterName = getScriptInstance().name[0];
                    m_World.getDialogManager().displaySubtitle(message.text, characterName);
                    m_World.getCameraController()->setDialogueTargetName(characterName);
                    m_World.getCameraController()->nextDialogueShot();
                    subtitleBox.setScaling(0.0);
                    subtitleBox.setGrowDirection(+1.0f);
                }

                // Play the random dialog gesture
                startDialogAnimation();
                // Play sound of this conv-message
                message.soundTicket = m_World.getAudioWorld().playSound(message.name, getEntityTransform().Translation(), DEFAULT_CHARACTER_SOUND_RANGE);
            }

            if (message.status == ConversationMessage::Status::PLAYING)
            {
                bool playingFinished;
#ifdef RE_USE_SOUND
                if (message.canceled)
                {
                    m_World.getAudioWorld().stopSound(message.soundTicket);
                }
                // toggle this bool to switch auto skip when sound ended
                // TODO: read from config maybe
                const bool autoPlay = true;
                if (autoPlay)
                {
                    bool isPlaying = m_World.getAudioWorld().soundIsPlaying(message.soundTicket);
                    playingFinished = !isPlaying;
                }
                else
                {
                    playingFinished = message.canceled;
                }
#else
                // when sound is disabled, message must be skipped manually
                playingFinished = message.canceled;
#endif
                if (playingFinished)
                {
                    message.status = ConversationMessage::Status::FADING_OUT;
                    if (!isMonolog)
                        subtitleBox.setGrowDirection(-1.0f);
                }
            }

            if (message.status == ConversationMessage::Status::FADING_OUT)
            {
                if (isMonolog || subtitleBox.getScaling() == 0.0f)
                {
                    if (!isMonolog)
                        m_World.getDialogManager().stopDisplaySubtitle();
                    getModelVisual()->stopAnimations();
                    return true;
                }
            }
            return false;
        }
        break;
        case ConversationMessage::ST_OutputEnd:
            m_World.getDialogManager().updateChoices(getScriptHandle());
            return true;
        case ConversationMessage::ST_Cutscene:
            break;
        case ConversationMessage::ST_WaitTillEnd:
            return message.canceled;
        case ConversationMessage::ST_Ask:
            break;
        case ConversationMessage::ST_WaitForQuestion:
            break;
        case ConversationMessage::ST_StopLookAt:
            break;
        case ConversationMessage::ST_StopPointAt:
            break;
        case ConversationMessage::ST_PointAt:
            break;
        case ConversationMessage::ST_QuickLook:
            break;
        case ConversationMessage::ST_PlayAni_NoOverlay:
            break;
        case ConversationMessage::ST_PlayAni_Face:
            break;
        case ConversationMessage::ST_ProcessInfos:
            break;
        case ConversationMessage::ST_StopProcessInfos:
            m_World.getDialogManager().endDialog();
            return true;
        case ConversationMessage::ST_OutputSVM_Overlay:
            break;
        case ConversationMessage::ST_SndPlay:
            break;
        case ConversationMessage::ST_SndPlay3d:
            break;
        case ConversationMessage::ST_PrintScreen:
            break;
        case ConversationMessage::ST_StartFx:
            break;
        case ConversationMessage::ST_StopFx:
            break;
        case ConversationMessage::ST_ConvMax:
            break;
    }

    return false;
}

bool PlayerController::EV_Magic(std::shared_ptr<EventMessages::MagicMessage> message, Handle::EntityHandle sourceVob)
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
    if (getModelVisual()->isAnimPlaying("S_BENCH_S1") || getModelVisual()->isAnimPlaying("S_THRONE_S1"))
        d *= -1.0f;

    // Set direction
    setEntityTransform(Math::Matrix::CreateLookAt(m_MoveState.position,
                                                  m_MoveState.position + d,
                                                  Math::float3(0, 1, 0))
                           .Invert());
}

void PlayerController::applyRotationY(float rad)
{
    m_MoveState.direction = Math::Matrix::CreateRotationY(rad) * m_MoveState.direction;
    setDirection(m_MoveState.direction);
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

    const int NUM_DIALOG_ANIMATIONS = 20;  // This is hardcoded in the game
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
    const float MAX_ANGLE = 0.5f * Math::PI;  // 90 degrees

    Components::PositionComponent& otherPos = m_World.getEntity<Components::PositionComponent>(entity);

    // Trace from the top of our BBox (eyes)
    Math::float3 start = getEntityTransform().Translation() + Math::float3(0.0f, m_NPCProperties.collisionBBox[1].y, 0.0f);

    Math::float3 end = otherPos.m_WorldMatrix.Translation();

    // Check senses_range first
    float len2 = (end - start).length();

    unsigned int sensesRange = static_cast<unsigned int>(getScriptInstance().senses_range);
    if (static_cast<uint32_t>(len2) > sensesRange * sensesRange)
        return false;

    // Do the raytest to the other object
    Physics::RayTestResult res = m_World.getPhysicsSystem().raytrace(
        start,
        end,
        Physics::CollisionShape::CT_WorldMesh);  // FIXME: Should trace everything except the two objects in question!

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
    Math::float3 start = getEntityTransform().Translation() + Math::float3(0.0f, m_NPCProperties.collisionBBox[1].y, 0.0f);

    // Do the raytest to the other object
    Physics::RayTestResult res = m_World.getPhysicsSystem().raytrace(
        start,
        target,
        Physics::CollisionShape::CT_WorldMesh);  // FIXME: Should trace everything except the two objects in question!

    return !res.hasHit;
}

float PlayerController::getAngleTo(const Math::float3& pos)
{
    Math::float3 dir = (getEntityTransform().Translation() - pos).normalize();

    return atan(m_MoveState.direction.dot(dir));
}

void PlayerController::standUp(bool walkingAllowed, bool startAniTransition)
{
    setBodyState(BS_STAND);

    if (!startAniTransition)
    {
        // Just jump to the idle-animation
        getModelVisual()->stopAnimations();
    }

    m_AIHandler.standup();
}

void PlayerController::setRoutineFunc(size_t symRoutine)
{
    getScriptInstance().daily_routine = static_cast<uint32_t>(symRoutine);
    m_AIStateMachine.reinitRoutine();
}

void PlayerController::changeRoutine(const std::string& routineName)
{
    if (routineName.empty())
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
    if (!canUse(item))
        return false;

    Daedalus::GEngineClasses::C_Item& data = m_World.getScriptEngine().getGameState().getItem(item);

    // Food?
    if ((data.flags & Daedalus::GEngineClasses::C_Item::ITM_CAT_FOOD) != 0)
    {
        // Nutrition doesn't seem to be used anywhere...
        changeAttribute(Daedalus::GEngineClasses::C_Npc::EATR_HITPOINTS, data.nutrition);
    }

    // Weapon?
    if ((data.mainflag & Daedalus::GEngineClasses::C_Item::ITM_CAT_NF) != 0 || (data.mainflag & Daedalus::GEngineClasses::C_Item::ITM_CAT_FF) != 0 || (data.mainflag & Daedalus::GEngineClasses::C_Item::ITM_CAT_ARMOR) != 0 || (data.mainflag & Daedalus::GEngineClasses::C_Item::ITM_CAT_MAGIC) != 0)
    {
        // FIXME: Hack to only allow equipping when no weapon is drawn
        if (getWeaponMode() == EWeaponMode::WeaponNone)
            equipItem(item);
        return false;
    }

    // Call script function to be executed on use
    if (data.on_state[0])
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
    if (!item.isValid())
        return false;

    // TODO: Check talents

    // Check attributes
    Daedalus::GEngineClasses::C_Item& data = m_World.getScriptEngine().getGameState().getItem(item);
    Daedalus::GEngineClasses::C_Npc& npc = getScriptInstance();
    ScriptEngine& s = m_World.getScriptEngine();

    for (int i = 0; i < Daedalus::GEngineClasses::C_Item::COND_ATR_MAX; i++)
    {
        // Why is 0 not allowed? That's how gothic is doing it though, as it seems...
        if (data.cond_atr[i] > 0)
        {
            assert(data.cond_atr[i] < Daedalus::GEngineClasses::C_Npc::EATR_MAX);

            // Check for enough strength, etc.
            if (npc.attribute[data.cond_atr[i]] < data.cond_value[i])
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

    if ((uint32_t)atr > Daedalus::GEngineClasses::C_Npc::EATR_MAX)
        return;

    // Apply change
    npc.attribute[atr] = value;

    // Clamp to 0
    if (npc.attribute[atr] < 0)
        npc.attribute[atr] = 0;

    // Clamp to max
    if (atr == Daedalus::GEngineClasses::C_Npc::EATR_HITPOINTS)
        npc.attribute[atr] = std::min(npc.attribute[atr],
                                      npc.attribute[Daedalus::GEngineClasses::C_Npc::EATR_HITPOINTSMAX]);

    if (atr == Daedalus::GEngineClasses::C_Npc::EATR_MANA)
        npc.attribute[atr] = std::min(npc.attribute[atr],
                                      npc.attribute[Daedalus::GEngineClasses::C_Npc::EATR_MANAMAX]);
}

void PlayerController::changeAttribute(Daedalus::GEngineClasses::C_Npc::EAttributes atr, int change)
{
    Daedalus::GEngineClasses::C_Npc& npc = getScriptInstance();
    // TODO: Godmode right here

    if (!change)
        return;  // Nothing to do here...

    if ((uint32_t)atr > Daedalus::GEngineClasses::C_Npc::EATR_MAX)
        return;  // Invalid value set from script?

    // Check for immortality
    if (atr == Daedalus::GEngineClasses::C_Npc::EATR_HITPOINTS && npc.flags & Daedalus::GEngineClasses::C_Npc::EFLAG_IMMORTAL && change != -999)  // Yep, this is in the game!
    {
        return;
    }

    // Apply change
    npc.attribute[atr] += change;

    // Clamp to 0
    if (npc.attribute[atr] < 0)
        npc.attribute[atr] = 0;

    // Clamp to max
    if (atr == Daedalus::GEngineClasses::C_Npc::EATR_HITPOINTS)
        npc.attribute[atr] = std::min(npc.attribute[atr],
                                      npc.attribute[Daedalus::GEngineClasses::C_Npc::EATR_HITPOINTSMAX]);

    if (atr == Daedalus::GEngineClasses::C_Npc::EATR_MANA)
        npc.attribute[atr] = std::min(npc.attribute[atr],
                                      npc.attribute[Daedalus::GEngineClasses::C_Npc::EATR_MANAMAX]);

    // TODO: Switch animation overlay (wounded, etc)

    // Check for death
    if (atr == Daedalus::GEngineClasses::C_Npc::EATR_HITPOINTS)
    {
        if (npc.attribute[atr] == 0)
            die(m_Entity);
        //else if(npc.attribute[atr] == 1)
        // TODO: Drop unconscious
    }
}

void PlayerController::giveItem(const std::string& instanceName, unsigned int count)
{
    size_t symIdx = m_World.getScriptEngine().getSymbolIndexByName(instanceName);

    if (symIdx == static_cast<size_t>(-1))
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
    j["scriptObj"]["aivar"] = Utils::putArray(scriptObj.aivar);
    j["scriptObj"]["wp"] = scriptObj.wp;
    j["scriptObj"]["exp"] = scriptObj.exp;
    j["scriptObj"]["exp_next"] = scriptObj.exp_next;
    j["scriptObj"]["lp"] = scriptObj.lp;

    // Export inventory
    m_Inventory.exportInventory(j["inventory"]);

    // Export equipped items
    {
        j["equipped"] = json::array();
        for (auto item : m_EquipmentState.equippedItemsAll)
        {
            // Write instance of the equipped item
            Daedalus::GEngineClasses::C_Item& data = m_World.getScriptEngine().getGameState().getItem(item);
            std::string instanceName = m_World.getScriptEngine().getVM().getDATFile().getSymbolByIndex(data.instanceSymbol).name;

            j["equipped"].push_back(instanceName);
        }
    }

    // export refusetalktime
    j["refusetalktime"] = this->m_RefuseTalkTime;

    // Import state
    m_AIStateMachine.exportScriptState(j["AIState"]);
}

void PlayerController::importObject(const json& j, bool noTransform)
{
    if (!noTransform)
    {
        Controller::importObject(j);

        // Teleport to position
        {
            // need to copy since changing position sets the direction of the transform matrix
            auto forward = getEntityTransform().Forward();
            teleportToPosition(getEntityTransform().Translation());
            setDirection(-1.0f * forward);
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
        Utils::putArray(scriptObj.aivar, j["scriptObj"]["aivar"]);
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

        for (const std::string& sym : j["equipped"])
        {
            Daedalus::GameState::ItemHandle h = inv.getItem(sym);

            assert(h.isValid());  // Item to equip MUST be inside the inventory

            equipItem(h);
        }
    }

    // import refusetalktime
    this->setRefuseTalkTime(static_cast<float>(j["refusetalktime"]));

    // Import state
    m_AIStateMachine.importScriptState(j["AIState"]);

    // Check for death
    if (getScriptInstance().attribute[Daedalus::GEngineClasses::C_Npc::EATR_HITPOINTS] == 0)
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

    if (!e.isValid())
        return Handle::EntityHandle::makeInvalidHandle();

    VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(world, e);

    // Load controller-values
    npc.playerController->importObject(j);

    return e;
}

void PlayerController::die(Handle::EntityHandle attackingNPC)
{
    if (getBodyState() == EBodyState::BS_DEAD)
        return;

    interrupt();

    // TODO: Drop weapons held in hand

    setBodyState(EBodyState::BS_DEAD);

    if (!m_AIStateMachine.isInState(NPC_PRGAISTATE_DEAD))
    {
        Daedalus::GameState::NpcHandle oldOther = m_World.getScriptEngine().getNPCFromSymbol("other");

        if (attackingNPC.isValid())
        {
            VobTypes::NpcVobInformation attacker = VobTypes::asNpcVob(m_World, attackingNPC);
            m_World.getScriptEngine().setInstanceNPC("other", VobTypes::getScriptHandle(attacker));
        }
        else
        {
            m_World.getScriptEngine().setInstanceNPC("other", Daedalus::GameState::NpcHandle());
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
        "T_WOUNDEDB_TRY"};

    for (size_t i = 0; i < Utils::arraySize(woundedAnims); i++)
    {
        if (getModelVisual()->isAnimPlaying(woundedAnims[i]))
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

    hud.setHealth(stats.attribute[Daedalus::GEngineClasses::C_Npc::EATR_HITPOINTS],
                  stats.attribute[Daedalus::GEngineClasses::C_Npc::EATR_HITPOINTSMAX]);

    hud.setMana(stats.attribute[Daedalus::GEngineClasses::C_Npc::EATR_MANA],
                stats.attribute[Daedalus::GEngineClasses::C_Npc::EATR_MANAMAX]);
}

std::string PlayerController::getGuildName()
{
    // Guilds are stored as an array in a symbol called "TXT_GUILDS"
    auto& sym = m_World.getScriptEngine().getVM().getDATFile().getSymbolByName("TXT_GUILDS");
    return sym.getString((unsigned int)getScriptInstance().guild);
}

void PlayerController::updateStatusScreen(UI::Menu_Status& statsScreen)
{
    auto& stats = getScriptInstance();

    statsScreen.setAttribute(UI::Menu_Status::A_STR, stats.attribute[Daedalus::GEngineClasses::C_Npc::EATR_STRENGTH]);
    statsScreen.setAttribute(UI::Menu_Status::A_DEX, stats.attribute[Daedalus::GEngineClasses::C_Npc::EATR_DEXTERITY]);

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

ZenLoad::MaterialGroup PlayerController::getMaterial(uint32_t triangleIdx)
{
    Math::float3 v3[3];
    uint8_t matgroup;
    // Beware! If triangle index is given such that the triangle is a building triangle of a VOB, this function will return material of the underlying worldmesh!!!
    m_World.getWorldMesh().getTriangle(triangleIdx, v3, matgroup);
    return static_cast<ZenLoad::MaterialGroup>(matgroup);
}

ZenLoad::MaterialGroup PlayerController::getSurfaceMaterial()
{
    if (m_MoveState.ground.successful)
    {
        return getMaterial(m_MoveState.ground.triangleIndex);
    }
    else
    {
        return ZenLoad::MaterialGroup::UNDEF;
    }
}

void PlayerController::traceDownNPCGround()
{
    m_MoveState.ground.successful = false;  // initial condition defaulted to false
    Math::float3 to = getEntityTransform().Translation();
    Math::float3 from = to;
    Math::float3 entityPos = to;
    to.y = -1000.0f;
    from.y = 10000.0f;
    std::vector<Physics::RayTestResult> hitall = m_World.getPhysicsSystem().raytraceAll(from, to, Physics::CollisionShape::CT_WorldMesh);
    if (hitall.empty())
    {
        return;
    }
    std::sort(hitall.begin(), hitall.end(), [](const Physics::RayTestResult& a, const Physics::RayTestResult& b) {
        return a.hitPosition.y > b.hitPosition.y;
    });
    if (DEBUG_PLAYER)
    {
        LogInfo() << "traceDownNPCGround ITERATION BEGIN";
    }
    Physics::RayTestResult result = hitall[0];
    Physics::RayTestResult resultWater = hitall[0];
    bool waterMatFound = false;
    float closestGroundSurfacePos = std::numeric_limits<float>::max();
    float underWaterGroundPos = std::numeric_limits<float>::min();
    float waterSurfacePos = 0;
    for (const auto& a : hitall)
    {
        if (!a.hasHit)
            continue;
        if ((m_MoveState.ground.successful = m_MoveState.ground.successful || a.hasHit))
        {
            auto diff = std::abs(entityPos.y - a.hitPosition.y);

            if (ZenLoad::MaterialGroup::WATER == getMaterial(a.hitTriangleIndex))
            {
                resultWater = a;
                waterSurfacePos = a.hitPosition.y;
                waterMatFound = true;  // found water material
            }
            else if (closestGroundSurfacePos > diff)
            {
                result = a;
                closestGroundSurfacePos = diff;
                underWaterGroundPos = a.hitPosition.y;
            }
        }
    }

    float feet = getModelVisual()->getModelRoot().y;

    // FIXME: Actually read the flying-flag of the MDS
    if (feet == 0.0f)
    {
        feet = 0.9762f;  // FIXME: Boundingbox of the animation or something should be used instead
    }

    float feetPos = entityPos.y - feet;
    if (waterMatFound /*&& feetPos < waterSurfacePos*/)
    {
        m_MoveState.ground.waterDepth = std::abs(waterSurfacePos - underWaterGroundPos);
        if (DEBUG_PLAYER)
        {
            LogInfo() << "Water depth: " << m_MoveState.ground.waterDepth;
        }
    }
    else
    {
        m_MoveState.ground.waterDepth = 0.0f;
    }
    if (DEBUG_PLAYER)
    {
        LogInfo() << "Initial position: " << (getEntityTransform().Translation()).x << " " << (getEntityTransform().Translation()).y << " " << (getEntityTransform().Translation()).z;
        LogInfo() << "From: " << (from).x << " " << (from).y << " " << (from).z;
        LogInfo() << "To: " << (to).x << " " << (to).y << " " << (to).z;
        LogInfo() << "traceDownNPCGround ITERATION END";
    }
    m_MoveState.ground.successful = true;
    m_MoveState.ground.triangleIndex = result.hitTriangleIndex;
    m_MoveState.ground.trianglePosition = result.hitPosition;
}

void PlayerController::resetKeyStates()
{
    m_isForward = false;
    m_MoveSpeed1 = false;
    m_MoveSpeed2 = false;
}

void PlayerController::updatePfxPosition(const pfxEvent &e)
{
    Vob::VobInformation vob = Vob::asVob(m_World, e.entity);
    auto &boneTransforms = getNpcAnimationHandler().getAnimHandler().getObjectSpaceTransforms();
    auto &transform = getEntityTransform();
    size_t nodeIndex = getModelVisual()->findNodeIndex(e.bodyPosition);
    auto position = transform * boneTransforms[nodeIndex];
    Vob::setTransform(vob, std::move(position));
}

void PlayerController::updatePfx()
{
    //First remove all invalid handles / emitter that are in "canBeRemoved" state (all particles are dead)
    for(auto it = m_activePfxEvents.begin(); it<m_activePfxEvents.end();)
    {
        if(((PfxVisual*)Vob::asVob(m_World, (*it).entity).visual)->canBeRemoved())
        {
            m_World.removeEntity((*it).entity);
            it = m_activePfxEvents.erase(it);
        }
        else
        {
            //If pfx is attached to body, update the position
            if((*it).isAttached)
                updatePfxPosition(*it);

            ++it;
        }
    }
}

void PlayerController::AniEvent_SFX(const ZenLoad::zCModelScriptEventSfx& sfx)
{
    // FIXME: Workaround. "Whoosh"-sound is used as kill trigger
    if (Utils::toUpper(sfx.m_Name) == "WHOOSH")
    {
        Handle::EntityHandle nearestNPC;
        float shortestDistNPC = 3.0f;
        for (const Handle::EntityHandle& h : m_World.getScriptEngine().getWorldNPCs())
        {
            if (h != m_World.getScriptEngine().getPlayerEntity())
            {
                VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(m_World, h);

                float dist = (Vob::getTransform(npc).Translation() -
                              getEntityTransform().Translation())
                                 .lengthSquared();
                if (dist < shortestDistNPC && npc.playerController->getBodyState() != EBodyState::BS_DEAD)
                {
                    nearestNPC = h;
                    shortestDistNPC = dist;
                }
            }
        }

        VobTypes::NpcVobInformation toKill = VobTypes::asNpcVob(m_World, nearestNPC);
        if (toKill.isValid())
        {
            toKill.playerController->die(m_Entity);
        }
    }

    if(!sfx.m_EmptySlot && m_World.getAudioWorld().soundIsPlaying(m_MainNoiseSoundSlot))
    {
        // If emptyslot is not set, the currently played sound shall be stopped
        m_World.getAudioWorld().stopSound(m_MainNoiseSoundSlot);
    }

    // Play sound specified in the event
    float range = sfx.m_Range != 0.0f ? sfx.m_Range : DEFAULT_CHARACTER_SOUND_RANGE;

    auto ticket = m_World.getAudioWorld().playSound(sfx.m_Name, getEntityTransform().Translation(), range);

    if(!sfx.m_EmptySlot)
    {
        // If emptyslot is not set, the currently played sound shall be stopped

        if(m_World.getAudioWorld().soundIsPlaying(m_MainNoiseSoundSlot))
            m_World.getAudioWorld().stopSound(m_MainNoiseSoundSlot);

        m_MainNoiseSoundSlot = ticket;
    }
}

void PlayerController::AniEvent_SFXGround(const ZenLoad::zCModelScriptEventSfx& sfx)
{
    if (m_MoveState.ground.successful)
    {
        // Play sound depending on ground type
        ZenLoad::MaterialGroup mat = getMaterial(m_MoveState.ground.triangleIndex);

        float range = sfx.m_Range != 0.0f ? sfx.m_Range : DEFAULT_CHARACTER_SOUND_RANGE;

        std::string soundfile = sfx.m_Name + "_" + ZenLoad::zCMaterial::getMatGroupString(mat);

        auto ticket = m_World.getAudioWorld().playSoundVariantRandom(soundfile, getEntityTransform().Translation(), range);

        if(!sfx.m_EmptySlot)
        {
            // If emptyslot is not set, the currently played sound shall be stopped

            if(m_World.getAudioWorld().soundIsPlaying(m_MainNoiseSoundSlot))
                m_World.getAudioWorld().stopSound(m_MainNoiseSoundSlot);

            m_MainNoiseSoundSlot = ticket;
        }
    }
}

void PlayerController::AniEvent_Tag(const ZenLoad::zCModelScriptEventTag& tag)
{
    //if(tag.m_Tag == )
}
void PlayerController::AniEvent_PFX(const ZenLoad::zCModelScriptEventPfx& pfx)
{
    if(!m_World.getPfxManager().hasPFX(pfx.m_Name))
    {
        return;
    }
    pfxEvent event = {Vob::constructVob(m_World), pfx.m_Pos, pfx.m_isAttached, pfx.m_Num};
    //From world of gothic animation events
    if(event.bodyPosition.empty())
    {
        event.bodyPosition = "BIP01";
    }
    m_activePfxEvents.push_back(std::move(event));
    Vob::VobInformation vob = Vob::asVob(m_World, m_activePfxEvents.back().entity);
    Vob::setVisual(vob, pfx.m_Name+".PFX");
	Vob::setTransform(vob, getEntityTransform());
}
void PlayerController::AniEvent_PFXStop(const ZenLoad::zCModelScriptEventPfxStop& pfxStop)
{
	//FIXME there is the error (at icedragon at oldworld) that there are pfxStop Events when no active pfx events are present...
	if(m_activePfxEvents.empty())
		LogWarn() << "No corresponding pfx Event for Stop Event of Animation " + getNpcAnimationHandler().getAnimHandler().getActiveAnimationPtr()->m_Name;

    //Kill pfx with the corresponding number
    for (auto &pfx : m_activePfxEvents) {
		if (pfx.m_Num == pfxStop.m_Num) {
			Vob::VobInformation vob = Vob::asVob(m_World, pfx.entity);
			PfxVisual *visual = (PfxVisual *) vob.visual;
			if (visual->isDead()) {
				continue;
			}
			visual->killPfx();
			break;
		}
	}


}

World::Waynet::WaypointIndex PlayerController::getClosestWaypoint()
{
    return World::Waynet::findNearestWaypointTo(m_World.getWaynet(), getEntityTransform().Translation());
}

void PlayerController::setWalkMode(PlayerController::WalkMode walkMode)
{
    m_NPCAnimationHandler.setWalkMode(walkMode);
}

World::Waynet::WaypointIndex PlayerController::getSecondClosestWaypoint()
{
    using namespace World::Waynet;

    const WaynetInstance& waynet = m_World.getWaynet();
    if (waynet.waypoints.size() < 2)
        return INVALID_WAYPOINT;

    const Math::float3 position = getEntityTransform().Translation();

    std::vector<std::pair<float, size_t>> distances;
    distances.reserve(waynet.waypoints.size());
    for (size_t i = 0; i < waynet.waypoints.size(); i++)
    {
        float l2 = (position - waynet.waypoints[i].position).lengthSquared();
        distances.emplace_back(l2, i);
    }
    auto compare = [](const auto& pair1, const auto& pair2){
        return pair1.first < pair2.first;
    };
    std::nth_element(distances.begin(), distances.begin() + 1, distances.end(), compare);
    return distances[1].second;
}

void PlayerController::onAction(Engine::ActionType actionType, bool triggered, float intensity)
{
    using Engine::ActionType;

    m_AIHandler.onAction(actionType, triggered);

    switch (actionType)
    {
        case ActionType::PlayerForward:
        {
            // Increment state, if currently using a mob
            if (getUsedMob().isValid() && triggered)
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
            }
        }
            break;
        case ActionType::PlayerBackward:
        {
            // Increment state, if currently using a mob
            if (getUsedMob().isValid() && triggered)
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
            }
        }
            break;

        case ActionType::DebugMoveSpeed:
            m_MoveSpeed1 = m_MoveSpeed1 || triggered;
            break;
        case ActionType::DebugMoveSpeed2:
            m_MoveSpeed2 = m_MoveSpeed2 || triggered;
            break;
        case ActionType::PlayerRotate:
        {
            auto dir = getDirection();
            auto deltaPhi = 0.02f * intensity; // TODO window width influences this???
            dir = Math::Matrix::rotatedPointAroundLine(dir, {0, 0, 0}, getEntityTransform().Up(), deltaPhi);
            setDirection(dir);
        }
            break;
        case ActionType::PlayerAction:
        {
            if (triggered)
            {
                if (m_World.getDialogManager().isDialogActive())
                    return;

                if (getWeaponMode() != EWeaponMode::WeaponNone)
                    return;

                // ----- ITEMS -----
                Handle::EntityHandle nearestItem;
                float shortestDistItem = 5.0f;
                const std::set<Handle::EntityHandle>& items = m_World.getScriptEngine().getWorldItems();
                for (Handle::EntityHandle h : items)
                {
                    Math::Matrix& m = m_World.getEntity<Components::PositionComponent>(h).m_WorldMatrix;

                    float dist = (m.Translation() -
                        getEntityTransform().Translation())
                        .lengthSquared();
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
                            getEntityTransform().Translation())
                            .lengthSquared();
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
                        getEntityTransform().Translation())
                        .lengthSquared();

                    if (dist < shortestDistMob)
                    {
                        nearestMob = h;
                        shortestDistMob = dist;
                    }
                }

                int nearest = 0;

                if (shortestDistItem < shortestDistMob && shortestDistItem < shortestDistNPC)
                    nearest = 1;

                if (shortestDistMob < shortestDistItem && shortestDistMob < shortestDistNPC)
                    nearest = 3;

                if (shortestDistNPC < shortestDistItem && shortestDistNPC < shortestDistMob)
                    nearest = 2;

                if (nearest == 1 && nearestItem.isValid())
                {
                    // Pick it up
                    VobTypes::ItemVobInformation item = VobTypes::asItemVob(m_World, nearestItem);
                    item.itemController->pickUp(m_Entity);

                    getEM().onMessage(EventMessages::ConversationMessage::playAnimation("c_Stand_2_IGet_1"));
                    getEM().onMessage(EventMessages::ConversationMessage::playAnimation("c_IGet_2_Stand_1"));

                    return;
                }

                // ----- TALKING ----

                if (nearest == 2 && nearestNPC.isValid())
                {
                    VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(m_World, nearestNPC);

                    if (npc.playerController->getBodyState() == BS_STAND)
                    {
                        Daedalus::GameState::NpcHandle shnpc = VobTypes::getScriptHandle(npc);
                        m_World.getDialogManager().assessTalk(shnpc);
                    }
                    else if (npc.playerController->getBodyState() == BS_UNCONSCIOUS || npc.playerController->getBodyState() == BS_DEAD)
                    {
                        // Take all his items
                        auto& inv = npc.playerController->getInventory();
                        for (auto h : inv.getItems())
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
                if (!getInventory().getItems().empty())
                {
                    Daedalus::GameState::ItemHandle lastItem = getInventory().getItems().back();
                    if (lastItem.isValid())
                    {
                        if (useItem(lastItem))
                            getInventory().removeItem(lastItem);
                    }
                }
            }
        }
            break;


        case ActionType::PlayerActionContinous:
            break;

        default:
            break;
    }
}
