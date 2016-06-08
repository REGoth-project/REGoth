#pragma once
#include "../VisualController.h"
#include <handle/HandleDef.h>

namespace Logic
{
    class ModelVisual : public VisualController
    {
    public:
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
        void setHeadMesh(const std::string& head);
    protected:
        /**
         * Rebuilds the main entity list from everything found inside the PartEntityLists
         */
        void rebuildMainEntityList();

        /**
         * Vectors of the entities created for the different parts of a model
         */
        struct
        {
            std::vector<Handle::EntityHandle> mainSkelMeshEntities;
            std::vector<Handle::EntityHandle> headMeshEntities;
            std::vector<Handle::EntityHandle> armorEntities;
        }m_PartEntities;
    };
}
