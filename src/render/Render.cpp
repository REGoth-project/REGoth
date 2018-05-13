//
// Created by andre on 13.05.18.
//

#include <vector>
#include "Render.h"
#include "RenderObject.h"
#include <memory/StaticReferencedAllocator.h>

using namespace Render;

struct RenderObjectWrapper : public Handle::HandleTypeDescriptor<Handle::RenderHandle>
{
    RenderObjectWrapper() : pObject(nullptr) {}
    RenderObjectWrapper(RenderObject::Base* pObject) : pObject(pObject) {}

    RenderObject::Base* pObject;
};

static Memory::StaticReferencedAllocator<RenderObjectWrapper, 0xFFFF> renderObjects;


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

Handle::RenderHandle Render::addRenderObject(ERenderClass renderClass)
{
    auto obj = newRenderObjectFromClass(renderClass);

    if(!obj)
        return Handle::RenderHandle::makeInvalidHandle();

    Handle::RenderHandle handle = renderObjects.createObject();

    if(!handle.isValid())
    {
        delete obj;
        return Handle::RenderHandle::makeInvalidHandle();
    }

    renderObjects.getElement(handle).pObject = obj;

    return handle;
}

void Render::removeRenderObject(Handle::RenderHandle renderObject)
{
    renderObjects.removeObject(renderObject);
}

void Render::setTransformOn(Handle::RenderHandle renderObject, const Math::Matrix& transform)
{
    renderObjects.getElement(renderObject).pObject->transform = transform;
}

void Render::setHidden(Handle::RenderHandle renderObject, bool hide)
{
    renderObjects.getElement(renderObject).pObject->isHidden = hide;
}

void Render::showDrawLocatorOn(Handle::RenderHandle renderObject, bool showLocator)
{
    renderObjects.getElement(renderObject).pObject->showLocator = showLocator;
}

void Render::setDebugTagOf(Handle::RenderHandle renderObject, const std::string& debugTag)
{
    renderObjects.getElement(renderObject).pObject->debugTag = debugTag;
}

void Render::draw()
{
    RenderObjectWrapper* pRenderObjects = renderObjects.getElements();

    for(size_t i = 0; i < renderObjects.getNumObtainedElements(); i++)
    {
        if(pRenderObjects[i].pObject)
        {
            RenderObject::draw(*pRenderObjects[i].pObject);
        }
    }

}
