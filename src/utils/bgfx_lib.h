#pragma once
#include <bgfx/bgfx.h>
#include <content/StaticLevelMesh.h>

namespace BgfxUtils
{
    /**
	 * @brief Initializes the given StaticLevelMeshes vertex- and index-buffers
	 */
    template <typename V, typename I>
    void finalizeStaticLevelMesh(LevelMesh::StaticLevelMesh<V, I>& msh)
    {
        msh.m_VertexBufferHandle = bgfx::createVertexBuffer(
            // Static data can be passed with bgfx::makeRef
            bgfx::makeRef(msh.m_Vertices.data(), msh.m_Vertices.size() * sizeof(V)), V::ms_decl);

        msh.m_IndexBufferHandle = bgfx::createIndexBuffer(
            // Static data can be passed with bgfx::makeRef
            bgfx::makeRef(msh.m_Indices.data(), msh.m_Indices.size() * sizeof(I)), sizeof(I) == 4 ? BGFX_BUFFER_INDEX32 : 0);
    }
}