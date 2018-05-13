//
// Created by andre on 13.05.18.
//

#pragma once


#include <math/mathlib.h>
#include <stdint.h>

namespace RenderUtils
{
    /**
     * Draws a 3d-Cross at the given transform
     */
    void drawLocator(const Math::Matrix& transform, uint32_t abgr = 0xFFFFFFFF);
};



