//
// Created by andre on 13.05.18.
//

#include "RenderObject.h"
#include "RenderUtils.h"

using namespace RenderObject;

void RenderObject::draw(const StaticMesh& renderObject)
{
    RenderUtils::drawLocator(renderObject.transform, 0xFFFFFFFF);
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
