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
    struct PhysicsObject : public Handle::HandleTypeDescriptor<Handle::PhysicsObjectHandle>
    {
        btRigidBody* rigidBody;
        Handle::CollisionShapeHandle collisionShape;

        static void clean(PhysicsObject& s)
        {
            delete s.rigidBody;
        }
    };

    struct CollisionShape : public Handle::HandleTypeDescriptor<Handle::CollisionShapeHandle>
    {
        enum EShapeType
        {
            Box,
            TriangleMesh,
            Compound,
            ConvexMesh,
            Other
        };

        enum ECollisionType
        {
            CT_Any = -1,
            CT_WorldMesh = (1 << 1),
            CT_Object = (1 << 2),
        };

        btCollisionShape* collisionShape;
        EShapeType shapeType;
        ECollisionType collisionType;

        static void clean(CollisionShape& s)
        {
            delete s.collisionShape;
        }
    };

    struct RayTestResult
    {
        /**
         * Whether this result contains valid results
         */
        bool hasHit;

        /**
         * @brief Position of where the ray hit
         */
        Math::float3 hitPosition;

        /**
         * @brief Index of the trianlge the ray has potentially hit
         */
        uint32_t hitTriangleIndex;

        /**
         * @brief Userflags set in the hit rigidbody
         */
        uint32_t hitFlags;

        /**
         * @brief Hit collision shape
         */
        Handle::CollisionShapeHandle hitCollisionShape;

        /**
         * @brief Hit rigidbody
         */
        Handle::PhysicsObjectHandle hitPhysicsObject;
    };

    /**
     * Default allocator-type
     */
    typedef Memory::StaticReferencedAllocator<PhysicsObject, Config::MAX_NUM_LEVEL_ENTITIES> PhysicsObjectAllocator;
    typedef Memory::StaticReferencedAllocator<CollisionShape, Config::MAX_NUM_LEVEL_ENTITIES> CollisionShapeAllocator;

    class PhysicsSystem
    {
        friend class RigidBody;

    public:
        PhysicsSystem(World::WorldInstance& world, float gravity = -9.1f);
        ~PhysicsSystem();

        /**
         * Post-processing after loading. This will make sure all bodies are in place and usable without running
         * a single fake-step
         */
        void postProcessLoad();

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
        btDiscreteDynamicsWorld* getDynamicsWorld() { return m_pDynamicsWorld; }
        /**
         * Creates a new collisionshape from the given mesh
         * @param mesh Mesh to use as base
         * @return Static collision-shape using this mesh
         */
        Handle::CollisionShapeHandle makeCollisionShapeFromMesh(const Meshes::WorldStaticMesh& mesh, CollisionShape::ECollisionType type = CollisionShape::CT_Any, const std::string& name = "");
        Handle::CollisionShapeHandle makeCollisionShapeFromMesh(const std::vector<ZenLoad::WorldTriangle>& triangles, CollisionShape::ECollisionType type = CollisionShape::CT_Any, const std::string& name = "");

        /**
         * Creates a box-like collision-shape
         * @param halfExtends Length from center to the sides of the box
         * @return Static collision-shape using the box
         */
        Handle::CollisionShapeHandle makeBoxCollisionShape(const Math::float3& halfExtends);

        /**
         * Makes a convex hull from the given mesh
         * @param mesh Concave mesh to simplify
         * @param name Optional name to store this
         * @return Convex-hull
         */
        Handle::CollisionShapeHandle makeConvexCollisionShapeFromMesh(const Meshes::WorldStaticMesh& mesh, const std::string& name);

        /**
         * Creates a compound-shape, being able to store multiple sub-shapes inside
         * @param name Cache-name of this shape
         * @return Handle to the created shape
         */
        Handle::CollisionShapeHandle makeCompoundCollisionShape(CollisionShape::ECollisionType type = CollisionShape::CT_Any, const std::string& name = "");

        /**
         * Creates a new rigid-body using the given collision-shape
         * @param shape Shape to use for collision
         * @param mass Mass of the object. A value of 0 means, that this object should be static.
         * @param transform Place to put the object.
         * @return Handle to the created object
         */
        Handle::PhysicsObjectHandle makeRigidBody(Handle::CollisionShapeHandle shape, const Math::Matrix& transform, float mass = 0.0f);

        /**
         * Adds the specified child-shape to the given compound-shape
         * @param target Target compound-shape to add the child to
         * @param childShape Child to add to the compound-shape
         * @param localTransform Transform relative to the origin of the compound-mesh
         */
        void compoundShapeAddChild(Handle::CollisionShapeHandle target, Handle::CollisionShapeHandle childShape, const Math::Matrix& localTransform = Math::Matrix::CreateIdentity());

        /**
         * Deletes a collisionshape from the cache
         */
        void deleteCollisionShape(Handle::CollisionShapeHandle shape);

        /**
         * Does a simple raytrace
         * @param from Source
         * @param to Destination
         * @return Distance between source and destination. "to" if not hit
         */
        RayTestResult raytrace(const Math::float3& from, const Math::float3& to, CollisionShape::ECollisionType filtertype = CollisionShape::CT_Any);
        /**
         * Does a simple multi-raytrace
         * @param from Source
         * @param to Destination
         * @return Vector of all RayTestResults assosiated with colissionshapes hit by the ray
         */
        std::vector<RayTestResult> raytraceAll(const Math::float3& from, const Math::float3& to, CollisionShape::ECollisionType filtertype = CollisionShape::CT_Any);

        /**
         * @return Physics-object of the given handle
         */
        PhysicsObject& getPhysicsObject(Handle::PhysicsObjectHandle h)
        {
            return m_PhysicsObjectAllocator.getElement(h);
        }

        /**
         * @return CollsionShape of the given handle
         */
        CollisionShape& getCollisionShape(Handle::CollisionShapeHandle h)
        {
            return m_CollisionShapeAllocator.getElement(h);
        }

    private:
        /**
         * Adds an internal rigid-body to the system
         */
        void addRigidBody(btRigidBody* body);

        /**
         * Adds a static collision-shape
         */
        void addStaticCollisionShape(std::string type, btCollisionShape* shape, const Math::Matrix& transform);

        /**
         * Removes a static collision-shape
         */
        void removeStaticCollisionShape(std::string type, btCollisionShape* shape);

        /**
         * Removes an internal rigid-body from the system
         */
        void removeRigidBody(btRigidBody* body);

        /**
         * Bullet engine
         */
        btDbvtBroadphase* m_pBroadphase;
        btDefaultCollisionConfiguration* m_pCollisionConfiguration;
        btCollisionDispatcher* m_pDispatcher;
        btSequentialImpulseConstraintSolver* m_pSolver;
        btDiscreteDynamicsWorld* m_pDynamicsWorld;
        btSortedOverlappingPairCache* m_pPairCache;

        /**
         * World this is for
         */
        World::WorldInstance& m_World;

        /**
         * Allocator for the rigid-bodies
         */
        PhysicsObjectAllocator m_PhysicsObjectAllocator;
        CollisionShapeAllocator m_CollisionShapeAllocator;

        /**
         * Map of collision-shapes by name
         */
        std::unordered_map<std::string, Handle::CollisionShapeHandle> m_ShapeCache;
    };
}
