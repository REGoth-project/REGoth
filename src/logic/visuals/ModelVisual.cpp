#include "ModelVisual.h"
#include "StaticMeshVisual.h"
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
        : VisualController(world, entity),
          m_LastKnownAnimationState(static_cast<size_t>(-1))
{

}

ModelVisual::~ModelVisual()
{

}

bool ModelVisual::load(const std::string& visual)
{
    VisualController::load(visual);

    m_BodyState.bodyVisual = visual;

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

    for(Handle::EntityHandle e : m_PartEntities.mainSkelMeshEntities)
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
    if(m_VisualAttachments.size() < getAnimationHandler().getObjectSpaceTransforms().size())
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

			if(!amh.isValid())
				continue;

            Meshes::WorldStaticMesh amdata = m_World.getStaticMeshAllocator().getMesh(amh);

            // Add the entities to our lists
            std::vector<Handle::EntityHandle> tmp = Content::entitifyMesh(m_World, amh, amdata);
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

    // Create m_VisualEntites-list
    rebuildMainEntityList();

	// Make sure we got the right spots for our attachments
	updateAttachmentVisuals();

    return true;
}

void ModelVisual::setHeadMesh(const std::string& head, size_t headTextureIdx, size_t teethTextureIdx)
{
    std::string v = head;

	if(v.empty())
		return;

    if(v.find(".") == std::string::npos)
        v += ".MMB";

    m_BodyState.headVisual = v;
    m_BodyState.headTextureIdx = headTextureIdx;
    m_BodyState.teethTextureIdx = teethTextureIdx;

    setBodyState(m_BodyState);
}

