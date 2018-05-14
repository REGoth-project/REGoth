//
// Created by andre on 13.05.18.
//

#include "RenderObject.h"
#include "RenderUtils.h"
#include "ViewList.h"
#include "RenderSystem.h"
#include "Render.h"

using namespace Render;
using namespace RenderObject;

void RenderObject::draw(const StaticMesh& renderObject)
{
    RenderUtils::drawLocator(renderObject.transform, 0xFFFFFFFF);

    if(!renderObject.mesh)
        return;

    if(!renderObject.material)
        return;

    tl::optional<Meshes::WorldStaticMesh&> mesh = renderObject.mesh->get();

    if(!mesh)
        return;

    if(!mesh->loaded)
        return;

    tl::optional<const RenderConfig&> config = Render::getConfig();

    if(!config)
        return;

    bgfx::setTransform(renderObject.transform.m);
    renderObject.material.map(&Content::MeshMaterial::bind);

    // TODO: Maybe copy this into the render-object instead of risking a cache miss
    const Meshes::SubmeshVxInfo& submesh = mesh->mesh.m_SubmeshStarts[renderObject.mesh->submeshIdx];

    if (mesh->mesh.m_IndexBufferHandle.idx != bgfx::kInvalidHandle)
    {
        bgfx::setVertexBuffer(0, mesh->mesh.m_VertexBufferHandle);
        bgfx::setIndexBuffer(mesh->mesh.m_IndexBufferHandle,
                             submesh.m_StartIndex,
                             submesh.m_NumIndices);
    }
    else
    {
        bgfx::setVertexBuffer(0, mesh->mesh.m_VertexBufferHandle,
                              submesh.m_StartIndex,
                              submesh.m_NumIndices);
    }

    bgfx::submit(RenderViewList::DEFAULT, config->programs.mainWorldProgram);
}

void RenderObject::draw(const PointLight& renderObject)
{
    RenderUtils::drawLocator(renderObject.transform, 0xFF00FFFF);
}

void RenderObject::draw(const SkeletalMesh& renderObject)
{
    RenderUtils::drawLocator(renderObject.transform, 0xFFFF0000);
}

void RenderObject::draw(const ParticleEffect& renderObject)
{
    RenderUtils::drawLocator(renderObject.transform, 0xFFFF00FF);
}

void RenderObject::draw(const Base& renderObject)
{
    RenderUtils::drawLocator(renderObject.transform, 0xFF0000FF);
}
