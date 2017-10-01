//
// Created by desktop on 07.08.16.
//

#include "Sky.h"
#include <cstdint>
#include <iostream>
#include "engine/Input.h"
#include <engine/GameEngine.h>
#include <engine/GameSession.h>
#include <engine/World.h>
#include <entry/input.h>
#include <utils/Utils.h>
#include <utils/cli.h>
#include <utils/logger.h>

using namespace Content;

const float TIME_KEY_0 = 0.00f;
const float TIME_KEY_1 = 0.25f;
const float TIME_KEY_2 = 0.30f;
const float TIME_KEY_3 = 0.35f;
const float TIME_KEY_4 = 0.50f;
const float TIME_KEY_5 = 0.65f;
const float TIME_KEY_6 = 0.70f;
const float TIME_KEY_7 = 0.75f;

namespace Flags
{
    Cli::Flag skyType("s", "sky", 1, "Selects the sky to render. Possible options: auto, g1, g2", {"auto"}, "Game");
}

Sky::Sky(World::WorldInstance& world)
    : m_World(world)
{
    m_MasterState.time = 0.5f;
    m_FarPlane = FLT_MAX;

    fillSkyStates();
}

Sky::~Sky()
{
}

void Sky::calculateLUT_ZenGin(const Math::float3& col0, const Math::float3& col1, Math::float4* pLut)
{
    Math::float3 delta = col1 - col0;

    // Put base color into upper half
    uint32_t ic[] = {Utils::round<uint32_t>(255.0f * col0.x) << 16,
                     Utils::round<uint32_t>(255.0f * col0.y) << 16,
                     Utils::round<uint32_t>(255.0f * col0.z) << 16};

    // Inflate color-delta to the lower 16-bits
    uint32_t idelta[] = {Utils::round<uint32_t>(delta.x * 65535),
                         Utils::round<uint32_t>(delta.y * 65535),
                         Utils::round<uint32_t>(delta.z * 65535)};

    for (int i = 0; i < 256; i++)
    {
        // Magic?
        ic[0] += idelta[0];
        ic[1] += idelta[1];
        ic[2] += idelta[2];

        // Plug the color-bytes together
        /*uint32_t final =  (((ic[0] >> 16) << 24) &      0xFF000000) // r
                          + (((ic[1] >> 16) << 16) &    0x00FF0000) // g
                          + (((ic[2] >> 16) << 8) &     0x0000FF00)  // b
                          + 0xFF;                                 // a
         */

        pLut[i].x = (ic[0] >> 16) / 255.0f;
        pLut[i].y = (ic[1] >> 16) / 255.0f;
        pLut[i].z = (ic[2] >> 16) / 255.0f;
        pLut[i].w = 1.0f;
    }
}

void Sky::interpolate()
{
    // time since 12:00 in days
    double skyTime = std::fmod(m_World.getEngine()->getGameClock().getTimeOfDay() + 0.5, 1.0);
    m_MasterState.time = static_cast<float>(skyTime);

    // init with values for case: time >= TIME_KEY_7 (= 0.75f)
    size_t si0 = ESPT_NUM_PRESETS - 1, si1 = 0;

    // Find the two states we're interpolating
    for (size_t i = 0; i < ESkyPresetType::ESPT_NUM_PRESETS; i++)
    {
        // Interpolate between the current and the next
        if (m_MasterState.time < m_SkyStates[i].time)
        {
            si0 = (i - 1) % ESkyPresetType::ESPT_NUM_PRESETS;
            si1 = (si0 + 1) % ESkyPresetType::ESPT_NUM_PRESETS;
            break;
        }
    }

    SkyState& s0 = m_SkyStates[si0];
    SkyState& s1 = m_SkyStates[si1];

    // Handle case time >= TIME_KEY_7 (= 0.75f)
    float timeS1 = s1.time < s0.time ? s1.time + 1.0f : s1.time;

    // Scale up time difference to [0,1]
    float t = (m_MasterState.time - s0.time) / (timeS1 - s0.time);

    /*
    static size_t last_si0 = si0;
    if (last_si0 != si0)
    {
        last_si0 = si0;
        LogInfo() << "state = " << si0 << ", clock = " << m_World.getWorldInfo().getDateTimeFormatted();
    }
     */

    // Interpolate values
    m_MasterState.baseColor = s0.baseColor + t * (s1.baseColor - s0.baseColor);
    m_MasterState.fogColor = s0.fogColor + t * (s1.fogColor - s0.fogColor);
    m_MasterState.fogDistance = s0.fogDistance + t * (s1.fogDistance - s0.fogDistance);
    m_MasterState.domeColorUpper = s0.domeColorUpper + t * (s1.domeColorUpper - s0.domeColorUpper);

    // FIXME: There is some stuff about levelchanges here. Like, "turn off rain when on dragonisland"

    // FIXME: Multiply fogColor with 0.8 when using dome!

    // Calculate LUT based on our interpolated colors
    calculateLUT_ZenGin(Math::float3(0, 0, 0), m_MasterState.baseColor, m_LUT);

    // TODO: Fix up the textures here
}

