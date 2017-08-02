#include "MobController.h"
#include "PlayerController.h"
#include "visuals/ModelVisual.h"
#include <ZenLib/utils/logger.h>
#include <ZenLib/zenload/zTypes.h>
#include <components/EntityActions.h>
#include <components/Vob.h>
#include <components/VobClasses.h>
#include <debugdraw/debugdraw.h>
#include <engine/BaseEngine.h>
#include <logic/mobs/Bed.h>
#include <logic/mobs/Container.h>
#include <logic/mobs/Ladder.h>

using namespace Logic;

MobController* MobCore::getMobController()
{
    return (MobController*)m_World.getEntity<Components::LogicComponent>(m_Entity).m_pLogicController;
}

void MobCore::exportCore(json& j)
{
    j["scheme"] = m_Scheme;
    j["stateNum"] = m_StateNum;
}

void MobCore::importCore(const json& j)
{
    m_Scheme = j["scheme"];
    m_StateNum = j["stateNum"];
}

MobController::MobController(World::WorldInstance& world, Handle::EntityHandle entity)
    : Controller(world, entity)
{
    m_NumNpcsMax = 0;
    m_NumNpcsCurrent = 0;
    m_MobCore = nullptr;
    m_lockCamera = true;

    m_World.getScriptEngine().registerMob(m_Entity);
}

MobController::~MobController()
{
    m_World.getScriptEngine().unregisterMob(m_Entity);

    delete m_MobCore;
}

ModelVisual* MobController::getModelVisual()
{
    Vob::VobInformation vob = Vob::asVob(m_World, m_Entity);

    if (!vob.visual || vob.visual->getVisualType() != EVisualType::Model)
        return nullptr;

    return reinterpret_cast<ModelVisual*>(vob.visual);
}

void MobController::onUpdate(float deltaTime)
{
    Controller::onUpdate(deltaTime);
    ModelVisual* model = getModelVisual();

    // Update node attachments
    if (model)
    {
        // Update model for this frame
        model->onFrameUpdate(deltaTime);
    }

    /*ddPush();

    ddSetTransform(nullptr);
    ddSetColor(0xFFFFFFFF);
    //ddSetStipple(true, 1.0f);

    const float wpAxisLen = 1.0f;

    for(auto& i : m_InteractPositions)
    {
        Math::float3 p = getEntityTransform() * i.transform.Translation();
        ddMoveTo(getEntityTransform().Translation().v);
        ddLineTo(p.v);

        ddDrawAxis(p.x, p.y, p.z, wpAxisLen);
    }


    ddPop();*/

    //if(getModelVisual())
    //   getModelVisual()->getAnimationHandler().debugDrawSkeleton(getEntityTransform());
}

void MobController::findInteractPositions()
{
    ModelVisual* model = getModelVisual();

    // Some mobs seem to be just static meshes without any nodes
    if (!model || model->getAnimationHandler().getObjectSpaceTransforms().empty())
        return;

    const std::vector<ZenLoad::ModelNode>& nodes = model->getMeshLib().getNodes();

    for (size_t i = 0; i < nodes.size(); i++)
    {
        const ZenLoad::ModelNode& n = nodes[i];

        // Check if this is a position-node
        if (n.name.find("ZS_POS") != std::string::npos)
        {
            m_InteractPositions.emplace_back();

            // Get transformed position in object-space
            m_InteractPositions.back().transform = model->getAnimationHandler().getObjectSpaceTransforms()[i];
            m_InteractPositions.back().distance = n.name.find("DIST") != std::string::npos;
            m_InteractPositions.back().nodeName = n.name;

            //LogInfo() << "Interact-position: " << n.name;

            // Check if this can be used by multiple npcs
            if (n.name.find("NPC") != std::string::npos)
                m_NumNpcsMax++;
        }
    }

    if (!m_NumNpcsMax)
        m_NumNpcsMax = 1;
}

