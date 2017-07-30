//
// Created by andre on 02.06.16.
//

#include "VobClasses.h"
#include "EntityActions.h"
#include <engine/BaseEngine.h>
#include <engine/World.h>
#include <logic/ItemController.h>
#include <logic/MobController.h>
#include <logic/PlayerController.h>
#include <logic/SoundController.h>
#include <logic/mobs/Container.h>
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

    Components::PositionComponent& pos = world.getEntity<Components::PositionComponent>(e);
    pos.m_DrawDistanceFactor = 0.25f;  // TODO: Config entry for this...

    // Setup the playercontroller
    Components::LogicComponent& logic = world.getEntity<Components::LogicComponent>(e);
    logic.m_pLogicController = new Logic::PlayerController(world, e, scriptInstance);

    // Assign a default visual
    Vob::VobInformation vob = Vob::asVob(world, e);
    Vob::setVisual(vob, "HUM_BODY_NAKED0.MDM");

    // FIXME: Debug, remove this
    Components::BBoxComponent& bbox = world.getEntity<Components::BBoxComponent>(e);
    bbox.m_DebugColor = 0;  //0xFFFFFFFF;
    bbox.m_BBox3D.min = Math::float3(-1, -1, -1);
    bbox.m_BBox3D.max = Math::float3(1, 1, 1);

    return e;
}

Handle::EntityHandle VobTypes::initItemFromScript(World::WorldInstance& world, size_t scriptInstance)
{
    Handle::EntityHandle e = Vob::constructVob(world);

    // Get values
    Daedalus::GameState::ItemHandle h = world.getScriptEngine().getGameState().insertItem(scriptInstance);
    Daedalus::GEngineClasses::C_Item& data = world.getScriptEngine().getGameState().getItem(h);

    // Extract visual and
    std::string visual = data.visual;

    // Kill script-object
    world.getScriptEngine().getGameState().removeItem(h);

    // Setup itemcontroller
    Components::LogicComponent& logic = world.getEntity<Components::LogicComponent>(e);
    logic.m_pLogicController = new Logic::ItemController(world, e, scriptInstance);

    Vob::VobInformation vob = Vob::asVob(world, e);
    Vob::setVisual(vob, visual);

    return e;
}

Handle::EntityHandle VobTypes::createMob(World::WorldInstance& world)
{
    Handle::EntityHandle e = Vob::constructVob(world);

    // Setup controller
    Components::LogicComponent& logic = world.getEntity<Components::LogicComponent>(e);
    logic.m_pLogicController = new Logic::MobController(world, e);

    // Initialize animations
    Components::AnimationComponent& anim = Components::Actions::initComponent<Components::AnimationComponent>(world.getComponentAllocator(), e);

    //anim.m_AnimHandler.setWorld(world);
    //anim.m_AnimHandler.loadMeshLibFromVDF(libName, world->getEngine()->getVDFSIndex());

    return e;
}

Handle::EntityHandle VobTypes::createSound(World::WorldInstance& world)
{
    Handle::EntityHandle e = Vob::constructVob(world);

    // Setup controller
    Components::LogicComponent& logic = world.getEntity<Components::LogicComponent>(e);
    logic.m_pLogicController = new Logic::SoundController(world, e);

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
    NpcVobInformation v;

    // Fill base-class values (just POD-values in here)
    *reinterpret_cast<Vob::VobInformation*>(&v) = Vob::asVob(world, e);

    // Check the controller
    if (v.logic && v.logic->getControllerType() != Logic::EControllerType::PlayerController)
    {
        // Invalidate instance and return
        v = {};
        return v;
    }

    // Enter new information
    v.playerController = reinterpret_cast<Logic::PlayerController*>(v.logic);

    return v;
}

VobTypes::ItemVobInformation VobTypes::asItemVob(World::WorldInstance& world, Handle::EntityHandle e)
{
    Vob::VobInformation v = Vob::asVob(world, e);
    ItemVobInformation item;

    // Check the controller
    if (v.logic && v.logic->getControllerType() != Logic::EControllerType::ItemController)
    {
        // Invalidate instance and return
        item = {};
        return item;
    }

    // Copy over everything from the subclass. This is safe, as VobInformation is just a POD.
    memcpy(&item, &v, sizeof(v));

    // Enter new information
    item.itemController = reinterpret_cast<Logic::ItemController*>(item.logic);

    return item;
}

