#pragma once
#include <vector>
#include <handle/HandleDef.h>
#include <utils/Utils.h>
#include <zenload/zTypes.h>

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
        std::vector<NodeIndex> findLeafOf(const Utils::BBox3D& bbox);

        /**
         * Goes through all nodes and checks if it can be seen by the given View-Projection-Matrix.
         * @param frameNow Number of this frame (ie. running index)
         */
        void markNodesVisibleNow(const Math::Matrix& viewProj, unsigned int frameNow);

        /**
         * Debug-rendering
         */
        void debugDraw();

    private:

        void debugDrawNode(NodeIndex n, uint32_t argb);

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
