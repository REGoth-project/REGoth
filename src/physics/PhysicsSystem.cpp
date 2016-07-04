//
// Created by desktop on 26.06.16.
//

#include <components/EntityActions.h>
#include "PhysicsSystem.h"
#include <logic/VisualController.h>
#include <logic/Controller.h>
#include "DebugDrawer.h"
#include <engine/World.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>

using namespace Physics;

const int NUM_MAX_SUB_STEPS = 3;

PhysicsSystem::PhysicsSystem(World::WorldInstance& world, float gravity)
    : m_Broadphase(&m_PairCache),
	  m_Dispatcher(&m_CollisionConfiguration),
      m_DynamicsWorld(&m_Dispatcher, &m_Broadphase, &m_Solver, &m_CollisionConfiguration),
      m_World(world)
{
	// Bullet would update each AABB, even though most of the world is static. We'll do this ourselfes for static objects.
	//m_DynamicsWorld.setForceUpdateAllAabbs(false);

    //btGImpactCollisionAlgorithm::registerAlgorithm(&m_Dispatcher);
    m_DynamicsWorld.setGravity(btVector3(0, gravity, 0));

    m_DynamicsWorld.setDebugDrawer(new DebugDrawer);
    m_DynamicsWorld.getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
}

PhysicsSystem::~PhysicsSystem()
{
    for(size_t i=0;i<m_PhysicsObjectAllocator.getNumObtainedElements();i++)
        PhysicsObject::clean(m_PhysicsObjectAllocator.getElements()[i]);

    for(size_t i=0;i<m_CollisionShapeAllocator.getNumObtainedElements();i++)
        CollisionShape::clean(m_CollisionShapeAllocator.getElements()[i]);

    delete m_DynamicsWorld.getDebugDrawer();
}

void PhysicsSystem::debugDraw()
{
    m_DynamicsWorld.debugDrawWorld();
}



void PhysicsSystem::update(double dt)
{
    m_DynamicsWorld.stepSimulation(static_cast<btScalar>(dt));

    const auto& ctuple = m_World.getComponentDataBundle().m_Data;
    size_t num = m_World.getComponentDataBundle().m_NumElements;

    // Copy all physics-transforms to the position-components
    Components::EntityComponent* ents = std::get<Components::EntityComponent*>(ctuple);
    Components::PhysicsComponent* phys = std::get<Components::PhysicsComponent*>(ctuple);
    Components::PositionComponent* pos = std::get<Components::PositionComponent*>(ctuple);
    Components::LogicComponent* log = std::get<Components::LogicComponent*>(ctuple);
    Components::VisualComponent* vis = std::get<Components::VisualComponent*>(ctuple);

    for (size_t i = 0; i<num; i++)
    {
        Components::ComponentMask mask = ents[i].m_ComponentMask;

        if((mask & Components::PhysicsComponent::MASK) != 0 && !phys[i].m_IsStatic)
        {
            // Copy to position-component
            pos[i].m_WorldMatrix = Components::Actions::Physics::getRigidBodyTransform(phys[i]);

            // Broadcast to others
            if((mask & Components::LogicComponent::MASK) != 0 && log[i].m_pLogicController)
                log[i].m_pLogicController->onTransformChanged();

            if((mask & Components::VisualComponent::MASK) != 0 && vis[i].m_pVisualController)
                vis[i].m_pVisualController->onTransformChanged();
        }
    }
}

void PhysicsSystem::addRigidBody(btRigidBody *body)
{
    m_DynamicsWorld.addRigidBody(body);
}

void PhysicsSystem::removeRigidBody(btRigidBody *body)
{
    m_DynamicsWorld.removeRigidBody(body);
}

Handle::CollisionShapeHandle PhysicsSystem::makeCollisionShapeFromMesh(const Meshes::WorldStaticMesh &mesh, const std::string &name)
{
    if(m_ShapeCache.find(name) != m_ShapeCache.end())
        return m_ShapeCache[name];

    // Init collision
    btTriangleMesh* wm = new btTriangleMesh;

    for(size_t s=0;s<mesh.m_SubmeshStarts.size();s++)
    {
        if(!mesh.m_SubmeshMaterials[s].m_NoCollision)
        {
            for (size_t j = 0; j < mesh.m_SubmeshStarts[s].m_NumIndices; j += 3)
            {
                size_t i = mesh.m_SubmeshStarts[s].m_StartIndex + j;

                // TODO: Filter no-collision materials
                auto &v0 = mesh.m_Vertices[mesh.m_Indices[i]].Position;
                auto &v1 = mesh.m_Vertices[mesh.m_Indices[i + 1]].Position;
                auto &v2 = mesh.m_Vertices[mesh.m_Indices[i + 2]].Position;

                // Convert to btvector
                btVector3 v[] = {{v0.x, v0.y, v0.z},
                                 {v1.x, v1.y, v1.z},
                                 {v2.x, v2.y, v2.z}};
                wm->addTriangle(v[0], v[1], v[2]);
            }
        }
    }

    if(wm->getNumTriangles() == 0)
    {
        delete wm;
        return Handle::CollisionShapeHandle::makeInvalidHandle();
    }

    Handle::CollisionShapeHandle csh = m_CollisionShapeAllocator.createObject();
    CollisionShape& cs = getCollisionShape(csh);
    cs.collisionShape = new btBvhTriangleMeshShape(wm, true);
    cs.shapeType = CollisionShape::TriangleMesh;

    if(!name.empty())
        m_ShapeCache[name] = csh;

    return csh;
}

