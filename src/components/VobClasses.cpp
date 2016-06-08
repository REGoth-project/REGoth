//
// Created by andre on 02.06.16.
//

#include "VobClasses.h"
#include <engine/World.h>
#include <logic/PlayerController.h>
#include <logic/visuals/ModelVisual.h>
#include <utils/logger.h>

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
    bbox.m_DebugColor = 0xFFFFFFFF;
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