InteractPosition* MobController::findFreePosition(Handle::EntityHandle npc, float maxDistance)
{
    if (m_NumNpcsCurrent >= m_NumNpcsMax)
        return nullptr;  // Mob is full

    // Square for faster distance computation
    float maxDistance2 = maxDistance * maxDistance;

    // Get npcs position
    Math::float3 npcPosition = m_World.getEntity<Components::PositionComponent>(npc).m_WorldMatrix.Translation();
    VobTypes::NpcVobInformation npcVob = VobTypes::asNpcVob(m_World, npc);

    bool wrongSide = false;
    float minDist = FLT_MAX;
    bool playerTooFar = false;
    InteractPosition* found = nullptr;
    for (InteractPosition& p : m_InteractPositions)
    {
        if (p.usedByNpc == npc)
            return &p;  // This npc already using the mob

        if (p.usedByNpc.isValid())
            return nullptr;  // Other npc using this

        Math::float3 worldPosition = p.transform.Translation() + getEntityTransform().Translation();

        // Get distance from npc to node
        float dist2 = (worldPosition - npcPosition).lengthSquared();

        if (dist2 < minDist)
        {
            // Check if we can see this position if not only distance matters
            if (dist2 < maxDistance2 && !p.distance)
            {
                if (!npcVob.playerController->freeLineOfSight(worldPosition))
                {
                    wrongSide = true;
                    continue;
                }
            }

            // If this is a player-charakter, also check the max allowed distance, since he should not run all the way there
            // by himself, but let the actual player do that
            if (npcVob.playerController->isPlayerControlled())
            {
                if (!p.distance && dist2 > maxDistance2)
                {
                    playerTooFar = true;
                    continue;
                }
            }

            found = &p;
            minDist = dist2;
        }
    }

    if (!found && npcVob.playerController->isPlayerControlled())
    {
        // Notfiy the player about the mob being too far or standing on the wrong side
        if (playerTooFar)
        {
            EventMessages::ManipulateMessage msg;
            msg.subType = EventMessages::ManipulateMessage::ST_CallScript;
            msg.symIdx = m_World.getScriptEngine().getSymbolIndexByName(PlayerScriptInfo::PLAYER_MOB_TOO_FAR_AWAY);

            npcVob.playerController->getEM().onMessage(msg, m_Entity);
        }
        else if (wrongSide)
        {
            EventMessages::ManipulateMessage msg;
            msg.subType = EventMessages::ManipulateMessage::ST_CallScript;
            msg.symIdx = m_World.getScriptEngine().getSymbolIndexByName(PlayerScriptInfo::PLAYER_MOB_WRONG_SIDE);

            npcVob.playerController->getEM().onMessage(msg, m_Entity);
        }
    }

    return found;
}

void MobController::initFromVobDescriptor(const ZenLoad::zCVobData& vob)
{
    m_FocusName = vob.vobName;
    m_zObjectClass = vob.objectClass;

    if (m_FocusName == "Bed")
        m_MobCore = new MobCores::Bed(m_World, m_Entity);
    else if (m_FocusName == "Ladder")
        m_MobCore = new MobCores::Ladder(m_World, m_Entity);
    else if (m_zObjectClass.find("oCMobContainer:") != std::string::npos)
    {
        MobCores::Container* cnt = new MobCores::Container(m_World, m_Entity);

        // Insert the items into the container (chest)
        cnt->createContents(vob.oCMobContainer.contains);

        m_MobCore = cnt;
    }
    else
        m_MobCore = new MobCore(m_World, m_Entity);

    assert(m_MobCore);

    // Apply animation scheme
    m_MobCore->setSchemeName(m_FocusName);

    // Setting the visual AFTER the mobcore is important, since then the scheme-name will be propergated
    // since it is derived from the visual-name
}

void MobController::initFromJSONDescriptor(const json& j)
{
    m_FocusName = j["focusName"];
    m_zObjectClass = j["objectClass"];

    if (m_FocusName == "Bed")
        m_MobCore = new MobCores::Bed(m_World, m_Entity);
    else if (m_FocusName == "Ladder")
        m_MobCore = new MobCores::Ladder(m_World, m_Entity);
    else if (m_zObjectClass.find("oCMobContainer:") != std::string::npos)
        m_MobCore = new MobCores::Container(m_World, m_Entity);
    else
        m_MobCore = new MobCore(m_World, m_Entity);

    assert(m_MobCore);

    // Apply animation scheme
    m_MobCore->setSchemeName(m_FocusName);

    m_MobCore->importCore(j["core"]);

    // Setting the visual AFTER the mobcore is important, since then the scheme-name will be propergated
    // since it is derived from the visual-name
}

