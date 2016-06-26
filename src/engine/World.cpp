#include <iostream>
#include "World.h"
#include <bitset>
#include <zenload/zenParser.h>
#include <zenload/zCMesh.h>
#include "BaseEngine.h"
#include "GameEngine.h"
#include <content/ContentLoad.cpp>
#include <utils/logger.h>
#include <components/EntityActions.h>
#include <logic/PlayerController.h>
#include <stdlib.h>

#include <engine/GameEngine.h>
#include <debugdraw/debugdraw.h>
#include <components/Vob.h>
#include <components/VobClasses.h>

using namespace World;

WorldInstance::WorldInstance()
	: m_WorldMesh(*this),
      m_ScriptEngine(*this)
{
	
}

void WorldInstance::init(Engine::BaseEngine& engine)
{
	m_Allocators.m_LevelTextureAllocator.setVDFSIndex(&engine.getVDFSIndex());
    m_Allocators.m_LevelStaticMeshAllocator.setVDFSIndex(&engine.getVDFSIndex());
    m_Allocators.m_LevelSkeletalMeshAllocator.setVDFSIndex(&engine.getVDFSIndex());

    m_pEngine = &engine;
}

void WorldInstance::init(Engine::BaseEngine& engine, const std::string& zen)
{
	// Call other overload
	init(engine);

	// Load ZEN
	std::vector<uint8_t> data;
	engine.getVDFSIndex().getFileData(zen, data);

	ZenLoad::ZenParser parser(data.data(), data.size());

	parser.readHeader();
	ZenLoad::oCWorldData world = parser.readWorld();

	ZenLoad::zCMesh* worldMesh = parser.getWorldMesh();
	ZenLoad::PackedMesh packedWorldMesh;
	worldMesh->packMesh(packedWorldMesh, 0.01f);

	//m_WorldMesh.load(packed);

    Handle::MeshHandle worldMeshHandle = getStaticMeshAllocator().loadFromPacked(packedWorldMesh);
    Meshes::WorldStaticMesh& worldMeshData = getStaticMeshAllocator().getMesh(worldMeshHandle);

    // TODO: Put these into a compound-component or something
    std::vector<Handle::EntityHandle> ents = Content::entitifyMesh(*this, worldMeshHandle, worldMeshData);

    for(Handle::EntityHandle e : ents)
    {
        // Init positions
        Components::EntityComponent& entity = getEntity<Components::EntityComponent>(e);
        Components::addComponent<Components::PositionComponent>(entity);

        // Copy world-matrix
        Components::PositionComponent& pos = getEntity<Components::PositionComponent>(e);
        pos.m_WorldMatrix = Math::Matrix::CreateIdentity();
        pos.m_WorldMatrix.Translation(pos.m_WorldMatrix.Translation() * (1.0f / 100.0f));
        pos.m_DrawDistanceFactor = 0.0f; // Always draw the worldmesh
    }


    // Load vobs
    // TODO: Refractor
    /*std::function<void(const std::vector<ZenLoad::zCVobData>)> vobLoad = [&](const std::vector<ZenLoad::zCVobData>& vobs) {
        for (const ZenLoad::zCVobData& v : vobs)
        {
            vobLoad(v.childVobs);

            // Check if we can render this
            if(v.visual.find(".3DS") == std::string::npos)
                continue;

            Handle::MeshHandle msh = getStaticMeshAllocator().loadMeshVDF(v.visual);

            if(!msh.isValid())
                continue;

            Meshes::WorldStaticMesh& mdata = getStaticMeshAllocator().getMesh(msh);

            // TODO: Put these into a compound-component or something
            std::vector<Handle::EntityHandle> ents = Content::entitifyMesh(*this, msh, mdata);

            for(Handle::EntityHandle e : ents)
            {
                // Init positions
                Components::EntityComponent& entity = getEntity<Components::EntityComponent>(e);
                Components::addComponent<Components::PositionComponent>(entity);

                // Copy world-matrix
                Components::PositionComponent& pos = getEntity<Components::PositionComponent>(e);
                pos.m_WorldMatrix = Math::Matrix(v.worldMatrix.mv);
                pos.m_WorldMatrix.Translation(pos.m_WorldMatrix.Translation() * (1.0f / 100.0f));
            }
        }
    };*/

    std::function<void(const std::vector<ZenLoad::zCVobData>)> vobLoad = [&](const std::vector<ZenLoad::zCVobData>& vobs) {
        for (const ZenLoad::zCVobData& v : vobs)
        {
            vobLoad(v.childVobs);

            if(v.visual.empty())
                continue; // TODO: We need those as well!

            // Make vob
            Handle::EntityHandle e = Vob::constructVob(*this);
            Vob::VobInformation vob = Vob::asVob(*this, e);

            // Setup
            Vob::setName(vob, v.vobName);

            Math::Matrix m = Math::Matrix(v.worldMatrix.mv);
            m.Translation(m.Translation() * (1.0f / 100.0f));
            Vob::setTransform(vob, m);
            Vob::setVisual(vob, v.visual);

            //if(!vob.visual)
            //    LogInfo() << "No visual for: " << v.visual;

            Vob::setBBox(vob, Math::float3(v.bbox[0].v) * (1.0f / 100.0f) - m.Translation(),
                         Math::float3(v.bbox[1].v) * (1.0f / 100.0f) - m.Translation(),
                         vob.visual ? 0 : 0xFF00AA00);

        }
    };

    vobLoad(world.rootVobs);

    // Load waynet
    m_Waynet = Waynet::makeWaynetFromZen(world);

    // Init script-engine
    initializeScriptEngineForZenWorld(zen.substr(0, zen.find('.')));

    /*Handle::EntityHandle e = VobTypes::initNPCFromScript(*this, "");

    Logic::PlayerController* cnt = reinterpret_cast<Logic::PlayerController*>(Vob::asVob(*this, e).logic);

    cnt->teleportToWaypoint(493);

    std::vector<size_t> routine;
    routine.push_back(493);

    size_t wp = 493;
    for(int i=0;i<40;i++)
    {
        for(int j=0;j<(rand() % 50) + 20;j++)
            wp = m_Waynet.waypoints[wp].edges[rand() % m_Waynet.waypoints[wp].edges.size()];

        routine.push_back(wp);
    }

    cnt->setDailyRoutine(routine);

    m_TestEntity = e;*/
}

