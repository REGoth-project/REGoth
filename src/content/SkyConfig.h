#pragma once
#include <string>
#include <math/mathlib.h>
#include <array>
#include <map>

namespace Content
{
    class SkyConfig
    {
    public:

        enum class SkyGametype
        {
            Gothic1,
            Gothic2
        };

        struct SkyDayColorSet
        {
            std::string world;
            std::array<Math::float3, 4> colors;
        };

        /**
         * Initializes this config from a json-string ("content/sky.json").
         * Check with "isValid()" on whether it contains useful information.
         *
         * @param json Contents of the json-file.
         * @param gametype Which game we're currently in
         */
        SkyConfig(const std::string& jsonString, SkyGametype gametype);
        ~SkyConfig();

        /**
         * @return Whether the loaded config was valid and this contains useful data
         */
        bool isValid() const;

        /**
         * @param world Zen-name (without extension) to get the color-set for.
         * @return Colorset matching the given name best
         *         If an invalid config was loaded, this contains a special color-set.
         */
        const SkyDayColorSet& findColorSetOfWorld(const std::string& world) const;


    private:

        bool hasExplicitColorSetFor(const std::string& world) const;
        const SkyDayColorSet& getExplicitColorSetFor(const std::string& world) const;

        void addColorSet(const SkyDayColorSet& set);
        const SkyDayColorSet& getDefaultColorSet() const;

        bool loadFromJson(const std::string& jsonString);

        bool doesStringMatchGametype(const std::string& str, SkyGametype gametype) const;

        std::map<std::string, SkyDayColorSet> m_SkyDayColorsPerWorld;
        SkyGametype m_GameType;
    };
}