void Sky::initSkyState(World::WorldInstance& world, ESkyPresetType type, Sky::SkyState& s, Textures::TextureAllocator& texAlloc)
{
    Math::float3 skyColor_g1 = Math::float3(114, 93, 82) / 255.0f;    // G1
    Math::float3 skyColor_g2 = Math::float3(120, 140, 180) / 255.0f;  // G2

    Math::float3 skyColor = skyColor_g2;

    if (Flags::skyType.isSet())
    {
        if (Flags::skyType.getParam(0).empty() || Flags::skyType.getParam(0) == "auto")
        {
            if (world.getBasicGameType() == Daedalus::GameType::GT_Gothic2)
                skyColor = skyColor_g2;
            else
                skyColor = skyColor_g1;
        }
        else
        {
            if (Flags::skyType.getParam(0) == "g1")
                skyColor = skyColor_g1;
            else if (Flags::skyType.getParam(0) == "g2")
                skyColor = skyColor_g2;
            else
                LogWarn() << "Invalid sky-type supplied on commandline: " << Flags::skyType.getParam(0);
        }
    }

    switch (type)
    {
        case ESkyPresetType::ESPT_Day0:
            s.time = TIME_KEY_7;

            s.baseColor = Math::float3(255, 250, 235) / 255.0f;
            s.fogColor = skyColor;
            s.domeColorUpper = Math::float3(255, 255, 255) / 255.0f;

            s.fogDistance = 0.2f;
            s.sunEnabled = true;

            s.layers[0].texture = texAlloc.loadTextureVDF("SKYDAY_LAYER1_A0.TGA");
            s.layers[0].textureAlpha = 0.0f;
            s.layers[0].textureSpeed = Math::float2(0, 0);
            s.layers[0].textureScale = 1.0f;
            s.layers[1].texture = texAlloc.loadTextureVDF("SKYDAY_LAYER0_A0.TGA");
            s.layers[1].textureAlpha = 1.0f;
            s.layers[1].textureSpeed = Math::float2(0, 0);
            s.layers[1].textureScale = 1.0f;
            break;

        case ESkyPresetType::ESPT_Day1:
            s.time = TIME_KEY_0;

            s.baseColor = Math::float3(255, 250, 235) / 255.0f;
            s.fogColor = skyColor;
            s.domeColorUpper = Math::float3(255, 255, 255) / 255.0f;

            s.fogDistance = 0.05f;
            s.sunEnabled = true;

            s.layers[0].texture = texAlloc.loadTextureVDF("SKYDAY_LAYER1_A0.TGA");
            s.layers[0].textureAlpha = 215.0f / 255.0f;
            s.layers[0].textureSpeed = Math::float2(0, 0);
            s.layers[0].textureScale = 1.0f;
            s.layers[1].texture = texAlloc.loadTextureVDF("SKYDAY_LAYER0_A0.TGA");
            s.layers[1].textureAlpha = 1.0f;
            s.layers[1].textureSpeed = Math::float2(0, 0);
            s.layers[1].textureScale = 1.0f;
            break;

        case ESkyPresetType::ESPT_Day2:
            s.time = TIME_KEY_1;

            s.baseColor = Math::float3(255, 250, 235) / 255.0f;
            s.fogColor = skyColor;
            s.domeColorUpper = Math::float3(255, 255, 255) / 255.0f;

            s.fogDistance = 0.05f;
            s.sunEnabled = true;

            s.layers[0].texture = texAlloc.loadTextureVDF("SKYDAY_LAYER1_A0.TGA");
            s.layers[0].textureAlpha = 0.0f;
            s.layers[0].textureSpeed = Math::float2(0, 0);
            s.layers[0].textureScale = 1.0f;
            s.layers[1].texture = texAlloc.loadTextureVDF("SKYDAY_LAYER0_A0.TGA");
            s.layers[1].textureAlpha = 1.0f;
            s.layers[1].textureSpeed = Math::float2(0, 0);
            s.layers[1].textureScale = 1.0f;
            break;

        case ESkyPresetType::ESPT_Evening:
            s.time = TIME_KEY_2;

            s.baseColor = Math::float3(255, 185, 170) / 255.0f;
            s.fogColor = Math::float3(170, 70, 50) / 255.0f;
            s.domeColorUpper = Math::float3(255, 255, 255) / 255.0f;

            s.fogDistance = 0.2f;
            s.sunEnabled = true;

            s.layers[0].texture = texAlloc.loadTextureVDF("SKYDAY_LAYER1_A0.TGA");
            s.layers[0].textureAlpha = 0.5f;
            s.layers[0].textureSpeed = Math::float2(0, 0);
            s.layers[0].textureScale = 1.0f;
            s.layers[1].texture = texAlloc.loadTextureVDF("SKYDAY_LAYER0_A0.TGA");
            s.layers[1].textureAlpha = 0.5f;
            s.layers[1].textureSpeed = Math::float2(0, 0);
            s.layers[1].textureScale = 1.0f;
            break;

        case ESkyPresetType::ESPT_Night0:
            s.time = TIME_KEY_3;

            s.baseColor = Math::float3(105, 105, 195) / 255.0f;
            s.fogColor = Math::float3(20, 20, 60) / 255.0f;
            s.domeColorUpper = Math::float3(55, 55, 155) / 255.0f;

            s.fogDistance = 0.1f;
            s.sunEnabled = false;

            s.layers[0].texture = texAlloc.loadTextureVDF("SKYNIGHT_LAYER0_A0.TGA");
            s.layers[0].textureAlpha = 1.0f;
            s.layers[0].textureSpeed = Math::float2(0, 0);
            s.layers[0].textureScale = 4.0f;
            s.layers[1].texture = texAlloc.loadTextureVDF("SKYNIGHT_LAYER1_A0.TGA");
            s.layers[1].textureAlpha = 0.0f;
            s.layers[1].textureSpeed = Math::float2(0, 0);
            s.layers[1].textureScale = 1.0f;
            break;

        case ESkyPresetType::ESPT_Night1:
            s.time = TIME_KEY_4;

            s.baseColor = Math::float3(40, 60, 210) / 255.0f;
            s.fogColor = Math::float3(5, 5, 20) / 255.0f;
            s.domeColorUpper = Math::float3(55, 55, 155) / 255.0f;

            s.fogDistance = 0.1f;
            s.sunEnabled = false;

            s.layers[0].texture = texAlloc.loadTextureVDF("SKYNIGHT_LAYER0_A0.TGA");
            s.layers[0].textureAlpha = 1.0f;
            s.layers[0].textureSpeed = Math::float2(0, 0);
            s.layers[0].textureScale = 4.0f;
            s.layers[1].texture = texAlloc.loadTextureVDF("SKYNIGHT_LAYER1_A0.TGA");
            s.layers[1].textureAlpha = 215.0f / 255.0f;
            s.layers[1].textureSpeed = Math::float2(0, 0);
            s.layers[1].textureScale = 1.0f;
            break;

        case ESkyPresetType::ESPT_Night2:
            s.time = TIME_KEY_5;

            s.baseColor = Math::float3(40, 60, 210) / 255.0f;
            s.fogColor = Math::float3(5, 5, 20) / 255.0f;
            s.domeColorUpper = Math::float3(55, 55, 155) / 255.0f;

            s.fogDistance = 0.1f;
            s.sunEnabled = false;

            s.layers[0].texture = texAlloc.loadTextureVDF("SKYNIGHT_LAYER0_A0.TGA");
            s.layers[0].textureAlpha = 1.0f;
            s.layers[0].textureSpeed = Math::float2(0, 0);
            s.layers[0].textureScale = 4.0f;
            s.layers[1].texture = texAlloc.loadTextureVDF("SKYNIGHT_LAYER1_A0.TGA");
            s.layers[1].textureAlpha = 0.0f;
            s.layers[1].textureSpeed = Math::float2(0, 0);
            s.layers[1].textureScale = 1.0f;
            break;

        case ESkyPresetType::ESPT_Dawn:
            s.time = TIME_KEY_6;

            s.baseColor = Math::float3(190, 160, 255) / 255.0f;
            s.fogColor = Math::float3(80, 60, 105) / 255.0f;
            s.domeColorUpper = Math::float3(255, 255, 255) / 255.0f;

            s.fogDistance = 0.5f;
            s.sunEnabled = true;

            s.layers[0].texture = texAlloc.loadTextureVDF("SKYNIGHT_LAYER0_A0.TGA");
            s.layers[0].textureAlpha = 0.5f;
            s.layers[0].textureSpeed = Math::float2(0, 0);
            s.layers[0].textureScale = 1.0f;
            s.layers[1].texture = texAlloc.loadTextureVDF("SKYNIGHT_LAYER1_A0.TGA");
            s.layers[1].textureAlpha = 0.5f;
            s.layers[1].textureSpeed = Math::float2(0, 0);
            s.layers[1].textureScale = 1.0f;
            break;

        default:
            break;
    }
}

