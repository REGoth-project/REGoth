//
// Created by desktop on 07.08.16.
//

#include "Sky.h"
#include <cstdint>
#include <iostream>
#include "engine/Input.h"
#include "SkyConfig.h"
#include <engine/BaseEngine.h>
#include <engine/World.h>
#include <entry/input.h>
#include <utils/Utils.h>
#include <utils/cli.h>
#include <utils/logger.h>
#include <engine/WorldMesh.h>
#include <ZenLib/zenload/zCProgMeshProto.h>
#include <content/StaticMeshAllocator.h>

using namespace Content;

const float TIME_KEY_0 = 0.00f;
const float TIME_KEY_1 = 0.25f;
const float TIME_KEY_2 = 0.30f;
const float TIME_KEY_3 = 0.35f;
const float TIME_KEY_4 = 0.50f;
const float TIME_KEY_5 = 0.65f;
const float TIME_KEY_6 = 0.70f;
const float TIME_KEY_7 = 0.75f;

static const std::string DOME_COLORLAYER_MESH_FILE = "SKYDOME_COLORLAYER.MRM";

namespace Flags
{
    Cli::Flag skyType("s", "sky", 1, "Selects the sky to render. Possible options: auto, g1, g2", {"auto"}, "Game");
}


static Handle::MeshHandle loadMeshAndApplyFadeToLowerPart(const std::string& filename, World::WorldInstance& world);
static Handle::MeshHandle loadMeshAndApplyFadeToDomeColorPart(const std::string& filename, World::WorldInstance& world);
static Math::float3 applySkyColor(const Math::float3& col0, const Math::float3& col1, uint8_t c);
static Handle::MeshHandle createSkyPlaneMesh(World::WorldInstance& world, size_t layerIdx);

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
    for (int i = 0; i < 256; i++)
    {
        Math::float3 c = applySkyColor(col0, col1, (uint8_t)i);
        pLut[i].x = c.x;
        pLut[i].y = c.y;
        pLut[i].z = c.z;
        pLut[i].w = 1.0f;
    }
}


static Math::float3 applySkyColor(const Math::float3& col0, const Math::float3& col1, uint8_t c)
{
    return Math::float3::lerp(col0, col1, (float)c / 255.0f);
}



