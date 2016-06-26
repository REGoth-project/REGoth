#include "ModelVisual.h"
#include <engine/World.h>
#include <content/ContentLoad.h>
#include <zenload/zCModelMeshLib.h>
#include <utils/logger.h>
#include <content/SkeletalMeshAllocator.h>
#include <components/Vob.h>

using namespace Logic;

const std::string MODEL_NODE_NAME_R_HAND	= "BIP01 R HAND";
const std::string MODEL_NODE_NAME_L_HAND	= "BIP01 L HAND";
const std::string MODEL_NODE_NAME_R_FOOT	= "BIP01 R FOOT";
const std::string MODEL_NODE_NAME_L_FOOT	= "BIP01 L FOOT";
const std::string MODEL_NODE_NAME_HEAD		= "BIP01 HEAD";
const std::string MODEL_NODE_NAME_FOOTSTEPS	= " FOOTSTEPS";

const std::string NPC_NODE_RIGHTHAND	= "ZS_RIGHTHAND";
const std::string NPC_NODE_LEFTHAND		= "ZS_LEFTHAND";
const std::string NPC_NODE_SWORD		= "ZS_SWORD";
const std::string NPC_NODE_LONGSWORD	= "ZS_LONGSWORD";
const std::string NPC_NODE_BOW			= "ZS_BOW";
const std::string NPC_NODE_CROSSBOW		= "ZS_CROSSBOW";
const std::string NPC_NODE_SHIELD		= "ZS_SHIELD";
const std::string NPC_NODE_HELMET		= "ZS_HELMET";
const std::string NPC_NODE_JAWS			= "ZS_JAWS";
const std::string NPC_NODE_TORSO		= "ZS_TORSO";

ModelVisual::ModelVisual(World::WorldInstance& world, Handle::EntityHandle entity)
        : VisualController(world, entity)
{

}

ModelVisual::~ModelVisual()
{

}

bool ModelVisual::load(const std::string& visual)
{
    VisualController::load(visual);

    assert(visual.find(".MDM") != std::string::npos
           || visual.find(".MDL") != std::string::npos
           || visual.find(".ASC") != std::string::npos
           || visual.find(".MDS") != std::string::npos);

    m_MainMeshHandle = m_World.getSkeletalMeshAllocator().loadMeshVDF(visual);

    if(!m_MainMeshHandle.isValid())
        return false;

    Meshes::WorldSkeletalMesh& mdata = m_World.getSkeletalMeshAllocator().getMesh(m_MainMeshHandle);
    const ZenLoad::zCModelMeshLib& zLib = m_World.getSkeletalMeshAllocator().getMeshLib(m_MainMeshHandle);

    // TODO: Put these into a compound-component or something
    m_PartEntities.mainSkelMeshEntities = Content::entitifyMesh(m_World, m_MainMeshHandle, mdata);
    m_VisualEntities.insert(m_VisualEntities.end(),
                            m_PartEntities.mainSkelMeshEntities.begin(), m_PartEntities.mainSkelMeshEntities.end());

    for(Handle::EntityHandle e : m_VisualEntities)
    {
        // Init positions
        Components::EntityComponent& entity = m_World.getEntity<Components::EntityComponent>(e);
        Components::addComponent<Components::PositionComponent>(entity);

        // Copy world-matrix
        Components::PositionComponent& pos = m_World.getEntity<Components::PositionComponent>(e);
        pos.m_WorldMatrix = getEntityTransform();

        // Init animation components
        Components::addComponent<Components::AnimationComponent>(entity);
        Components::AnimationComponent& anim = m_World.getEntity<Components::AnimationComponent>(e);

        // Assign the main-vob as animation controller
        anim.m_ParentAnimHandler = m_Entity;
    }

    // Read attachments
    m_VisualAttachments.resize(getAnimationHandler().getObjectSpaceTransforms().size());

    if(m_VisualAttachments.empty())
        m_VisualAttachments.resize(zLib.getNodes().size());

    if(!m_VisualAttachments.empty())
    {
        // Need to get the bind-pose out
        if(zLib.getNodes().empty()) // No nodes here mean, that this is only a mesh. NPC for example.
            getAnimationHandler().setBindPose(true);

        for (size_t i = 0; i < zLib.getAttachments().size(); i++)
        {
            auto &m = zLib.getAttachments()[i];
            size_t nodeIdx = 0;

            if(zLib.getNodes().empty())
                nodeIdx = getAnimationHandler().getMeshLib().findNodeIndex(m.first);
            else
                nodeIdx = zLib.findNodeIndex(m.first);

            // Failed to find the node?
            if(nodeIdx == static_cast<size_t>(-1))
                continue;

            // Load the attached mesh
            ZenLoad::PackedMesh packed;
            m.second.packMesh(packed, 1.0f / 100.0f);
            Handle::MeshHandle amh = m_World.getStaticMeshAllocator().loadFromPacked(packed);
            Meshes::WorldStaticMesh amdata = m_World.getStaticMeshAllocator().getMesh(amh);

            // Add the entities to our lists
            std::vector<Handle::EntityHandle> tmp = Content::entitifyMesh(m_World, amh, amdata);
            m_VisualEntities.insert(m_VisualEntities.end(), tmp.begin(), tmp.end());
            m_PartEntities.dynamicAttachments.insert(m_PartEntities.dynamicAttachments.end(),
                                                             tmp.begin(), tmp.end());

            m_VisualAttachments[nodeIdx].insert(m_VisualAttachments[nodeIdx].end(), tmp.begin(), tmp.end());

            // Initialize the created entities
            for (Handle::EntityHandle e : tmp)
            {
                // Init positions
                Components::EntityComponent &entity = m_World.getEntity<Components::EntityComponent>(e);
                Components::addComponent<Components::PositionComponent>(entity);
            }
        }

        // Calculate bind-pose matrices
        if(zLib.getNodes().empty())
            m_BindPoseTransforms = getAnimationHandler().getObjectSpaceTransforms();
        else
        {
            m_BindPoseTransforms.resize(zLib.getNodes().size());

            std::vector<Math::Matrix> nodeTransforms(zLib.getNodes().size());
            for (size_t i = 0; i < zLib.getNodes().size(); i++)
                nodeTransforms[i] = Math::Matrix(zLib.getNodes()[i].transformLocal.mv);

            for (size_t i = 0; i < zLib.getNodes().size(); i++)
            {
                // TODO: There is a flag indicating whether the animation root should translate the vob position
                if (i == 0)
                    nodeTransforms[i].Translation(Math::float3(0.0f, 0.0f, 0.0f));

                if (zLib.getNodes()[i].parentValid())
                    m_BindPoseTransforms[i] =
                            m_BindPoseTransforms[zLib.getNodes()[i].parentIndex] * nodeTransforms[i];
                else
                    m_BindPoseTransforms[i] = nodeTransforms[i];
            }
        }

        // Make sure the attachments go to the right place
        updateAttachmentTransforms(m_BindPoseTransforms);
    }

    Handle::EntityHandle e = m_World.addEntity(Components::BBoxComponent::MASK);
    m_World.getEntity<Components::BBoxComponent>(e).m_DebugColor = 0xFFFFFFFF;

    return true;
}

