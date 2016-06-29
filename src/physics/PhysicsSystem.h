#pragma once
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#include <content/StaticMeshAllocator.h>

namespace World
{
    class WorldInstance;
}

namespace Physics
{
    class PhysicsSystem
    {
        friend class RigidBody;
    public:
        PhysicsSystem(World::WorldInstance& world, float gravity = -9.1f);
        ~PhysicsSystem();

        /**
         * Updates the physics simulation
         * @param dt Time since last frame
         */
        void update(double dt);

        /**
         * Does some debug-drawing
         */
        void debugDraw();

        /**
         * @return the Dynamics world
         */
        btDiscreteDynamicsWorld* getDynamicsWorld(){ return &m_DynamicsWorld; }

        /**
         * Creates a new collisionshape from the given mesh
         * @param mesh Mesh to use as base
         * @return Static collision-shape using this mesh
         */
        CollisionShape* makeCollisionShapeFromMesh(const Meshes::WorldStaticMesh& mesh, const std::string &name = "");

        /**
         * Creates a box-like collision-shape
         * @param halfExtends Length from center to the sides of the box
         * @return Static collision-shape using the box
         */
        CollisionShape* makeBoxCollisionShape(const Math::float3& halfExtends);

        /**
         * Makes a convex hull from the given mesh
         * @param mesh Concave mesh to simplify
         * @param name Optional name to store this
         * @return Convex-hull
         */
        CollisionShape* makeConvexCollisionShapeFromMesh(const Meshes::WorldStaticMesh& mesh, const std::string& name);

        /**
         * Deletes a collisionshape from the cache
         */
        void deleteCollisionShape(CollisionShape* shape);

        /**
         * Does a simple raytrace
         * @param from Source
         * @param to Destination
         * @return Distance between source and destination. "to" if not hit
         */
        Math::float3 raytrace(const Math::float3& from, const Math::float3& to);
    private:

        /**
         * Adds an internal rigid-body to the system
         */
        void addRigidBody(btRigidBody* body);

        /**
         * Removes an internal rigid-body from the system
         */
        void removeRigidBody(btRigidBody* body);

        /**
         * Bullet engine
         */
        btDbvtBroadphase m_Broadphase;
        btDefaultCollisionConfiguration m_CollisionConfiguration;
        btCollisionDispatcher m_Dispatcher;
        btSequentialImpulseConstraintSolver m_Solver;
        btDiscreteDynamicsWorld m_DynamicsWorld;

        /**
         * Total list of collisionshapes we created
         */
        std::list<CollisionShape*> m_CollisionShapes;

        /**
         * Convex cache
         */
        std::unordered_map<std::string, CollisionShape*> m_ShapeCache;

        /**
         * World this is for
         */
        World::WorldInstance& m_World;
    };
}