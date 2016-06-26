#pragma once
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>


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
        PhysicsSystem(float gravity = -9.1f);
        ~PhysicsSystem();

        /**
         * Updates the physics simulation
         * @param dt Time since last frame
         */
        void update(World::WorldInstance& world, double dt);

        /**
         * @return the Dynamics world
         */
        btDiscreteDynamicsWorld* getDynamicsWorld(){ return &m_DynamicsWorld; }

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
    };
}