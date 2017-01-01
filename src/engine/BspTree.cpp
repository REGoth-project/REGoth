#include "BspTree.h"
#include <debugdraw/debugdraw.h>

using namespace World;



BspTree::BspTree(WorldInstance& world) : m_World(world)
{

}

NodeIndex BspTree::addEntity(Handle::EntityHandle entity)
{
    
}


NodeIndex BspTree::findLeafOf(const Math::float3& position)
{
    
}


void BspTree::loadBspTree(const ZenLoad::zCBspTreeData& data) 
{
    using namespace ZenLoad;

    m_Nodes.reserve(data.nodes.size());

    // Extract the bsp-nodes
    for(const zCBspNode& s : data.nodes)
    {
        m_Nodes.emplace_back();

        BspNode& n = m_Nodes.back();
        n.front = s.front != zCBspNode::INVALID_NODE ? (int)s.front : -1;
        n.back = s.back != zCBspNode::INVALID_NODE ? (int)s.back : -1;
        n.parent = s.parent != zCBspNode::INVALID_NODE ? (int)s.parent: -1;

        n.bbox.min = s.bbox3dMin.v;
        n.bbox.max = s.bbox3dMax.v;

        n.bbox.min *= 1.0f / 100.0f;
        n.bbox.max *= 1.0f / 100.0f;

        n.plane = s.plane.v;

        
    }
}


void BspTree::debugDraw()
{
    ddPush();

    for(BspNode& n : m_Nodes)
    {
        if(n.isLeaf())
        {
            Aabb b;
            memcpy(&b, &n.bbox, sizeof(b));
            ddDraw(b);
        }
    }

    ddPop();
}
