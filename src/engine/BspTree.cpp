#include "BspTree.h"
#include <debugdraw/debugdraw.h>
#include <engine/World.h>
#include <utils/logger.h>

#include <engine/BaseEngine.h>

using namespace World;

enum class ECameraClipType
{
    In,
    Out,
    Crossing,
};

enum ClipFlags : int
{
    CLIP_FLAGS_FULL	= 63,
    CLIP_FLAGS_FULL_WO_FAR = 15
};

BspTree::BspTree(WorldInstance& world)
    : m_World(world)
{
}



void BspTree::loadBspTree(const ZenLoad::zCBspTreeData& data)
{
    loadBspNodes(data);
    loadBspSectors(data);
}

void BspTree::loadBspNodes(const ZenLoad::zCBspTreeData& data)
{
    using namespace ZenLoad;

    m_Nodes.reserve(data.nodes.size());

    // Extract the bsp-nodes
    for (const zCBspNode& s : data.nodes)
    {
        NodeIndex thisIndex = static_cast<NodeIndex>(m_Nodes.size());

        m_Nodes.emplace_back();

        BspNode& n = m_Nodes.back();
        n.front = s.front != zCBspNode::INVALID_NODE ? (int)s.front : -1;
        n.back = s.back != zCBspNode::INVALID_NODE ? (int)s.back : -1;
        n.parent = s.parent != zCBspNode::INVALID_NODE ? (int)s.parent : -1;

        n.bbox.min = s.bbox3dMin.v;
        n.bbox.max = s.bbox3dMax.v;

        n.bboxCenter = (n.bbox.max + n.bbox.min) / 2.0f;
        n.bboxRadius = (n.bbox.max - n.bbox.min).length() / 2.0f;

        n.plane = s.plane.v;

        if(n.isLeaf())
            m_Leafs.push_back(thisIndex);
    }
}

void BspTree::loadBspSectors(const ZenLoad::zCBspTreeData& data)
{
    using namespace ZenLoad;

    for(const zCSector& s : data.sectors)
    {
        m_Sectors.emplace_back();
        m_Sectors.back().name = s.name;
        m_SectorIndicesByName[s.name] = m_Sectors.size() - 1;
    }
}


NodeIndex BspTree::addEntity(Handle::EntityHandle entity)
{
    Math::float3 position = m_World.getEntity<Components::PositionComponent>(entity).m_WorldMatrix.Translation();

    // FIXME: Use actual BBox, but the vobs haven't got them initialized yet
    Utils::BBox3D bbox = {position - Math::float3(1, 1, 1), position + Math::float3(1, 1, 1)};

    std::vector<NodeIndex> nodes = findLeafOf(bbox);

    //LogInfo() << "Nodes: " << nodes;

    if (nodes.empty())
        return INVALID_NODE;

    std::set<NodeIndex>& nodesOfEntity = m_NodesByEntities[entity];
    for(NodeIndex n : nodes)
    {
        m_Nodes[n].entitiesInside.insert(entity);
        nodesOfEntity.insert(n);
    }

    // Return one of the leafs
    return nodes.front();
}

void BspTree::removeEntity(Handle::EntityHandle entity)
{
    std::set<NodeIndex>& nodesOfEntity = m_NodesByEntities[entity];

    for(NodeIndex n : nodesOfEntity)
    {
        m_Nodes[n].entitiesInside.erase(entity);
    }

    nodesOfEntity.clear();
}

void BspTree::updateEntity(Handle::EntityHandle entity)
{
    removeEntity(entity);
    addEntity(entity);
}


NodeIndex BspTree::findLeafOf(const Math::float3& position)
{
    std::function<NodeIndex(NodeIndex)> rec = [&](NodeIndex n) -> NodeIndex {

        //LogInfo() << "Traversed to: " << n;
        Aabb b;
        memcpy(&b, &m_Nodes[n].bbox, sizeof(b));

        ddDraw(b);

        if (m_Nodes[n].isLeaf())
            return n;

        int p = Utils::pointClassifyToPlane(position, m_Nodes[n].plane);

        switch (p)
        {
            case 1:  // Front
                if (m_Nodes[n].front != INVALID_NODE)
                    return rec(m_Nodes[n].front);
            break;

            case 2:  // Back
                if (m_Nodes[n].back != INVALID_NODE)
                    return rec(m_Nodes[n].back);
                break;

            case 3:  // Split
            default:
                break;
        }

        // No front or back, but not a leaf either?
        return n;
    };

    //ddSetTransform(nullptr);
    //ddSetColor(0xFF0000FF);
    assert(!m_Nodes.empty());
    return rec(0);
}

