#pragma once
#include <utils/Utils.h>
#include <zenload/zTypes.h>
#include <vector>
#include <handle/HandleDef.h>

namespace World
{
    class WorldInstance;
    typedef int NodeIndex;

    /**
     * Contains the original BSP-Tree from the game, which is used for all kinds of performance optimization.
     * All Entities with a logic-component are stored in here and updated as they move or get created/destroyed.
     */
    class BspTree
    {
    public:
        enum { INVALID_NODE = -1 };

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

            /**
             * Plane dividing this node
             */
            Math::float4 plane;

            /**
             * @return Whether this is a leaf
             */
            bool isLeaf() { return front == INVALID_NODE || back == INVALID_NODE; }
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
         * Returns the node-index of the given position
         * @param position Position to check
         * @return node this position is in, or INVALID_NODE if none
         */
        NodeIndex findLeafOf(const Math::float3& position);

        /**
         * Debug-rendering
         */
        void debugDraw();
    private:

        /**
         * Nodes stored in this tree. First one is the root-node
         */
        std::vector<BspNode> m_Nodes;

        /**
         * World this represents
         */
        WorldInstance& m_World;
    };
}
