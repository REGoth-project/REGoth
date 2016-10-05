#pragma once
#include "Controller.h"
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
        virtual EControllerType getControllerType(){ return EControllerType::VisualController; }

        /**
         * Loads the visual from the given name from the worlds VDFS
         * Note: The type must match this class
         * @param visual Filename of the visual. Uncompiled-version of the name!
         */
        virtual bool load(const std::string& visual){ m_Name = visual; return false;};

        /**
         * @brief Called when something else modified the transform of the underlaying entity
         */
        virtual void onTransformChanged();

        /**
         * @return Entites created by this visual
         */
        const std::vector<Handle::EntityHandle>& getVisualEntities(){ return m_VisualEntities; }

        /**
         * Updates the collision-hull depending on this visual
         */
        virtual void updateCollision(){};

		/**
		 * @return The name of this visual
		 */
		const std::string& getName(){ return m_Name; }

        /**
         * Sets the shadow-value from the worldmesh for this visual-controller
         */
        virtual void setShadowValue(float shadow){}
    protected:

        /**
         * Entites of the visual registered here
         */
        std::vector<Handle::EntityHandle> m_VisualEntities;

        /**
         * Name of the loaded visual
         */
        std::string m_Name;
    };
}
