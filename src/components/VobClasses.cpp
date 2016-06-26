//
// Created by andre on 02.06.16.
//

#include "VobClasses.h"
#include <engine/World.h>
#include <logic/PlayerController.h>
#include <logic/visuals/ModelVisual.h>
#include <utils/logger.h>
#include "EntityActions.h"
#include <engine/BaseEngine.h>

Handle::EntityHandle VobTypes::initNPCFromScript(World::WorldInstance& world, Daedalus::GameState::NpcHandle scriptInstance)
{
    Handle::EntityHandle e = Vob::constructVob(world);

    Daedalus::GEngineClasses::C_Npc& npc = world.getScriptEngine().getGameState().getNpc(scriptInstance);

    //LogInfo() << "Instance: " << scriptInstance.index;
    //LogInfo() << "Creating vob for: " << npc.name[0];

              // Link the script instance to our entity
    ScriptInstanceUserData* userData = new ScriptInstanceUserData;
    userData->vobEntity = e;
    userData->world = world.getMyHandle();

    world.getScriptEngine().getGameState().getNpc(scriptInstance).userPtr = userData;

    // Setup the playercontroller
    Components::LogicComponent& logic = world.getEntity<Components::LogicComponent>(e);
    logic.m_pLogicController = new Logic::PlayerController(world, e, scriptInstance);

    // Assign a default visual
    Vob::VobInformation vob = Vob::asVob(world, e);
    Vob::setVisual(vob, "HUM_BODY_NAKED0.MDM");

    // FIXME: Debug, remove this
    Components::BBoxComponent& bbox = world.getEntity<Components::BBoxComponent>(e);
    bbox.m_DebugColor = 0;//0xFFFFFFFF;
    bbox.m_BBox3D.min = Math::float3(-1,-1,-1);
    bbox.m_BBox3D.max = Math::float3(1,1,1);

    return e;
}

void ::VobTypes::unlinkNPCFromScriptInstance(World::WorldInstance& world, Handle::EntityHandle entity,
                                             Daedalus::GameState::NpcHandle scriptInstance)
{
    ScriptInstanceUserData* userdata = reinterpret_cast<ScriptInstanceUserData*>(world.getScriptEngine().getGameState().getNpc(scriptInstance).userPtr);
    delete userdata;

    world.getScriptEngine().getGameState().getNpc(scriptInstance).userPtr = nullptr;
}

VobTypes::NpcVobInformation VobTypes::asNpcVob(World::WorldInstance& world, Handle::EntityHandle e)
{
    Vob::VobInformation v = Vob::asVob(world, e);
    NpcVobInformation npc;

    // Copy over everything from the subclass. This is safe, as VobInformation is just a POD.
    memcpy(&npc, &v, sizeof(v));

    // TODO: Add some typechecking
    // Enter new information
    npc.playerController = reinterpret_cast<Logic::PlayerController*>(npc.logic);

    return npc;
}

Handle::EntityHandle VobTypes::getEntityFromScriptInstance(World::WorldInstance& world, Daedalus::GameState::NpcHandle npc)
{
    void* userptr = world.getScriptEngine().getGameState().getNpc(npc).userPtr;

    if(!userptr)
        return Handle::EntityHandle::makeInvalidHandle();

    assert(world.getMyHandle() == reinterpret_cast<ScriptInstanceUserData*>(userptr)->world);

    return reinterpret_cast<ScriptInstanceUserData*>(userptr)->vobEntity;
}

void ::VobTypes::NPC_SetModelVisual(VobTypes::NpcVobInformation& vob, const std::string& visual)
{
    Handle::EntityHandle e = vob.entity;

    // Initialize animations
    Components::AnimationComponent& anim = Components::Actions::initComponent<Components::AnimationComponent>(vob.world->getComponentAllocator(), e);

    // Strip extension
    std::string libName = visual.substr(0, visual.find_last_of('.'));

    anim.m_AnimHandler.loadMeshLibFromVDF(libName, vob.world->getEngine()->getVDFSIndex());

    // TODO: Move to other place
    anim.m_AnimHandler.addAnimation(libName + "-S_RUNL.MAN", vob.world->getEngine()->getVDFSIndex());
    anim.m_AnimHandler.addAnimation(libName + "-S_WALKL.MAN", vob.world->getEngine()->getVDFSIndex());
    anim.m_AnimHandler.addAnimation(libName + "-S_FISTRUNL.MAN", vob.world->getEngine()->getVDFSIndex());
    anim.m_AnimHandler.addAnimation(libName + "-S_FISTWALKL.MAN", vob.world->getEngine()->getVDFSIndex());

    anim.m_AnimHandler.addAnimation(libName + "-S_RUN.MAN", vob.world->getEngine()->getVDFSIndex());
    anim.m_AnimHandler.addAnimation(libName + "-S_WALK.MAN", vob.world->getEngine()->getVDFSIndex());
    anim.m_AnimHandler.addAnimation(libName + "-S_FISTRUN.MAN", vob.world->getEngine()->getVDFSIndex());
    anim.m_AnimHandler.addAnimation(libName + "-S_FISTWALK.MAN", vob.world->getEngine()->getVDFSIndex());

    anim.m_AnimHandler.playAnimation("S_RUNL");
}

void ::VobTypes::NPC_SetHeadMesh(VobTypes::NpcVobInformation &vob, const std::string &visual, size_t headTextureIdx,
                                 size_t teethTextureIdx)
{
    // TODO: Use head/teeth texture indices
    reinterpret_cast<Logic::ModelVisual*>(vob.visual)->setHeadMesh(visual);
}

void ::VobTypes::NPC_ReplaceMainVisual(VobTypes::NpcVobInformation &vob, const std::string &visual)
{
    Logic::ModelVisual* model = reinterpret_cast<Logic::ModelVisual*>(vob.visual);
    Logic::ModelVisual::BodyState oldState = model->getBodyState();

    // Replace visual
    Vob::setVisual(vob, visual);

    // Pointer changed, re-get this
    model = reinterpret_cast<Logic::ModelVisual*>(vob.visual);

    // Set the other stuff again
    model->setHeadMesh(oldState.headVisual);
}

void ::VobTypes::NPC_EquipWeapon(VobTypes::NpcVobInformation &vob, Daedalus::GameState::ItemHandle weapon)
{
    Daedalus::GEngineClasses::C_Item& itemData = vob.world->getScriptEngine().getGameState().getItem(weapon);
    Logic::ModelVisual* model = reinterpret_cast<Logic::ModelVisual*>(vob.visual);

    Logic::EModelNode node = Logic::EModelNode::Bow;

    if((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_2HD_AXE) != 0)
        node = Logic::EModelNode::Longsword;
    else if((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_2HD_SWD) != 0)
        node = Logic::EModelNode::Longsword;
    else if((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_CROSSBOW) != 0)
        node = Logic::EModelNode::Crossbow;
    else if((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_BOW) != 0)
        node = Logic::EModelNode::Bow;
    else if((itemData.flags & Daedalus::GEngineClasses::C_Item::ITEM_SWD) != 0)
        node = Logic::EModelNode::Sword;

    // TODO: This is only doing visuals right now!
    // Close-ranged (Swords, etc)
    if((itemData.mainflag & Daedalus::GEngineClasses::C_Item::ITM_CAT_NF) != 0)
    {
        model->setNodeVisual(itemData.visual, node);
    }else if((itemData.mainflag & Daedalus::GEngineClasses::C_Item::ITM_CAT_FF) != 0)
    {
        model->setNodeVisual(itemData.visual, node);
    }
}



