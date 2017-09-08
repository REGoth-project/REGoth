#pragma once
#include <vector>
#include <handle/HandleDef.h>
#include <utils/Utils.h>
#include <zenload/zTypes.h>
#include <set>

namespace World
{
    class WorldInstance;
    typedef int NodeIndex;

    struct BspSector
    {
        std::string name;

        int ownerGuildNr;
    };

    /**
     * Contains the original BSP-Tree from the game, which is used for all kinds of performance optimization.
     * All Entities with a logic-component are stored in here and updated as they move or get created/destroyed.
     */
    class BspTree
    {
    public:
        enum
        {
            INVALID_NODE = -1
        };

        struct BspNode
        {
            /**
             * Front and back indices. (-1) if none present.
             * If one of front or back is -1, this is a leaf.
             * If parent is -1, it is the root node.
             */
            NodeIndex front, back, parent;

            /**
             * AABB of this node
             */
            Utils::BBox3D bbox;
            Math::float3 bboxCenter;
            float bboxRadius;

            /**
             * Plane dividing this node
             */
            Math::float4 plane;

            /**
             * Plane this node failed the frustum test agains last time.
             * It is likely to fail again against that plane, so cache it!
             */
            int lastFailingFrustumPlane;

            /**
             * Last frame this passed the visibility test
             */
            unsigned int lastFrameVisible;

            /**
             * @return Whether this is a leaf
             */
            bool isLeaf() const { return front == INVALID_NODE && back == INVALID_NODE; }

            /**
             * List of static entities inside this Nodes BBox
             */
            std::set<Handle::EntityHandle> entitiesInside;
        };

        BspTree(WorldInstance& world);

        /**
         * Copies the BSP-Tree from a loaded .zen-file
         * @param data BSP data
         */
        void loadBspTree(const ZenLoad::zCBspTreeData& data);

        /**
         * Adds the given entity to this tree. Will look up the position
         * from its PositionComponent.
         * @param entity Entity to add. Must have a PositionComponent!
         * @return Index of the node this was put into
         */
        NodeIndex addEntity(Handle::EntityHandle entity);

        /**
         * Removes the given entity from the tree
         * @param entity Entity to remove
         */
        void removeEntity(Handle::EntityHandle entity);

        /**
         * Updates the nodes the given entity belongs to. Must be called after the
         * entity has moved or changed in size.
         * If the entity is not already inside the tree, it will be added.
         * @param entity Entity to update
         */
        void updateEntity(Handle::EntityHandle entity);

        /**
         * Returns the node-index of the given position
         * @param position Position to check
         * @return node this position is in, or INVALID_NODE if none
         */
        NodeIndex findLeafOf(const Math::float3& position);
        std::vector<NodeIndex> findLeafOf(const Utils::BBox3D& bbox);

        /**
         * Goes through all nodes and checks if it can be seen by the given View-Projection-Matrix.
         * @param frameNow Number of this frame (ie. running index)
         */
        void markNodesVisibleNow(const Math::Matrix& viewProj, unsigned int frameNow);

        /**
         * Marks all entites inside the tree visible, if their nodes are visible.
         *
         * A node is "visible" if it's lastFrameVisible matches "frameNow".
         * @param frameNow Number of this frame (ie. running index)
         */
        void markEntitiesVisibleIfNodeIsVisible(unsigned int frameNow);

        /**
         * Debug-rendering
         */
        void debugDraw();

        /**
         * @return Sector found under the given name. nullptr, if not found.
         *         Note: You should not store this pointer somewhere!
         */
        const BspSector* findSectorByName(const std::string& name);

        /**
         * @return Looks up the sector with the given index
         */
        const BspSector& getSectorByIndex(size_t index);

        /**
         * @return Guild of the sector with the given name. -1 of not found.
         */
        int getGuildOfSector(const std::string& sectorname);
        int getGuildOfSector(ZenLoad::SectorIndex sectorindex);

        /**
         * Sets the guild that the given sector belongs to
         */
        void setGuildOfSector(const std::string& sectorname, int guildId);
        void setGuildOfSector(ZenLoad::SectorIndex sectorindex, int guildId);

    private:

        void loadBspNodes(const ZenLoad::zCBspTreeData& data);
        void loadBspSectors(const ZenLoad::zCBspTreeData& data);

        void debugDrawNode(NodeIndex n, uint32_t argb);

        /**
         * Nodes stored in this tree. First one is the root-node
         */
        std::vector<BspNode> m_Nodes;
        std::vector<NodeIndex> m_Leafs;

        /**
         * Map of Nodes by their entities
         */
        std::map<Handle::EntityHandle, std::set<NodeIndex>> m_NodesByEntities;

        /**
         * Sectors for Rooms, Houses, etc (Inside-locations)
         */
        std::vector<BspSector> m_Sectors;
        std::map<std::string, size_t> m_SectorIndicesByName;

        /**
         * World this represents
         */
        WorldInstance& m_World;
    };
}
