#include <components/Vob.h>
#include <components/VobClasses.h>
#include <ZenLib/zenload/zTypes.h>
#include <ZenLib/utils/logger.h>
#include <logic/mobs/Bed.h>
#include "MobController.h"
#include "visuals/ModelVisual.h"
#include "PlayerController.h"

using namespace Logic;

MobController* MobCore::getMobController()
{
    return (MobController*)m_World.getEntity<Components::LogicComponent>(m_Entity).m_pLogicController;
}

MobController::MobController(World::WorldInstance& world, Handle::EntityHandle entity, const ZenLoad::zCVobData& vob) :
        Controller(world, entity)
{
    m_NumNpcsMax = 0;
    m_NumNpcsCurrent = 0;
    m_MobCore = nullptr;

    m_World.getScriptEngine().registerMob(m_Entity);

    initFromVobDescriptor(vob);
}

MobController::~MobController()
{
    m_World.getScriptEngine().unregisterMob(m_Entity);

    delete m_MobCore;
}

ModelVisual* MobController::getModelVisual()
{
    Vob::VobInformation vob = Vob::asVob(m_World, m_Entity);

    // TODO: Bring in some type-checking here
    return reinterpret_cast<ModelVisual*>(vob.visual);
}

void MobController::onUpdate(float deltaTime)
{
    Controller::onUpdate(deltaTime);
}

void MobController::findInteractPositions()
{
    ModelVisual* model = getModelVisual();

    assert(model);

    const std::vector<ZenLoad::ModelNode>& nodes = model->getMeshLib().getNodes();

    for(size_t i=0;nodes.size();i++)
    {
        const ZenLoad::ModelNode& n = nodes[i];

        // Check if this is a position-node
        if(n.name.find("ZS_POS") != std::string::npos)
        {
            m_InteractPositions.emplace_back();

            // Get transformed position in object-space
            m_InteractPositions.back().transform = model->getAnimationHandler().getObjectSpaceTransforms()[i];
            m_InteractPositions.back().distance = n.name.find("DIST") != std::string::npos;
            m_InteractPositions.back().nodeName = n.name;

            // Check if this can be used by multiple npcs
            if(n.name.find("NPC") != std::string::npos)
                m_NumNpcsMax++;
        }
    }

    if(!m_NumNpcsMax)
        m_NumNpcsMax = 1;
}