void Sky::fillSkyStates()
{
    // Fill all sky-states, in order
    for (int i = 0; i < ESkyPresetType::ESPT_NUM_PRESETS; i++)
    {
        initSkyState(m_World, static_cast<ESkyPresetType>(i), m_SkyStates[i], m_World.getTextureAllocator());
    }
}

void Sky::getFogValues(const Math::float3& cameraWorld, float& _near, float& _far, Math::float3& fogColor) const
{
    // Note: These are some magic values to match what gothic does
    float fogMidrange = m_FarPlane * 0.4f;
    float fogMidDelta = m_FarPlane - fogMidrange;

    // Do heightfog-approximation
    float ytotal = m_World.getWorldMesh().getBBoxMax().y - m_World.getWorldMesh().getBBoxMin().y;
    float fogMinY = m_World.getWorldMesh().getBBoxMin().y + 0.5f * ytotal;
    float fogMaxY = m_World.getWorldMesh().getBBoxMin().y + 0.7f * ytotal;

    // Skale fog back depending on how high the camera is
    float fogScale = fogMaxY - fogMinY != 0 ? (cameraWorld.y - fogMinY) / (fogMaxY - fogMinY) : 0;
    fogScale = std::min(1.0f, std::max(0.0f, fogScale));       // Clamp to 0..1
    fogScale = std::max(m_MasterState.fogDistance, fogScale);  // Fog should be at least our set distance

    _far = fogMidrange + (1.0f - fogScale) * fogMidDelta;

    // Apply some user value // FIXME: This should have a getter/setter and all that stuff
    const float userFogScale = 1.0f;
    _far *= userFogScale;
    _near = _far * 0.3f;

    // REGoth - specific: Let the fog be a little closer because of the long view-distances
    _near *= 0.5f;

    _far *= 0.4f;
    _near *= 0.4f;

    // Fix up the fog color. The fog should get less intense with decrasing fogFar

    // Compute intensity based on the ZenGins color-base
    Math::float3 base = Math::float3(0.299f, 0.587f, 0.114f);
    Math::float3 baseColor = Math::float3(m_MasterState.fogColor.x, m_MasterState.fogColor.x, m_MasterState.fogColor.x);  // Yup, all red.

    // Calculate intensity
    float intensityValue = std::min(baseColor.dot(base), 120.0f / 255.0f);
    Math::float3 intensity = Math::float3(intensityValue, intensityValue, intensityValue);
    float intensityScale = fogScale * 0.5f;

    // Calculate actual fog color
    fogColor = (1.0f - intensityScale) * m_MasterState.fogColor + intensityScale * intensity;
}
