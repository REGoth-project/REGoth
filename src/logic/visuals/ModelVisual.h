#pragma once
#include "../VisualController.h"
#include <handle/HandleDef.h>

namespace Logic
{
    enum class EModelNode
    {
        Righthand,
        Lefthand,
        Sword,
        Longsword,
        Bow,
        Crossbow,
        Shield,
        Helmet,
        Jaws,
        Torso,
    };

    class ModelVisual : public VisualController
    {
    public:

        enum EModelAnimType
        {
            Walk,
            Run,
            FistWalk,
            FistRun,
            Idle
        };

        struct BodyState
        {
            std::string headVisual;
            size_t headTextureIdx;
            size_t teethTextureIdx;
        };

        /**
         * @param world World of the underlaying entity
         * @param entity Entity owning this controller
         */
        ModelVisual(World::WorldInstance& world, Handle::EntityHandle entity);
        virtual ~ModelVisual();

        /**
         * Loads the visual from the given name from the worlds VDFS
         * Note: The type must match this class
         * @param visual Filename of the visual. Uncompiled-version of the name!
         */
        virtual bool load(const std::string& visual);

        /**
         * Sets the headmesh for this model
         * @param head File of the mesh to use as head
         */
        void setHeadMesh(const std::string& head, size_t headTextureIdx=0, size_t teethTextureIdx=0);

        /**
         * Attaches a visual to the given node
         * @param visual The visual to load and attach
         * @param node to attach the visual to
         */
        Handle::EntityHandle setNodeVisual(const std::string& visual, size_t nodeIndex);
        Handle::EntityHandle setNodeVisual(const std::string& visual, EModelNode node);

        /**
         * @brief Sets the currently playing animation. Empty string for none
         */
        void setAnimation(const std::string& anim);

        /**
         * @brief Sets the closest animation to the specified type. A goblin uses FistWalk instead of Run, for example.
         */
        void setAnimation(EModelAnimType type);

        /**
         * @brief Updates the attachments transforms using the given object-space transforms
         */
        void updateAttachmentTransforms(const std::vector<Math::Matrix>& transforms);
        void updateAttachmentTransforms();

        /**
         * @brief Returns the underlaying animation-handler of this model
         */
        Components::AnimHandler& getAnimationHandler();

        /**
         * @brief Called when something else modified the transform of the underlaying entity
         */
        virtual void onTransformChanged() override;

        /**
         * Searches through the list of nodes and returns the index of the node with the given name
         * @param name Name to look for
         * @return Index of the node with the given name
         */
        size_t findNodeIndex(const std::string& name);

        /**
         * @return Visuals this body uses other than the main one (Head, armor, etc)
         */
        const BodyState& getBodyState(){ return m_BodyState; }
    protected:
        /**
         * Rebuilds the main entity list from everything found inside the PartEntityLists
         */
        void rebuildMainEntityList();

        /**
         * @return Skeleton-information for the currently set mesh
         */
        const ZenLoad::zCModelMeshLib& getMeshLib();

        /**
         * Handle to the main-mesh and it's skeleton information
         */
        Handle::MeshHandle m_MainMeshHandle;

        /**
         * Vectors of the entities created for the different parts of a model
         */
        struct
        {
            std::vector<Handle::EntityHandle> mainSkelMeshEntities;
            std::vector<Handle::EntityHandle> headMeshEntities;
            std::vector<Handle::EntityHandle> armorEntities;
            std::vector<Handle::EntityHandle> dynamicAttachments;
        }m_PartEntities;

        /**
         * Visuals attached to the models nodes. First vector: Index <-> node
         */
        std::vector<std::vector<Handle::EntityHandle>> m_VisualAttachments;

        /**
         * Bind-pose matrices
         */
        std::vector<Math::Matrix> m_BindPoseTransforms;

        /**
         * Visuals this body uses other than the main one (Head, armor, etc)
         */
        BodyState m_BodyState;
    };
}
