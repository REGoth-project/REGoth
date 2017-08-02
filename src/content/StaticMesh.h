#pragma once
#include <vector>
#include "Materials.h"
#include <handle/Handle.h>
#include <handle/HandleDef.h>

namespace Meshes
{
    /**
     * Basic information every mesh needs.
     * Contains vertices, indices and handles to the VB/IB
     */
    template <typename V, typename I, typename VHDL, typename IHDL>
    struct StaticMeshInfo
    {
        // Global information
        std::vector<V> m_Vertices;
        std::vector<I> m_Indices;

        VHDL m_VertexBufferHandle;
        IHDL m_IndexBufferHandle;
    };

    /**
     * Holds where a submesh starts and how many indices it posesses
     */
    struct SubmeshVxInfo
    {
        uint32_t m_StartIndex;
        uint32_t m_NumIndices;
    };

    /**
     * Static mesh, contains buffer-handles and submeshes
     * @param V Vertex-typename
     * @param I Index-typename
     * @param MHDL Material handle-typename
     * @param VHDL Vertexbuffer handle typename
     * @param IHDL Indexbuffer handle typename
     * @param NMSH Number of submeshes stored here.
     */
    template <typename V, typename I, typename MHDL, typename VHDL, typename IHDL, int NMSH>
    struct StaticMeshFixedSize : public StaticMeshInfo<V, I, VHDL, IHDL>
    {
        // Submesh-Information
        SubmeshVxInfo m_SubmeshStarts[NMSH];
        MHDL m_SubmeshMaterials[NMSH];

        enum
        {
            NUM_MESHES = NMSH
        };
    };

    /**
     * Static mesh with dynamic amount of submeshes
     * @param V Vertex-typename
     * @param I Index-typename
     * @param MHDL Material handle-typename
     * @param VHDL Vertexbuffer handle typename
     * @param IHDL Indexbuffer handle typename
     */
    template <typename V, typename I, typename MHDL, typename VHDL, typename IHDL>
    struct StaticMeshDynSize : public StaticMeshInfo<V, I, VHDL, IHDL>
    {
        // Submesh information
        std::vector<SubmeshVxInfo> m_SubmeshStarts;
        std::vector<Materials::TexturedMaterial> m_SubmeshMaterials;
        std::vector<std::string> m_SubmeshMaterialNames;  // TODO: Remove this and only use handles
    };

    /**
     * Removes all data from a StaticMeshDynSize-Instance
     */
    template <typename V, typename I, typename MHDL, typename VHDL, typename IHDL>
    void clearStaticMeshDynSize(StaticMeshDynSize<V, I, MHDL, VHDL, IHDL>& target)
    {
        target.m_SubmeshMaterials.clear();
        target.m_SubmeshStarts.clear();
        target.m_Indices.clear();
        target.m_Vertices.clear();
    }
}