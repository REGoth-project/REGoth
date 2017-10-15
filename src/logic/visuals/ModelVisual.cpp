#include "ModelVisual.h"
#include "StaticMeshVisual.h"
#include <components/EntityActions.h>
#include <components/Vob.h>
#include <content/ContentLoad.h>
#include <content/SkeletalMeshAllocator.h>
#include <engine/BaseEngine.h>
#include <engine/GameEngine.h>
#include <engine/World.h>
#include <utils/logger.h>
#include <zenload/zCModelMeshLib.h>

using namespace Logic;

const std::string MODEL_NODE_NAME_R_HAND = "BIP01 R HAND";
const std::string MODEL_NODE_NAME_L_HAND = "BIP01 L HAND";
const std::string MODEL_NODE_NAME_R_FOOT = "BIP01 R FOOT";
const std::string MODEL_NODE_NAME_L_FOOT = "BIP01 L FOOT";
const std::string MODEL_NODE_NAME_HEAD = "BIP01 HEAD";
const std::string MODEL_NODE_NAME_FOOTSTEPS = " FOOTSTEPS";

const std::string NPC_NODE_RIGHTHAND = "ZS_RIGHTHAND";
const std::string NPC_NODE_LEFTHAND = "ZS_LEFTHAND";
const std::string NPC_NODE_SWORD = "ZS_SWORD";
const std::string NPC_NODE_LONGSWORD = "ZS_LONGSWORD";
const std::string NPC_NODE_BOW = "ZS_BOW";
const std::string NPC_NODE_CROSSBOW = "ZS_CROSSBOW";
const std::string NPC_NODE_SHIELD = "ZS_SHIELD";
const std::string NPC_NODE_HELMET = "ZS_HELMET";
const std::string NPC_NODE_JAWS = "ZS_JAWS";
const std::string NPC_NODE_TORSO = "ZS_TORSO";

