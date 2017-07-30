#pragma once
#include <components/Entities.h>
#include <engine/World.h>
#include <logic/VisualController.h>

namespace Logic
{
    class ObjectController;
    class VisualController;
}

namespace World
{
    class WorldInstance;
}

namespace Vob
{
    struct VobInformation
    {
        Logic::Controller* logic;
        Logic::VisualController* visual;
        Components::ObjectComponent* object;
        Components::PositionComponent* position;
        World::WorldInstance* world;
        Handle::EntityHandle entity;

        bool isValid()
        {
            return entity.isValid();
        }
    };

    /**
     * Adds a new vob to the given world
     */
    Handle::EntityHandle constructVob(World::WorldInstance& world);

    /**
     * Extracts the vob-information from the given entity
     * NOTE: ONLY FOR TEMPORARY USE. DO NOT SAVE THE RETURNED OBJECT FOR LATER USE.
     */
    VobInformation asVob(World::WorldInstance& world, Handle::EntityHandle e);

    /**
     * Sets the position on the given vob
     */
    void setPosition(VobInformation& vob, const Math::float3& position);

    /**
     * Sets the whole transform-matrix on the given vob
     */
    void setTransform(VobInformation& vob, const Math::Matrix& transform);

    /**
     * @return Transform of the given vob
     */
    const Math::Matrix& getTransform(VobInformation& vob);

    /**
     * @return The world this vob is in
     */
    World::WorldInstance& getWorld(VobInformation& vob);

    /**
     * @return Visual of this vob
     */
    Logic::VisualController* getVisual(VobInformation& vob);

    /**
     * @return Name of the vob
     */
    std::string getName(VobInformation& vob);

    /**
     * Sets the visual for the given vob to use
     */
    void setVisual(VobInformation& vob, const std::string& visual);

    /**
     * Sets the name of the given vob
     */
    void setName(VobInformation& vob, const std::string& name);

    /**
     * Sets the BBox of the given vob
     */
    void setBBox(VobInformation& vob, const Math::float3& min, const Math::float3& max, uint32_t debugColor = 0xFFFFFFFF);

    /**
     * Sets/Gets whether to enable collision on this vob
     */
    void setCollisionEnabled(VobInformation& vob, bool value);
    bool getCollisionEnabled(VobInformation& vob);

    /**
     * Notifies all controllers about a transform-change on the given vob
     */
    void broadcastTransformChange(VobInformation& vob);
}