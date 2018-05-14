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
     * @return Handle to the created Render-Object for further interaction
     */
    Handle::StaticMeshRenderHandle addStaticMesh();
    Handle::ParticleSystemRenderHandle addParticleEffect();
    Handle::SkeletalMeshRenderHandle addSkeletalMesh();
    Handle::PointLightRenderHandle addPointLight();

    /**
     * Sets the debug-tag of the given render-object
     */
    void setDebugTagOf(Handle::StaticMeshRenderHandle obj, const std::string& debugTag);
    void setDebugTagOf(Handle::ParticleSystemRenderHandle obj, const std::string& debugTag);
    void setDebugTagOf(Handle::SkeletalMeshRenderHandle obj, const std::string& debugTag);
    void setDebugTagOf(Handle::PointLightRenderHandle obj, const std::string& debugTag);

    /**
     * Removes the given Render-Object from the internal renderer.
     */
    void remove(Handle::StaticMeshRenderHandle obj);
    void remove(Handle::ParticleSystemRenderHandle obj);
    void remove(Handle::SkeletalMeshRenderHandle obj);
    void remove(Handle::PointLightRenderHandle obj);

    /**
     * Sets the main transform of the given render-object.
     */
    void setTransformOn(Handle::StaticMeshRenderHandle obj, const Math::Matrix& transform);
    void setTransformOn(Handle::ParticleSystemRenderHandle obj, const Math::Matrix& transform);
    void setTransformOn(Handle::SkeletalMeshRenderHandle obj, const Math::Matrix& transform);
    void setTransformOn(Handle::PointLightRenderHandle obj, const Math::Matrix& transform);

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
};