const char* ANIMATION_NAMES[] = {

    // Normal
    "S_WALKL",       // Walk,
    "S_RUNL",        // Run,
    "S_RUN",         // Idle,
    "T_JUMPB",       // Backpedal,
    "T_RUNSTRAFEL",  // StrafeLeft,
    "T_RUNSTRAFER",  // StrafeRight,
    "S_JUMP",        // Jump
    "T_RUNTURNL",    // Turn Left
    "T_RUNTURNR",    // Turn Right

    // 1h
    "S_1HWALKL",        // Walk1h,
    "S_1HRUNL",         // Run1h,
    "S_1HRUN",          // Idle1h,
    "T_1HJUMPB",        // Backpedal1h,
    "T_1HRUNSTRAFEL",   // StrafeLeft1h,
    "T_1HRUNSTRAFER",   // StrafeRight1h,
    "S_1HATTACK",       // Attack1h,
    "T_1HATTACKL",      // Attack1h_L,
    "T_1HATTACKR",      // Attack1h_R,
    "T_1HATTACKMOVE",   // Attack1h_Move,
    "T_1HSFINISH",      // Attack1h_Finish,
    "T_1HPARADE_O",     // Parade1h,
    "T_1HPARADEJUMPB",  // Parade1h_Back,
    "T_1H_2_1HRUN",     // Draw1h,

    // 2h
    "S_2HWALKL",        // Walk2h,
    "S_2HRUNL",         // Run2h,
    "S_2HRUN",          // Idle2h,
    "T_2HJUMPB",        // Backpedal2h,
    "T_2HRUNSTRAFEL",   // StrafeLeft2h,
    "T_2HRUNSTRAFER",   // StrafeRight2h,
    "S_2HATTACK",       // Attack2h,
    "T_2HATTACKL",      // Attack2h_L,
    "T_2HATTACKR",      // Attack2h_R,
    "T_2HATTACKMOVE",   // Attack2h_Move,
    "T_2HSFINISH",      // Attack2h_Finish,
    "T_2HPARADE_O",     // Parade2h,
    "T_2HPARADEJUMPB",  // Parade2h_Back,
    "T_2H_2_2HRUN",     // Draw2h,

    // Bow
    "S_BOWWALKL",         // WalkBow,
    "S_BOWRUNL",          // RunBow,
    "S_BOWRUN",           // IdleBow,
    "T_BOWJUMPB",         // BackpedalBow,
    "T_BOWRUNSTRAFEL",    // StrafeLeftBow,
    "T_BOWRUNSTRAFER",    // StrafeRightBow,
    "T_BOWRUN_2_BOWAIM",  // AttackBow,
    "T_BOW_2_BOWRUN",     // DrawBow,

    // Cbow
    "S_CBOWWALKL",          // WalkCBow,
    "S_CBOWRUNL",           // RunCBow,
    "S_CBOWRUN",            // IdleCBow,
    "T_CBOWJUMPB",          // BackpedalCBow,
    "T_CBOWRUNSTRAFEL",     // StrafeLeftCBow,
    "T_CBOWRUNSTRAFER",     // StrafeRightCBow,
    "T_CBOWRUN_2_CBOWAIM",  // AttackCBow,
    "T_CBOW_2_CBOWRUN",     // DrawCBow,

    // Fist
    "S_FISTWALKL",        // WalkFist,
    "S_FISTRUNL",         // RunFist,
    "S_FIST",             // IdleFist,
    "T_FISTJUMPB",        // BackpedalFist,
    "T_FISTRUNSTRAFEL",   // StrafeLeftFist,
    "T_FISTRUNSTRAFER",   // StrafeRightFist,
    "S_FISTATTACK",       // AttackFist,
    "T_FISTPARADE_O",     //ParadeFist,
    "T_FISTPARADEJUMPB",  //ParadeFist_Back,
    "T_FIST_2_FISTRUN",   // DrawFist,

    // Swimming
    "S_SWIM",
    "S_SWIMB",
    "S_SWIMF",
    "T_SWIMTURNL",
    "T_SWIMTURNR",
    "T_SWIM_2_DIVE",
    "T_SWIM_2_HANG",
    "S_WALKWL",  // Wade,
};

ModelVisual::ModelVisual(World::WorldInstance& world, Handle::EntityHandle entity)
    : VisualController(world, entity)
    , m_LastKnownAnimationState(static_cast<size_t>(-1))
{
}

ModelVisual::~ModelVisual()
{
}

