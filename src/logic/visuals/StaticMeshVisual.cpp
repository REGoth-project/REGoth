#include "StaticMeshVisual.h"
#include <engine/World.h>
#include <content/ContentLoad.h>
#include <engine/BaseEngine.h>
#include <utils/logger.h>

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
    m_VisualEntities = Content::entitifyMesh(m_World, m_MeshHandle, mdata);

    for(Handle::EntityHandle e : m_VisualEntities)
    {
        // Init positions
        Components::EntityComponent& entity = m_World.getEntity<Components::EntityComponent>(e);
        Components::addComponent<Components::PositionComponent>(entity);

        // Copy world-matrix
        Components::PositionComponent& pos = m_World.getEntity<Components::PositionComponent>(e);
        pos.m_WorldMatrix = getEntityTransform();
    }

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
