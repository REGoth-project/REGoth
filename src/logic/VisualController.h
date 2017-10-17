#pragma once
#include "Controller.h"
#include "visuals/VisualTypes.h"
#include <handle/HandleDef.h>

namespace Logic
{
    class VisualController : public Controller
    {
    public:
        /**
         * @param world World of the underlaying entity
         * @param entity Entity owning this controller
         */
        VisualController(World::WorldInstance& world, Handle::EntityHandle entity);
        virtual ~VisualController();

        /**
         * @return The type of this class. If you are adding a new base controller, be sure to add it to ControllerTypes.h
         */
        virtual EControllerType getControllerType() override { return EControllerType::VisualController; }
        /**
         * @return Classtype of this visual
         */
        virtual EVisualType getVisualType() { return EVisualType::Base; }
        /**
         * Loads the visual from the given name from the worlds VDFS
         * Note: The type must match this class
         * @param visual Filename of the visual. Uncompiled-version of the name!
         */
        virtual bool load(const std::string& visual)
        {
            m_Name = visual;
            return false;
        };

        /**
         * @brief Called when something else modified the transform of the underlaying entity
         */
        void onTransformChanged() override;

        /**
         * @return Entites created by this visual
         */
        const std::vector<Handle::EntityHandle>& getVisualEntities() { return m_VisualEntities; }
        /**
         * Updates the collision-hull depending on this visual
         */
        virtual void updateCollision(){};

        /**
         * @return The name of this visual
         */
        const std::string& getName() { return m_Name; }
        /**
         * Sets the shadow-value from the worldmesh for this visual-controller
         */
        virtual void setShadowValue(float shadow) {}
        /**
         * @return Classes which want to get exported on save should return true here
         */
        bool shouldExport() override { return !m_Transient; }
        /**
         * @param value Whether to save this visual when exporting a world
         */
        void setTransient(bool value) { m_Transient = value; }

    protected:
        void exportPart(json& j) override;

        /**
         * Entites of the visual registered here
         */
        std::vector<Handle::EntityHandle> m_VisualEntities;

        /**
         * Name of the loaded visual
         */
        std::string m_Name;

        /**
         * Whether to put this visual into a savegame
         */
        bool m_Transient;
    };
}
