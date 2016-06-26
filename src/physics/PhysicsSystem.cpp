//
// Created by desktop on 26.06.16.
//

#include <components/EntityActions.h>
#include "PhysicsSystem.h"
#include <logic/VisualController.h>
#include <logic/Controller.h>
#include "DebugDrawer.h"
#include <engine/World.h>

using namespace Physics;

const int NUM_MAX_SUB_STEPS = 3;

PhysicsSystem::PhysicsSystem(float gravity)
    : m_Dispatcher(&m_CollisionConfiguration),
      m_DynamicsWorld(&m_Dispatcher, &m_Broadphase, &m_Solver, &m_CollisionConfiguration)
{
    btGImpactCollisionAlgorithm::registerAlgorithm(&m_Dispatcher);
    m_DynamicsWorld.setGravity(btVector3(0, gravity, 0));

    m_DynamicsWorld.setDebugDrawer(new DebugDrawer);
    m_DynamicsWorld.getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
}

PhysicsSystem::~PhysicsSystem()
{
    delete m_DynamicsWorld.getDebugDrawer();
}

void PhysicsSystem::update(World::WorldInstance& world, double dt)
{
    m_DynamicsWorld.stepSimulation(static_cast<btScalar>(dt), NUM_MAX_SUB_STEPS);

    const auto& ctuple = world.getComponentDataBundle().m_Data;
    size_t num = world.getComponentDataBundle().m_NumElements;

    // Copy all physics-transforms to the position-components
    Components::EntityComponent* ents = std::get<Components::EntityComponent*>(ctuple);
    Components::PhysicsComponent* phys = std::get<Components::PhysicsComponent*>(ctuple);
    Components::PositionComponent* pos = std::get<Components::PositionComponent*>(ctuple);
    Components::LogicComponent* log = std::get<Components::LogicComponent*>(ctuple);
    Components::VisualComponent* vis = std::get<Components::VisualComponent*>(ctuple);

    for (size_t i = 0; i<num; i++)
    {
        Components::ComponentMask mask = ents[i].m_ComponentMask;

        if((mask & Components::PhysicsComponent::MASK) != 0)
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