InteractPosition* MobController::findFreePosition(Handle::EntityHandle npc, float maxDistance)
{
    if(m_NumNpcsCurrent >= m_NumNpcsMax)
        return nullptr; // Mob is full

    // Square for faster distance computation
    float maxDistance2 = maxDistance * maxDistance;

    // Get npcs position
    Math::float3 npcPosition = m_World.getEntity<Components::PositionComponent>(npc).m_WorldMatrix.Translation();
    VobTypes::NpcVobInformation npcVob = VobTypes::asNpcVob(m_World, npc);


    bool wrongSide = false;
    float minDist = FLT_MAX;
    bool playerTooFar = false;
    InteractPosition* found = nullptr;
    for(InteractPosition& p : m_InteractPositions)
    {
        if(p.usedByNpc == npc)
            return &p; // This npc already using the mob

        if(p.usedByNpc.isValid())
            return nullptr; // Other npc using this

        Math::float3 worldPosition = p.transform.Translation() + getEntityTransform().Translation();

        // Get distance from npc to node
        float dist2 = (worldPosition - npcPosition).lengthSquared();

        if(dist2 < minDist)
        {
            // Check if we can see this position if not only distance matters
            if(dist2 < maxDistance2 && !p.distance)
            {
                if(!npcVob.playerController->freeLineOfSight(worldPosition))
                {
                    wrongSide = true;
                    continue;
                }
            }

            // If this is a player-charakter, also check the max allowed distance, since he should not run all the way there
            // by himself, but let the actual player do that
            if(npcVob.playerController->isPlayerControlled())
            {
                if(!p.distance && dist2 > maxDistance2)
                {
                    playerTooFar = true;
                    continue;
                }
            }

            found = &p;
            minDist = dist2;
        }
    }

    if(!found && npcVob.playerController->isPlayerControlled())
    {
        // Notfiy the player about the mob being too far or standing on the wrong side
        if(playerTooFar)
        {
            EventMessages::ManipulateMessage msg;
            msg.subType = EventMessages::ManipulateMessage::ST_CallScript;
            msg.symIdx = m_World.getScriptEngine().getSymbolIndexByName(PlayerScriptInfo::PLAYER_MOB_TOO_FAR_AWAY);

            npcVob.playerController->getEM().onMessage(msg, m_Entity);
        }else if(wrongSide)
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
    LogInfo() << "Creating mob: " << vob.oCMOB.focusName;

    if(vob.oCMOB.focusName == "Bed")
        m_MobCore = new MobCores::Bed(m_World, m_Entity);

    Vob::VobInformation v = Vob::asVob(m_World, m_Entity);
    Vob::setVisual(v, vob.visual);

    if(getModelVisual())
        findInteractPositions();
}

void MobController::onMessage(EventMessages::EventMessage& message, Handle::EntityHandle sourceVob)
{
    Controller::onMessage(message, sourceVob);
    assert(message.messageType == EventMessages::EventMessageType::Mob);

    EventMessages::MobMessage& msg = (EventMessages::MobMessage&)message;
    switch((EventMessages::MobMessage::MobSubType)msg.subType)
    {
        case EventMessages::MobMessage::ST_STARTINTERACTION:break;
        case EventMessages::MobMessage::ST_STARTSTATECHANGE:break;
        case EventMessages::MobMessage::ST_ENDINTERACTION:break;
        case EventMessages::MobMessage::ST_UNLOCK:break;
        case EventMessages::MobMessage::ST_LOCK:break;
        case EventMessages::MobMessage::ST_CALLSCRIPT: callOnStateFunc(msg.npc, msg.stateTo); break;
    }
}

void MobController::callOnStateFunc(Handle::EntityHandle npc, int state)
{
    ScriptEngine& s = m_World.getScriptEngine();

    std::string symName = m_OnStateFuncName + "_S" + std::to_string(state);
    size_t idx = s.getSymbolIndexByName(symName);

    if(idx != static_cast<size_t>(-1))
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
    if(!m_OnStateFuncName.empty())
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
    if(isInteractingWith(npc))
        return; // Don't allow interacting twice on the same npc

    // Move NPC to it's position
    setIdealPosition(npc);

    // Play starting animation
    VobTypes::NpcVobInformation nv = VobTypes::asNpcVob(m_World, npc);
    nv.playerController->getModelVisual()->setAnimation("T_" + m_MobCore->getSchemeName() + "_STAND_2_S" + std::to_string(m_MobCore->getState()));

    m_NumNpcsCurrent++;
}

void MobController::startStateChange(Handle::EntityHandle npc, int from, int to)
{

}

void MobController::stopInteraction(Handle::EntityHandle npc)
{
    if (!isInteractingWith(npc))
        return;

    // Free position used by the npc
    for(InteractPosition& p : m_InteractPositions)
    {
        if(p.usedByNpc == npc)
        {
            p.usedByNpc.invalidate();
            break;
        }
    }

    m_NumNpcsCurrent--;
}

void MobController::getAnimationTransitionNames(int stateFrom, int stateTo,
                                                std::string& outMobAnimation,
                                                std::string& outNpcAnimation)
{
    if(!m_MobCore)
        return;

    if(stateTo == -1)
    {
        // Go from current state to stand
        outMobAnimation = "";
        outNpcAnimation = "T_" + m_MobCore->getSchemeName() + "_S" + std::to_string(stateFrom) + "_2_STAND";
    }else
    {
        // Go from current state to the next one
        outMobAnimation	 = "T_S" + std::to_string(stateFrom)  + "_2_S" + std::to_string(stateTo);
        outNpcAnimation	 = "T_"  + m_MobCore->getSchemeName() + "_S" + std::to_string(stateFrom) + "_2_S" + std::to_string(stateTo);
    }
}

bool MobController::isInteractingWith(Handle::EntityHandle npc)
{
    for(InteractPosition& p : m_InteractPositions)
    {
        if(p.usedByNpc == npc)
            return true;
    }

    return false;
}

void MobController::setIdealPosition(Handle::EntityHandle npc)
{
    if(!m_MobCore)
        return;

    // Find a free position
    InteractPosition* p = findFreePosition(npc);

    if(!p)
        return;

    // Notify the core about this position being used now
    m_MobCore->onFreePositionFound(npc, p);

    VobTypes::NpcVobInformation nv = VobTypes::asNpcVob(m_World, npc);

    if(p->distance)
    {
        // Just look at the mob
        nv.playerController->setDirection((getEntityTransform().Translation()
                                           - nv.position->m_WorldMatrix.Translation()).normalize());
    }else
    {
        nv.playerController->teleportToPosition(p->transform.Translation());

        // Just look at the mob for now //TODO: Implement the transform-stuff
        nv.playerController->setDirection((getEntityTransform().Translation()
                                           - nv.position->m_WorldMatrix.Translation()).normalize());
    }
}

void MobController::useMobToState(Handle::EntityHandle npc, int target)
{
    if(!m_MobCore)
        return;

    // Check if this is the initial start of the interaction
    if(!isInteractingWith(npc))
    {
        // TODO: Check if the NPC is allowed to use this mob (Only one npc per mob, etc)

        // Begin use if we will either set a new state or the mob is already in a state
        // other than the default one, which will set it back.
        if(target > 0 || (target == 0 && m_MobCore->getState() > 0))
        {
            EventMessages::MobMessage msg;
            msg.subType = EventMessages::MobMessage::ST_STARTINTERACTION;
            msg.stateTo = target;
            msg.npc = npc;

            getEM().onMessage(msg, npc);

            return;
        }
    }

    // FIXME: Already interacting with a mob!
    LogInfo() << "FIXME: Already interacting with a mob!";

}
