//
// Created by andre on 14.05.18.
//

#pragma once

#include <content/StaticMeshAllocator.h>
#include <handle/HandleDef.h>
#include <optional/tl/optional.hpp>

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

            tl::optional<Meshes::WorldStaticMesh &> get()
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
    }
}