void WorldInstance::initializeScriptEngineForZenWorld(const std::string& worldName)
{
    // Default to the path G2 uses
    std::string datFile = Utils::getCaseSensitivePath(m_pEngine->getEngineArgs().gameBaseDirectory,"/_work/data/Scripts/_compiled/GOTHIC.DAT");

    // Check G2 variant
    if(Utils::fileExists(datFile))
    {
        m_ScriptEngine.loadDAT(datFile);
    } else
    {
        // Check G1 variant
        datFile = Utils::getCaseSensitivePath(m_pEngine->getEngineArgs().gameBaseDirectory,"/_work/DATA/scripts/_compiled/GOTHIC.DAT");
        if(Utils::fileExists(datFile))
        {
            m_ScriptEngine.loadDAT(datFile);
        } else
        {
            LogError() << "Failed to find GOTHIC.DAT!";

#if !defined(WIN32) && !defined(_WIN32)
            LogInfo() << "The path to the GOTHIC.DAT is case sensitive and must be exactly like this:" << std::endl
                      << "   - /_work/DATA/scripts/_compiled/GOTHIC.DAT" ;
#endif
        }
    }
    LogInfo() << "Initializing scripts for world: " << worldName;
    m_ScriptEngine.initForWorld(worldName);
}

Components::ComponentAllocator::Handle WorldInstance::addEntity(Components::ComponentMask components)
{
    auto h = m_Allocators.m_ComponentAllocator.createObject();

    Components::Actions::forAllComponents(m_Allocators.m_ComponentAllocator, h, [&](auto& c)
    {
        c.init(c);
    });

    Components::EntityComponent& entity = m_Allocators.m_ComponentAllocator.getElement<Components::EntityComponent>(h);
    entity.m_ComponentMask = components;

    /*Components::BBoxComponent& bbox = m_Allocators.m_ComponentAllocator.getElement<Components::BBoxComponent>(h);
    bbox.m_BBox3D.min = -1.0f * Math::float3(rand() % 1000,rand() % 1000,rand() % 1000);
    bbox.m_BBox3D.max = Math::float3(rand() % 1000,rand() % 1000,rand() % 1000);
    entity.m_ComponentMask |= Components::BBoxComponent::MASK;*/

    Components::LogicComponent& logic = m_Allocators.m_ComponentAllocator.getElement<Components::LogicComponent>(h);
    logic.m_pLogicController = nullptr;

    // TODO: Make generic "on entity created"-method or something

    return h;
}