Handle::CollisionShapeHandle PhysicsSystem::makeCompoundCollisionShape(const std::string &name)
{
    if(m_ShapeCache.find(name) != m_ShapeCache.end())
        return m_ShapeCache[name];

    Handle::CollisionShapeHandle csh = m_CollisionShapeAllocator.createObject();
    CollisionShape& cs = getCollisionShape(csh);

    // TODO: Find out if "dynamicAABBTree" can be set to false for performance?
    cs.collisionShape = new btCompoundShape();
    cs.shapeType = CollisionShape::Compound;

    if(!name.empty())
        m_ShapeCache[name] = csh;

    return csh;
}

void PhysicsSystem::deleteCollisionShape(Handle::CollisionShapeHandle shape)
{
    // Remove from convex-cache
    for(auto& c : m_ShapeCache)
    {
        if(c.second == shape)
        {
            m_ShapeCache.erase(c.first);
            break;
        }
    }


}

Handle::CollisionShapeHandle PhysicsSystem::makeBoxCollisionShape(const Math::float3 &halfExtends)
{
    btBoxShape* s = new btBoxShape(btVector3(halfExtends.x, halfExtends.y, halfExtends.z));

    Handle::CollisionShapeHandle csh = m_CollisionShapeAllocator.createObject();
    CollisionShape& cs = getCollisionShape(csh);
    cs.collisionShape = s;
    cs.shapeType = CollisionShape::Box;

    return csh;
}

Math::float3 PhysicsSystem::raytrace(const Math::float3 &from, const Math::float3 &to)
{
    btVector3 btFrom(from.x, from.y, from.z);
    btVector3 btTo(to.x, to.y, to.z);
    btCollisionWorld::ClosestRayResultCallback res(btFrom, btTo);

    m_DynamicsWorld.rayTest(btFrom, btTo, res); // m_btWorld is btDiscreteDynamicsWorld

    return res.hasHit() ? Math::float3(res.m_hitPointWorld.m_floats) : to;
}

Handle::CollisionShapeHandle PhysicsSystem::makeConvexCollisionShapeFromMesh(const Meshes::WorldStaticMesh &mesh, const std::string &name)
{
    if(m_ShapeCache.find(name) != m_ShapeCache.end())
        return m_ShapeCache[name];


    // Init collision
    btTriangleMesh* wm = new btTriangleMesh;

    for(size_t s=0;s<mesh.m_SubmeshStarts.size();s++)
    {
        for(size_t j=0;j<mesh.m_SubmeshStarts[s].m_NumIndices;j+=3)
        {
            size_t i = mesh.m_SubmeshStarts[s].m_StartIndex + j;

            // TODO: Filter no-collision materials
            auto &v0 = mesh.m_Vertices[mesh.m_Indices[i]].Position;
            auto &v1 = mesh.m_Vertices[mesh.m_Indices[i + 1]].Position;
            auto &v2 = mesh.m_Vertices[mesh.m_Indices[i + 2]].Position;

            // Convert to btvector
            btVector3 v[] = {{v0.x, v0.y, v0.z},
                             {v1.x, v1.y, v1.z},
                             {v2.x, v2.y, v2.z}};
            wm->addTriangle(v[0], v[1], v[2]);
        }
    }

    btConvexShape* tmpShape = new btConvexTriangleMeshShape(wm);
    btShapeHull* hull = new btShapeHull(tmpShape);

    btScalar margin = tmpShape->getMargin();
    hull->buildHull(margin);
    tmpShape->setUserPointer(hull);

    btConvexHullShape* simplifiedConvexShape = new btConvexHullShape((btScalar*)hull->getVertexPointer(),hull->numVertices());

    Handle::CollisionShapeHandle csh = m_CollisionShapeAllocator.createObject();
    CollisionShape& cs = getCollisionShape(csh);
    cs.collisionShape = simplifiedConvexShape;
    cs.shapeType = CollisionShape::ConvexMesh;

    delete tmpShape;
    delete hull;

    if(!name.empty())
        m_ShapeCache[name] = csh;

    return csh;
}

Handle::PhysicsObjectHandle
PhysicsSystem::makeRigidBody(Handle::CollisionShapeHandle shape, const Math::Matrix &transform, float mass)
{
    Handle::PhysicsObjectHandle ph = m_PhysicsObjectAllocator.createObject();
    PhysicsObject& p = getPhysicsObject(ph);
    CollisionShape& s = getCollisionShape(shape);

    p.collisionShape = shape;
    p.rigidBody = new btRigidBody(mass, nullptr, s.collisionShape); // TODO: Implement motionstate and inertia

    // Add to physics-world
    m_DynamicsWorld.addRigidBody(p.rigidBody);

    return Handle::PhysicsObjectHandle();
}

void PhysicsSystem::compoundShapeAddChild(Handle::CollisionShapeHandle target, Handle::CollisionShapeHandle childShape, const Math::Matrix& localTransform)
{
    CollisionShape& ts = getCollisionShape(target);
    CollisionShape& cs = getCollisionShape(childShape);

    assert(ts.shapeType == CollisionShape::Compound);

    btCompoundShape* compShape = reinterpret_cast<btCompoundShape*>(ts.collisionShape);

    btTransform btr;
    btr.setFromOpenGLMatrix(localTransform.mv);
    compShape->addChildShape(btr,cs.collisionShape);
}