void ModelVisual::setHeadMesh(const std::string& head)
{
    std::string v = head;

    if(v.find(".") == std::string::npos)
        v += ".MMB";

    m_BodyState.headVisual = v;

    setNodeVisual(v, findNodeIndex(MODEL_NODE_NAME_HEAD));
}

void ModelVisual::rebuildMainEntityList()
{
    m_VisualEntities.clear();

    m_VisualEntities.insert(m_VisualEntities.end(), m_PartEntities.mainSkelMeshEntities.begin(), m_PartEntities.mainSkelMeshEntities.end());
    m_VisualEntities.insert(m_VisualEntities.end(), m_PartEntities.headMeshEntities.begin(), m_PartEntities.headMeshEntities.end());
    m_VisualEntities.insert(m_VisualEntities.end(), m_PartEntities.armorEntities.begin(), m_PartEntities.armorEntities.end());
    m_VisualEntities.insert(m_VisualEntities.end(), m_PartEntities.dynamicAttachments.begin(), m_PartEntities.dynamicAttachments.end());
}

void ModelVisual::setAnimation(const std::string& anim)
{
    Components::AnimHandler& animHandler = m_World.getEntity<Components::AnimationComponent>(m_Entity).m_AnimHandler;

    if(!anim.empty())
        animHandler.setAnimation(anim);
    else
        animHandler.stopAnimation();
}


void ModelVisual::setAnimation(ModelVisual::EModelAnimType type)
{
    switch(type)
    {
        case Walk:
            if(getAnimationHandler().hasAnimation("S_WALKL"))
            {
                setAnimation("S_WALKL");
                break;
            }
            // Walk not found, try fistwalk
        case FistWalk:
            if(getAnimationHandler().hasAnimation("S_FISTWALKL"))
                setAnimation("S_FISTWALKL");
            break;

        case Run:
            if(getAnimationHandler().hasAnimation("S_RUNL"))
            {
                setAnimation("S_RUNL");
                break;
            }
            // Walk not found, try fistrun
        case FistRun:
            if(getAnimationHandler().hasAnimation("S_FISTRUNL"))
                setAnimation("S_FISTRUNL");
            break;

        case Idle:
            if(getAnimationHandler().hasAnimation("S_RUN"))
                setAnimation("S_RUN");
            else if(getAnimationHandler().hasAnimation("S_WALK"))
                setAnimation("S_WALK");
            else if(getAnimationHandler().hasAnimation("S_FISTRUN"))
                setAnimation("S_FISTRUN");
            else if(getAnimationHandler().hasAnimation("S_FISTWALK"))
                setAnimation("S_FISTWALK");
            break;
    }
}