void WorldInstance::onFrameUpdate(double deltaTime, float updateRangeSquared, const Math::Matrix& cameraWorld)
{
    /*Components::LogicComponent& logic = m_Allocators.m_ComponentAllocator.getElement<Components::LogicComponent>(m_TestEntity);
    Logic::PlayerController* ai = reinterpret_cast<Logic::PlayerController*>(logic.m_pLogicController);

    // FIXME: Debug only, take out!
    Math::float3 cp = reinterpret_cast<Engine::GameEngine*>(m_pEngine)->getMainCameraController()->getEntityTransform().Translation();
    size_t nearest = Waynet::findNearestWaypointTo(m_Waynet, cp);

    bgfx::dbgTextPrintf(0, 4, 0x0f, "Camera: %s", cp.toString().c_str());


    if(nearest != static_cast<size_t>(-1))
    {
        Math::float3 npos = m_Waynet.waypoints[nearest].position;

        bgfx::dbgTextPrintf(0, 5, 0x0f, "Nearest wp: (%d) %s", nearest, npos.toString().c_str());

        ddPush();
            ddSetColor(0xFFFFFFFF);
            ddDrawOrb(npos.x, npos.y, npos.z, 1.0f);
        ddPop();

        ai->gotoWaypoint(nearest);
    }
*/

    size_t num = getComponentAllocator().getNumObtainedElements();
    const auto& ctuple = getComponentDataBundle().m_Data;

    Components::EntityComponent* ents = std::get<Components::EntityComponent*>(ctuple);
    Components::LogicComponent* logics = std::get<Components::LogicComponent*>(ctuple);
    Components::AnimationComponent* anims = std::get<Components::AnimationComponent*>(ctuple);
    Components::PositionComponent* positions = std::get<Components::PositionComponent*>(ctuple);
    for (size_t i = 0; i<num; i++)
    {
        // Simple distance-check // TODO: Frustum/Occlusion-Culling
        if(Components::hasComponent<Components::PositionComponent>(ents[i]))
        {
            if ((positions[i].m_WorldMatrix.Translation() - cameraWorld.Translation()).lengthSquared() *
                positions[i].m_DrawDistanceFactor > updateRangeSquared)
                continue;
        }

        Components::ComponentMask mask = ents[i].m_ComponentMask;
        if((mask & Components::LogicComponent::MASK) != 0)
        {
            if(logics[i].m_pLogicController)
            {
                logics[i].m_pLogicController->onUpdate(deltaTime);
            }
        }

        // Update animations, only if there isn't a valid parent registered
        if((mask & Components::AnimationComponent::MASK) != 0
           && !anims[i].m_ParentAnimHandler.isValid())
        {
            anims[i].m_AnimHandler.updateAnimations(deltaTime);
        }
    }

    /*static float s_testp = 0.0f;
    static std::vector<size_t> path;
    static float pathLength;

    if(m_Waynet.waypoints.empty())
        return;

    if(path.empty())
    {
        path = World::Waynet::findWay(m_Waynet,
                                      493,
                                      342);

        pathLength = World::Waynet::getPathLength(m_Waynet, path);
    }

    s_testp += deltaTime / pathLength * 20.0f;

    if(s_testp > 1.0f)
        s_testp = 0.0f;

    Components::PositionComponent& testPosition = getEntity<Components::PositionComponent>(m_TestEntity);

    testPosition.m_WorldMatrix = Math::Matrix::CreateTranslation(World::Waynet::interpolatePositionOnPath(m_Waynet, path, s_testp));

    //testPosition.m_WorldMatrix = Math::Matrix::CreateTranslation(m_Waynet.waypoints[path[s_testp * path.size()]].position);

    LogInfo() << "Position: [" << s_testp << "]" << testPosition.m_WorldMatrix.Translation().toString();*/

    /*Components::EntityComponent* entities = m_Allocators.m_ComponentAllocator.getElements<Components::EntityComponent>();
    Components::PositionComponent* positions = m_Allocators.m_ComponentAllocator.getElements<Components::PositionComponent>();
    Components::BBoxComponent* bboxes = m_Allocators.m_ComponentAllocator.getElements<Components::BBoxComponent>();
    size_t range = m_Allocators.m_ComponentAllocator.getNumObtainedElements();

    m_TransientEntityFeatures.m_VisibleEntities.clear();

    for (size_t i = 0; i < range; i++)
    {
        // Try to cull the entities
        // TODO: Can do this in paralell! (Don't forget to aggregate m_VisibleEntities afterwards against cache-trashing)
        if((entities[i].m_ComponentMask & Components::BBoxComponent::MASK) != 0)
        {
            // TODO: Actually cull

            // Entity is visible, save that
            m_TransientEntityFeatures.m_VisibleEntities.push_back(i);
        }
    }*/
}

void WorldInstance::removeEntity(Handle::EntityHandle h)
{
    // Clean all components
    Components::Actions::forAllComponents(getComponentAllocator(), h, [](auto& c)
    {
        Components::Actions::destroyComponent(c);
    });

    getComponentAllocator().removeObject(h);
}









