#pragma once
#include "../VisualController.h"
#include <content/StaticMeshAllocator.h>
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
         * @return Classtype of this visual
         */
        EVisualType getVisualType() override { return EVisualType::StaticMesh; }
        /**
         * Loads the visual from the given name from the worlds VDFS
         * Note: The type must match this class
         * @param visual Filename of the visual. Uncompiled-version of the name!
         */
        bool load(const std::string& visual) override;

        /**
         * Sets the texture on the specified submesh
         * @param index Submesh to set the texture of
         * @param texture New texture to laod and set
         */
        void setDiffuseTexture(size_t index, const std::string& texture);

        /**
         * Disables/enables this mesh from instancing. This is needed if there are multiple instances of the same
         * mesh but with different textures around, as instancing doesn't care about the texture.
         */
        void setInstancingEnabled(bool value);

        /**
         * Returns the texture used for the mesh of the given index
         * @param index submesh index
         * @return Texture this submesh is using
         */
        std::string getDiffuseTexture(size_t index);

        /**
         * @return Mesh this visual displays
         */
        Handle::MeshHandle getMesh() { return m_MeshHandle; }
        /**
         * Sets the textures animation from on the given submesh
         * @param submeshIndex Submesh to use
         * @param frame Animation-frame to use
         */
        void setAnimationFrame(size_t submeshIndex, size_t frame);

        /**
         * Updates the collision-hull depending on this visual
         */
        void updateCollision() override;

        /**
         * Sets the shadow-value from the worldmesh for this visual-controller
         */
        void setShadowValue(float shadow) override;

        /**
         * @brief Called when something else modified the transform of the underlaying entity
         */
        void onTransformChanged() override;

    protected:
        /**
         * Updates all render-object transforms
         */
        void updateRenderObjectTransforms();

        /**
         * Mesh this is using
         */
        Handle::MeshHandle m_MeshHandle;

        /**
         * Renderhandles for each submesh
         */
        std::vector<Handle::RenderHandle> m_SubmeshesRenderHandles;
    };
}
