#include <bitset>
#include <cstdlib>
#include <iostream>
#include <iterator>

#include <ZenLib/zenload/zTypes.h>
#include <components/EntityActions.h>
#include <components/Vob.h>
#include <components/VobClasses.h>
#include <content/AnimationLibrary.h>
#include <content/ContentLoad.cpp>
#include <debugdraw/debugdraw.h>
#include <engine/BaseEngine.h>
#include <engine/GameEngine.h>
#include <engine/World.h>
#include <entry/input.h>
#include <handle/HandleDef.h>
#include <logic/MobController.h>
#include <logic/PlayerController.h>
#include <logic/SoundController.h>
#include <logic/MusicController.h>
#include <ui/Hud.h>
#include <ui/LoadingScreen.h>
#include <ui/PrintScreenMessages.h>
#include <utils/logger.h>
#include <zenload/zCMesh.h>
#include <zenload/zenParser.h>
#include <type_traits>
#include "BspTree.h"
#include "WorldMesh.h"
#include <physics/PhysicsSystem.h>
#include <content/Sky.h>
#include <logic/DialogManager.h>
#include <logic/PfxManager.h>
#include <logic/ScriptEngine.h>
#include "WorldAllocators.h"

using namespace World;

class WorldInstance::ClassContents
{
public:
    ClassContents(WorldInstance& world)
        : worldMesh(world)
        , scriptEngine(world)
        , physicsSystem(world)
        , animationLibrary(world)
        , sky(world)
        , dialogManager(world)
        , bspTree(world)
        , pfxManager(world)
        , audioWorld(nullptr)
    {}

    WorldMesh worldMesh;
    BspTree bspTree;
    Waynet::WaynetInstance waynet;
    Logic::ScriptEngine scriptEngine;
    Physics::PhysicsSystem physicsSystem;
    Animations::AnimationLibrary animationLibrary;
    std::unique_ptr<World::AudioWorld> audioWorld;
    Content::Sky sky;
    Logic::DialogManager dialogManager;
    Logic::PfxManager pfxManager;
};

struct LoadSection
{
    int p1;
    int p2;
    std::string info;
};

const LoadSection LOAD_SECTION_LOADSCRIPTS = {0, 10, "Loading scripts"};
const LoadSection LOAD_SECTION_ZENFILE = {10, 20, "Loading worldfile"};
const LoadSection LOAD_SECTION_WORLDMESH = {20, 40, "Processing worldmesh"};
const LoadSection LOAD_SECTION_COLLISION = {40, 60, "Generating world collision"};
const LoadSection LOAD_SECTION_VOBS = {60, 80, "Loading objects"};
const LoadSection LOAD_SECTION_RUNSCRIPTS = {80, 100, "Running startup scripts"};

WorldInstance::WorldInstance(Engine::BaseEngine& engine)
    : m_pEngine(&engine)
    , m_Allocators(std::make_unique<WorldAllocators>(engine))
    , m_ClassContents(std::make_unique<ClassContents>(*this))
{
}

WorldInstance::~WorldInstance()
{
    // kick out player if he is still in this world
    /*
    // FIXME currently crashes game if player is not PC_HERO
    auto player = getScriptEngine().getPlayerEntity();
    if (player.isValid() && false)
        VobTypes::Wld_RemoveNpc(*this, player);
    */

    // Destroy all allocated components
    // Loop because some destructor may create more entities
    while(getComponentAllocator().getNumObtainedElements() != 0)
    {
        auto ents = getComponentAllocator().getDataBundle();
        Components::EntityComponent* entityComponents = std::get<Components::EntityComponent*>(ents.m_Data);

        // Need to make a list of all entites because some destructors could remove some entites inside
        std::vector<Handle::EntityHandle> allEntities;
        for (size_t i = 0; i < ents.m_NumElements; i++)
        {
            allEntities.push_back(entityComponents[i].m_ThisEntity);
        }

        for (Handle::EntityHandle e : allEntities)
        {
            if (isEntityValid(e))
            {
                Components::Actions::forAllComponents(getComponentAllocator(), e, [&](auto& v) {
                    Components::Actions::destroyComponent(v);
                });

                getComponentAllocator().removeObject(e);
            }
        }
    }
}

