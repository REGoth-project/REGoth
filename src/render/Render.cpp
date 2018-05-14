//
// Created by andre on 13.05.18.
//

#include <vector>
#include "Render.h"
#include "RenderObject.h"
#include <memory/StaticReferencedAllocator.h>

using namespace Render;

namespace Allocators
{
    static Memory::StaticReferencedAllocator<RenderObject::StaticMesh, 0xFFFF> staticMeshes;
    static Memory::StaticReferencedAllocator<RenderObject::SkeletalMesh, 0xFFFF> skeletalMeshes;
    static Memory::StaticReferencedAllocator<RenderObject::ParticleEffect, 0xFFFF> particleSystems;
    static Memory::StaticReferencedAllocator<RenderObject::PointLight, 0xFFFF> pointLights;
}

static void setDebugTagOfBase(RenderObject::Base& base, const std::string& debugTag);
static void setTransformOnBase(RenderObject::Base& base, const Math::Matrix& transform);
static void setHiddenOnBase(RenderObject::Base& base, bool hide);
static void setDrawLocatorOnBase(RenderObject::Base& base, bool showLocator);

Handle::SkeletalMeshRenderHandle Render::addSkeletalMesh()
{
    return Allocators::skeletalMeshes.createObject();
}

Handle::PointLightRenderHandle Render::addPointLight()
{
    return Allocators::pointLights.createObject();
}

Handle::ParticleSystemRenderHandle Render::addParticleEffect()
{
    return Allocators::particleSystems.createObject();
}

Handle::StaticMeshRenderHandle Render::addStaticMesh()
{
    return Allocators::staticMeshes.createObject();
}

/**
 * Sets the debug-tag of the given render-object
 */
void Render::setDebugTagOf(Handle::StaticMeshRenderHandle obj, const std::string& debugTag)
{
    setDebugTagOfBase(Allocators::staticMeshes.getElement(obj), debugTag);
}

void Render::setDebugTagOf(Handle::ParticleSystemRenderHandle obj, const std::string& debugTag)
{
    setDebugTagOfBase(Allocators::particleSystems.getElement(obj), debugTag);
}

void Render::setDebugTagOf(Handle::SkeletalMeshRenderHandle obj, const std::string& debugTag)
{
    setDebugTagOfBase(Allocators::skeletalMeshes.getElement(obj), debugTag);
}

void Render::setDebugTagOf(Handle::PointLightRenderHandle obj, const std::string& debugTag)
{
    setDebugTagOfBase(Allocators::pointLights.getElement(obj), debugTag);
}

static void setDebugTagOfBase(RenderObject::Base& base, const std::string& debugTag)
{
    base.debugTag = debugTag;
}

/**
 * Removes the given Render-Object from the internal renderer.
 */
void Render::remove(Handle::StaticMeshRenderHandle obj)
{
    Allocators::staticMeshes.removeObject(obj);
}

void Render::remove(Handle::ParticleSystemRenderHandle obj)
{
    Allocators::particleSystems.removeObject(obj);
}

void Render::remove(Handle::SkeletalMeshRenderHandle obj)
{
    Allocators::skeletalMeshes.removeObject(obj);
}

void Render::remove(Handle::PointLightRenderHandle obj)
{
    Allocators::pointLights.removeObject(obj);
}


/**
 * Sets the main transform of the given render-object.
 */
void Render::setTransformOn(Handle::StaticMeshRenderHandle obj, const Math::Matrix& transform)
{
    setTransformOnBase(Allocators::staticMeshes.getElement(obj), transform);
}

void Render::setTransformOn(Handle::ParticleSystemRenderHandle obj, const Math::Matrix& transform)
{
    setTransformOnBase(Allocators::particleSystems.getElement(obj), transform);
}

void Render::setTransformOn(Handle::SkeletalMeshRenderHandle obj, const Math::Matrix& transform)
{
    setTransformOnBase(Allocators::skeletalMeshes.getElement(obj), transform);
}

