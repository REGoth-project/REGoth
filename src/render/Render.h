//
// Created by andre on 13.05.18.
//

#pragma once
#include <handle/HandleDef.h>
#include <math/mathlib.h>

namespace Render
{
    enum class ERenderClass
    {
        StaticMesh,
        ParticleEffect,
        SkeletalMesh,
        PointLight,
    };

    /**
     * Draws the scene, as it currently is
     */
    void draw();

    /**
     * Creates a new render-object with the given class. This registers the object inside
     * the internal renderer and sets it up for displaying.
     *
     * @param renderClass Renderclass to create the object with.
     * @return Handle to the created Render-Object for further interaction
     */
    Handle::RenderHandle addRenderObject(ERenderClass renderClass);

    /**
     * Sets the debug-tag of the given render-object
     */
    void setDebugTagOf(Handle::RenderHandle renderObject, const std::string& debugTag);

    /**
     * Removes the given Render-Object from the internal renderer.
     */
    void removeRenderObject(Handle::RenderHandle renderObject);

    /**
     * Sets the main transform of the given render-object.
     */
    void setTransformOn(Handle::RenderHandle renderObject, const Math::Matrix& transform);

    /**
     * Sets whether the given render-object should be invisible
     */
    void setHidden(Handle::RenderHandle renderObject, bool hide);

    /**
     * Sets whether a debug-locator should be shown in the objects center
     */
    void showDrawLocatorOn(Handle::RenderHandle renderObject, bool showLocator);
};