bool ModelVisual::load(const std::string& visual)
{
    VisualController::load(visual);

    m_BodyState.bodyVisual = visual;

    assert(visual.find(".MDM") != std::string::npos || visual.find(".MDL") != std::string::npos || visual.find(".ASC") != std::string::npos || visual.find(".MDS") != std::string::npos);

    m_MainMeshHandle = m_World.getSkeletalMeshAllocator().loadMeshVDF(visual);

    if (!m_MainMeshHandle.isValid())
        return false;

    Meshes::WorldSkeletalMesh& mdata = m_World.getSkeletalMeshAllocator().getMesh(m_MainMeshHandle);
    const ZenLoad::zCModelMeshLib& zLib = m_World.getSkeletalMeshAllocator().getMeshLib(m_MainMeshHandle);

    /****
     * Push all parts of the skeletal mesh as entity
     ****/

    // TODO: Put these into a compound-component or something
    m_PartEntities.mainSkelMeshEntities = Content::entitifyMesh(m_World, m_MainMeshHandle, mdata);
    m_VisualEntities.insert(m_VisualEntities.end(),
                            m_PartEntities.mainSkelMeshEntities.begin(), m_PartEntities.mainSkelMeshEntities.end());

    Components::PositionComponent& hostPos = m_World.getEntity<Components::PositionComponent>(m_Entity);

    for (Handle::EntityHandle e : m_PartEntities.mainSkelMeshEntities)
    {
        // Init positions
        Components::EntityComponent& entity = m_World.getEntity<Components::EntityComponent>(e);
        Components::Actions::initComponent<Components::PositionComponent>(m_World.getComponentAllocator(), e);

        // Copy world-matrix
        Components::PositionComponent& pos = m_World.getEntity<Components::PositionComponent>(e);
        pos = hostPos;

        // Init animation components
        Components::Actions::initComponent<Components::AnimationComponent>(m_World.getComponentAllocator(), e);
        Components::AnimationComponent& anim = m_World.getEntity<Components::AnimationComponent>(e);

        // Assign the main-vob as animation controller
        anim.m_ParentAnimHandler = m_Entity;

        Components::Actions::initComponent<Components::BBoxComponent>(m_World.getComponentAllocator(), e);
        Components::BBoxComponent& bbox = m_World.getEntity<Components::BBoxComponent>(e);
        bbox.m_SphereRadius = m_World.getSkeletalMeshAllocator().getMeshBoundingSphereRadius(m_MainMeshHandle);
        bbox.m_BBox3D = m_World.getSkeletalMeshAllocator().getMeshBBox3d(m_MainMeshHandle);

    }

    /****
     * Read nodes and attachments
     ****/

    // In case this wasn't already initialized...
    Components::Actions::initComponent<Components::AnimationComponent>(m_World.getComponentAllocator(), m_Entity);

    // Read attachments
    if (m_VisualAttachments.size() < getAnimationHandler().getObjectSpaceTransforms().size())
        m_VisualAttachments.resize(getAnimationHandler().getObjectSpaceTransforms().size());

    if (m_VisualAttachments.empty())
        m_VisualAttachments.resize(zLib.getNodes().size());

    if (!m_VisualAttachments.empty())
    {
        // Need to get the bind-pose out
        if (zLib.getNodes().empty() && !getAnimationHandler().getActiveAnimationPtr())  // No nodes here mean, that this is only a mesh
            getAnimationHandler().setBindPose(true);

        for (size_t i = 0; i < zLib.getAttachments().size(); i++)
        {
            auto& m = zLib.getAttachments()[i];
            size_t nodeIdx = 0;

            if (zLib.getNodes().empty())
                nodeIdx = getAnimationHandler().getMeshLib().findNodeIndex(m.first);
            else
                nodeIdx = zLib.findNodeIndex(m.first);

            // Failed to find the node?
            if (nodeIdx == static_cast<size_t>(-1))
                continue;

            // Load the attached mesh
            ZenLoad::PackedMesh packed;
            m.second.packMesh(packed, 1.0f / 100.0f);
            Handle::MeshHandle amh = m_World.getStaticMeshAllocator().loadFromPacked(packed);

            if (!amh.isValid())
                continue;

            Meshes::WorldStaticMesh& amdata = m_World.getStaticMeshAllocator().getMesh(amh);

            // Add the entities to our lists
            std::vector<Handle::EntityHandle> tmp = Content::entitifyMesh(m_World, amh, amdata.mesh);
            m_PartEntities.dynamicAttachments.insert(m_PartEntities.dynamicAttachments.end(),
                                                     tmp.begin(), tmp.end());

            // If we haven't already, create an instancebuffer for this mesh
            //if(amdata.instanceDataBufferIndex == (uint32_t)-1)
            //    amdata.instanceDataBufferIndex = ((Engine::GameEngine&)m_World.getEngine()).getDefaultRenderSystem().requestInstanceDataBuffer();

            m_VisualAttachments[nodeIdx].insert(m_VisualAttachments[nodeIdx].end(), tmp.begin(), tmp.end());

            // Initialize the created entities
            for (Handle::EntityHandle e : tmp)
            {
                // Init positions
                Components::EntityComponent& entity = m_World.getEntity<Components::EntityComponent>(e);
                Components::Actions::initComponent<Components::PositionComponent>(m_World.getComponentAllocator(), e);

                // Important: Apply draw distance, and every other setting from the host
                Components::PositionComponent& pos = m_World.getEntity<Components::PositionComponent>(e);
                pos = hostPos;

                Components::Actions::initComponent<Components::BBoxComponent>(m_World.getComponentAllocator(), e);
                Components::BBoxComponent& bbox = m_World.getEntity<Components::BBoxComponent>(e);
                bbox.m_SphereRadius = amdata.boundingSphereRadius;
                bbox.m_BBox3D = amdata.bBox3D;
            }
        }

        // Calculate bind-pose matrices
        if (zLib.getNodes().empty())
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

    /*Handle::EntityHandle e = m_World.addEntity(Components::BBoxComponent::MASK);
    m_World.getEntity<Components::BBoxComponent>(e).m_DebugColor = 0xFFFFFFFF;

    Math::float3 bb[2];
    getCollisionBBox(bb);

    // Assign BBox as debug
    m_World.getEntity<Components::BBoxComponent>(e).m_BBox3D.min = bb[0];
    m_World.getEntity<Components::BBoxComponent>(e).m_BBox3D.max = bb[1];*/

    // Create m_VisualEntites-list
    rebuildMainEntityList();

    // Make sure we got the right spots for our attachments
    updateAttachmentVisuals();

    return true;
}

void ModelVisual::setHeadMesh(const std::string& head, int headTextureIdx, int teethTextureIdx)
{
    std::string v = head;

    if (v.empty())
        return;

    if (v.find(".MMB") == std::string::npos && v.find(".MMS") == std::string::npos && v.find(".ASC") == std::string::npos)
    {
        // Valentino has an extra "." at the end of the filename:
        // B_SetNpcVisual 		(self, MALE, "Hum_Head_Bald.", Face_N_Normal03, BodyTex_N, ITAR_VLK_H);

        // Remove all "."
        v.erase(std::remove(v.begin(), v.end(), '.'), v.end());

        v += ".MMB";
    }

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

void ModelVisual::setAnimation(const std::string& anim, bool loop)
{
    Components::AnimHandler& animHandler = m_World.getEntity<Components::AnimationComponent>(m_Entity).getAnimHandler();

    if (!anim.empty())
        if (loop)
            animHandler.setAnimation(anim);
        else
            animHandler.playAnimation(anim);
    else
        animHandler.stopAnimation();
}

void ModelVisual::setAnimation(ModelVisual::EModelAnimType type, bool loop)
{
    const char* str = ANIMATION_NAMES[type];

    setAnimation(str, loop);
}

void ModelVisual::playAnimation(ModelVisual::EModelAnimType type)
{
    const char* str = ANIMATION_NAMES[type];

    // TODO: Implement the priority stuff
    if (getAnimationHandler().hasAnimation(str))
    {
        setAnimation(str, false);  // Don't loop
    }
}

void ModelVisual::playAnimation(const std::string& anim)
{
    // TODO: Implement the priority stuff
    if (getAnimationHandler().hasAnimation(anim))
    {
        setAnimation(anim, false);  // Don't loop
    }
}

void ModelVisual::playAnimation(Handle::AnimationHandle anim)
{
    Components::AnimHandler& animHandler = m_World.getEntity<Components::AnimationComponent>(m_Entity).getAnimHandler();

    animHandler.playAnimation(anim);
}

const char* ModelVisual::getAnimationName(ModelVisual::EModelAnimType type)
{
    return ANIMATION_NAMES[type];
}

void ModelVisual::updateAttachmentTransforms(const std::vector<Math::Matrix>& transforms)
{
    if (m_VisualAttachments.empty())
        return;

    Math::Matrix t = getEntityTransform();

    for (size_t i = 0; i < m_VisualAttachments.size(); i++)
    {
        auto& a = m_VisualAttachments[i];

        // Initialize the created entities
        for (Handle::EntityHandle e : a)
        {
            // Set world-matrix
            Components::PositionComponent& pos = m_World.getEntity<Components::PositionComponent>(e);

            if (transforms.empty())
                pos.m_WorldMatrix = t;
            else
                pos.m_WorldMatrix = t * transforms[i];

            // If this is a vob, notify everything about the changed visual
            Vob::VobInformation vob = Vob::asVob(m_World, e);
            if (vob.visual)
                vob.visual->onTransformChanged();
        }
    }
}

void ModelVisual::updateAttachmentTransforms()
{
    Components::EntityComponent& ent = m_World.getEntity<Components::EntityComponent>(m_Entity);

    if (Components::hasComponent<Components::AnimationComponent>(ent))
        updateAttachmentTransforms(getAnimationHandler().getObjectSpaceTransforms());
    else
        updateAttachmentTransforms(m_BindPoseTransforms);
}

Components::AnimHandler& ModelVisual::getAnimationHandler()
{
    Components::AnimationComponent& anim = m_World.getEntity<Components::AnimationComponent>(m_Entity);
    return anim.getAnimHandler();
}

void ModelVisual::onTransformChanged()
{
    VisualController::onTransformChanged();

    // Update attachments with the last state of the animation-handler
    updateAttachmentTransforms();
}

Handle::EntityHandle ModelVisual::setNodeVisual(const std::string& visual, const std::string& nodeName)
{
    // Register first
    m_AttachmentVisualsByNode[nodeName] = visual;

    size_t nodeIndex = findNodeIndex(nodeName);
    if (nodeIndex > getMeshLib().getNodes().size())
        return Handle::EntityHandle::makeInvalidHandle();  // Invalid node?

    // TODO: Allow to overwrite attachments!
    //if(!m_VisualAttachments[nodeIndex].empty())
    //    return Handle::EntityHandle::makeInvalidHandle();

    if (visual.empty())
    {
        if (m_VisualAttachments[nodeIndex].empty())
            return Handle::EntityHandle::makeInvalidHandle();  // Nothing to do here

        // Clear whatever there was here
        for (Handle::EntityHandle e : m_VisualAttachments[nodeIndex])
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
    }
    else
    {
        // See these attachments as vob, for simplicities sake
        Handle::EntityHandle avh = Vob::constructVob(m_World);

        Vob::VobInformation vob = Vob::asVob(m_World, avh);

        // Carry over draw distance-mod // TODO: Should be in some other structure, this is too arbitrary
        vob.position->m_DrawDistanceFactor = m_World.getEntity<Components::PositionComponent>(m_Entity).m_DrawDistanceFactor;

        Vob::setVisual(vob, visual);

        if (vob.visual)
            vob.visual->setTransient(true);  // Don't export these objects

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

size_t ModelVisual::findNodeIndex(const std::string& name)
{
    return getMeshLib().findNodeIndex(name);
}

const ZenLoad::zCModelMeshLib& ModelVisual::getMeshLib()
{
    const ZenLoad::zCModelMeshLib& zLib = m_World.getSkeletalMeshAllocator().getMeshLib(m_MainMeshHandle);

    if (zLib.getNodes().empty())
        return getAnimationHandler().getMeshLib();
    else
        return zLib;
}

Handle::EntityHandle ModelVisual::setNodeVisual(const std::string& visual, EModelNode node)
{
    switch (node)
    {
        case EModelNode::Righthand:
            return setNodeVisual(visual, NPC_NODE_RIGHTHAND);
            break;
        case EModelNode::Lefthand:
            return setNodeVisual(visual, NPC_NODE_LEFTHAND);
            break;
        case EModelNode::Sword:
            return setNodeVisual(visual, NPC_NODE_SWORD);
            break;
        case EModelNode::Longsword:
            return setNodeVisual(visual, NPC_NODE_LONGSWORD);
            break;
        case EModelNode::Bow:
            return setNodeVisual(visual, NPC_NODE_BOW);
            break;
        case EModelNode::Crossbow:
            return setNodeVisual(visual, NPC_NODE_CROSSBOW);
            break;
        case EModelNode::Shield:
            return setNodeVisual(visual, NPC_NODE_SHIELD);
            break;
        case EModelNode::Helmet:
            return setNodeVisual(visual, NPC_NODE_HELMET);
            break;
        case EModelNode::Jaws:
            return setNodeVisual(visual, NPC_NODE_JAWS);
            break;

        // TODO: Switch main mesh here
        case EModelNode::Torso:
            return setNodeVisual(visual, NPC_NODE_TORSO);
            break;
        case EModelNode::None: /*TODO handle this somehow?  */
            break;
    }

    return Handle::EntityHandle::makeInvalidHandle();
}

void ModelVisual::getCollisionBBox(Math::float3* bb)
{
    bb[0] = Math::float3(getMeshLib().getBBoxMin().v);
    bb[1] = Math::float3(getMeshLib().getBBoxMax().v);

    // These can be in various places...
    if (bb[0].lengthSquared() == 0.0f && bb[1].lengthSquared() == 0.0f)
    {
        bb[0] = Math::float3(getMeshLib().getBBoxCollisionMin().v);
        bb[1] = Math::float3(getMeshLib().getBBoxCollisionMax().v);

        if (bb[0].lengthSquared() == 0.0f && bb[1].lengthSquared() == 0.0f)
        {
            const ZenLoad::zCModelMeshLib& zLib = m_World.getSkeletalMeshAllocator().getMeshLib(m_MainMeshHandle);
            bb[0] = Math::float3(zLib.getBBoxMin().v);
            bb[1] = Math::float3(zLib.getBBoxMax().v);
        }
    }
}

Math::float3 ModelVisual::getModelRoot()
{
    return getAnimationHandler().getRootNodePosition();
    //return Math::float3(getMeshLib().getRootNodeTranslation().v);
}

void ModelVisual::onFrameUpdate(float dt)
{
    size_t newHash = getAnimationHandler().getAnimationStateHash();
    if (newHash != m_LastKnownAnimationState)
    {
        updateAttachmentTransforms();
        m_LastKnownAnimationState = newHash;
    }
}

void ModelVisual::setBodyState(const ModelVisual::BodyState& state)
{
    m_BodyState = state;

    updateBodyMesh();
    updateHeadMesh();
}

void ModelVisual::updateBodyMesh()
{
    // Remove all created entities from the world
    for (Handle::EntityHandle e : m_PartEntities.mainSkelMeshEntities)
        m_World.removeEntity(e);

    m_PartEntities.mainSkelMeshEntities.clear();

    load(m_BodyState.bodyVisual);

    // Fix body-texture and skin-color
    if (!m_PartEntities.mainSkelMeshEntities.empty())
    {
        for (size_t i = 0; i < m_PartEntities.mainSkelMeshEntities.size(); i++)
        {
            Handle::EntityHandle bodyMain = m_PartEntities.mainSkelMeshEntities[i];

            Components::EntityComponent& ent = m_World.getEntity<Components::EntityComponent>(bodyMain);
            if (Components::hasComponent<Components::StaticMeshComponent>(ent))
            {
                Components::StaticMeshComponent& sm = m_World.getEntity<Components::StaticMeshComponent>(bodyMain);

                //if(m_BodyState.bodySkinColorIdx > 0 || m_BodyState.bodyTextureIdx > 0)
                {
                    if (sm.m_Texture.isValid())
                    {
                        // Get texture parts
                        const std::string& textureName = m_World.getTextureAllocator().getTexture(sm.m_Texture).m_TextureName;
                        std::string newTextureName = textureName;

                        bool isBodyTexture = textureName.find("_BODY") != std::string::npos;
                        // Only modify if that is the core body texture (not armor)
                        if (isBodyTexture)
                        {
                            if (m_BodyState.bodyTextureIdx != 0)
                                Utils::replace(newTextureName, "_V0", "_V" + std::to_string(m_BodyState.bodyTextureIdx));

                            if (m_BodyState.bodySkinColorIdx != 0)
                                Utils::replace(newTextureName, "_C0", "_C" + std::to_string(m_BodyState.bodySkinColorIdx));
                        }
                        sm.m_Texture = m_World.getTextureAllocator().loadTextureVDF(newTextureName);
                    }
                }
            }
        }
    }
}

void ModelVisual::updateHeadMesh()
{
    for (Handle::EntityHandle e : m_PartEntities.headMeshEntities)
        m_World.removeEntity(e);

    m_PartEntities.headMeshEntities.clear();
    m_PartEntities.headEntity.invalidate();

    // Load and attach the visual
    Handle::EntityHandle headEntity = setNodeVisual(m_BodyState.headVisual, MODEL_NODE_NAME_HEAD);

    if (!headEntity.isValid())
        return;

    m_PartEntities.headEntity = headEntity;

    // Fix head texture and color
    Vob::VobInformation head = Vob::asVob(m_World, m_PartEntities.headEntity);
    if (Vob::getVisual(head))
    {
        // TODO: Type safety!
        StaticMeshVisual* visual = reinterpret_cast<StaticMeshVisual*>(Vob::getVisual(head));

        Meshes::WorldStaticMesh& mesh = m_World.getStaticMeshAllocator().getMesh(visual->getMesh());
        visual->setInstancingEnabled(false);  // Disable instancing because of the texture changes

        //if(m_BodyState.headTextureIdx != -1 || m_BodyState.bodySkinColorIdx != 1)
        {
            // Get head-texture parts
            std::vector<std::string> headTxParts = Utils::split(visual->getDiffuseTexture(0), "0123456789");

            if (headTxParts.size() == 3)  // [HUM_HEAD_...V, _C, .TGA]
            {
                std::string newHeadTx = headTxParts[0] + std::to_string(m_BodyState.headTextureIdx) + headTxParts[1] + std::to_string(m_BodyState.bodySkinColorIdx) + headTxParts[2];

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
    for (auto& p : m_AttachmentVisualsByNode)
    {
        size_t nodeIdx = findNodeIndex(p.first);
        if (nodeIdx != static_cast<size_t>(-1))
        {
            //TODO update is not used, what's the intention there?
            bool update = false;

            // Update if there isn't something attached yet
            if (m_VisualAttachments[nodeIdx].empty())
            {
                update = true;
            }
            else  // Also update on change
            {
                Handle::EntityHandle e = m_VisualAttachments[nodeIdx][0];
                Vob::VobInformation vob = Vob::asVob(m_World, e);
                VisualController* vis = Vob::getVisual(vob);
                if (vis && vis->getName() != p.second)
                    update = true;
            }

            // Set the new visual
            setNodeVisual(p.second, p.first);
        }
    }
}

void ModelVisual::setShadowValue(float shadow)
{
    VisualController::setShadowValue(shadow);

    // Set shadow for all visual entities
    for (Handle::EntityHandle& e : m_VisualEntities)
    {
        Components::EntityComponent& ent = m_World.getEntity<Components::EntityComponent>(e);
        Components::VisualComponent& vis = m_World.getEntity<Components::VisualComponent>(e);
        Components::StaticMeshComponent& msh = m_World.getEntity<Components::StaticMeshComponent>(e);

        if (Components::hasComponent<Components::VisualComponent>(ent) && vis.m_pVisualController)
            vis.m_pVisualController->setShadowValue(shadow);

        if (Components::hasComponent<Components::StaticMeshComponent>(ent))
            msh.m_Color = Math::float4(shadow, shadow, shadow, 1.0f).toABGR8();
    }
}

void ModelVisual::stopAnimations()
{
    getAnimationHandler().stopAnimation();
}

void ModelVisual::applyOverlay(const std::string& mds)
{
    getAnimationHandler().setOverlay(mds);
}

bool ModelVisual::isAnimPlaying(const std::string& name)
{
    return getAnimationHandler().getActiveAnimationPtr() && getAnimationHandler().getActiveAnimationPtr()->m_Name == name;
}
