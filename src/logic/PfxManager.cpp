//
// Created by desktop on 24.01.17.
//

#include "PfxManager.h"
#include <ZenLib/utils/logger.h>
#include <daedalus/DaedalusVM.h>
#include <engine/BaseEngine.h>
#include <engine/World.h>
#include <utils/Utils.h>
#include <map>

Logic::PfxManager::PfxManager(World::WorldInstance& world) : m_pVM(nullptr), m_World(world)
{
}

Logic::PfxManager::~PfxManager()
{
    delete m_pVM;
}

bool Logic::PfxManager::loadParticleFXDAT()
{
    if (m_pVM)
        return true;  // Nothing to do, already loaded

    std::string datPath = "/_work/data/Scripts/_compiled/PARTICLEFX.DAT";
    std::string datFile = Utils::getCaseSensitivePath(datPath, m_World.getEngine()->getEngineArgs().gameBaseDirectory);

    if (!Utils::fileExists(datFile))
    {
        LogError() << "Failed to find PARTICLEFX.DAT at: " << datFile;
        return false;
    }

    // Load DAT-File...
    m_pVM = new Daedalus::DaedalusVM(datFile);
    Daedalus::registerGothicEngineClasses(*m_pVM);

    m_DefaultEmitter = {};

    return m_pVM != nullptr;
}

bool Logic::PfxManager::hasPFX(const std::string& name)
{
    if (!loadParticleFXDAT())
        return false;

    return m_pVM->getDATFile().hasSymbolName(name);
}