void ModelVisual::updateAttachmentTransforms(const std::vector<Math::Matrix>& transforms)
{
    if(m_VisualAttachments.empty())
        return;

    Math::Matrix t = getEntityTransform();




    for(size_t i=0;i<m_VisualAttachments.size();i++)
    {
        auto& a = m_VisualAttachments[i];

        // Initialize the created entities
        for (Handle::EntityHandle e : a)
        {
            // Set world-matrix
            Components::PositionComponent &pos = m_World.getEntity<Components::PositionComponent>(e);

            if(transforms.empty())
                pos.m_WorldMatrix = t;
            else
                pos.m_WorldMatrix = t * transforms[i];

            // If this is a vob, notify everything about the changed visual
            Vob::VobInformation vob = Vob::asVob(m_World, e);
            if(vob.visual)
                vob.visual->onTransformChanged();
        }
    }
}

void ModelVisual::updateAttachmentTransforms()
{
    Components::EntityComponent &ent = m_World.getEntity<Components::EntityComponent>(m_Entity);

    if(Components::hasComponent<Components::AnimationComponent>(ent))
        updateAttachmentTransforms(getAnimationHandler().getObjectSpaceTransforms());
    else
        updateAttachmentTransforms(m_BindPoseTransforms);
}

Components::AnimHandler &ModelVisual::getAnimationHandler()
{
    Components::AnimationComponent &anim = m_World.getEntity<Components::AnimationComponent>(m_Entity);
    return anim.m_AnimHandler;
}

void ModelVisual::onTransformChanged()
{
    VisualController::onTransformChanged();

    // Update attachments with the last state of the animation-handler
    updateAttachmentTransforms();
}

void ModelVisual::setNodeVisual(const std::string &visual, size_t nodeIndex)
{
    if(nodeIndex > getMeshLib().getNodes().size())
        return; // Invalid node?

    // TODO: Allow to overwrite attachments!
    if(!m_VisualAttachments[nodeIndex].empty())
        return;

    if(visual.empty())
    {
        if(m_VisualAttachments[nodeIndex].empty())
            return; // Nothing to do here

        // Clear whatever there was here
        for(Handle::EntityHandle e : m_VisualAttachments[nodeIndex])
        {
            m_World.removeEntity(e);

            // Remove from attachment list
            m_PartEntities.dynamicAttachments.erase(
                    std::remove(m_PartEntities.dynamicAttachments.begin(),
                                m_PartEntities.dynamicAttachments.end(),
                                e),
                    m_PartEntities.dynamicAttachments.end());
        }

        m_VisualAttachments[nodeIndex].clear();



        rebuildMainEntityList();
    }else
    {
        // See these attachments as vob, for simplicities sake
        Handle::EntityHandle avh = Vob::constructVob(m_World);

        Vob::VobInformation vob = Vob::asVob(m_World, avh);
        Vob::setVisual(vob, visual);

        // Clear old visual, if there was one
        setNodeVisual("", nodeIndex);

        // Only add the main vob, all submeshes etc. will be updated using onTransformChanged()
        m_VisualAttachments[nodeIndex].push_back(avh);
        m_PartEntities.dynamicAttachments.push_back(avh);

        rebuildMainEntityList();
    }
}

size_t ModelVisual::findNodeIndex(const std::string &name)
{
    return getMeshLib().findNodeIndex(name);
}

const ZenLoad::zCModelMeshLib &ModelVisual::getMeshLib()
{
    const ZenLoad::zCModelMeshLib& zLib = m_World.getSkeletalMeshAllocator().getMeshLib(m_MainMeshHandle);

    if(zLib.getNodes().empty())
        return getAnimationHandler().getMeshLib();
    else
        return zLib;
}

void ModelVisual::setNodeVisual(const std::string &visual, EModelNode node)
{
    switch(node)
    {
        case EModelNode::Righthand:setNodeVisual(visual, findNodeIndex(NPC_NODE_RIGHTHAND));break;
        case EModelNode::Lefthand:setNodeVisual(visual, findNodeIndex(NPC_NODE_LEFTHAND));break;
        case EModelNode::Sword:setNodeVisual(visual, findNodeIndex(NPC_NODE_SWORD));break;
        case EModelNode::Longsword:setNodeVisual(visual, findNodeIndex(NPC_NODE_LONGSWORD));break;
        case EModelNode::Bow:setNodeVisual(visual, findNodeIndex(NPC_NODE_BOW));break;
        case EModelNode::Crossbow:setNodeVisual(visual, findNodeIndex(NPC_NODE_CROSSBOW));break;
        case EModelNode::Shield:setNodeVisual(visual, findNodeIndex(NPC_NODE_SHIELD));break;
        case EModelNode::Helmet:setNodeVisual(visual, findNodeIndex(NPC_NODE_HELMET));break;
        case EModelNode::Jaws:setNodeVisual(visual, findNodeIndex(NPC_NODE_JAWS));break;

            // TODO: Switch main mesh here
        case EModelNode::Torso:setNodeVisual(visual, findNodeIndex(NPC_NODE_TORSO));break;
    }
}

