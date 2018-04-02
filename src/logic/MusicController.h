#pragma once
#include <array>
#include "Controller.h"
#include "LogicDef.h"
#include <daedalus/DaedalusGameState.h>

namespace Logic {
    class MusicController : public Controller {
    public:
        /**
        * @param world World of the underlaying entity
        * @param entity Entity owning this controller
        */
        MusicController(World::WorldInstance& world, Handle::EntityHandle entity);

        /**
        * Called on game-tick
        */
        void onUpdate(float deltaTime) override;

        /**
        * Initializes this controller with data loaded from a .ZEN-file
        * @param vob Data loaded from the ZEN
        */
        void initFromVobDescriptor(const ZenLoad::zCVobData& vob);

        /**
        * @return The type of this class. If you are adding a new base controller, be sure to add it to ControllerTypes.h
        */
        EControllerType getControllerType() override { return EControllerType::MusicController; }

        /**
        * @return Classes which want to get exported on save should return true here
        */
        bool shouldExport() override { return false; }
        void importObject(const json& j) override;

    protected:
        /**
        * @return True, if the current camera is in the bounding box of the zone
        */
        bool isInBoundingBox();

        void exportPart(json& j) override;

    private:

        std::array<ZMath::float3, 2> m_bbox;

        bool m_isPlaying;

        std::string m_zoneName, m_instancePrefix;

        enum EMusicTime {
            MT_Day = 0,
            MT_Ngt = 3,
        };

        enum EMusicType {
            MT_Std = 0,
            MT_Thr = 1,
            MT_Fgt = 2,
        };

        static const std::array<const std::string, 6> m_instanceSuffixes;
    };
}
