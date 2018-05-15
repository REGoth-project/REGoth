//
// Created by andre on 13.05.18.
//

#pragma once

#include <math/mathlib.h>
#include <handle/HandleDef.h>
#include "Material.h"
#include "Mesh.h"

namespace Render
{
    namespace RenderObject
    {

        struct Base {
            Math::Matrix transform = Math::Matrix::CreateIdentity();
            float boundingRadius = 0.0f;
            bool isHidden = false;
            bool showLocator = false;
            std::string debugTag; // ie. file this was loaded from
        };

        struct StaticMesh
                : Base,
                  public Handle::HandleTypeDescriptor<Handle::StaticMeshRenderHandle>
        {
            tl::optional<Content::MeshMaterial> material;
            tl::optional<Content::StaticMesh> mesh;
        };

        struct PointLight
                : Base,
                  public Handle::HandleTypeDescriptor<Handle::PointLightRenderHandle>
        {

        };

        struct SkeletalMesh
                : Base,
                  public Handle::HandleTypeDescriptor<Handle::SkeletalMeshRenderHandle>
        {
            tl::optional<Content::MeshMaterial> material;
            tl::optional<Content::SkeletalMesh> mesh;
            std::vector<Math::Matrix> boneTransforms;
        };

        struct ParticleEffect
                : Base,
                  public Handle::HandleTypeDescriptor<Handle::ParticleSystemRenderHandle>
        {

        };

        void draw(const StaticMesh &renderObject);
        void draw(const PointLight &renderObject);
        void draw(const SkeletalMesh &renderObject);
        void draw(const ParticleEffect &renderObject);

        /**
         * Dummy, for non-implemented types
         */
        void draw(const Base &renderObject);

        /**
         * @return Whether this object is visible from the current camera position
         */
        bool isVisible(const Base& renderObject);
    }
}


