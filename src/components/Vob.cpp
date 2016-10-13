//
// Created by andre on 02.06.16.
//

#include "Vob.h"
#include <logic/Controller.h>
#include <logic/VisualController.h>
#include <logic/visuals/StaticMeshVisual.h>
#include <engine/World.h>
#include <components/EntityActions.h>
#include <logic/visuals/ModelVisual.h>
#include <utils/logger.h>

Handle::EntityHandle Vob::constructVob(World::WorldInstance& world)
{
    // Create main entity
    Handle::EntityHandle e = world.addEntity();
    Components::EntityComponent& entity = world.getEntity<Components::EntityComponent>(e);

    // Add components
    Components::addComponent<Components::LogicComponent>(entity);
    Components::Actions::initComponent<Components::LogicComponent>(world.getComponentAllocator(), e);

    Components::addComponent<Components::VisualComponent>(entity);
    Components::Actions::initComponent<Components::VisualComponent>(world.getComponentAllocator(), e);

    Components::addComponent<Components::BBoxComponent>(entity);
    Components::Actions::initComponent<Components::BBoxComponent>(world.getComponentAllocator(), e);

    Components::addComponent<Components::ObjectComponent>(entity);
    Components::ObjectComponent& obj = Components::Actions::initComponent<Components::ObjectComponent>(world.getComponentAllocator(), e);
    obj.m_Type = Components::ObjectComponent::Other;

    Components::addComponent<Components::PositionComponent>(entity);
    Components::Actions::initComponent<Components::PositionComponent>(world.getComponentAllocator(), e);

    //Components::addComponent<Components::PhysicsComponent>(entity);
    //Components::Actions::initComponent<Components::PhysicsComponent>(world.getComponentAllocator(), e);

    return e;
}

Vob::VobInformation Vob::asVob(World::WorldInstance& world, Handle::EntityHandle e)
{
    VobInformation info;
    Components::ComponentAllocator& alloc = world.getComponentAllocator();
    Components::EntityComponent entity = alloc.getElement<Components::EntityComponent>(e);

    info.entity = e;
    info.world = &world;

    if(Components::hasComponent<Components::LogicComponent>(entity))
        info.logic = alloc.getElement<Components::LogicComponent>(e).m_pLogicController;
    else
        info.logic = nullptr;

    if(Components::hasComponent<Components::VisualComponent>(entity))
        info.visual = alloc.getElement<Components::VisualComponent>(e).m_pVisualController;
    else
        info.visual = nullptr;

    if(Components::hasComponent<Components::ObjectComponent>(entity))
        info.object = &alloc.getElement<Components::ObjectComponent>(e);
    else
        info.object = nullptr;

    if(Components::hasComponent<Components::PositionComponent>(entity))
        info.position = &alloc.getElement<Components::PositionComponent>(e);
    else
        info.position = nullptr;



    return info;
}

void ::Vob::setPosition(VobInformation& vob, const Math::float3& position)
{
    vob.position->m_WorldMatrix.Translation(position);

    broadcastTransformChange(vob);
}

void ::Vob::setTransform(VobInformation& vob, const Math::Matrix& transform)
{
    vob.position->m_WorldMatrix = transform;

    broadcastTransformChange(vob);
}

void ::Vob::broadcastTransformChange(VobInformation& vob)
{
    if(vob.logic)
        vob.logic->onTransformChanged();

    if(vob.visual)
        vob.visual->onTransformChanged();
}

void ::Vob::setVisual(VobInformation& vob, const std::string& _visual)
{
    std::string visual = _visual;
    std::transform(visual.begin(), visual.end(), visual.begin(), ::toupper);

    // Don't set twice
    if(vob.visual && vob.visual->getName() == visual)
        return;

    // Clear old visual
    delete vob.visual;

    Logic::VisualController** ppVisual
            = &vob.world->getComponentAllocator().getElement<Components::VisualComponent>(vob.entity).m_pVisualController;

    vob.visual = nullptr;
    *ppVisual = nullptr;



    // Check type of visual
    if(visual.find(".3DS") != std::string::npos
       || visual.find(".MMB") != std::string::npos
		|| visual.find(".MMS") != std::string::npos
		|| visual.find(".MDMS") != std::string::npos)
    {
        Logic::VisualController* ld = new Logic::StaticMeshVisual(*vob.world, vob.entity);
        if(ld->load(visual))
            (*ppVisual) = ld;
        else
            delete ld;
    }else if(visual.find(".MDM") != std::string::npos
             || visual.find(".MDL") != std::string::npos
             || visual.find(".MDS") != std::string::npos
             || visual.find(".ASC") != std::string::npos)
    {
        Logic::VisualController* ld = new Logic::ModelVisual(*vob.world, vob.entity);
        if(ld->load(visual))
        {
            (*ppVisual) = ld;
        }
        else
            delete ld;
    }

    // Notify the logic controller
    if(vob.visual != (*ppVisual))
    {
        if(vob.logic)
            vob.logic->onVisualChanged();
    }

    vob.visual = (*ppVisual);
}

void ::Vob::setName(VobInformation& vob, const std::string& name)
{
    if(vob.object)
        vob.object->m_Name = name;
}

void ::Vob::setBBox(VobInformation& vob, const Math::float3& min, const Math::float3& max, uint32_t debugColor)
{
    vob.world->getComponentAllocator().getElement<Components::BBoxComponent>(vob.entity).m_BBox3D.min = min;
    vob.world->getComponentAllocator().getElement<Components::BBoxComponent>(vob.entity).m_BBox3D.max = max;
    vob.world->getComponentAllocator().getElement<Components::BBoxComponent>(vob.entity).m_DebugColor = debugColor;
}

const Math::Matrix &::Vob::getTransform(Vob::VobInformation &vob)
{
    return vob.position->m_WorldMatrix;
}

World::WorldInstance &::Vob::getWorld(Vob::VobInformation &vob)
{
    return *vob.world;
}

Logic::VisualController *::Vob::getVisual(Vob::VobInformation &vob)
{
    return vob.visual;
}

std::string Vob::getName(Vob::VobInformation& vob)
{
    if(vob.object)
        return vob.object->m_Name;

    return "";
}

void ::Vob::setCollisionEnabled(VobInformation& vob, bool value)
{
    if(vob.object)
        vob.object->m_EnableCollision = value;
}

bool ::Vob::getCollisionEnabled(VobInformation& vob)
{
    if(vob.object)
        return vob.object->m_EnableCollision;

    return false;
}




















