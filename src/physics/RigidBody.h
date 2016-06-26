#pragma once
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#include "CollisionShape.h"
#include "MotionState.h"
#include "PhysicsSystem.h"

namespace Physics
{
    class RigidBody
    {
    public:
        RigidBody() noexcept :
                m_pDynamicsWorld(nullptr),
                m_pRigidBody(nullptr)
        {
        }

        RigidBody(const RigidBody &) noexcept :
                m_pDynamicsWorld(nullptr),
                m_pRigidBody(nullptr)
        {
        }

        RigidBody(RigidBody &&other) noexcept :
                m_pDynamicsWorld(other.m_pDynamicsWorld),
                m_pRigidBody(other.m_pRigidBody)
        {
            other.invalidate();
        }

        RigidBody &operator=(RigidBody &&other) noexcept
        {
            m_pDynamicsWorld = other.m_pDynamicsWorld;
            m_pRigidBody = other.m_pRigidBody;

            other.invalidate();
            return *this;
        }

        ~RigidBody() noexcept
        {
            cleanUp();
        }

        void invalidate()
        {
            m_pDynamicsWorld = nullptr;
            m_pRigidBody = nullptr;
        }

        /**
         * Creates this rigidbody
         * @param pPhysicsSystem Physics-system this should be created in
         * @param collisionShape Collisionshape to use
         * @param mass Mass of this object
         * @return Whether the initialization was successful
         */
        bool initPhysics(PhysicsSystem *pPhysicsSystem, CollisionShape &collisionShape, float mass = 0)
        {
            if(m_pRigidBody)
                return false;

            // Create motionstate
            btMotionState *pMotionState = new MotionState();

            // Use mass to calculate the inertia
            btVector3 inertia;
            if(mass)
                collisionShape.shape()->calculateLocalInertia(mass, inertia);

            // Construct the actual rigidbody
            m_pRigidBody = new btRigidBody(mass, pMotionState, collisionShape.shape(), inertia);

            // Add the body to the physicssystem
            pPhysicsSystem->addRigidBody(m_pRigidBody);
            m_pDynamicsWorld = pPhysicsSystem->getDynamicsWorld();

            return true;
        }

        /**
         * Deletes the memory used by this rigidbody
         */
        void cleanUp()
        {
            if(m_pRigidBody && m_pDynamicsWorld)
            {
                m_pDynamicsWorld->removeRigidBody(m_pRigidBody);
                delete m_pRigidBody->getMotionState();
                delete m_pRigidBody;
                m_pRigidBody = nullptr;
                m_pDynamicsWorld = nullptr;
            }
        }

        /**
         * Behavior-related functions
         */
        void setFriction(float friction)
        {
            m_pRigidBody->setFriction(friction);
        }

        void setRestitution(float restitution)
        {
            m_pRigidBody->setRestitution(restitution);
        }

        void applyCentralImpulse(float x, float y, float z)
        {
            m_pRigidBody->applyCentralImpulse(btVector3(x, y, z));
        }

        void applyCentralImpulse(const Math::float3 &impulse)
        {
            m_pRigidBody->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));
        }

        /**
         * @return The motionstate, which holds the transform of this body
         */
        MotionState *getMotionState()
        {
            return reinterpret_cast<MotionState*>(m_pRigidBody->getMotionState());
        }

    private:
        btDiscreteDynamicsWorld *m_pDynamicsWorld;
        btRigidBody *m_pRigidBody;
    };
}