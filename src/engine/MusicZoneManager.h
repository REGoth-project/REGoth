#pragma once

#include <vector>

#include <engine/World.h>

namespace Engine
{
    /**
     * @brief Handles transitions between music zones
     * 
     */
    class MusicZoneManager
    {
    public:
        MusicZoneManager(World::WorldInstance& world);

        /**
         * @brief Adds a zone to the zone list
         */
        void addZone(const ZenLoad::zCVobData& vob);

        /**
         * @brief Sets the world's default zone
         */
        void setDefaultZone(const std::string& zone) { m_defaultZonePrefix = zone; }
        void onUpdate();

    private:
        World::WorldInstance& m_world;
        std::vector<ZenLoad::zCVobData> m_zones;
        std::string m_defaultZonePrefix;
        
        enum EMusicTime {
            MT_Day = 0,
            MT_Ngt = 3,
        };

        enum EMusicType {
            MT_Std = 0,
            MT_Thr = 1,
            MT_Fgt = 2,
        };

        void playTheme(const std::string& prefix, EMusicType type, EMusicTime time);
    };
}