bool WorldInstance::init(const std::string& zen,
                         const json& worldJson,
                         const json& scriptEngine,
                         const json& dialogManager,
                         const json& logManager)
{
    m_ZenFile = zen;
    Engine::BaseEngine& engine = *m_pEngine;

    if (!m_ClassContents->animationLibrary.loadAnimations())
        LogError() << "failed to load animations!";

    // Create static-collision shape beforehand
    m_StaticWorldObjectCollsionShape = m_ClassContents->physicsSystem.makeCompoundCollisionShape(Physics::CollisionShape::CT_Object);
    m_StaticWorldMeshCollsionShape = m_ClassContents->physicsSystem.makeCompoundCollisionShape(Physics::CollisionShape::CT_WorldMesh);
    m_StaticWorldObjectPhysicsObject = m_ClassContents->physicsSystem.makeRigidBody(m_StaticWorldObjectCollsionShape, Math::Matrix::CreateIdentity());
    m_StaticWorldMeshPhysicsObject = m_ClassContents->physicsSystem.makeRigidBody(m_StaticWorldMeshCollsionShape, Math::Matrix::CreateIdentity());

    // Notify user
    m_pEngine->getHud().getLoadingScreen().startSection(
        LOAD_SECTION_LOADSCRIPTS.p1,
        LOAD_SECTION_LOADSCRIPTS.p2,
        LOAD_SECTION_LOADSCRIPTS.info);

    // Init daedalus-vm
    std::string datPath = "/_work/data/Scripts/_compiled/GOTHIC.DAT";
    std::string datFile = Utils::getCaseSensitivePath(datPath, m_pEngine->getEngineArgs().gameBaseDirectory);

    if (Utils::fileExists(datFile))
    {
        m_ClassContents->scriptEngine.loadDAT(datFile);
    }
    else
    {
        LogError() << "Failed to find GOTHIC.DAT at: " << datFile;
    }

    // Load world
    if (!zen.empty())
    {
        // Notify user
        m_pEngine->getHud().getLoadingScreen().startSection(
            LOAD_SECTION_ZENFILE.p1,
            LOAD_SECTION_ZENFILE.p2,
            LOAD_SECTION_ZENFILE.info);

        // Load ZEN
        ZenLoad::ZenParser parser(zen, engine.getVDFSIndex());

        m_pEngine->getHud().getLoadingScreen().setSectionProgress(20);

        parser.readHeader();

        m_pEngine->getHud().getLoadingScreen().setSectionProgress(60);

        ZenLoad::oCWorldData world;
        parser.readWorld(world);

        ZenLoad::zCMesh* worldMesh = parser.getWorldMesh();

        m_pEngine->getHud().getLoadingScreen().setSectionProgress(80);

        LogInfo() << "Initilizing BSP-Tree...";
        m_ClassContents->bspTree.loadBspTree(world.bspTree);

        LogInfo() << "Postprocessing worldmesh...";

        // Notify user
        m_pEngine->getHud().getLoadingScreen().startSection(
            LOAD_SECTION_WORLDMESH.p1,
            LOAD_SECTION_WORLDMESH.p2,
            LOAD_SECTION_WORLDMESH.info);

        ZenLoad::PackedMesh packedWorldMesh;
        worldMesh->packMesh(packedWorldMesh, 0.01f, false);

        m_pEngine->getHud().getLoadingScreen().setSectionProgress(20);

        // Init worldmesh-wrapper
        m_ClassContents->worldMesh.load(packedWorldMesh);

        /*for (auto& sm : packedWorldMesh.subMeshes)
        {
            size_t k = 0;
            for (auto& lm : sm.triangleLightmapIndices)
            {
                if (lm != -1)
                {
                    packedWorldMesh.vertices[sm.indices[k * 3 + 0]].Color = 0xFFAAAAAA;
                    packedWorldMesh.vertices[sm.indices[k * 3 + 1]].Color = 0xFFAAAAAA;
                    packedWorldMesh.vertices[sm.indices[k * 3 + 2]].Color = 0xFFAAAAAA;
                }

                k++;
            }
        }*/

        m_pEngine->getHud().getLoadingScreen().setSectionProgress(40);

        // TODO: Put these into a compound-component or something
        std::vector<Handle::EntityHandle> ents;
        Handle::MeshHandle worldMeshHandle = getStaticMeshAllocator().loadFromPacked(packedWorldMesh, "WORLDMESH.3DS");

        m_pEngine->getHud().getLoadingScreen().setSectionProgress(80);

        /*Handle::EntityHandle eh = Vob::constructVob(*this);
        Vob::VobInformation wvob = Vob::asVob(*this, eh);
        wvob.position->m_DrawDistanceFactor = 0.0f;
        Vob::setVisual(wvob, "WORLDMESH.3DS");
        Vob::setTransform(wvob, Math::Matrix::CreateIdentity());
*/

        Meshes::WorldStaticMesh& worldMeshData = getStaticMeshAllocator().getMesh(worldMeshHandle);

        for (size_t i = 0; i < packedWorldMesh.subMeshes.size(); i++)
        {
            Handle::MeshHandle h = getStaticMeshAllocator().loadFromPackedSubmesh(packedWorldMesh, i, "");
            Meshes::WorldStaticMesh& hdata = getStaticMeshAllocator().getMesh(h);

            std::vector<Handle::EntityHandle> subents = Content::entitifyMesh(*this, h, hdata.mesh);

            ents.insert(ents.end(), subents.begin(), subents.end());
        }

        // Notify user
        m_pEngine->getHud().getLoadingScreen().startSection(
            LOAD_SECTION_COLLISION.p1,
            LOAD_SECTION_COLLISION.p2,
            LOAD_SECTION_COLLISION.info);

        // If we haven't already, create an instancebuffer for this mesh
        //if(worldMeshData.instanceDataBufferIndex == (uint32_t)-1)
        //    worldMeshData.instanceDataBufferIndex = ((Engine::GameEngine*)m_pEngine)->getDefaultRenderSystem().requestInstanceDataBuffer();

        // Create collisionmesh for the world
        if (!ents.empty())
        {
            LogInfo() << "Generating world collision mesh...";

            // Create world-object using the static collision-shape
            Components::PhysicsComponent& phys = Components::Actions::initComponent<Components::PhysicsComponent>(getComponentAllocator(), ents.front());

            phys.m_PhysicsObject = m_StaticWorldMeshCollsionShape;
            phys.m_IsStatic = true;

            m_pEngine->getHud().getLoadingScreen().setSectionProgress(50);

            // Add world-mesh collision
            Handle::CollisionShapeHandle wmch = getPhysicsSystem().makeCollisionShapeFromMesh(packedWorldMesh.triangles, Physics::CollisionShape::CT_WorldMesh);
            getPhysicsSystem().compoundShapeAddChild(m_StaticWorldMeshCollsionShape, wmch);

        }

        // Make sure static collision is initialized before adding the VOBs
        m_ClassContents->physicsSystem.postProcessLoad();

        for (Handle::EntityHandle e : ents)
        {
            // Init positions
            Components::EntityComponent& entity = getEntity<Components::EntityComponent>(e);
            Components::Actions::initComponent<Components::PositionComponent>(getComponentAllocator(), e);

            // Copy world-matrix (These are all identiy on the worldmesh)
            Components::PositionComponent& pos = getEntity<Components::PositionComponent>(e);
            pos.m_WorldMatrix = Math::Matrix::CreateIdentity();
            pos.m_WorldMatrix.Translation(pos.m_WorldMatrix.Translation() * (1.0f / 100.0f));
            pos.m_DrawDistanceFactor = -1.0f;  // Always draw the worldmesh

            Components::StaticMeshComponent& sm = getEntity<Components::StaticMeshComponent>(e);
            sm.m_InstanceDataIndex = (uint32_t)-2;  // Disable instancing
        }

        // TODO: Refractor. Make a map of all vobs by classes or something.
        ZenLoad::zCVobData startPoint;

        size_t numVobsLoaded = 0;
        std::function<void(const std::vector<ZenLoad::zCVobData>)> vobLoad = [&](
            const std::vector<ZenLoad::zCVobData>& vobs) {

            for (const ZenLoad::zCVobData& v : vobs)
            {
                vobLoad(v.childVobs);

                // We're loading one vob here, update progressbar
                numVobsLoaded += 1;
                m_pEngine->getHud().getLoadingScreen().setSectionProgress((100 * (int)numVobsLoaded) / (int)world.numVobsTotal);

                bool allowCollision = true;  // FIXME: Hack. Items shouldn't be placed into physicsworld right now

                // Check for special vobs // FIXME: Should be somewhere else
                Vob::VobInformation vob;
                Handle::EntityHandle e;

                if (v.objectClass == "oCItem:zCVob")
                {
                    // Get item instance
                    if (getScriptEngine().hasSymbol(v.oCItem.instanceName))
                    {
                        e = VobTypes::createItem(*this, v.oCItem.instanceName);

                        vob = Vob::asVob(*this, e);

                        allowCollision = false;
                    }
                    else
                    {
                        LogWarn() << "Invalid item instance: " << v.oCItem.instanceName;
                    }
                }
                else if (v.objectClass.find("oCMobInter:oCMOB") != std::string::npos)
                {
                    e = VobTypes::createMob(*this);
                    VobTypes::MobVobInformation mob = VobTypes::asMobVob(*this, e);
                    mob.mobController->initFromVobDescriptor(v);

                    vob = Vob::asVob(*this, e);
                }
                else if (v.objectClass.find("zCVobSound") != std::string::npos)
                {
                    e = VobTypes::createSound(*this);

                    VobTypes::SoundVobInformation snd = VobTypes::asSoundVob(*this, e);
                    snd.soundController->initFromVobDescriptor(v);

                    vob = Vob::asVob(*this, e);
                }
                else if (v.objectClass == "oCZoneMusic:zCVob")
                {
                    e = VobTypes::createMusic(*this);

                    VobTypes::MusicVobInformation mus = VobTypes::asMusicVob(*this, e);
                    mus.musicController->initFromVobDescriptor(v);

                    vob = Vob::asVob(*this, e);
                }
                else if (v.objectClass == "oCZoneMusicDefault:oCZoneMusic:zCVob")
                {
                    std::string zoneName = v.vobName.substr(v.vobName.find('_') + 1);
                    Logic::MusicController::setDefaultZone(zoneName);

                    LogInfo() << "Found default music zone: " << v.vobName;
                }
                else
                {
                    // Normal zCVob or not implemented subclass
                    e = Vob::constructVob(*this);
                    vob = Vob::asVob(*this, e);
                }

                if (!vob.isValid())
                    continue;

                // Setup
                if (!v.vobName.empty())
                {
                    Vob::setName(vob, v.vobName);

                    // Add to name-map
                    m_VobsByNames[v.vobName] = e;
                }

                // Set position
                Math::Matrix m = Math::Matrix(v.worldMatrix.mv);
                m.Translation(m.Translation() * (1.0f / 100.0f));
                Vob::setTransform(vob, m);

                // TODO: Need those without visual as well!
                if (v.visual.empty())
                {
                    // Check for startingpoint
                    if (v.objectClass == "zCVobStartpoint:zCVob")
                    {
                        startPoint = v;
                    }

                    // Check for freepoint
                    if (v.objectClass == "zCVobSpot:zCVob")
                    {
                        // Register freepoint
                        Components::SpotComponent& spot = Components::Actions::initComponent<Components::SpotComponent>(getComponentAllocator(), vob.entity);

                        m_FreePoints[v.vobName] = vob.entity;
                    }
                }
                else
                {
                    // Set whether we want collision with this vob
                    Vob::setCollisionEnabled(vob, v.cdDyn && allowCollision);

                    Utils::BBox3D bbox = {Math::float3(v.bbox[0].v) * (1.0f / 100.0f),
                                          Math::float3(v.bbox[1].v) * (1.0f / 100.0f)};

                    //LogInfo() << "Vobsize (" << v.visual << "): " << (bbox.max - bbox.min).length() / 10.0f;
                    vob.position->m_DrawDistanceFactor = std::max(0.12f, std::min(1.0f, (bbox.max - bbox.min).length() / 10.0f));
#ifdef ANDROID
                    vob.position->m_DrawDistanceFactor *= 0.6f;
#endif
                    //LogInfo() << "DistanceFactor (" << v.visual << "): " << vob.position->m_DrawDistanceFactor;

                    Vob::setVisual(vob, v.visual);

                    //if(!vob.visual)
                    //    LogInfo() << "No visual for: " << v.visual;

                    Vob::setBBox(vob, Math::float3(v.bbox[0].v) * (1.0f / 100.0f) - m.Translation(),
                                 Math::float3(v.bbox[1].v) * (1.0f / 100.0f) - m.Translation(),
                                 0 /*vob.visual ? 0 : 0xFF00AA00*/);

                    // Trace down from this vob to get the shadow-value from the worldmesh
                    Math::float3 traceStart = Math::float3(m.Translation().x, v.bbox[1].y * (1.0f / 100.0f), m.Translation().z);
                    Math::float3 traceEnd = Math::float3(m.Translation().x, (v.bbox[0].y * (1.0f / 100.0f)) - 5.0f, m.Translation().z);
                    Physics::RayTestResult hit = m_ClassContents->physicsSystem.raytrace(traceStart, traceEnd,
                                                                          Physics::CollisionShape::CT_WorldMesh);  // FIXME: Use boundingbox for this

                    if (hit.hasHit)
                    {
                        float shadow = m_ClassContents->worldMesh.interpolateTriangleShadowValue(hit.hitTriangleIndex, hit.hitPosition);

                        if (Vob::getVisual(vob))
                            Vob::getVisual(vob)->setShadowValue(shadow);
                    }
                    else
                    {
                        if (Vob::getVisual(vob))
                            Vob::getVisual(vob)->setShadowValue(0.6);
                    }
                }
            }
        };

        // Notify user
        m_pEngine->getHud().getLoadingScreen().startSection(
            LOAD_SECTION_VOBS.p1,
            LOAD_SECTION_VOBS.p2,
            LOAD_SECTION_VOBS.info);

        bool worldUnknownToPlayer = worldJson.empty();
        if (worldUnknownToPlayer)
        {
            // Load vobs from zen (initial load)
            LogInfo() << "Inserting vobs from zen...";
            vobLoad(world.rootVobs);
        }
        else
        {
            // Load vobs from saved json (Savegame)
            LogInfo() << "Inserting vobs from json...";
            importVobs(worldJson["vobs"]);
        }

        LogInfo() << "Done!";

        // Make sure static collision is initialized before adding the NPCs
        m_ClassContents->physicsSystem.postProcessLoad();

        // Load waynet
        m_ClassContents->waynet = Waynet::makeWaynetFromZen(world);

        // Insert startpoint as a waypoint with the name zCVobStartpoint:zCVob.
        if (!startPoint.objectClass.empty())
        {
            Waynet::Waypoint startWP;
            startWP.classname = startPoint.objectClass;
            startWP.direction = Math::float3(startPoint.rotationMatrix.Forward().v);
            startWP.name = startPoint.objectClass;
            startWP.position = (1.0f / 100.0f) * Math::float3(startPoint.position.v);
            startWP.underWater = false;
            startWP.waterDepth = 0;
            Waynet::addWaypoint(m_ClassContents->waynet, startWP);
        }

        // Notify user
        m_pEngine->getHud().getLoadingScreen().startSection(
            LOAD_SECTION_RUNSCRIPTS.p1,
            LOAD_SECTION_RUNSCRIPTS.p2,
            LOAD_SECTION_RUNSCRIPTS.info);

        LogInfo() << "Creating AudioWorld";
        // must create AudioWorld before initializeScriptEngineForZenWorld, because startup_<worldname> calls snd_play
        m_ClassContents->audioWorld = std::make_unique<World::AudioWorld>(*m_pEngine,
                                                                          m_pEngine->getAudioEngine(),
                                                                          engine.getVDFSIndex());

        m_pEngine->getHud().getLoadingScreen().setSectionProgress(20);

        // Load script engine if one is provided. Always the case except "start new game"
        if (!scriptEngine.empty())
        {
            // TODO move the importScriptEngine above NPC/ITEM/PARTICLE import
            // no script function should run before importScriptEngine
            getScriptEngine().importScriptEngine(scriptEngine);
        }

        LogInfo() << "Initialize dialog manager";
        // Initialize dialog manager
        if (!getDialogManager().init())
        {
            LogError() << "Failed to initialize dialog manager";
            return false;
        }
        // Load dialogManager if one is provided. Only after loading a savegame
        if (!dialogManager.empty())
        {
            m_ClassContents->dialogManager.importDialogManager(dialogManager);
        }
        // Load logManager if one is provided.
        if (!logManager.empty())
        {
            engine.getSession().getLogManager().importLogManager(logManager);
        }

        m_pEngine->getHud().getLoadingScreen().setSectionProgress(40);

        LogInfo() << "Running startup-scripts";

        // Init script-engine
        initializeScriptEngineForZenWorld(zen.substr(0, zen.find('.')), worldUnknownToPlayer);

        m_pEngine->getHud().getLoadingScreen().setSectionProgress(100);

        // Wait for a bit, so the user sees the loading-bar go to 100%
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    else
    {
        // Dump a list of possible zens
        std::vector<std::string> zenFiles;
        for (const auto& f : m_pEngine->getVDFSIndex().getKnownFiles())
        {
            if (f.find(".ZEN") != std::string::npos)
            {
                zenFiles.push_back(f);
            }
        }

        LogInfo() << "ZEN-Files found in the currently loaded Archives: " << zenFiles;
    }

    // Initialize the sky, so it will get the right values
    m_ClassContents->sky.onWorldNameChanged(getWorldName());

    /*Handle::EntityHandle e = VobTypes::initNPCFromScript(*this, "");

    Logic::PlayerController* cnt = reinterpret_cast<Logic::PlayerController*>(Vob::asVob(*this, e).logic);

    cnt->teleportToWaypoint(493);

    std::vector<size_t> routine;
    routine.push_back(493);

    size_t wp = 493;
    for(int i=0;i<40;i++)
    {
        for(int j=0;j<(rand() % 50) + 20;j++)
            wp = m_ClassContents->waynet.waypoints[wp].edges[rand() % m_ClassContents->waynet.waypoints[wp].edges.size()];

        routine.push_back(wp);
    }

    cnt->setDailyRoutine(routine);

    m_TestEntity = e;*/

    if (!getEngine()->getEngineArgs().testVisual.empty())
    {
        LogInfo() << "Testing visual: " << getEngine()->getEngineArgs().testVisual;
        Handle::EntityHandle e = Vob::constructVob(*this);
        Vob::VobInformation vob = Vob::asVob(*this, e);

        Vob::setVisual(vob, getEngine()->getEngineArgs().testVisual);
    }

    LogInfo() << "Done loading world!";
    return true;
}

void WorldInstance::initializeScriptEngineForZenWorld(const std::string& worldName, bool firstStart)
{
    if (!worldName.empty())
    {
        LogInfo() << "Initializing scripts for world: " << worldName;
        getScriptEngine().initForWorld(worldName, firstStart);
    }
    LogInfo() << "Script-initialization done!";
}

Handle::EntityHandle WorldInstance::addEntity(Components::ComponentMask components)
{
    auto h = m_Allocators->m_ComponentAllocator.createObject();

    Components::EntityComponent& entity = m_Allocators->m_ComponentAllocator.getElement<Components::EntityComponent>(h);
    entity.m_ComponentMask = components;
    entity.m_ThisEntity = h;

    Components::Actions::forAllComponents(m_Allocators->m_ComponentAllocator, h, [&](auto& c) {
        c.init(c);
    });

    /*Components::BBoxComponent& bbox = m_Allocators->m_ComponentAllocator.getElement<Components::BBoxComponent>(h);
    bbox.m_BBox3D.min = -1.0f * Math::float3(rand() % 1000,rand() % 1000,rand() % 1000);
    bbox.m_BBox3D.max = Math::float3(rand() % 1000,rand() % 1000,rand() % 1000);
    entity.m_ComponentMask |= Components::BBoxComponent::MASK;*/

    Components::LogicComponent& logic = m_Allocators->m_ComponentAllocator.getElement<Components::LogicComponent>(h);
    logic.m_pLogicController = nullptr;

    // TODO: Make generic "on entity created"-method or something

    return h;
}

void WorldInstance::onFrameUpdate(double deltaTime, float updateRangeSquared, const Math::Matrix& cameraWorld)
{
    // Tell script engine the frame started
    m_ClassContents->scriptEngine.onFrameStart();

    // Update physics
    m_ClassContents->physicsSystem.update(deltaTime);

    // Update sky
    m_ClassContents->sky.interpolate();

    size_t num = getComponentAllocator().getNumObtainedElements();
    const auto& ctuple = getComponentDataBundle().m_Data;

    Components::EntityComponent* ents = std::get<Components::EntityComponent*>(ctuple);
    Components::StaticMeshComponent* sms = std::get<Components::StaticMeshComponent*>(ctuple);
    Components::LogicComponent* logics = std::get<Components::LogicComponent*>(ctuple);
    Components::AnimationComponent* anims = std::get<Components::AnimationComponent*>(ctuple);
    Components::PositionComponent* positions = std::get<Components::PositionComponent*>(ctuple);
    Components::VisualComponent* visuals = std::get<Components::VisualComponent*>(ctuple);

    //#pragma omp parallel for
    for (size_t i = 0; i < num; i++)
    {
        // Simple distance-check // TODO: Frustum/Occlusion-Culling
        if (Components::hasComponent<Components::PositionComponent>(ents[i]))
        {
            if ((positions[i].m_WorldMatrix.Translation() - cameraWorld.Translation()).lengthSquared() >
                updateRangeSquared * positions[i].m_DrawDistanceFactor)
                continue;
        }

        Components::ComponentMask mask = ents[i].m_ComponentMask;
        if (Components::hasComponent<Components::LogicComponent>(ents[i]))
        {
            if (logics[i].m_pLogicController)
            {
                logics[i].m_pLogicController->onUpdate(deltaTime);
            }
        }

        if (Components::hasComponent<Components::VisualComponent>(ents[i]))
        {
            if (visuals[i].m_pVisualController)
            {
                visuals[i].m_pVisualController->onUpdate(deltaTime);
            }
        }

        // Update animations, only if there isn't a valid parent registered
        if (Components::hasComponent<Components::AnimationComponent>(ents[i]) && !anims[i].m_ParentAnimHandler.isValid())
        {
            anims[i].getAnimHandler().updateAnimations(deltaTime);
        }
    }

    // TODO: Move this somewhere else, where other game-logic is!
    // TODO: Must be done before the main-camera gets updated, actually
    if (m_ClassContents->scriptEngine.getPlayerEntity().isValid())
    {
        VobTypes::NpcVobInformation player = VobTypes::asNpcVob(*this, m_ClassContents->scriptEngine.getPlayerEntity());

        if (player.playerController)
            player.playerController->onUpdateByInput(deltaTime);
    }

    // Update sound-listener position
    getAudioWorld().setListenerPosition(getCameraController()->getEntityTransform().Translation());
    //getAudioWorld().setListenerVelocity(); // don't need this for now, no need for the Doppler effect
    const auto& camMatrix = getCameraController()->getEntityTransform();
    getAudioWorld().setListenerOrientation(camMatrix.Forward(), camMatrix.Up());

    // Update dialogs
    m_ClassContents->dialogManager.update(deltaTime);

    // Tell script engine the frame ended
    m_ClassContents->scriptEngine.onFrameEnd();

    // Update hud
    m_pEngine->getHud().setDateTimeDisplay(m_pEngine->getGameClock().getDateTimeFormatted());

    m_ClassContents->bspTree.debugDraw();

    /*for(const auto& fp : m_FreePoints)
    {
        Math::float3 fpPosition = getEntity<Components::PositionComponent>(fp.second).m_WorldMatrix.Translation();
        ddDrawAxis(fpPosition.x, fpPosition.y, fpPosition.z, 0.5f);
    }*/
}

void WorldInstance::removeEntity(Handle::EntityHandle h)
{
    if(!isEntityValid(h))
    {
        // FIXME: Reason for this is, that on the final world-clean, entities are removed in the wrong order.
        // FIXME: Thus, when cleaning complex objects, there sub-entites might already be deleted and so their handle has become invalid.

        LogWarn() << "Tried to delete an entity with an invalid handle!";
        return; // Already deleted!
    }

    // Clean all components
    Components::EntityComponent& entityComponent = getEntity<Components::EntityComponent>(h);

    Components::Actions::forAllComponents(getComponentAllocator(), h, [&](auto& c) {
        Components::Actions::destroyComponent(c);
    });

    getComponentAllocator().removeObject(h);
}

std::vector<size_t> WorldInstance::findStartPoints()
{
    std::vector<size_t> pts;

    // General

    /*
    // Gothic 1
    auto it = m_ClassContents->waynet.waypointsByName.find("WP_INTRO_SHORE");
    if(it != m_ClassContents->waynet.waypointsByName.end())
        pts.push_back((*it).second);
     */

    auto it = m_ClassContents->waynet.waypointsByName.find("zCVobStartpoint:zCVob");
    if (it != m_ClassContents->waynet.waypointsByName.end())
        pts.push_back((*it).second);

    if (!pts.empty())
        return pts;

    for (size_t i = 0; i < m_ClassContents->waynet.waypoints.size(); i++)
    {
        // From spacer-doc:
        // Der Name eines Startpoints muss den Instanznamen des Spielers enthalten, der an diesem Punkt starten soll.
        // Format: Beispiel: START_PC_HERO für die Instanz "PC_HERO" oder START_OLD_MAN ...
        // => Name of a startpoint always starts with "START_"

        // Doesn't actually seem the case, tho!

        // Take anything with START for now
        if (m_ClassContents->waynet.waypoints[i].name.find("START") != std::string::npos)
            pts.push_back(i);
    }

    return pts;
}

std::vector<Handle::EntityHandle>
WorldInstance::getFreepointsInRange(const Math::float3& center, float distance, const std::string& name, bool closestOnly,
                                    Handle::EntityHandle inst)
{
    std::vector<Handle::EntityHandle> m;

    Handle::EntityHandle closestFP;

    float closest2 = FLT_MAX;
    float distance2 = distance * distance;
    std::vector<Handle::EntityHandle> fps = getFreepoints(name);
    for (auto& fp : fps)
    {
        Components::SpotComponent& sp = getEntity<Components::SpotComponent>(fp);
        Components::PositionComponent& pos = getEntity<Components::PositionComponent>(fp);

        if (!isFreepointOccupied(fp))
        {
            float fpd2 = (center - pos.m_WorldMatrix.Translation()).lengthSquared();
            if (fpd2 < distance2)
            {
                if (fpd2 < closest2)
                {
                    closest2 = fpd2;
                    closestFP = fp;
                }
            }

            if (!closestOnly)
                m.push_back(fp);
        }

    }

    if (closestOnly)
        return closestFP.isValid() ? std::vector<Handle::EntityHandle>({closestFP}) : std::vector<Handle::EntityHandle>();
    else
        return m;
}

std::vector<Handle::EntityHandle> WorldInstance::getFreepoints(const std::string& tag)
{
    std::vector<Handle::EntityHandle> mp;

    auto cacheIt = m_FreePointTagCache.find(tag);
    if(cacheIt != m_FreePointTagCache.end())
        return cacheIt->second;

    // Tag not cached, do full search now and cache it
    for(auto& fp : m_FreePoints)
    {
        if(fp.first.find(tag) != std::string::npos)
            mp.push_back(fp.second);
    }

    m_FreePointTagCache[tag] = mp;

    return mp;
}

bool WorldInstance::doesFreepointExist(const std::string& name)
{
    return m_FreePoints.find(name) != m_FreePoints.end();

}

Handle::EntityHandle WorldInstance::getFreepoint(const std::string& name)
{
    if(!doesFreepointExist(name))
        return Handle::EntityHandle::makeInvalidHandle();

    return m_FreePoints[name];
}

Math::float3 WorldInstance::getFreepointPosition(const std::string& name)
{
    Handle::EntityHandle fp = getFreepoint(name);

    if(!fp.isValid())
        return Math::float3(0,0,0);

    Components::PositionComponent& pos = getEntity<Components::PositionComponent>(fp);
    return pos.m_WorldMatrix.Translation();
}

void WorldInstance::markFreepointOccupied(Handle::EntityHandle freepoint, Handle::EntityHandle usingEntity,
                                      float occupiedForSeconds)
{
    using namespace Components;

    assert(hasComponent<SpotComponent>(getEntity<EntityComponent>(freepoint)));

    Components::SpotComponent& sp = getEntity<Components::SpotComponent>(freepoint);
    sp.m_UsingEntity = usingEntity;
    sp.m_UseEndTime = getEngine()->getGameClock().getTotalSeconds() + occupiedForSeconds;
}

bool WorldInstance::isFreepointOccupied(Handle::EntityHandle freepoint)
{
    using namespace Components;

    assert(hasComponent<SpotComponent>(getEntity<EntityComponent>(freepoint)));

    Components::SpotComponent& sp = getEntity<Components::SpotComponent>(freepoint);

    return sp.m_UsingEntity.isValid() && sp.m_UseEndTime > getEngine()->getGameClock().getTotalSeconds();
}

Daedalus::GameType WorldInstance::getBasicGameType()
{
    return m_pEngine->getBasicGameType();
}

void WorldInstance::exportWorld(json& j, std::set<Handle::EntityHandle> skip)
{
    // Write initial ZEN for loading the worldmesh later
    j["zenfile"] = m_ZenFile;

    // Write Vobs
    {
        json& jvobs = j["vobs"];

        size_t num = getComponentAllocator().getNumObtainedElements();
        const auto& ctuple = getComponentDataBundle().m_Data;

        Components::EntityComponent* ents = std::get<Components::EntityComponent*>(ctuple);
        Components::LogicComponent* logics = std::get<Components::LogicComponent*>(ctuple);
        Components::VisualComponent* visuals = std::get<Components::VisualComponent*>(ctuple);

        // TODO: This could be done in parallel
        for (size_t i = 0; i < num; i++)
        {
            if (skip.find(ents[i].m_ThisEntity) != skip.end())
                continue;
            Logic::Controller* logicController = nullptr;
            Logic::VisualController* visualController = nullptr;
            if (Components::hasComponent<Components::LogicComponent>(ents[i]))
                logicController = logics[i].m_pLogicController;
            if (Components::hasComponent<Components::VisualComponent>(ents[i]))
                visualController = visuals[i].m_pVisualController;

            // Do the actual export
            exportControllers(logicController, visualController, jvobs["controllers"][i]);
        }
    }
}

Handle::EntityHandle WorldInstance::importSingleVob(const json& j)
{
    // This has a logic and visual controller

    Handle::EntityHandle entity;

    if (j.find("logic") != j.end())
    {
        // TODO: Automate this...
        if (j["logic"]["type"] == "PlayerController")
        {
            entity = Logic::PlayerController::importPlayerController(*this, j["logic"]);
        }
        else if (j["logic"]["type"] == "MobController")
        {
            entity = VobTypes::createMob(*this);
            VobTypes::MobVobInformation vob = VobTypes::asMobVob(*this, entity);
            vob.mobController->importObject(j["logic"]);
        }
        else if (j["logic"]["type"] == "ItemController")
        {
            size_t sym = j["logic"]["instanceSymbol"];
            entity = VobTypes::createItem(*this, sym);
        }
    }
    else
    {
        entity = Vob::constructVob(*this);
    }

    if (!entity.isValid())
        return Handle::EntityHandle::makeInvalidHandle();

    if (j.find("visual") != j.end())
    {
        Vob::VobInformation vob = Vob::asVob(*this, entity);

        // Need to enable collision before setting the visual
        Vob::setCollisionEnabled(vob, j["visual"]["collision"]);

        // Get transform before setting the visual, so staic collision doesn't end up at world-origin
        // TODO: There must be a nicer way of writing this
        auto& jtrans = j["visual"]["transform"];
        Math::Matrix transform;

        // Parse transform from json
        for (int i = 0; i < 16; i++)
            if (!jtrans[i].is_null())
                transform.mv[i] = jtrans[i];

        Vob::setTransform(vob, transform);

        // Need to set the visual before actually importing the object
        Vob::setVisual(vob, j["visual"]["name"]);
        vob = Vob::asVob(*this, entity);

        // Now proceed with the other values
        if (vob.visual)
            vob.visual->importObject(j["visual"]);
    }
    return entity;
}

void WorldInstance::importVobs(const json& j)
{
    // j must be an array of vobs
    size_t numImported = 0;
    size_t numTotal = j["controllers"].size();
    for (const json& vob : j["controllers"])
    {
        if (!vob.is_null())
        {
            importSingleVob(vob);
        }

        numImported++;
        m_pEngine->getHud().getLoadingScreen().setSectionProgress((100 * (int)numImported) / (int)numTotal);
    }
}

bool WorldInstance::isEntityValid(Handle::EntityHandle e)
{
    return m_Allocators->m_ComponentAllocator.isHandleValid(e);
}

Handle::EntityHandle WorldInstance::createCamera()
{
    // Add player-camera
    m_Camera = addEntity(Components::PositionComponent::MASK);

    Math::Matrix lookAt = Math::Matrix::CreateLookAt(Math::float3(0, 0, 0), Math::float3(1, 0, 0), Math::float3(0, 1, 0));
    getCameraComp<Components::PositionComponent>().m_WorldMatrix = lookAt.Invert();

    getCameraComp<Components::PositionComponent>().m_WorldMatrix.Translation(Math::float3(0.0f, 50.0f, 50.0f));

    Components::LogicComponent& logic = Components::Actions::initComponent<Components::LogicComponent>(
        getComponentAllocator(),
        m_Camera);

    Logic::CameraController* cam = new Logic::CameraController(*this, m_Camera);
    logic.m_pLogicController = cam;

    LogInfo() << "Setting camera mode to third-person";
    cam->setCameraMode(Logic::CameraController::ECameraMode::ThirdPerson);
    //cam->getCameraSettings().freeCameraSettings.moveSpeed = 5.0f;
    //cam->getCameraSettings().freeCameraSettings.turnSpeed = 3.5f;

    return m_Camera;
}

Handle::EntityHandle WorldInstance::getCamera()
{
    return m_Camera;
}

json WorldInstance::exportAndRemoveNPC(Handle::EntityHandle entityHandle)
{
    json j = exportNPC(entityHandle);
    VobTypes::Wld_RemoveNpc(*this, entityHandle);
    return j;
}

void WorldInstance::exportControllers(Logic::Controller* logicController, Logic::VisualController* visualController, json& j)
{
    if (logicController && logicController->shouldExport())
        logicController->exportObject(j["logic"]);
    if (visualController && visualController->shouldExport())
        visualController->exportObject(j["visual"]);
}

void WorldInstance::takeControlOver(Handle::EntityHandle entityHandle)
{
    VobTypes::NpcVobInformation newPlayer = VobTypes::asNpcVob(*this, entityHandle);
    getScriptEngine().setPlayerEntity(entityHandle);
    getScriptEngine().setInstanceNPC("hero", VobTypes::getScriptHandle(newPlayer));
}

Handle::EntityHandle WorldInstance::importVobAndTakeControl(const json& j)
{
    auto player = importSingleVob(j);
    takeControlOver(player);
    return player;
}

json WorldInstance::exportNPC(Handle::EntityHandle entityHandle)
{
    VobTypes::NpcVobInformation playerVob = VobTypes::asNpcVob(*this, entityHandle);
    json j;
    exportControllers(playerVob.playerController, playerVob.visual, j);
    return j;
}

UI::PrintScreenMessages& WorldInstance::getPrintScreenManager()
{
    return m_pEngine->getHud().getPrintScreenManager();
}

const std::map<std::string, Handle::EntityHandle>& WorldInstance::getFreepoints() const
{
    return m_FreePoints;
}

const Waynet::WaynetInstance& WorldInstance::getWaynet()
{
    return m_ClassContents->waynet;
}

std::string WorldInstance::getWorldName()
{
    return Utils::stripExtension(m_ZenFile);
}

Logic::ScriptEngine& WorldInstance::getScriptEngine()
{
    return m_ClassContents->scriptEngine;
}

Physics::PhysicsSystem& WorldInstance::getPhysicsSystem()
{
    return m_ClassContents->physicsSystem;
}

WorldMesh& WorldInstance::getWorldMesh()
{
    return m_ClassContents->worldMesh;
}

Content::Sky& WorldInstance::getSky()
{
    return m_ClassContents->sky;
}

Logic::DialogManager& WorldInstance::getDialogManager()
{
    return m_ClassContents->dialogManager;
}

World::AudioWorld& WorldInstance::getAudioWorld()
{
    return *m_ClassContents->audioWorld;
}

Logic::PfxManager& WorldInstance::getPfxManager()
{
    return m_ClassContents->pfxManager;
}

Animations::AnimationLibrary& WorldInstance::getAnimationLibrary()
{
    return m_ClassContents->animationLibrary;
}

Components::ComponentAllocator::DataBundle WorldInstance::getComponentDataBundle()
{
    return m_Allocators->m_ComponentAllocator.getDataBundle();
}

Textures::TextureAllocator& WorldInstance::getTextureAllocator()
{
    return m_Allocators->m_LevelTextureAllocator;
}

Components::ComponentAllocator& WorldInstance::getComponentAllocator()
{
    return m_Allocators->m_ComponentAllocator;
}

Meshes::StaticMeshAllocator& WorldInstance::getStaticMeshAllocator()
{
    return m_Allocators->m_LevelStaticMeshAllocator;
}

Meshes::SkeletalMeshAllocator& WorldInstance::getSkeletalMeshAllocator()
{
    return m_Allocators->m_LevelSkeletalMeshAllocator;
}

Animations::AnimationAllocator& WorldInstance::getAnimationAllocator()
{
    return m_Allocators->m_AnimationAllocator;
}

Animations::AnimationDataAllocator& WorldInstance::getAnimationDataAllocator()
{
    return m_Allocators->m_AnimationDataAllocator;
}

World::WorldAllocators& WorldInstance::getAllocators()
{
    return *m_Allocators;
}

Logic::CameraController* WorldInstance::getCameraController()
{
    Logic::Controller* ptr = getCameraComp<Components::LogicComponent>().m_pLogicController;
    return dynamic_cast<Logic::CameraController*>(ptr);
}