std::vector<NodeIndex> BspTree::findLeafOf(const Utils::BBox3D& bbox)
{
    std::function<std::vector<NodeIndex>(const Utils::BBox3D&, NodeIndex)> rec = [&](const Utils::BBox3D& bbox, NodeIndex n) -> std::vector<NodeIndex> {

        // LogInfo() << "Traversed to: " << n << " (front: " << m_Nodes[n].front << ", back: " << m_Nodes[n].back << ")";
        /*Aabb b;
        memcpy(&b, &m_Nodes[n].bbox, sizeof(b));

        if (m_Nodes[n].isLeaf())
            ddSetColor(0xFF0000FF);
        else
            ddSetColor(0xFF00FF00);

        ddDraw(b);*/

        if (m_Nodes[n].isLeaf())
            return {n};

        int p = Utils::bboxClassifyToPlaneSides(bbox, m_Nodes[n].plane);

        switch (p)
        {
            case Utils::PLANE_INFRONT:  // Front
                if (m_Nodes[n].front != INVALID_NODE)
                    return rec(bbox, m_Nodes[n].front);
                break;

            case Utils::PLANE_BEHIND:  // Back
                if (m_Nodes[n].back != INVALID_NODE)
                    return rec(bbox, m_Nodes[n].back);
                break;

            case Utils::PLANE_SPANNING:  // Split
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

    //ddSetTransform(nullptr);

    assert(!m_Nodes.empty());
    return rec(bbox, 0);
}


void BspTree::debugDraw()
{
    return;
    Math::float3 pp = m_World.getEntity<Components::PositionComponent>(m_World.getScriptEngine().getPlayerEntity()).m_WorldMatrix.Translation();
    Utils::BBox3D bb = {pp - Math::float3(1, 1, 1), pp + Math::float3(1, 1, 1)};
    std::vector<NodeIndex> pn = findLeafOf(bb);

    Aabb b;
    memcpy(&b, &bb, sizeof(b));

    ddSetColor(0xFFFF0000);
    ddDraw(b);

    LogInfo() << "pn: " << pn;
    LogInfo() << "pp: " << pp.toString();

    static unsigned frame = 0;
    frame++;

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


static ECameraClipType frustrumContainsSphere(const Plane* frustumPlanes, const Math::float3& sphereCenter, float sphereRadius)
{
    // various distances
    float fDistance;

    // calculate our distances to each of the planes
    for(int i = 0; i < 6; ++i) {

        // find the distance to this plane
        fDistance = Math::float3(frustumPlanes[i].m_normal).dot(sphereCenter) + frustumPlanes[i].m_dist;

        // if this distance is < -sphere.radius, we are outside
        if(fDistance < -sphereRadius)
            return ECameraClipType::Out;

        // else if the distance is between +- radius, then we intersect
        if((float)fabs(fDistance) < sphereRadius)
            return ECameraClipType::Crossing;
    }

    // otherwise we are fully in view
    return ECameraClipType::In;
}

void BspTree::markNodesVisibleNow(const Math::Matrix& viewProj, unsigned int frameNow)
{
    Math::Matrix vpTransp = viewProj.Transpose();

    Plane planes[6];
    buildFrustumPlanes(planes, viewProj.mv);

    ddDrawFrustum(viewProj.m);

    std::function<void(NodeIndex)> setSubtreeVisible = [&](NodeIndex n) {

        m_Nodes[n].lastFrameVisible = frameNow;

        if (m_Nodes[n].back != INVALID_NODE)
            setSubtreeVisible(m_Nodes[n].back);

        if (m_Nodes[n].front != INVALID_NODE)
            setSubtreeVisible(m_Nodes[n].front);
    };

    std::function<void(NodeIndex, int)> rec = [&](NodeIndex n, int clipflags) {

        BspNode& node = m_Nodes[n];

        ECameraClipType clipType = frustrumContainsSphere(planes, node.bboxCenter, node.bboxRadius);
        //ECameraClipType clipType = bbox3DInFrustumCached(node.bbox, planes, node.lastFailingFrustumPlane, clipflags);

        if (clipType == ECameraClipType::Out)
        {
            // Nothing
        }
        else if (clipType == ECameraClipType::In)
        {
            //debugDrawNode(n, 0xFFFFFFFF);

            setSubtreeVisible(n);

            // Mark whole subtree as visible using this
            m_Nodes[n].lastFrameVisible = frameNow;
        }
        else
        {
            // If this is only crossing, go further into the tree to minimize those parts of it that are out of view
            if (node.isLeaf())
            {
                m_Nodes[n].lastFrameVisible = frameNow;
                //debugDrawNode(n, 0xFFFF00FF);
            }


            if (m_Nodes[n].back != INVALID_NODE)
                rec(m_Nodes[n].back, CLIP_FLAGS_FULL);

            if (m_Nodes[n].front != INVALID_NODE)
                rec(m_Nodes[n].front, CLIP_FLAGS_FULL);
        }
    };

    rec(0, CLIP_FLAGS_FULL);
}

void BspTree::debugDrawNode(NodeIndex n, uint32_t argb)
{
    Aabb b;
    memcpy(&b, &m_Nodes[n].bbox, sizeof(b));

    ddSetColor(argb);
    ddDraw(b);
}

void BspTree::markEntitiesVisibleIfNodeIsVisible(unsigned int frameNow)
{
    auto markEntitesVisible = [&](const std::set<Handle::EntityHandle>& entities)
    {
        for(Handle::EntityHandle h : entities)
        {
            m_World.getEntity<Components::PositionComponent>(h).m_LastFrameVisible = frameNow;
        }
    };

    for(NodeIndex n : m_Leafs)
    {
        const BspNode& node = m_Nodes[n];

        if(node.lastFrameVisible == frameNow)
        {
            markEntitesVisible(node.entitiesInside);
        }
    }
}

const BspSector* BspTree::findSectorByName(const std::string& name)
{
    auto it = m_SectorIndicesByName.find(name);

    if(it == m_SectorIndicesByName.end())
        return nullptr;

    return &m_Sectors[it->second];
}