VobTypes::MobVobInformation VobTypes::asMobVob(World::WorldInstance& world, Handle::EntityHandle e)
{
    Vob::VobInformation v = Vob::asVob(world, e);
    MobVobInformation mob;

    // Check the controller
    if (v.logic && v.logic->getControllerType() != Logic::EControllerType::MobController)
    {
        // Invalidate instance and return
        mob = {};
        return mob;
    }

    // Copy over everything from the subclass. This is safe, as VobInformation is just a POD.
    memcpy(&mob, &v, sizeof(v));

    // Enter new information
    mob.mobController = reinterpret_cast<Logic::MobController*>(mob.logic);

    return mob;
}

VobTypes::SoundVobInformation VobTypes::asSoundVob(World::WorldInstance& world, Handle::EntityHandle e)
{
    Vob::VobInformation v = Vob::asVob(world, e);
    SoundVobInformation snd;

    // Check the controller
    if (v.logic && v.logic->getControllerType() != Logic::EControllerType::SoundController)
    {
        // Invalidate instance and return
        snd = {};
        return snd;
    }

    // Copy over everything from the subclass. This is safe, as VobInformation is just a POD.
    memcpy(&snd, &v, sizeof(v));

    // Enter new information
    snd.soundController = reinterpret_cast<Logic::SoundController*>(snd.logic);

    return snd;
}

Handle::EntityHandle VobTypes::getEntityFromScriptInstance(World::WorldInstance& world, Daedalus::GameState::NpcHandle npc)
{
    if (!npc.isValid())
        return Handle::EntityHandle::makeInvalidHandle();

    void* userptr = world.getScriptEngine().getGameState().getNpc(npc).userPtr;

    if (!userptr)
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

    anim.getAnimHandler().setWorld(*vob.world);
    anim.getAnimHandler().loadMeshLibFromVDF(libName, vob.world->getEngine()->getVDFSIndex());

    // Meshlib changed, re-get all animation
    vob.playerController->getNpcAnimationHandler().initAnimations();

    // TODO: Move to other place (MDS)
    // Load all default animations
    for (int i = 0; i < Logic::ModelVisual::NUM_ANIMATIONS; i++)
    {
        const char* name = Logic::ModelVisual::getAnimationName(static_cast<Logic::ModelVisual::EModelAnimType>(i));

        anim.getAnimHandler().addAnimation(name);
    }

    for (int i = 0; i < 20; i++)
    {
        std::string ns = std::to_string(i + 1);
        if (ns.size() == 1)
            ns = "0" + ns;

        anim.getAnimHandler().addAnimation("T_DIALOGGESTURE_" + ns);
    }

    anim.getAnimHandler().addAnimation("-S_RUNL");
    anim.getAnimHandler().addAnimation("-S_WALKL");
    anim.getAnimHandler().addAnimation("-S_FISTRUNL");
    anim.getAnimHandler().addAnimation("-S_FISTWALKL");

    anim.getAnimHandler().addAnimation("-S_RUN");
    anim.getAnimHandler().addAnimation("-S_WALK");
    anim.getAnimHandler().addAnimation("-S_FISTRUN");
    anim.getAnimHandler().addAnimation("-S_FISTWALK");

    anim.getAnimHandler().addAnimation("-T_JUMPB");
    anim.getAnimHandler().addAnimation("-T_RUNSTRAFEL");
    anim.getAnimHandler().addAnimation("-T_RUNSTRAFER");

    // Fist
    anim.getAnimHandler().addAnimation("-S_FISTATTACK");

    // 1H
    anim.getAnimHandler().addAnimation("-S_1HATTACK");

    //anim.getAnimHandler().playAnimation("S_RUNL");
}

void ::VobTypes::NPC_SetHeadMesh(VobTypes::NpcVobInformation& vob, const std::string& visual, int headTextureIdx,
                                 int teethTextureIdx)
{
    Logic::ModelVisual* model = reinterpret_cast<Logic::ModelVisual*>(vob.visual);

    // TODO: Use head/teeth texture indices
    model->setHeadMesh(visual, headTextureIdx, teethTextureIdx);
}

void ::VobTypes::NPC_SetBodyMesh(VobTypes::NpcVobInformation& vob, const std::string& visual, int bodyTexIdx, int skinColorIdx)
{
    Logic::ModelVisual* model = reinterpret_cast<Logic::ModelVisual*>(vob.visual);
    Logic::ModelVisual::BodyState state = model->getBodyState();

    state.bodyVisual = visual;

    if (state.bodyVisual.find_first_of('.') == std::string::npos)
        state.bodyVisual += ".MDM";

    if (bodyTexIdx != -1)
        state.bodyTextureIdx = static_cast<int>(bodyTexIdx);
    //else
    //    state.bodyTextureIdx = 0; // Need to reset these to 0, otherwise the skin-texture would stay in some cases

    if (skinColorIdx != -1)  // Leave skin color as it is when only equipping an armor
        state.bodySkinColorIdx = static_cast<int>(skinColorIdx);

    model->setBodyState(state);

    if (vob.logic)
        vob.logic->onVisualChanged();
}

