#include "StaticMeshVisual.h"
#include <engine/World.h>
#include <content/ContentLoad.h>
#include <engine/GameEngine.h>
#include <utils/logger.h>
#include <components/EntityActions.h>
#include <components/Vob.h>

using namespace Logic;

StaticMeshVisual::StaticMeshVisual(World::WorldInstance& world, Handle::EntityHandle entity)
        : VisualController(world, entity)
{

}

StaticMeshVisual::~StaticMeshVisual()
{

}

bool StaticMeshVisual::load(const std::string& visual)
{
    VisualController::load(visual);
    assert(visual.find(".3DS") != std::string::npos || visual.find(".MMB") != std::string::npos || visual.find(".MMS") != std::string::npos 
		|| visual.find(".MDMS") != std::string::npos);

    m_MeshHandle = m_World.getStaticMeshAllocator().loadMeshVDF(visual);

    if(!m_MeshHandle.isValid())
        return false;

    Meshes::WorldStaticMesh& mdata = m_World.getStaticMeshAllocator().getMesh(m_MeshHandle);

    // TODO: Put these into a compound-component or something
    m_VisualEntities = Content::entitifyMesh(m_World, m_MeshHandle, mdata.mesh);

    // If we haven't already, create an instancebuffer for this mesh
    //if(mdata.instanceDataBufferIndex == (uint32_t)-1)
    //    mdata.instanceDataBufferIndex = ((Engine::GameEngine&)m_World.getEngine()).getDefaultRenderSystem().requestInstanceDataBuffer();

    Components::PositionComponent& hostPos = m_World.getEntity<Components::PositionComponent>(m_Entity);

    for(Handle::EntityHandle e : m_VisualEntities)
    {
        // Init positions
        Components::EntityComponent& entity = m_World.getEntity<Components::EntityComponent>(e);
        Components::addComponent<Components::PositionComponent>(entity);

        // Copy world-matrix
        Components::PositionComponent& pos = m_World.getEntity<Components::PositionComponent>(e);
        pos = hostPos;

    }

    updateCollision();

    return true;
}

void StaticMeshVisual::setDiffuseTexture(size_t index, const std::string &texture)
{
    assert(index < m_VisualEntities.size());

    Components::StaticMeshComponent& sm = m_World.getEntity<Components::StaticMeshComponent>(m_VisualEntities[index]);

    Handle::TextureHandle tx = m_World.getTextureAllocator().loadTextureVDF(m_World.getEngine()->getVDFSIndex(), texture);

    // Check load
    if(!tx.isValid())
        return;

    sm.m_Texture = tx;
}

std::string StaticMeshVisual::getDiffuseTexture(size_t index)
{
    Components::StaticMeshComponent& sm = m_World.getEntity<Components::StaticMeshComponent>(m_VisualEntities[index]);

    if(sm.m_Texture.isValid())
        return m_World.getTextureAllocator().getTexture(sm.m_Texture).m_TextureName;

    return "";
}

void StaticMeshVisual::setAnimationFrame(size_t submeshIndex, size_t frame)
{
    std::string tx = getDiffuseTexture(submeshIndex);

    // Find the last number
    size_t dot = tx.find_last_of('.');
    if(dot != std::string::npos && dot > 0)
    {
        size_t dig=dot;
        do{
            dig--;

            if(!isdigit(tx[dig]))
                break;

        }while(dig != 0);

        // Not a valid animation-name
        if(dig == 0)
            return;

        std::string ext = tx.substr(dot);

        // Strip the end with the number
        tx = tx.substr(0, dig+1);

        // Add frame and ext
        tx += std::to_string(frame) + ext;

        std::string oldTex = getDiffuseTexture(submeshIndex);
        setDiffuseTexture(submeshIndex, tx);

        //if(oldTex == getDiffuseTexture(submeshIndex))
        //    LogWarn() << "Head-tex not found!";
    }
}

void StaticMeshVisual::updateCollision()
{
    VisualController::updateCollision();

    Vob::VobInformation vob = Vob::asVob(m_World, m_Entity);

    if(!Vob::getCollisionEnabled(vob))
        return;

    // Get mesh data
    Meshes::WorldStaticMesh& mdata = m_World.getStaticMeshAllocator().getMesh(m_MeshHandle);

    // Simplify mesh
    Handle::CollisionShapeHandle hullh = m_World.getPhysicsSystem().makeCollisionShapeFromMesh(mdata, Physics::CollisionShape::CT_Object, m_Name);

    if(!hullh.isValid())
        return;

    // Add static collision-mesh
    m_World.getPhysicsSystem().compoundShapeAddChild(m_World.getStaticObjectCollisionShape(), hullh, getEntityTransform());

    // TODO: Implement dynamic bodies
    /*
    // Create the component
    Components::Actions::initComponent<Components::PhysicsComponent>(m_World.getComponentAllocator(), m_Entity);
    Components::PhysicsComponent& phys = m_World.getEntity<Components::PhysicsComponent>(m_Entity);

    //

    // Set up rigid body (static)
    phys.m_RigidBody.initPhysics(&m_World.getPhysicsSystem(), *hull, "", 0.0f, getEntityTransform());
    phys.m_RigidBody.setFriction(1.0f);
    phys.m_RigidBody.setRestitution(0.1f);

    // Place rigid body onto the main entity
    //phys.m_RigidBody.setBodyTransform(getEntityTransform());
    //phys.m_RigidBody.getMotionState()->setWorldTransform(getEntityTransform());
     */
}

void StaticMeshVisual::setShadowValue(float shadow)
{
    for(Handle::EntityHandle e : m_VisualEntities)
    {
        Components::StaticMeshComponent& sm = m_World.getEntity<Components::StaticMeshComponent>(e);

        sm.m_Color = Math::float4(shadow, shadow, shadow, 1.0f).toRGBA8();
    }
}