void Sky::getSkyColors(Math::float4 &color0, Math::float4 &color1)
{
    color0 = {0.0f, 0.0f, 0.0f, 0.0f};
    color1 = {m_MasterState.baseColor.x,
              m_MasterState.baseColor.y,
              m_MasterState.baseColor.z,
              1};
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

    for(size_t i = 0; i < m_MasterState.layers.size(); i++)
    {
        // TODO: I guess we have to interpolate here...
        m_MasterState.layers[i] = s0.layers[i];
        m_MasterState.layers[i].textureAlpha = Math::lerp<float>(s0.layers[i].textureAlpha, s1.layers[i].textureAlpha, t);
        m_MasterState.layers[i].textureScale = Math::lerp<float>(s0.layers[i].textureScale, s1.layers[i].textureScale, t);
    }

    // FIXME: There is some stuff about levelchanges here. Like, "turn off rain when on dragonisland"

    // FIXME: Multiply fogColor with 0.8 when using dome!

    // Calculate LUT based on our interpolated colors
    //calculateLUT_ZenGin(Math::float3(0, 0, 0), m_MasterState.baseColor, m_LUT); // Commented out because getSkyColors is used now

    setupDomeMeshTexturesForCurrentTime();
    setupPlaneMeshTexturesForCurrentTime();
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

            s.layers[0].textureNameBase = "SKYDAY_LAYER1_A0.TGA";
            s.layers[0].textureAlpha = 0.0f;
            s.layers[0].textureSpeed = Math::float2(1.1f, 0);
            s.layers[0].textureScale = 1.0f;
            s.layers[1].textureNameBase = "SKYDAY_LAYER0_A0.TGA";
            s.layers[1].textureAlpha = 1.0f;
            s.layers[1].textureSpeed = Math::float2(0.3f, 0);
            s.layers[1].textureScale = 1.0f;
            break;

        case ESkyPresetType::ESPT_Day1:
            s.time = TIME_KEY_0;

            s.baseColor = Math::float3(255, 250, 235) / 255.0f;
            s.fogColor = skyColor;
            s.domeColorUpper = Math::float3(255, 255, 255) / 255.0f;

            s.fogDistance = 0.05f;
            s.sunEnabled = true;

            s.layers[0].textureNameBase = "SKYDAY_LAYER1_A0.TGA";
            s.layers[0].textureAlpha = 215.0f / 255.0f;
            s.layers[0].textureSpeed = Math::float2(1.1f, 0);
            s.layers[0].textureScale = 1.0f;
            s.layers[1].textureNameBase = "SKYDAY_LAYER0_A0.TGA";
            s.layers[1].textureAlpha = 1.0f;
            s.layers[1].textureSpeed = Math::float2(0.3f, 0);
            s.layers[1].textureScale = 1.0f;
            break;

        case ESkyPresetType::ESPT_Day2:
            s.time = TIME_KEY_1;

            s.baseColor = Math::float3(255, 250, 235) / 255.0f;
            s.fogColor = skyColor;
            s.domeColorUpper = Math::float3(255, 255, 255) / 255.0f;

            s.fogDistance = 0.05f;
            s.sunEnabled = true;

            s.layers[0].textureNameBase = "SKYDAY_LAYER1_A0.TGA";
            s.layers[0].textureAlpha = 0.0f;
            s.layers[0].textureSpeed = Math::float2(1.1f, 0);
            s.layers[0].textureScale = 1.0f;
            s.layers[1].textureNameBase = "SKYDAY_LAYER0_A0.TGA";
            s.layers[1].textureAlpha = 1.0f;
            s.layers[1].textureSpeed = Math::float2(0.3f, 0);
            s.layers[1].textureScale = 1.0f;
            break;

        case ESkyPresetType::ESPT_Evening:
            s.time = TIME_KEY_2;

            s.baseColor = Math::float3(255, 185, 170) / 255.0f;
            s.fogColor = Math::float3(170, 70, 50) / 255.0f;
            s.domeColorUpper = Math::float3(255, 255, 255) / 255.0f;

            s.fogDistance = 0.2f;
            s.sunEnabled = true;

            s.layers[0].textureNameBase = "SKYDAY_LAYER1_A0.TGA";
            s.layers[0].textureAlpha = 0.5f;
            s.layers[0].textureSpeed = Math::float2(1.1f, 0);
            s.layers[0].textureScale = 1.0f;
            s.layers[1].textureNameBase = "SKYDAY_LAYER0_A0.TGA";
            s.layers[1].textureAlpha = 0.5f;
            s.layers[1].textureSpeed = Math::float2(0.3f, 0);
            s.layers[1].textureScale = 1.0f;
            break;

        case ESkyPresetType::ESPT_Night0:
            s.time = TIME_KEY_3;

            s.baseColor = Math::float3(105, 105, 195) / 255.0f;
            s.fogColor = Math::float3(20, 20, 60) / 255.0f;
            s.domeColorUpper = Math::float3(55, 55, 155) / 255.0f;

            s.fogDistance = 0.1f;
            s.sunEnabled = false;

            s.layers[0].textureNameBase = "SKYNIGHT_LAYER0_A0.TGA";
            s.layers[0].textureAlpha = 1.0f;
            s.layers[0].textureSpeed = Math::float2(0.0f, 0);;
            s.layers[0].textureScale = 4.0f;
            s.layers[1].textureNameBase = "SKYNIGHT_LAYER1_A0.TGA";
            s.layers[1].textureAlpha = 0.0f;
            s.layers[1].textureSpeed = Math::float2(0.0f, 0);;
            s.layers[1].textureScale = 1.0f;
            break;

        case ESkyPresetType::ESPT_Night1:
            s.time = TIME_KEY_4;

            s.baseColor = Math::float3(40, 60, 210) / 255.0f;
            s.fogColor = Math::float3(5, 5, 20) / 255.0f;
            s.domeColorUpper = Math::float3(55, 55, 155) / 255.0f;

            s.fogDistance = 0.1f;
            s.sunEnabled = false;

            s.layers[0].textureNameBase = "SKYNIGHT_LAYER0_A0.TGA";
            s.layers[0].textureAlpha = 1.0f;
            s.layers[0].textureSpeed = Math::float2(0.0f, 0);;
            s.layers[0].textureScale = 4.0f;
            s.layers[1].textureNameBase = "SKYNIGHT_LAYER1_A0.TGA";
            s.layers[1].textureAlpha = 215.0f / 255.0f;
            s.layers[1].textureSpeed = Math::float2(0.0f, 0);;
            s.layers[1].textureScale = 1.0f;
            break;

        case ESkyPresetType::ESPT_Night2:
            s.time = TIME_KEY_5;

            s.baseColor = Math::float3(40, 60, 210) / 255.0f;
            s.fogColor = Math::float3(5, 5, 20) / 255.0f;
            s.domeColorUpper = Math::float3(55, 55, 155) / 255.0f;

            s.fogDistance = 0.1f;
            s.sunEnabled = false;

            s.layers[0].textureNameBase = "SKYNIGHT_LAYER0_A0.TGA";
            s.layers[0].textureAlpha = 1.0f;
            s.layers[0].textureSpeed = Math::float2(0.0f, 0);;
            s.layers[0].textureScale = 4.0f;
            s.layers[1].textureNameBase = "SKYNIGHT_LAYER1_A0.TGA";
            s.layers[1].textureAlpha = 0.0f;
            s.layers[1].textureSpeed = Math::float2(0.0f, 0);;
            s.layers[1].textureScale = 1.0f;
            break;

        case ESkyPresetType::ESPT_Dawn:
            s.time = TIME_KEY_6;

            s.baseColor = Math::float3(190, 160, 255) / 255.0f;
            s.fogColor = Math::float3(80, 60, 105) / 255.0f;
            s.domeColorUpper = Math::float3(255, 255, 255) / 255.0f;

            s.fogDistance = 0.5f;
            s.sunEnabled = true;

            s.layers[0].textureNameBase = "SKYNIGHT_LAYER0_A0.TGA";
            s.layers[0].textureAlpha = 0.5f;
            s.layers[0].textureSpeed = Math::float2(1.1f, 0);
            s.layers[0].textureScale = 1.0f;
            s.layers[1].textureNameBase = "SKYNIGHT_LAYER1_A0.TGA";
            s.layers[1].textureAlpha = 0.5f;
            s.layers[1].textureSpeed = Math::float2(0.3f, 0);
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

Handle::TextureHandle Sky::getSkyTextureOfLayer(size_t skyStateIdx, size_t layerIdx)
{
    assert(skyStateIdx < m_SkyStates.size());
    if(skyStateIdx >= m_SkyStates.size())
        return Handle::TextureHandle::makeInvalidHandle();

    assert(layerIdx < m_SkyStates[skyStateIdx].layers.size());
    if(layerIdx >= m_SkyStates[skyStateIdx].layers.size())
        return Handle::TextureHandle::makeInvalidHandle();

    return m_SkyStates[skyStateIdx].layers[layerIdx].texture;
}


void Sky::onWorldNameChanged(const std::string& newWorldName)
{
    LogInfo() << "Initializing sky for new world: " << newWorldName;

    // Must come before loading the config since fillSkyStates initializes everything with defaults
    fillSkyStates();

    loadSkyConfig();

    reloadAllSkyTextures(newWorldName);

    loadDomeLayerMeshes();
    createSkyPlaneMeshes();
}


void Sky::reloadAllSkyTextures(const std::string& worldName)
{
    for(size_t i = 0; i < m_SkyStates.size(); i++)
    {
        for(size_t j = 0; j < m_SkyStates[j].layers.size(); j++)
        {
            loadSkyTextureOfLayer(i, j, worldName);
        }
    }
}


bool Sky::loadSkyTextureOfLayer(size_t skyStateIdx, size_t layerIdx, const std::string& worldname)
{
    assert(skyStateIdx < m_SkyStates.size());
    if(skyStateIdx >= m_SkyStates.size())
        return false;

    assert(layerIdx < m_SkyStates[skyStateIdx].layers.size());
    if(layerIdx >= m_SkyStates[skyStateIdx].layers.size())
        return false;

    Textures::TextureAllocator& textureAllocator = m_World.getTextureAllocator();

    std::string texNameBase = m_SkyStates[skyStateIdx].layers[layerIdx].textureNameBase;
    std::string texNameWorld = insertWorldNameIntoSkyTextureBase(texNameBase, worldname);

    Handle::TextureHandle layerTexture = textureAllocator.loadTextureVDF(texNameWorld);

    if(!layerTexture.isValid())
    {
        layerTexture = textureAllocator.loadTextureVDF(texNameBase);

        if(!layerTexture.isValid())
        {
            LogError() << "Failed to find sky texture for state " << skyStateIdx << ", layer " << layerIdx << ". "
                       << "Tried " << texNameWorld << " and " << texNameBase << ".";

            return false;
        }else
        {
            LogInfo() << "Loaded sky texture (Base): " << texNameBase << ", couldn't find custom texture for world " << worldname << " (Tried " << texNameWorld << ")";
        }
    } else
    {
        LogInfo() << "Loaded sky texture (world-dependent): " << texNameWorld;
    }

    m_SkyStates[skyStateIdx].layers[layerIdx].texture = layerTexture;

    return true;
}

std::string Sky::insertWorldNameIntoSkyTextureBase(const std::string& skyTextureBase, const std::string& worldName)
{
    std::string texNameWorld = skyTextureBase;
    texNameWorld.insert(skyTextureBase.find_first_of('_'), "_" + Utils::uppered(worldName));

    return texNameWorld;
}


/**
 * Sets the alpha-values of the lowest vertices in the mesh to do some fading.
 *
 * Gothic does this after loading the mesh. A better way would have been to simply bake this into the mesh...
 */
static void applyAlphaFadeToLowerPart(ZenLoad::PackedMesh& mesh)
{
    float lowestVertex = mesh.bbox[0].y;
    float highestVertex = mesh.bbox[1].y;
    float heightTotal = highestVertex - lowestVertex;

    const float heightTotalInv = 1.0f / heightTotal;

    for(ZenLoad::WorldVertex& vx : mesh.vertices)
    {
        if(vx.Position.y > 0)
        {
            vx.Color = 0xFFFFFFFF;
        }
        else
        {
            float vertexHeightRatio = Math::clamp(1.0f - (vx.Position.y - lowestVertex) * heightTotalInv, 0.0f, 1.0f);

            const float alpha = 1.0f - Math::sinusSlowEnd(vertexHeightRatio);
            Math::float4 color = Math::float4(1.0f, 1.0f, 1.0f, alpha);

            vx.Color = color.toRGBA8();
        }
    }
}


/**
 * Sets the alpha-values of the lowest vertices in the mesh to do some fading.
 *
 * Gothic does this after loading the mesh. A better way would have been to simply bake this into the mesh...
 */
static void applyAlphaFadeToDomeColorPart(ZenLoad::PackedMesh& mesh)
{
    float lowestVertex = mesh.bbox[0].y;
    float highestVertex = mesh.bbox[1].y;
    float heightTotal = highestVertex - lowestVertex;

    const float heightTotalInv = 1.0f / heightTotal;
    const float fadeout_offset = 0.0001f; // Magic value from gothic

    for(ZenLoad::WorldVertex& vx : mesh.vertices)
    {

        float vertexHeightRatio = Math::clamp((vx.Position.y - lowestVertex) * heightTotalInv, 0.0f, 1.0f);


        float alpha;
        if(vertexHeightRatio < fadeout_offset)
            alpha = Math::lerp<float>(0.0f, 0.8f, Math::sinusSlowEnd(vertexHeightRatio  / fadeout_offset));
        else
            alpha = Math::lerp<float>(0.8f, 0.0f, Math::sinusSlowEnd(vertexHeightRatio) - fadeout_offset);

        Math::float4 color = Math::float4(1.0f, 1.0f, 1.0f, alpha);
        vx.Color = color.toRGBA8();
    }
}


void Sky::loadDomeLayerMeshes()
{
    for(size_t i = 0; i < m_DomeMeshesByLayer.size(); i++)
    {
        m_DomeMeshesByLayer[i] = loadMeshAndApplyFadeToLowerPart(getDomeLayerMeshFileName(i), m_World);
    }

    m_DomeColorLayerMesh = loadMeshAndApplyFadeToDomeColorPart(DOME_COLORLAYER_MESH_FILE, m_World);
}


static Handle::MeshHandle loadMeshAndApplyFadeToLowerPart(const std::string& filename, World::WorldInstance& world)
{
    ZenLoad::PackedMesh packed;

    // Try to load the mesh
    ZenLoad::zCProgMeshProto zmsh(filename, world.getEngine()->getVDFSIndex());

    // Failed?
    if (zmsh.getNumSubmeshes() == 0)
        return Handle::MeshHandle::makeInvalidHandle();

    // Pack the mesh
    zmsh.packMesh(packed, 1.0f / 100.0f);

    applyAlphaFadeToLowerPart(packed);

    return world.getStaticMeshAllocator().loadFromPacked(packed, filename);
}


static Handle::MeshHandle loadMeshAndApplyFadeToDomeColorPart(const std::string& filename, World::WorldInstance& world)
{
    ZenLoad::PackedMesh packed;

    // Try to load the mesh
    ZenLoad::zCProgMeshProto zmsh(filename, world.getEngine()->getVDFSIndex());

    // Failed?
    if (zmsh.getNumSubmeshes() == 0)
        return Handle::MeshHandle::makeInvalidHandle();

    // Pack the mesh
    zmsh.packMesh(packed, 1.0f / 100.0f);

    applyAlphaFadeToDomeColorPart(packed);

    return world.getStaticMeshAllocator().loadFromPacked(packed, filename);
}



void Sky::setupDomeMeshTexturesForCurrentTime()
{
    for(size_t i = 0; i < m_DomeMeshesByLayer.size(); i++)
    {
        if (!m_DomeMeshesByLayer[i].isValid())
            continue;

        Meshes::WorldStaticMesh& dome = m_World.getStaticMeshAllocator().getMesh(m_DomeMeshesByLayer[i]);

        for(Materials::TexturedMaterial& mat : dome.mesh.m_SubmeshMaterials)
        {
            mat.m_TextureHandle = m_MasterState.layers[i].texture;
        }
    }
}

void Sky::setupPlaneMeshTexturesForCurrentTime()
{
    for(size_t i = 0; i < m_PlaneMeshesByLayer.size(); i++)
    {
        if (!m_PlaneMeshesByLayer[i].isValid())
            continue;

        Meshes::WorldStaticMesh& plane = m_World.getStaticMeshAllocator().getMesh(m_PlaneMeshesByLayer[i]);

        for(Materials::TexturedMaterial& mat : plane.mesh.m_SubmeshMaterials)
        {
            mat.m_TextureHandle = m_MasterState.layers[i].texture;
        }
    }
}

const std::array<Handle::MeshHandle, Sky::NUM_SKY_LAYERS>& Sky::getDomeMeshes() const
{
    return m_DomeMeshesByLayer;
}


const std::array<Handle::MeshHandle, Sky::NUM_SKY_LAYERS>& Sky::getSkyPlaneMeshes() const
{
    return m_PlaneMeshesByLayer;
}


std::string Sky::getDomeLayerMeshFileName(size_t layerIdx)
{
    return "SKYDOME_LAYER" + std::to_string(layerIdx + 1) + ".MRM";
}


void Sky::createSkyPlaneMeshes()
{
    for(size_t i = 0; i < m_PlaneMeshesByLayer.size(); i++)
    {
        m_PlaneMeshesByLayer[i] = createSkyPlaneMesh(m_World, i);
    }
}


static Handle::MeshHandle createSkyPlaneMesh(World::WorldInstance& world, size_t layerIdx)
{
    ZenLoad::PackedMesh packed;

    const float PLANE_HALF_LENGTH_SIDE = 500;
    const float PLANE_HEIGHT = 10;

    packed.vertices.resize(4);
    packed.vertices[0].Position = { PLANE_HALF_LENGTH_SIDE, PLANE_HEIGHT, -PLANE_HALF_LENGTH_SIDE};
    packed.vertices[1].Position = { PLANE_HALF_LENGTH_SIDE, PLANE_HEIGHT,  PLANE_HALF_LENGTH_SIDE};
    packed.vertices[2].Position = {-PLANE_HALF_LENGTH_SIDE, PLANE_HEIGHT,  PLANE_HALF_LENGTH_SIDE};
    packed.vertices[3].Position = {-PLANE_HALF_LENGTH_SIDE, PLANE_HEIGHT, -PLANE_HALF_LENGTH_SIDE};

    /*
     * 3 --- 0
     * |   / |
     * | /   |
     * 2 --- 1
     */

    packed.vertices[0].Color = 0xFFFFFFFF;
    packed.vertices[1].Color = 0xFFFFFFFF;
    packed.vertices[2].Color = 0xFFFFFFFF;
    packed.vertices[3].Color = 0xFFFFFFFF;

    packed.vertices[0].TexCoord = {0.0f, 0.0f};
    packed.vertices[1].TexCoord = {1.0f, 0.0f};
    packed.vertices[2].TexCoord = {1.0f, 1.0f};
    packed.vertices[3].TexCoord = {0.0f, 1.0f};

    packed.subMeshes.emplace_back();
    packed.subMeshes[0].indices = {
            3,2,0,
            0,2,1
    };

    return world.getStaticMeshAllocator().loadFromPacked(packed, "SKYPLANE_LAYER" + std::to_string(layerIdx) + ".MRM");
}


void Sky::loadSkyConfig()
{
    const std::string configfile = m_World.getEngine()->getContentBasePath() + "sky.json";

    if(!Utils::fileExists(configfile))
    {
        LogError() << "Could not find sky-config at: " << configfile;
        return;
    }

    // TODO: Maybe move this file into an archive that can be loaded via PhysFS
    std::string contents = Utils::readFileContents(configfile);

    SkyConfig::SkyGametype gametype;
    switch(m_World.getBasicGameType())
    {
        case Daedalus::GameType::GT_Gothic1:
            gametype = SkyConfig::SkyGametype::Gothic1;
            break;

        case Daedalus::GameType::GT_Gothic2:
        default:
            gametype = SkyConfig::SkyGametype::Gothic2;
            break;
    }

    SkyConfig config(contents, gametype);

    if(!config.isValid())
    {
        LogError() << "Failed Sky-Config contains invalid data!";
        return;
    }

    fillColorsFromSkyConfig(config);

    LogInfo() << "Sky: Successfully loaded sky-config from: " << configfile;
}

void Sky::fillColorsFromSkyConfig(const SkyConfig& config)
{
    const SkyConfig::SkyDayColorSet& set = config.findColorSetOfWorld(m_World.getWorldName());

    m_SkyStates[ESPT_Day0].fogColor = set.colors[0];
    m_SkyStates[ESPT_Day1].fogColor = set.colors[0];
    m_SkyStates[ESPT_Day2].fogColor = set.colors[0];

    // Note: Color #0 seems to be the only one actually used by the game. w/e.
}

void Sky::getDomeColors(Math::float3& color0, Math::float3& color1)
{
    // Beware! Many magic numbers ahead to match gothics sky calculation!

    const float GOTHIC_COLOR_SUBTRACTION = 25.0f / 255.0f;

    color0 = m_MasterState.fogColor;
    color1 = m_MasterState.fogColor;

    color1.x = (color1.x * 0.4f) - GOTHIC_COLOR_SUBTRACTION;
    color1.y = (color1.y * 0.7f) - GOTHIC_COLOR_SUBTRACTION;
    color1.z = (color1.z * 0.9f) - GOTHIC_COLOR_SUBTRACTION;

    if(m_MasterState.time >= TIME_KEY_1 && m_MasterState.time <= TIME_KEY_2)
    {
        color0.x *= 20.0f;
        color0.y *= 20.0f;
        color0.z *= 2;
    }
    else
    {
        color0.x *= 10.0f;
        color0.y *= 10.0f;
        color0.z *= 8.0f;
    }

    color0.x = Math::clamp(color0.x, 0.0f, 1.0f);
    color0.y = Math::clamp(color0.y, 0.0f, 1.0f);
    color0.z = Math::clamp(color0.z, 0.0f, 1.0f);
    color1.x = Math::clamp(color1.x, 0.0f, 1.0f);
    color1.y = Math::clamp(color1.y, 0.0f, 1.0f);
    color1.z = Math::clamp(color1.z, 0.0f, 1.0f);
}

bool Sky::isNightTime() const
{
    return m_MasterState.time >= TIME_KEY_1 && m_MasterState.time <= TIME_KEY_7;
}

Math::float3 Sky::getPolyCloudsLayerColor()
{
    Math::float3 color;

    if(isNightTime())
        color = {1.0f, 1.0f, 1.0f};
    else
        color = getMasterState().domeColorUpper;

    if(m_MasterState.time >= TIME_KEY_3 && m_MasterState.time <= TIME_KEY_5)
    {
        color = 0.5f * (color + Math::float3(1.0f, 1.0f, 1.0f));
    }

    return color;
}