void MobController::onMessage(std::shared_ptr<EventMessages::EventMessage> message, Handle::EntityHandle sourceVob)
{
    Controller::onMessage(message, sourceVob);
    assert(message->messageType == EventMessages::EventMessageType::Mob);

    EventMessages::MobMessage& msg = *std::dynamic_pointer_cast<EventMessages::MobMessage>(message);
    switch ((EventMessages::MobMessage::MobSubType)msg.subType)
    {
        case EventMessages::MobMessage::ST_STARTINTERACTION:
            startInteraction(msg.npc);
            break;
        case EventMessages::MobMessage::ST_STARTSTATECHANGE:
            startStateChange(msg.npc, msg.stateFrom, msg.stateTo);
            break;
        case EventMessages::MobMessage::ST_ENDINTERACTION:
            stopInteraction(msg.npc);
            break;
        case EventMessages::MobMessage::ST_UNLOCK:
            break;
        case EventMessages::MobMessage::ST_LOCK:
            break;
        case EventMessages::MobMessage::ST_CALLSCRIPT:
            callOnStateFunc(msg.npc, msg.stateTo);
            break;
    }
}

void MobController::callOnStateFunc(Handle::EntityHandle npc, int state)
{
    ScriptEngine& s = m_World.getScriptEngine();

    std::string symName = m_OnStateFuncName + "_S" + std::to_string(state);
    size_t idx = s.getSymbolIndexByName(symName);

    if (idx != static_cast<size_t>(-1))
    {
        VobTypes::NpcVobInformation nv = VobTypes::asNpcVob(m_World, npc);
        s.setInstanceNPC("self", VobTypes::getScriptHandle(nv));
        s.setInstanceItem("item", nv.playerController->getInteractItem());

        s.prepareRunFunction();
        s.runFunctionBySymIndex(idx);
    }
}

void MobController::sendCallOnStateFunc(Handle::EntityHandle npc, int state)
{
    if (!m_OnStateFuncName.empty())
    {
        EventMessages::MobMessage msg;
        msg.subType = EventMessages::MobMessage::ST_CALLSCRIPT;
        msg.stateTo = state;
        msg.npc = npc;

        getEM().onMessage(msg, npc);
    }
}

void MobController::startInteraction(Handle::EntityHandle npc)
{
    if (isInteractingWith(npc))
        return;  // Don't allow interacting twice on the same npc

    // Move NPC to it's position
    setIdealPosition(npc);

    LogInfo() << "unlock camera";
    m_lockCamera = false;

    // Play starting animation
    VobTypes::NpcVobInformation nv = VobTypes::asNpcVob(m_World, npc);

    // This NPC is now using a mob
    nv.playerController->setUsedMob(m_Entity);

    std::string ani = "T_" + m_MobCore->getSchemeName() + "_STAND_2_S" + std::to_string(m_MobCore->getState());

    LogInfo() << "MOB: Playing animation on player: " << ani;
    nv.playerController->getModelVisual()->setAnimation(ani);

    EventMessages::ConversationMessage sm;
    sm.subType = EventMessages::ConversationMessage::ST_PlayAni;
    sm.animation = ani;

    // This is save, because mobs generally won't be deleted from the map.
    // If not, this will just do nothing
    sm.addDoneCallback(m_Entity, [=](Handle::EntityHandle hostVob, std::shared_ptr<EventMessages::EventMessage>) {

        LogInfo() << "lock camera";
        m_lockCamera = true;

        // Re-get everything to make sure it is still there
        /*VobTypes::NpcVobInformation nv = VobTypes::asNpcVob(m_World, npc);
        VobTypes::MobVobInformation mob = VobTypes::asMobVob(m_World, hostVob);

        if(!nv.isValid() || !mob.isValid())
            return; // Either mob or NPC isn't there anymore

        // Go to next state immediately, G2 style. // TODO: This is still only for testing
        mob.mobController->startStateChange(npc, 0, 1);*/
    });

    nv.playerController->getEM().onMessage(sm);

    m_NumNpcsCurrent++;
}