void ::VobTypes::NPC_EquipWeapon(VobTypes::NpcVobInformation& vob, Daedalus::GameState::ItemHandle weapon)
{
    vob.playerController->equipItem(weapon);
}

Daedalus::GameState::NpcHandle VobTypes::getScriptHandle(VobTypes::NpcVobInformation& vob)
{
    return vob.playerController->getScriptHandle();
}

Daedalus::GEngineClasses::C_Npc& ::VobTypes::getScriptObject(VobTypes::NpcVobInformation& vob)
{
    return vob.world->getScriptEngine().getGameState().getNpc(vob.playerController->getScriptHandle());
}

Handle::EntityHandle VobTypes::Wld_InsertNpc(World::WorldInstance& world, size_t instanceSymbol, const std::string& wpName)
{
    // Use script-engine to insert the NPC
    Daedalus::GameState::NpcHandle npc = world.getScriptEngine().getGameState().insertNPC(instanceSymbol, wpName);

    // Get engine-side entity of the created npc
    return getEntityFromScriptInstance(world, npc);
}

Handle::EntityHandle VobTypes::Wld_InsertNpc(World::WorldInstance& world, const std::string& instanceName, const std::string& wpName)
{
    // Use script-engine to insert the NPC
    Daedalus::GameState::NpcHandle npc = world.getScriptEngine().getGameState().insertNPC(instanceName, wpName);

    // Get engine-side entity of the created npc
    return getEntityFromScriptInstance(world, npc);
}

Daedalus::GameState::ItemHandle VobTypes::NPC_DrawMeleeWeapon(VobTypes::NpcVobInformation& npc)
{
    return npc.playerController->drawWeaponMelee();
}

void ::VobTypes::NPC_UndrawWeapon(VobTypes::NpcVobInformation& npc)
{
    npc.playerController->undrawWeapon();
}

VobTypes::NpcVobInformation VobTypes::getVobFromScriptHandle(World::WorldInstance& world, Daedalus::GameState::NpcHandle npc)
{
    Handle::EntityHandle e = getEntityFromScriptInstance(world, npc);

    return VobTypes::asNpcVob(world, e);
}

Handle::EntityHandle VobTypes::createItem(World::WorldInstance& world, const std::string& item)
{
    size_t h = world.getScriptEngine().getVM().getDATFile().getSymbolIndexByName(item);

    Handle::EntityHandle e = VobTypes::initItemFromScript(world, h);
    return e;
}

Handle::EntityHandle VobTypes::createItem(World::WorldInstance& world, size_t item)
{
    return VobTypes::initItemFromScript(world, item);
}

int ::VobTypes::MOB_GetItemCount(VobTypes::MobVobInformation mob, const std::string& instance)
{
    Logic::MobCores::Container* core = dynamic_cast<Logic::MobCores::Container*>(mob.mobController->getCore());

    if (core)
        return (int)core->getNumItemsOf(instance);

    return 0;
}

Handle::EntityHandle VobTypes::MOB_GetByName(World::WorldInstance& world, const std::string& name)
{
    for (Handle::EntityHandle e : world.getScriptEngine().getWorldMobs())
    {
        MobVobInformation mob = VobTypes::asMobVob(world, e);

        if (mob.isValid() && mob.mobController->getFocusName() == name)
            return e;
    }

    return Handle::EntityHandle::makeInvalidHandle();
}

void VobTypes::Wld_RemoveNpc(World::WorldInstance& world, Handle::EntityHandle npc)
{
    VobTypes::NpcVobInformation vob = VobTypes::asNpcVob(world, npc);

    // if npc is the current controlled entity clear up bindings, "hero" and invalidate player entity
    if (npc == world.getScriptEngine().getPlayerEntity())
    {
        // clear key bindings
        Engine::Input::clearActions();
        world.getScriptEngine().setPlayerEntity(Handle::EntityHandle::makeInvalidHandle());
        auto invalidHandle = Daedalus::GameState::NpcHandle();
        world.getScriptEngine().setInstanceNPC("hero", invalidHandle);
    }

    world.getScriptEngine().unregisterNpc(npc);
    world.getScriptEngine().getGameState().removeNPC(vob.playerController->getScriptHandle());
    world.removeEntity(npc);
}
