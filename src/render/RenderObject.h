//
// Created by andre on 13.05.18.
//

#pragma once

#include <math/mathlib.h>

namespace RenderObject
{
    struct Base
    {
        Math::Matrix transform;
        bool isHidden;
        bool showLocator;
        std::string debugTag; // ie. file this was loaded from
    };

    struct StaticMesh : Base
    {

    };

    struct PointLight : Base
    {

    };

    struct SkeletalMesh : Base
    {

    };

    struct ParticleEffect : Base
    {

    };

    void draw(const StaticMesh& renderObject);
    void draw(const PointLight& renderObject);
    void draw(const SkeletalMesh& renderObject);
    void draw(const ParticleEffect& renderObject);

    /**
     * Dummy, for non-implemented types
     */
    void draw(const Base& renderObject);
}


