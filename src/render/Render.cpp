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

static void setDebugTagOf(RenderObject::Base& base, const std::string& debugTag);
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


Handle::SkeletalMeshRenderHandle addSkeletalMesh()
{
    return Allocators::skeletalMeshes.createObject();
}

Handle::PointLightRenderHandle addPointLight()
{
    return Allocators::pointLights.createObject();
}

Handle::ParticleSystemRenderHandle addParticleEffect()
{
    return Allocators::particleSystems.createObject();
}

Handle::StaticMeshRenderHandle addStaticMesh()
{
    return Allocators::staticMeshes.createObject();
}

/**
 * Sets the debug-tag of the given render-object
 */
void setDebugTagOf(Handle::StaticMeshRenderHandle obj, const std::string& debugTag)
{
    setDebugTagOf(Allocators::staticMeshes.getElement(obj), debugTag);
}

void setDebugTagOf(Handle::ParticleSystemRenderHandle obj, const std::string& debugTag)
{
    setDebugTagOf(Allocators::particleSystems.getElement(obj), debugTag);
}

void setDebugTagOf(Handle::SkeletalMeshRenderHandle obj, const std::string& debugTag)
{
    setDebugTagOf(Allocators::skeletalMeshes.getElement(obj), debugTag);
}

void setDebugTagOf(Handle::PointLightRenderHandle obj, const std::string& debugTag)
{
    setDebugTagOf(Allocators::pointLights.getElement(obj), debugTag);
}

static void setDebugTagOf(RenderObject::Base& base, const std::string& debugTag)
{
    base.debugTag = debugTag;
}

/**
 * Removes the given Render-Object from the internal renderer.
 */
void remove(Handle::StaticMeshRenderHandle obj)
{
    Allocators::staticMeshes.removeObject(obj);
}

void remove(Handle::ParticleSystemRenderHandle obj)
{
    Allocators::particleSystems.removeObject(obj);
}

void remove(Handle::SkeletalMeshRenderHandle obj)
{
    Allocators::skeletalMeshes.removeObject(obj);
}

void remove(Handle::PointLightRenderHandle obj)
{
    Allocators::pointLights.removeObject(obj);
}


/**
 * Sets the main transform of the given render-object.
 */
void setTransformOn(Handle::StaticMeshRenderHandle obj, const Math::Matrix& transform)
{
    setTransformOnBase(Allocators::staticMeshes.getElement(obj), transform);
}

void setTransformOn(Handle::ParticleSystemRenderHandle obj, const Math::Matrix& transform)
{
    setTransformOnBase(Allocators::particleSystems.getElement(obj), transform);
}

void setTransformOn(Handle::SkeletalMeshRenderHandle obj, const Math::Matrix& transform)
{
    setTransformOnBase(Allocators::skeletalMeshes.getElement(obj), transform);
}

void setTransformOn(Handle::PointLightRenderHandle obj, const Math::Matrix& transform)
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
void setHidden(Handle::StaticMeshRenderHandle obj, bool hide);
void setHidden(Handle::ParticleSystemRenderHandle obj, bool hide);
void setHidden(Handle::SkeletalMeshRenderHandle obj, bool hide);
void setHidden(Handle::PointLightRenderHandle obj, bool hide);

/**
 * Sets whether a debug-locator should be shown in the objects center
 */
void showDrawLocatorOn(Handle::StaticMeshRenderHandle obj, bool showLocator);
void showDrawLocatorOn(Handle::ParticleSystemRenderHandle obj, bool showLocator);
void showDrawLocatorOn(Handle::SkeletalMeshRenderHandle obj, bool showLocator);
void showDrawLocatorOn(Handle::PointLightRenderHandle obj, bool showLocator);


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