void MobController::startStateChange(Handle::EntityHandle npc, int from, int to)
{
    VobTypes::NpcVobInformation nv = VobTypes::asNpcVob(m_World, npc);

    if (from == -1)
        from = m_MobCore->getState();

    if (to != -1)
        m_MobCore->onBeginStateChange(npc, from, to);

    // Find out which animations to play
    std::string mobAni, npcAni;
    getAnimationTransitionNames(from, to, mobAni, npcAni);

    // Push an animation-message for the npc
    EventMessages::ConversationMessage sm;
    sm.subType = EventMessages::ConversationMessage::ST_PlayAni;
    sm.animation = npcAni;

    // This is save, because vobs generally won't be deleted from the map.
    // If not, this will just do nothing
    sm.addDoneCallback(m_Entity, [=](Handle::EntityHandle hostVob, std::shared_ptr<EventMessages::EventMessage>) {

        // Re-get everything to make sure it is still there
        VobTypes::NpcVobInformation nv = VobTypes::asNpcVob(m_World, npc);
        VobTypes::MobVobInformation mob = VobTypes::asMobVob(m_World, hostVob);

        if (!nv.isValid() || !mob.isValid())
            return;  // Either mob or NPC isn't there anymore

        // First animation is done, thus, go to the next state
        if (to != -1)
            mob.mobController->m_MobCore->onEndStateChange(npc, from, to);
        else
            stopInteraction(npc);
    });

    nv.playerController->getEM().onMessage(sm);

    // Play the mobs animation
    if (getModelVisual())
        getModelVisual()->setAnimation(mobAni, false);
}

void MobController::stopInteraction(Handle::EntityHandle npc)
{
    ModelVisual* model = getModelVisual();

    if (!model)
        return;

    if (!isInteractingWith(npc))
        return;

    // Free position used by the npc
    for (InteractPosition& p : m_InteractPositions)
    {
        if (p.usedByNpc == npc)
        {
            p.usedByNpc.invalidate();
            break;
        }
    }

    // Play starting animation
    VobTypes::NpcVobInformation nv = VobTypes::asNpcVob(m_World, npc);

    LogInfo() << "MOB (" << m_FocusName << "): Stopping interaction with: " << nv.playerController->getScriptInstance().name[0];

    // This NPC is not using a mob anymore
    nv.playerController->setUsedMob(Handle::EntityHandle::makeInvalidHandle());

    m_NumNpcsCurrent--;

    // Stop all animations to get around a single state-animation getting looped all over again (Chest S0 to S0)
    model->getAnimationHandler().stopAnimation();
}

void MobController::getAnimationTransitionNames(int stateFrom, int stateTo,
                                                std::string& outMobAnimation,
                                                std::string& outNpcAnimation)
{
    if (!m_MobCore)
        return;

    if (stateTo == -1)
    {
        // Go from current state to stand
        outMobAnimation = "";
        outNpcAnimation = "T_" + m_MobCore->getSchemeName() + "_S" + std::to_string(stateFrom) + "_2_STAND";
    }
    else
    {
        // Go from current state to the next one
        outMobAnimation = "T_S" + std::to_string(stateFrom) + "_2_S" + std::to_string(stateTo);
        outNpcAnimation = "T_" + m_MobCore->getSchemeName() + "_S" + std::to_string(stateFrom) + "_2_S" + std::to_string(stateTo);
    }
}

bool MobController::isInteractingWith(Handle::EntityHandle npc)
{
    for (InteractPosition& p : m_InteractPositions)
    {
        if (p.usedByNpc == npc)
            return true;
    }

    return false;
}

