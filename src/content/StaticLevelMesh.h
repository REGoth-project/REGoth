#pragma once
#include "StaticMesh.h"
#include "VertexTypes.h"
#include "Materials.h"
#include <handle/HandleDef.h>

namespace World
{
    class WorldInstance;
}

namespace LevelMesh
{
    namespace Internal
    {
        /**
         * Type for Level-Levelmeshes
         */
        template<typename VHDL, typename IHDL, typename V, typename I>
        using StaticLevelMesh = Meshes::StaticMeshDynSize<V,
                I,
                Materials::Material::HandleType,
                VHDL,
                IHDL>;

        template<typename VHDL, typename IHDL, typename V, typename I, int NUM_SUB>
        using StaticLevelMeshFixed = Meshes::StaticMeshFixedSize<V,
                I,
                Materials::Material::HandleType,
                VHDL,
                IHDL,
                NUM_SUB>;
    }

    /**
     * Type for dynamically sized StaticLevelMeshes
     */
    template<typename V, typename I>
    using StaticLevelMesh = Internal::StaticLevelMesh<Handle::InternalVBHandle, Handle::InternalIBHandle, V, I>;

    /**
     * Type for fixed size StaticLevelMeshes
     */
    template<typename V, typename I, int NUM_SUB>
    using StaticLevelMeshFixed = Internal::StaticLevelMeshFixed<Handle::InternalVBHandle, Handle::InternalIBHandle, V, I, NUM_SUB>;

    /**
     * Generates a StaticLevelMesh from the given info.
     * This does not support submeshes.
     * @param target Mesh-Instance to store the data in
     * @param vertices vector of vertices of the given type to store inside the mesh
     * @param indices vector of the indices for the given vertices
     * @param materialHandle Material to use for the mesh
     */
    template<typename V, typename I>
    void fromVertexData(StaticLevelMesh<V, I>& target, const std::vector<V>& vertices,
                        const std::vector<I>& indices, Materials::Material::HandleType materialHandle)
    {
        // Only one submesh
		target.m_SubmeshStarts.resize(1);
		target.m_SubmeshMaterials.resize(1);
        target.m_SubmeshStarts[0] = {static_cast<I>(0),static_cast<I>(indices.size())};
        //target.m_SubmeshMaterials[0] = materialHandle;
        target.m_Vertices = vertices;
        target.m_Indices = indices;
    }


}


























