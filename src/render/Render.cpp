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

static RenderObject::Base* newRenderObjectFromClass(ERenderClass renderClass)
{
    switch(renderClass)
    {
        case ERenderClass::StaticMesh: return new RenderObject::StaticMesh;
        case ERenderClass::ParticleEffect: return new RenderObject::ParticleEffect;
        case ERenderClass::SkeletalMesh: return new RenderObject::SkeletalMesh;
        case ERenderClass::PointLight: return new RenderObject::PointLight;
        default: return nullptr;
    }
}


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
void Render::setHidden(Handle::StaticMeshRenderHandle obj, bool hide) {}
void Render::setHidden(Handle::ParticleSystemRenderHandle obj, bool hide) {}
void Render::setHidden(Handle::SkeletalMeshRenderHandle obj, bool hide) {}
void Render::setHidden(Handle::PointLightRenderHandle obj, bool hide) {}

/**
 * Sets whether a debug-locator should be shown in the objects center
 */
void Render::showDrawLocatorOn(Handle::StaticMeshRenderHandle obj, bool showLocator) {}
void Render::showDrawLocatorOn(Handle::ParticleSystemRenderHandle obj, bool showLocator) {}
void Render::showDrawLocatorOn(Handle::SkeletalMeshRenderHandle obj, bool showLocator) {}
void Render::showDrawLocatorOn(Handle::PointLightRenderHandle obj, bool showLocator) {}


void Render::draw()
{
    /*RenderObjectWrapper* pRenderObjects = renderObjects.getElements();

    for(size_t i = 0; i < renderObjects.getNumObtainedElements(); i++)
    {
        if(pRenderObjects[i].pObject)
        {
            RenderObject::draw(*pRenderObjects[i].pObject);
        }
    }*/

}

