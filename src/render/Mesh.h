//
// Created by andre on 14.05.18.
//

#pragma once

#include <content/StaticMeshAllocator.h>
#include <handle/HandleDef.h>
#include <optional/tl/optional.hpp>
#include <content/SkeletalMeshAllocator.h>

namespace Render
{
    namespace Content
    {
        struct StaticMesh
        {
            StaticMesh(Meshes::StaticMeshAllocator &allocator,
                       Handle::MeshHandle handle = Handle::MeshHandle::makeInvalidHandle(),
                       size_t submeshIdx = 0)
                    : pAllocator(&allocator),
                      handle(handle),
                      submeshIdx(submeshIdx)
            {}

            Handle::MeshHandle handle;
            size_t submeshIdx;
            std::string debugTag; // ie. file-name

            bool isLoaded() const {
                if(!get())
                    return false;

                return get()->loaded;
            }

            tl::optional<Meshes::WorldStaticMesh &> get() const
            {
                if (handle.isValid())
                {
                    return pAllocator->getMesh(handle);
                } else
                {
                    return tl::nullopt;
                }
            }

        private:
            Meshes::StaticMeshAllocator *pAllocator;
        };

        struct SkeletalMesh
        {
            SkeletalMesh(Meshes::SkeletalMeshAllocator &allocator,
                       Handle::MeshHandle handle = Handle::MeshHandle::makeInvalidHandle(),
                       size_t submeshIdx = 0)
                    : pAllocator(&allocator),
                      handle(handle),
                      submeshIdx(submeshIdx)
            {}

            Handle::MeshHandle handle;
            size_t submeshIdx;
            std::string debugTag; // ie. file-name

            bool isLoaded() const {
                return pAllocator->isLoaded(handle);
            }

            tl::optional<float> getBoundingSphereRadius() const
            {
                if (handle.isValid())
                {
                    return pAllocator->getMeshBoundingSphereRadius(handle);
                } else
                {
                    return tl::nullopt;
                }
            }

            tl::optional<Meshes::WorldSkeletalMesh &> get() const
            {
                if (handle.isValid())
                {
                    return pAllocator->getMesh(handle);
                } else
                {
                    return tl::nullopt;
                }
            }

        private:
            Meshes::SkeletalMeshAllocator *pAllocator;
        };
    }
}