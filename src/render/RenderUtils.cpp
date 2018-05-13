//
// Created by andre on 13.05.18.
//
#include "RenderUtils.h"
#include <bgfx/bgfx.h>
#include <debugdraw/debugdraw.h>

using namespace RenderUtils;

void RenderUtils::drawLocator(const Math::Matrix& transform, uint32_t abgr)
{
    ddPush();

    Math::float3 position = transform.Translation();

    ddSetColor(abgr);
    ddDrawAxis(position.x, position.y, position.z, 1.0);
    ddDrawAxis(position.x, position.y, position.z, -1.0);

    ddPop();
}
