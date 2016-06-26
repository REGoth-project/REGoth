#pragma once
#include "../VisualController.h"
#include <handle/HandleDef.h>

namespace Logic
{
    class StaticMeshVisual : public VisualController
    {
    public:
        /**
         * @param world World of the underlaying entity
         * @param entity Entity owning this controller
         */
        StaticMeshVisual(World::WorldInstance& world, Handle::EntityHandle entity);
        virtual ~StaticMeshVisual();

        /**
         * Loads the visual from the given name from the worlds VDFS
         * Note: The type must match this class
         * @param visual Filename of the visual. Uncompiled-version of the name!
         */
        virtual bool load(const std::string& visual);

        /**
         * Sets the texture on the specified submesh
         * @param index Submesh to set the texture of
         * @param texture New texture to laod and set
         */
        void setDiffuseTexture(size_t index, const std::string& texture);

        /**
         * Returns the texture used for the mesh of the given index
         * @param index submesh index
         * @return Texture this submesh is using
         */
        std::string getDiffuseTexture(size_t index);

        /**
         * Sets the textures animation from on the given submesh
         * @param submeshIndex Submesh to use
         * @param frame Animation-frame to use
         */
        void setAnimationFrame(size_t submeshIndex, size_t frame);
    protected:

        /**
         * Mesh this is using
         */
        Handle::MeshHandle m_MeshHandle;

    };
}
