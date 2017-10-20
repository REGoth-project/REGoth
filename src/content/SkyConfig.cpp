#include <utils/Utils.h>
#include "SkyConfig.h"
#include <json/json.hpp>
#include <ZenLib/utils/logger.h>

using namespace Content;
using json = nlohmann::json;

/**
 * Name of the colorset in the config to use as default
 */
const std::string DEFAULT_COLORSET_NAME = "default";

/**
 * Colorset selected when an invalid set has been requested
 */
const SkyConfig::SkyDayColorSet NOTIFY_INVALID_SET = {
        "INVALID CONFIG",
        {
                Math::float3(1.0f, 0.0f, 0.0f),
                Math::float3(1.0f, 0.0f, 0.0f),
                Math::float3(1.0f, 0.0f, 0.0f),
        }
};

static SkyConfig::SkyDayColorSet importColorSetFromJson(const std::string& world, const json& j);

SkyConfig::SkyConfig(const std::string& jsonString, SkyGametype gametype)
    : m_GameType(gametype)
{
    loadFromJson(jsonString);
}


SkyConfig::~SkyConfig()
{

}


void SkyConfig::addColorSet(const SkyConfig::SkyDayColorSet& set)
{
    m_SkyDayColorsPerWorld[Utils::uppered(set.world)] = set;
}


bool SkyConfig::isValid() const
{
    if(m_SkyDayColorsPerWorld.empty())
        return false;

    if(!hasExplicitColorSetFor(DEFAULT_COLORSET_NAME))
        return false;

    return true;
}


const SkyConfig::SkyDayColorSet& SkyConfig::findColorSetOfWorld(const std::string& world) const
{
    if(!isValid())
        return NOTIFY_INVALID_SET;

    if(hasExplicitColorSetFor(world))
        return getExplicitColorSetFor(world);

    return getDefaultColorSet();
}


bool SkyConfig::hasExplicitColorSetFor(const std::string& world) const
{
    auto it = m_SkyDayColorsPerWorld.find(Utils::uppered(world));

    return it != m_SkyDayColorsPerWorld.end();
}


const SkyConfig::SkyDayColorSet& SkyConfig::getExplicitColorSetFor(const std::string& world) const
{
    if(!hasExplicitColorSetFor(world))
        return NOTIFY_INVALID_SET;

    return m_SkyDayColorsPerWorld.at(Utils::uppered(world));
}


const SkyConfig::SkyDayColorSet& SkyConfig::getDefaultColorSet() const
{
    return getExplicitColorSetFor("default");
}


bool SkyConfig::loadFromJson(const std::string& jsonString)
{
    try
    {
        std::string jsonWithoutComments = Utils::stripJsonComments(jsonString);
        json j = json::parse(jsonWithoutComments);

        std::map<std::string, json> configByGametype;
        for(auto it = j.begin(); it != j.end(); it++)
        {
            configByGametype[Utils::uppered(it.key())] = it.value();
        }

        if(configByGametype.empty())
        {
            LogWarn() << "Did not find gametypes inside skyconfig!";
            return false;
        }

        for(const auto& p : configByGametype)
        {
            if(doesStringMatchGametype(p.first, m_GameType))
            {
                for(auto it = p.second.begin(); it != p.second.end(); it++)
                {
                    const std::string& worldname = it.key();
                    const json& jsonset = it.value();

                    SkyDayColorSet set = importColorSetFromJson(worldname, jsonset);
                    addColorSet(set);
                }

                return true; // Only need values for the current gametype
            }
        }

        LogWarn() << "Did not find the right gametype matching type-# " << (int)m_GameType << "in sky-config!";
        return false; // Did not find the matching gametype
    }
    catch(const std::invalid_argument& e)
    {
        LogError() << "Failed to parse Sky-config: " << e.what();
        return false;
    }

}


bool SkyConfig::doesStringMatchGametype(const std::string& str, SkyConfig::SkyGametype gametype) const
{
    switch(gametype)
    {
        case SkyGametype::Gothic1:
            return Utils::uppered(str) == "GOTHIC-1";

        case SkyGametype::Gothic2:
            return Utils::uppered(str) == "GOTHIC-2";

        default:
            return false;
    }
}


static SkyConfig::SkyDayColorSet importColorSetFromJson(const std::string& world, const json& j)
{
    SkyConfig::SkyDayColorSet set;
    set.world = world;

    std::vector<std::string> keys = {"DayColor0", "DayColor1", "DayColor2", "DayColor3"};

    for(size_t i = 0; i < keys.size(); i++)
    {
        int r = j[keys[i]]["r"];
        int g = j[keys[i]]["g"];
        int b = j[keys[i]]["b"];

        set.colors[i] = Math::float3(r / 255.0f, g / 255.0f, b / 255.0f);
    }
    return set;
}