void MobController::setIdealPosition(Handle::EntityHandle npc)
{
    if (!m_MobCore)
        return;

    // Find a free position
    InteractPosition* p = findFreePosition(npc);

    if (!p)
        return;

    p->usedByNpc = npc;

    // Notify the core about this position being used now
    m_MobCore->onFreePositionFound(npc, p);

    VobTypes::NpcVobInformation nv = VobTypes::asNpcVob(m_World, npc);

    if (p->distance)
    {
        // Just look at the mob
        nv.playerController->setDirection((getEntityTransform().Translation() - nv.position->m_WorldMatrix.Translation()).normalize());
    }
    else
    {
        nv.playerController->teleportToPosition(getEntityTransform() * p->transform.Translation());
        nv.playerController->setDirection(getEntityTransform() * (-1.0f * p->transform.Forward()) - getEntityTransform().Translation());

        // Just look at the mob for now //TODO: Implement the transform-stuff
        //nv.playerController->setDirection((getEntityTransform().Translation()
        //                                   - nv.position->m_WorldMatrix.Translation()).normalize());
    }
}

void MobController::useMobIncState(Handle::EntityHandle npc, MobController::EDirection direction)
{
    if (!m_MobCore)
        return;

    switch (direction)
    {
        case D_Forward:
            useMobToState(npc, m_MobCore->getState() + 1);
            break;
        case D_Backward:
            useMobToState(npc, m_MobCore->getState() - 1);
            break;

        case D_Left: /*useMobToState(npc, m_MobCore->getState() + 1); TODO */
            break;
        case D_Right: /*useMobToState(npc, m_MobCore->getState() + 1); TODO */
            break;
    }
}

void MobController::useMobToState(Handle::EntityHandle npc, int target)
{
    if (!m_MobCore)
        return;

    // Check if this is the initial start of the interaction
    if (!isInteractingWith(npc))
    {
        // TODO: Check if the NPC is allowed to use this mob (Only one npc per mob, etc)

        // Begin use if we will either set a new state or the mob is already in a state
        // other than the default one, which will set it back.
        if (target > 0 || (target == 0 && m_MobCore->getState() > 0))
        {
            EventMessages::MobMessage msg;
            msg.subType = EventMessages::MobMessage::ST_STARTINTERACTION;
            msg.stateTo = target;
            msg.npc = npc;

            getEM().onMessage(msg, npc);

            return;
        }
    }

    //if(target < 0 && getEM().isEmpty())
    {
        EventMessages::MobMessage msg;
        msg.subType = EventMessages::MobMessage::ST_STARTSTATECHANGE;
        msg.stateTo = target;
        msg.stateFrom = -1;
        msg.npc = npc;

        getEM().onMessage(msg, npc);

        return;
    }

    // FIXME: Already interacting with a mob!
    LogInfo() << "FIXME: Already interacting with a mob!";
}

void MobController::onVisualChanged()
{
    Controller::onVisualChanged();

    /****
    * Initialize animations
    ****/
    Vob::VobInformation v = Vob::asVob(m_World, m_Entity);

    if (!v.visual || v.visual->getVisualType() != EVisualType::Model)
        return;

    // Add animation-component, if not already done
    Components::AnimationComponent& anim = Components::Actions::initComponent<Components::AnimationComponent>(
        m_World.getComponentAllocator(), m_Entity);

    // Strip extension
    std::string libName = v.visual->getName().substr(0, v.visual->getName().find_last_of('.'));

    anim.getAnimHandler().setWorld(m_World);
    anim.getAnimHandler().loadMeshLibFromVDF(libName, m_World.getEngine()->getVDFSIndex());

    // find new interact positions now that the visual changed
    findInteractPositions();

    // Get new animation scheme. This is done by taking everything in front of the first _ inside the visual name.
    std::string scheme = v.visual->getName().substr(0, v.visual->getName().find_first_of('_'));
    if (m_MobCore)
        m_MobCore->setSchemeName(scheme);
}

void MobController::exportPart(json& j)
{
    Controller::exportPart(j);

    j["type"] = "MobController";
    j["focusName"] = m_FocusName;
    j["objectClass"] = m_zObjectClass;
    m_MobCore->exportCore(j["core"]);
}

void MobController::importObject(const json& j)
{
    Controller::importObject(j);

    initFromJSONDescriptor(j);
}