const Logic::PfxManager::Emitter& Logic::PfxManager::getParticleFX(const std::string& name)
{
    if (!loadParticleFXDAT())
        return m_DefaultEmitter;

    size_t symIdx = m_pVM->getDATFile().getSymbolIndexByName(name);

    // Check for cache
    auto it = m_PfxCache.find(symIdx);
    if (it != m_PfxCache.end())
        return (*it).second;

    using namespace Daedalus::GEngineClasses;
    using namespace Daedalus::GameState;

    // Create a new instance to look up the data
    PfxHandle h = m_pVM->getGameState().createPfx();
    m_pVM->initializeInstance(ZMemory::toBigHandle(h),
                              m_pVM->getDATFile().getSymbolIndexByName(name),
                              Daedalus::IC_Pfx);

    // Add to cache
    Emitter& e = m_PfxCache[symIdx];
    C_ParticleFX& fx = m_pVM->getGameState().getPfx(h);

    auto parseFloatList = [](const std::string& s) {
        std::vector<std::string> l = Utils::split(s, ' ');

        std::vector<float> lf(l.size());
        std::transform(l.begin(), l.end(), lf.begin(), [](const std::string& p) { return std::stof(p); });

        return lf;
    };

    auto parseFloat3 = [](const std::string& s) {

        if (s == "=")  // Seems to be a special case
            return Math::float3(0, 0, 0);

        std::vector<std::string> l = Utils::split(s, ' ');
        if (l.size() == 3)
            return Math::float3(std::stof(l[0]), std::stof(l[1]), std::stof(l[2]));
        else if (l.size() == 2)
            return Math::float3(std::stof(l[0]), std::stof(l[1]), 0);
        else if (l.size() == 1)
            return Math::float3(std::stof(l[0]), 0, 0);

        return Math::float3(0, 0, 0);
    };

    auto parseFloat2 = [](const std::string& s) {
        std::vector<std::string> l = Utils::split(s, ' ');
        return Math::float2(std::stof(l[0]), std::stof(l[1]));
    };

    // FIXME: Some of them are lowercase...
    std::map<std::string, EmitterShape> lut_EmitterShape = {{"POINT", ES_POINT},
                                                            {"LINE", ES_LINE},
                                                            {"BOX", ES_BOX},
                                                            {"CIRCLE", ES_CIRCLE},
                                                            {"SPHERE", ES_SPHERE},
                                                            {"MESH", ES_MESH}};

    std::map<std::string, EmitterFOR> lut_EmitterFOR = {{"WORLD", EFOR_WORLD},
                                                        {"OBJECT", EFOR_OBJECT}};

    std::map<std::string, EmitterDirMode> lut_EmitterDirMode = {{"NONE", EDM_NONE},
                                                                {"DIR", EDM_DIR},
                                                                {"TARGET", EDM_TARGET},
                                                                {"MESH", EDM_MESH}};

    std::map<std::string, EmitterVisOrient> lut_EmitterVisOrient = {{"NONE", EVO_NONE},
                                                                    {"VELO", EVO_VELO_ALIGNED},
                                                                    {"VELO3D", EVO_VOB_XZPLANE}};

    std::map<std::string, EmitterDistribType> lut_EmitterDistribType = {{"RAND", EDT_RAND},
                                                                        {"WALK", EDT_WALK},
                                                                        {"UNIFORM", EDT_UNIFORM}};

    std::map<std::string, EmitterBlendMode> lut_EmitterBlendMode = {{"NONE", EBM_None},
                                                                    {"BLEND", EBM_Blend},
                                                                    {"ADD", EBM_Add},
                                                                    {"MUL", EBM_Mul}};

    e.ppsValue = fx.ppsValue;
    e.ppsScaleKeys = parseFloatList(fx.ppsScaleKeys_S);
    e.ppsIsLooping = fx.ppsIsLooping != 0;
    e.ppsIsSmooth = fx.ppsIsSmooth != 0;
    e.ppsFPS = fx.ppsFPS;
    e.ppsCreateEm = fx.ppsCreateEm_S;
    e.ppsCreateEmDelay = fx.ppsCreateEmDelay;
    e.shpType = lut_EmitterShape[fx.shpType_S];
    e.shpFOR = lut_EmitterFOR[fx.shpFOR_S];
    e.shpOffset = parseFloat3(fx.shpOffsetVec_S) * 0.01f;
    e.shpDistribType = lut_EmitterDistribType[fx.shpDistribType_S];
    e.shpDistribWalkSpeed = fx.shpDistribWalkSpeed;
    e.shpIsVolume = fx.shpIsVolume;
    e.shpDim = parseFloat3(fx.shpDim_S) * 0.01f;
    e.shpMesh = fx.shpMesh_S;
    e.shpMeshRender = fx.shpMeshRender_B != 0;
    e.shpScaleKeys = parseFloatList(fx.shpScaleKeys_S);
    e.shpScaleIsLooping = fx.shpScaleIsLooping != 0;
    e.shpScaleIsSmooth = fx.shpScaleIsSmooth != 0;
    e.shpScaleFPS = fx.shpScaleFPS;
    e.dirMode = lut_EmitterDirMode[fx.dirMode_S];
    e.dirFOR = lut_EmitterFOR[fx.dirFOR_S];
    e.dirModeTargetFOR = lut_EmitterFOR[fx.dirModeTargetFOR_S];
    e.dirModeTargetPos = parseFloat3(fx.dirModeTargetPos_S) * 0.01f;
    e.dirAngleHead = fx.dirAngleHead;
    e.dirAngleHeadVar = fx.dirAngleHeadVar;
    e.dirAngleElev = fx.dirAngleElev;
    e.dirAngleElevVar = fx.dirAngleElevVar;
    e.velAvg = fx.velAvg * 0.01f * 1000.0f;                         // cm/ms -> m/s
    e.velVar = fx.velVar * 0.01f;                                   // cm/ms -> m/s
    e.lspPartAvg = fx.lspPartAvg / 1000.0f;                         // ms -> s
    e.lspPartVar = fx.lspPartVar / 1000.0f;                         // ms -> s
    e.flyGravity = parseFloat3(fx.flyGravity_S) * 0.01f * 1000.0f;  // cm/ms -> m/s;
    e.flyCollDet = (EmitterCollDetType)fx.flyCollDet_B;
    e.visName = fx.visName_S;
    e.visOrientation = lut_EmitterVisOrient[fx.visOrientation_S];
    e.visTexIsQuadPoly = fx.visTexIsQuadPoly != 0;
    e.visTexAniFPS = fx.visTexAniFPS;
    e.visTexAniIsLooping = fx.visTexAniIsLooping != 0;
    e.visTexColorStart = parseFloat3(fx.visTexColorStart_S) * 1.0f / 255.0f;
    e.visTexColorEnd = parseFloat3(fx.visTexColorEnd_S) * 1.0f / 255.0f;
    e.visSizeStart = parseFloat2(fx.visSizeStart_S) * 0.01f;
    e.visSizeEndScale = fx.visSizeEndScale;
    e.visAlphaFunc = lut_EmitterBlendMode[fx.visAlphaFunc_S];
    e.visAlphaStart = fx.visAlphaStart / 255.0f;
    e.visAlphaEnd = std::max(0.0f, fx.visAlphaEnd / 255.0f);  // Seems to be negative sometimes...
    e.trlFadeSpeed = fx.trlFadeSpeed;
    e.trlTexture = fx.trlTexture_S;
    e.trlWidth = fx.trlWidth;
    e.mrkFadeSpeed = fx.mrkFadeSpeed;
    e.mrkTexture = fx.mrkTexture_S;
    e.mrkSize = fx.mrkSize;

    // We don't support single triange-rendering. Thus, need to scale this up to match quad-rendering
    if (e.visTexIsQuadPoly)
        e.visSizeStart *= 2.0f;

    float sin_h = sin(e.dirAngleHead),
          sin_e = sin(e.dirAngleElev),
          cos_h = cos(e.dirAngleHead),
          cos_e = cos(e.dirAngleElev);

    e.dirDirection = Math::float3(-sin_h * cos_e, sin_e, cos_e * cos_h);

    // Crazy calculation to get actual BBox with variance
    {
        float head[2], elev[2];
        int headNum, elevNum;
        Math::float3 vec;
        e.dirAngleBox.min = e.dirAngleBox.max = e.dirDirection;

        if (e.dirAngleHeadVar != 0.0f)
        {
            head[0] = Math::DegToRad(e.dirAngleHead) + Math::DegToRad(e.dirAngleHeadVar);
            head[1] = Math::DegToRad(e.dirAngleHead) - Math::DegToRad(e.dirAngleHeadVar);
            headNum = 2;
        }
        else
        {
            head[0] = Math::DegToRad(e.dirAngleHead);
            headNum = 1;
        }

        if (e.dirAngleElevVar != 0.0f)
        {
            elev[0] = Math::DegToRad(e.dirAngleElev) + Math::DegToRad(e.dirAngleElevVar);
            elev[1] = Math::DegToRad(e.dirAngleElev) - Math::DegToRad(e.dirAngleElevVar);
            elevNum = 2;
        }
        else
        {
            elev[0] = Math::DegToRad(e.dirAngleElev);
            elevNum = 1;
        }

        for (int i = 0; i < headNum; i++)
        {
            for (int j = 0; j < elevNum; j++)
            {
                float _h = head[i];
                float _e = elev[j];

                sin_h = sin(_h);
                sin_e = sin(_e);
                cos_h = cos(_h);
                cos_e = cos(_e);

                Math::float3 vec(-sin_h * cos_e, sin_e, cos_e * cos_h);

                e.dirAngleBox.min.x = std::min(e.dirAngleBox.min.x, vec.x);
                e.dirAngleBox.min.y = std::min(e.dirAngleBox.min.y, vec.y);
                e.dirAngleBox.min.z = std::min(e.dirAngleBox.min.z, vec.z);
                e.dirAngleBox.max.x = std::max(e.dirAngleBox.max.x, vec.x);
                e.dirAngleBox.max.y = std::max(e.dirAngleBox.max.y, vec.y);
                e.dirAngleBox.max.z = std::max(e.dirAngleBox.max.z, vec.z);
            }
        }

        e.dirAngleBoxDim = e.dirAngleBox.max - e.dirAngleBox.min;
    }

    /* // Gothic 2?
    e.useEmittersFOR = fx.useEmittersFOR;
    e.timeStartEnd = fx.timeStartEnd;
    e.isAmbientPFX = fx.isAmbientPFX;*/

    return e;
}
