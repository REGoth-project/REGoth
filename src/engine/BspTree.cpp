#include "BspTree.h"
#include <debugdraw/debugdraw.h>
#include <engine/World.h>
#include <utils/logger.h>

using namespace World;



BspTree::BspTree(WorldInstance& world) : m_World(world)
{

}

NodeIndex BspTree::addEntity(Handle::EntityHandle entity)
{
    Math::float3 position = m_World.getEntity<Components::PositionComponent>(entity).m_WorldMatrix.Translation();

    // FIXME: Use actual BBox, but the vobs haven't got them initialized yet
    Utils::BBox3D bbox = { position - Math::float3(1,1,1), position + Math::float3(1,1,1) };
    
    std::vector<NodeIndex> nodes = findLeafsOf(bbox);

    LogInfo() << "Nodes: " << nodes;

    if(nodes.empty())
        return INVALID_NODE;

    // Return one of the leafs
    return nodes.front();
}

NodeIndex BspTree::findLeafOf(const Math::float3& position)
{
    std::function<NodeIndex(NodeIndex)> rec = [&](NodeIndex n) -> NodeIndex {

        LogInfo() << "Traversed to: " << n;
        Aabb b;
        memcpy(&b, &m_Nodes[n].bbox, sizeof(b));

        ddDraw(b);

        if(m_Nodes[n].isLeaf())
            return n;

        int p = Utils::pointClassifyToPlane(position, m_Nodes[n].plane);

        

        switch(p)
        {
        case 1: // Front
            if(m_Nodes[n].front != INVALID_NODE)
                return rec(m_Nodes[n].front);

        case 2: // Back
            if(m_Nodes[n].back != INVALID_NODE)
                return rec(m_Nodes[n].back);

        case 3: // Split
        default:
            break;
        }

        // No front or back, but not a leaf either?
        return n;
    };

    ddSetTransform(nullptr);
    ddSetColor(0xFF0000FF);
    assert(!m_Nodes.empty());
    return rec(0);
}

std::vector<NodeIndex> BspTree::findLeafsOf(const Utils::BBox3D& bbox)
{
    std::function<std::vector<NodeIndex>(const Utils::BBox3D&, NodeIndex)> rec = [&](const Utils::BBox3D& bbox, NodeIndex n) -> std::vector<NodeIndex> {

        // LogInfo() << "Traversed to: " << n << " (front: " << m_Nodes[n].front << ", back: " << m_Nodes[n].back << ")";
        Aabb b;
        memcpy(&b, &m_Nodes[n].bbox, sizeof(b));

        ddSetColor(0xFF0000FF);
        ddDraw(b);

        if(m_Nodes[n].isLeaf())
            return {n};

        int p = Utils::bboxClassifyToPlaneSides(bbox, m_Nodes[n].plane);

        switch(p)
        {
        case Utils::PLANE_INFRONT: // Front
            if(m_Nodes[n].front != INVALID_NODE)
                return rec(bbox, m_Nodes[n].front);

        case Utils::PLANE_BEHIND: // Back
            if(m_Nodes[n].back != INVALID_NODE)
                return rec(bbox, m_Nodes[n].back);

        case Utils::PLANE_SPANNING: // Split
            {
                std::vector<NodeIndex> f = m_Nodes[n].front != INVALID_NODE ? rec(bbox, m_Nodes[n].front) : std::vector<NodeIndex>();
                std::vector<NodeIndex> b = m_Nodes[n].back != INVALID_NODE ? rec(bbox, m_Nodes[n].back) : std::vector<NodeIndex>();

                f.insert(f.end(), b.begin(), b.end());

                return f;
            }

        default:
            return {}; 
        }
        
        return {};
    };

    ddSetTransform(nullptr);

    assert(!m_Nodes.empty());
    return rec(bbox, 0);
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

        n.plane = s.plane.v;
    }

    // Go through every leaf and insert the triangles
    // FIXME: Should really cache this somewhere!
    LogInfo() << "Gathering BSP-Triangles";
    size_t i = 0;
    for(const ZenLoad::WorldTriangle& tri : m_World.getWorldMesh().getTriangles())
    {
        for (BspNode& n : m_Nodes)
        {
            if(n.isLeaf() && Utils::triangleInBBox(tri.vertices[0].Position.v,
                                                   tri.vertices[1].Position.v,
                                                   tri.vertices[2].Position.v,
                                                   n.bbox))
            {
                n.nodeTriangles.push_back(i);
            }
        }

        i++;
    }
    LogInfo() << "Done gathering BSP-Triangles!";
}


void BspTree::debugDraw()
{
    return;
    Math::float3 pp = m_World.getEntity<Components::PositionComponent>(m_World.getScriptEngine().getPlayerEntity()).m_WorldMatrix.Translation();
    Utils::BBox3D bb = {pp - Math::float3(1,1,1), pp + Math::float3(1,1,1)};
    std::vector<NodeIndex> pn = findLeafsOf(bb);

    LogInfo() << "pn: " << pn;
    LogInfo() << "pp: " << pp.toString();
    return;
    /*
    int i=0;
    for(BspNode& n : m_Nodes)
    {
        if(n.isLeaf())
        {
            Aabb b;
            memcpy(&b, &n.bbox, sizeof(b));

            if(i == pn)
                ddSetColor(0xFF0000FF);
            else
                ddSetColor(0xFFFFFFFF);

            ddDraw(b);
        }


        i++;
    }

    ddPop();*/
}

void BspTree::addLightEntity(Handle::EntityHandle entity)
{
    // Make Bounding Box of the given light and get light radius
    Components::PositionComponent& p = m_World.getEntity<Components::PositionComponent>(entity);
    float radius = p.m_WorldMatrix.Forward().length();

    Utils::BBox3D bb = {Math::float3(-radius,-radius,-radius), Math::float3(radius,radius,radius)};

    // check in which BSP-Nodes this light would end up
    std::vector<NodeIndex> nodes = findLeafsOf(bb);

    for(NodeIndex i : nodes)
    {
        m_Nodes[i].lightEntities.push_back(entity);
    }
}
