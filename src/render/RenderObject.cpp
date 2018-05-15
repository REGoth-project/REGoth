//
// Created by andre on 13.05.18.
//

#include "RenderObject.h"
#include "RenderUtils.h"
#include "ViewList.h"
#include "RenderSystem.h"
#include "Render.h"
#include <bounds.h>

using namespace Render;
using namespace RenderObject;

enum class ECameraClipType
{
    In,
    Out,
    Crossing
};

static ECameraClipType frustrumContainsSphere(const Plane* frustumPlanes, const Math::float3& sphereCenter, float sphereRadius);
static bool isRenderObjectInFrustum(const Base& renderObject);
static bool isRenderObjectVisibleByDistance(const Base& renderObject);

void RenderObject::draw(const StaticMesh& renderObject)
{
    if(renderObject.showLocator)
        RenderUtils::drawLocator(renderObject.transform, 0xFFFFFFFF);

    if(!renderObject.mesh)
        return;

    if(!renderObject.material)
        return;

    tl::optional<Meshes::WorldStaticMesh&> mesh = renderObject.mesh->get();

    if(!mesh)
        return;

    if(!renderObject.mesh->isLoaded())
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
    if(renderObject.showLocator)
        RenderUtils::drawLocator(renderObject.transform, 0xFFFF0000);

    if(!renderObject.mesh)
        return;

    if(!renderObject.material)
        return;

    tl::optional<Meshes::WorldSkeletalMesh&> mesh = renderObject.mesh->get();

    if(!mesh)
        return;

    if(!renderObject.mesh->isLoaded())
        return;

    tl::optional<const RenderConfig&> config = Render::getConfig();

    if(!config)
        return;

    if(renderObject.boneTransforms.empty())
        return;

    renderObject.material.map(&Content::MeshMaterial::bind);

    // TODO: Maybe copy this into the render-object instead of risking a cache miss
    const Meshes::SubmeshVxInfo& submesh = mesh->m_SubmeshStarts[renderObject.mesh->submeshIdx];

    bgfx::setTransform(renderObject.boneTransforms.data(),
                       renderObject.boneTransforms.size());

    bgfx::setVertexBuffer(0, mesh->m_VertexBufferHandle);
    bgfx::setIndexBuffer(mesh->m_IndexBufferHandle,
                         submesh.m_StartIndex,
                         submesh.m_NumIndices);

    bgfx::submit(RenderViewList::DEFAULT, Render::getConfig()->programs.mainSkinnedMeshProgram);
}

void RenderObject::draw(const ParticleEffect& renderObject)
{
    RenderUtils::drawLocator(renderObject.transform, 0xFFFF00FF);
}

void RenderObject::draw(const Base& renderObject)
{
    RenderUtils::drawLocator(renderObject.transform, 0xFF0000FF);
}

bool RenderObject::isVisible(const Base &renderObject)
{
    if(!isRenderObjectVisibleByDistance(renderObject))
        return false;

    if(!isRenderObjectInFrustum(renderObject))
        return false;

    return true;
}

static bool isRenderObjectInFrustum(const Base& renderObject)
{
    assert(Render::getConfig().has_value());

    Math::float3 position =  renderObject.transform.Translation();

    float boundingRadius = renderObject.boundingRadius;

    return frustrumContainsSphere(Render::getConfig()->state.frustumPlanes, position, boundingRadius) != ECameraClipType::Out;
}


static ECameraClipType frustrumContainsSphere(const Plane* frustumPlanes, const Math::float3& sphereCenter, float sphereRadius)
{
    // various distances
    float fDistance;

    // calculate our distances to each of the planes
    for(int i = 0; i < 6; ++i) {

        // find the distance to this plane
        fDistance = Math::float3(frustumPlanes[i].m_normal).dot(sphereCenter) + frustumPlanes[i].m_dist;

        // if this distance is < -sphere.radius, we are outside
        if(fDistance < -sphereRadius)
            return ECameraClipType::Out;

        // else if the distance is between +- radius, then we intersect
        if((float)fabs(fDistance) < sphereRadius)
            return ECameraClipType::Crossing;
    }

    // otherwise we are fully in view
    return ECameraClipType::In;
}

static bool isRenderObjectVisibleByDistance(const Base& renderObject)
{
    assert(Render::getConfig().has_value());

    Math::float3 camera = Render::getConfig()->state.cameraWorld.Translation();
    Math::float3 position =  renderObject.transform.Translation();

    float drawDistanceSq = Render::getConfig()->state.drawDistanceSquared;
    float boundingRadiusSq = renderObject.boundingRadius * renderObject.boundingRadius;
    float distanceSq = (position - camera).lengthSquared() - boundingRadiusSq;

    return distanceSq < drawDistanceSq;
}