void ModelVisual::rebuildMainEntityList()
{
    m_VisualEntities.clear();

    m_VisualEntities.insert(m_VisualEntities.end(), m_PartEntities.mainSkelMeshEntities.begin(), m_PartEntities.mainSkelMeshEntities.end());
    m_VisualEntities.insert(m_VisualEntities.end(), m_PartEntities.headMeshEntities.begin(), m_PartEntities.headMeshEntities.end());
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

Handle::EntityHandle ModelVisual::setNodeVisual(const std::string &visual, const std::string& nodeName)
{
	// Register first
	m_AttachmentVisualsByNode[nodeName] = visual;

	size_t nodeIndex = findNodeIndex(nodeName);
    if(nodeIndex > getMeshLib().getNodes().size())
        return Handle::EntityHandle::makeInvalidHandle(); // Invalid node?

    // TODO: Allow to overwrite attachments!
    if(!m_VisualAttachments[nodeIndex].empty())
        return Handle::EntityHandle::makeInvalidHandle();

    if(visual.empty())
    {
        if(m_VisualAttachments[nodeIndex].empty())
            return Handle::EntityHandle::makeInvalidHandle(); // Nothing to do here

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

        return Handle::EntityHandle::makeInvalidHandle();
    }else
    {
        // See these attachments as vob, for simplicities sake
        Handle::EntityHandle avh = Vob::constructVob(m_World);

        Vob::VobInformation vob = Vob::asVob(m_World, avh);
        Vob::setVisual(vob, visual);

        // Clear old visual, if there was one
        setNodeVisual("", nodeName);

        // Only add the main vob, all submeshes etc. will be updated using onTransformChanged()
        m_VisualAttachments[nodeIndex].push_back(avh);
        m_PartEntities.dynamicAttachments.push_back(avh);

        rebuildMainEntityList();
        updateAttachmentTransforms();

        return avh;
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

Handle::EntityHandle ModelVisual::setNodeVisual(const std::string &visual, EModelNode node)
{
    switch(node)
    {
        case EModelNode::Righthand:return setNodeVisual(visual, NPC_NODE_RIGHTHAND);break;
        case EModelNode::Lefthand:return setNodeVisual(visual, NPC_NODE_LEFTHAND);break;
        case EModelNode::Sword:return setNodeVisual(visual, NPC_NODE_SWORD);break;
        case EModelNode::Longsword:return setNodeVisual(visual, NPC_NODE_LONGSWORD);break;
        case EModelNode::Bow:return setNodeVisual(visual, NPC_NODE_BOW);break;
        case EModelNode::Crossbow:return setNodeVisual(visual, NPC_NODE_CROSSBOW);break;
        case EModelNode::Shield:return setNodeVisual(visual, NPC_NODE_SHIELD);break;
        case EModelNode::Helmet:return setNodeVisual(visual, NPC_NODE_HELMET);break;
        case EModelNode::Jaws:return setNodeVisual(visual, NPC_NODE_JAWS);break;

            // TODO: Switch main mesh here
        case EModelNode::Torso:return setNodeVisual(visual, NPC_NODE_TORSO);break;
    }

	return Handle::EntityHandle::makeInvalidHandle();
}

void ModelVisual::getCollisionBBox(Math::float3* bb)
{
    bb[0] = Math::float3(getMeshLib().getBBoxMin().v);
    bb[1] = Math::float3(getMeshLib().getBBoxMax().v);

    // These can be in various places...
    if(bb[0].lengthSquared() == 0.0f && bb[1].lengthSquared() == 0.0f)
    {
        bb[0] = Math::float3(getMeshLib().getBBoxCollisionMin().v);
        bb[1] = Math::float3(getMeshLib().getBBoxCollisionMax().v);

        if(bb[0].lengthSquared() == 0.0f && bb[1].lengthSquared() == 0.0f)
        {
            const ZenLoad::zCModelMeshLib& zLib = m_World.getSkeletalMeshAllocator().getMeshLib(m_MainMeshHandle);
            bb[0] = Math::float3(zLib.getBBoxMin().v);
            bb[1] = Math::float3(zLib.getBBoxMax().v);
        }
    }
}

Math::float3 ModelVisual::getModelRoot()
{
    return Math::float3(getMeshLib().getRootNodeTranslation().v);
}

void ModelVisual::onFrameUpdate(float dt)
{
    size_t newHash = getAnimationHandler().getAnimationStateHash();
    if(newHash != m_LastKnownAnimationState)
    {
        updateAttachmentTransforms();
        m_LastKnownAnimationState = newHash;
    }
}

void ModelVisual::setBodyState(const ModelVisual::BodyState &state)
{
    m_BodyState = state;

    updateBodyMesh();
    updateHeadMesh();
}

void ModelVisual::updateBodyMesh()
{
    // Remove all created entities from the world
    for(Handle::EntityHandle e : m_PartEntities.mainSkelMeshEntities)
        m_World.removeEntity(e);

    m_PartEntities.mainSkelMeshEntities.clear();

    load(m_BodyState.bodyVisual);

    // Fix body-texture and skin-color
    if(!m_PartEntities.mainSkelMeshEntities.empty())
    {
        Handle::EntityHandle bodyMain = m_PartEntities.mainSkelMeshEntities[0];

        Components::EntityComponent& ent = m_World.getEntity<Components::EntityComponent>(bodyMain);
        if(Components::hasComponent<Components::StaticMeshComponent>(ent))
        {
            Components::StaticMeshComponent& sm = m_World.getEntity<Components::StaticMeshComponent>(bodyMain);

            if(m_BodyState.bodySkinColorIdx > 0 || m_BodyState.bodyTextureIdx > 0)
            {
                // Get texture parts
                std::vector<std::string> bodyTxParts = Utils::split(m_World.getTextureAllocator().getTexture(sm.m_Texture).m_TextureName, "0123456789");

                if (bodyTxParts.size() == 3) // [HUM_HEAD_...V, _C, .TGA]
                {
                    std::string newBodyTx = bodyTxParts[0]
                                            + std::to_string(m_BodyState.bodyTextureIdx)
                                            + bodyTxParts[1]
                                            + std::to_string(m_BodyState.bodySkinColorIdx)
                                            + bodyTxParts[2];

                    sm.m_Texture = m_World.getTextureAllocator().loadTextureVDF(newBodyTx);
                }
            }


        }
    }
}

void ModelVisual::updateHeadMesh()
{
    for(Handle::EntityHandle e : m_PartEntities.headMeshEntities)
        m_World.removeEntity(e);

    m_PartEntities.headMeshEntities.clear();
    m_PartEntities.headEntity.invalidate();

    // Load and attach the visual
    Handle::EntityHandle headEntity = setNodeVisual(m_BodyState.headVisual, MODEL_NODE_NAME_HEAD);

    if(!headEntity.isValid())
        return;

    m_PartEntities.headEntity = headEntity;

    // Fix head texture and color
    Vob::VobInformation head = Vob::asVob(m_World, m_PartEntities.headEntity);
    if(Vob::getVisual(head))
    {
        // TODO: Type safety!
        StaticMeshVisual* visual = reinterpret_cast<StaticMeshVisual*>(Vob::getVisual(head));

        if(m_BodyState.headTextureIdx > 0 || m_BodyState.bodySkinColorIdx > 0)
        {
            // Get head-texture parts
            std::vector<std::string> headTxParts = Utils::split(visual->getDiffuseTexture(0), "0123456789");

            if (headTxParts.size() == 3) // [HUM_HEAD_...V, _C, .TGA]
            {
                std::string newHeadTx = headTxParts[0]
                                        + std::to_string(m_BodyState.headTextureIdx)
                                        + headTxParts[1]
                                        + std::to_string(m_BodyState.bodySkinColorIdx)
                                        + headTxParts[2];

                visual->setDiffuseTexture(0, newHeadTx);
            }
        }
        // Submesh 0: Head
        // Submesh 1: Mouthhole
        // Submesh 2: Teeth
        //if(headTextureIdx > 0)
        //    visual->setAnimationFrame(0, headTextureIdx);

        //if(teethTextureIdx > 0)
        //    visual->setAnimationFrame(2, teethTextureIdx);
    }
}

void ModelVisual::updateAttachmentVisuals()
{
	for(auto& p : m_AttachmentVisualsByNode)
	{
		size_t nodeIdx = findNodeIndex(p.first);
		if(nodeIdx != static_cast<size_t>(-1))
		{
			bool update = false;

			// Update if there isn't something attached yet
			if(m_VisualAttachments[nodeIdx].empty())
			{
				update = true;
			}
			else // Also update on change
			{
				Handle::EntityHandle e = m_VisualAttachments[nodeIdx][0];
				Vob::VobInformation vob = Vob::asVob(m_World, e);
				VisualController* vis = Vob::getVisual(vob);
				if(vis && vis->getName() != p.second)
					update = true;
			}

			// Set the new visual
			setNodeVisual(p.second, p.first);
		}
	}
}