void Render::setTransformOn(Handle::PointLightRenderHandle obj, const Math::Matrix& transform)
{
    setTransformOnBase(Allocators::pointLights.getElement(obj), transform);
}

static void setTransformOnBase(RenderObject::Base& base, const Math::Matrix& transform)
{
    base.transform = transform;
}

/**
 * Sets whether the given render-object should be invisible
 */
void Render::setHidden(Handle::StaticMeshRenderHandle obj, bool hide)
{
    setHiddenOnBase(Allocators::staticMeshes.getElement(obj), hide);
}

void Render::setHidden(Handle::ParticleSystemRenderHandle obj, bool hide)
{
    setHiddenOnBase(Allocators::particleSystems.getElement(obj), hide);
}

void Render::setHidden(Handle::SkeletalMeshRenderHandle obj, bool hide)
{
    setHiddenOnBase(Allocators::skeletalMeshes.getElement(obj), hide);
}

void Render::setHidden(Handle::PointLightRenderHandle obj, bool hide)
{
    setHiddenOnBase(Allocators::pointLights.getElement(obj), hide);
}

static void setHiddenOnBase(RenderObject::Base& base, bool hide)
{
    base.isHidden = hide;
}

/**
 * Sets whether a debug-locator should be shown in the objects center
 */
void Render::showDrawLocatorOn(Handle::StaticMeshRenderHandle obj, bool showLocator)
{
    setDrawLocatorOnBase(Allocators::staticMeshes.getElement(obj), showLocator);
}

void Render::showDrawLocatorOn(Handle::ParticleSystemRenderHandle obj, bool showLocator)
{
    setDrawLocatorOnBase(Allocators::particleSystems.getElement(obj), showLocator);
}

void Render::showDrawLocatorOn(Handle::SkeletalMeshRenderHandle obj, bool showLocator)
{
    setDrawLocatorOnBase(Allocators::skeletalMeshes.getElement(obj), showLocator);
}

void Render::showDrawLocatorOn(Handle::PointLightRenderHandle obj, bool showLocator)
{
    setDrawLocatorOnBase(Allocators::pointLights.getElement(obj), showLocator);
}

static void setDrawLocatorOnBase(RenderObject::Base& base, bool showLocator)
{
    base.showLocator = showLocator;
}

void Render::draw()
{
    RenderObject::StaticMesh* pStaticMeshes = Allocators::staticMeshes.getElements();
    RenderObject::SkeletalMesh* pSkeletalMeshes = Allocators::skeletalMeshes.getElements();
    RenderObject::ParticleEffect* pParticleEffects = Allocators::particleSystems.getElements();
    RenderObject::PointLight* pPointLights = Allocators::pointLights.getElements();

    for(size_t i = 0; i < Allocators::staticMeshes.getNumObtainedElements(); i++)
    {
        RenderObject::draw(pStaticMeshes[i]);
    }

    for(size_t i = 0; i < Allocators::skeletalMeshes.getNumObtainedElements(); i++)
    {
        RenderObject::draw(pSkeletalMeshes[i]);
    }

    for(size_t i = 0; i < Allocators::particleSystems.getNumObtainedElements(); i++)
    {
        RenderObject::draw(pParticleEffects[i]);
    }

    for(size_t i = 0; i < Allocators::pointLights.getNumObtainedElements(); i++)
    {
        RenderObject::draw(pPointLights[i]);
    }
}

void Render::setMeshMaterialOn(Handle::SkeletalMeshRenderHandle obj, const Content::MeshMaterial &material)
{
    Allocators::skeletalMeshes.getElement(obj).material = material;
}

void Render::setMeshMaterialOn(Handle::StaticMeshRenderHandle obj, const Content::MeshMaterial &material)
{
    Allocators::staticMeshes.getElement(obj).material = material;
}

