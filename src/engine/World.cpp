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
#include <iterator>

#include <engine/GameEngine.h>
#include <debugdraw/debugdraw.h>
#include <components/Vob.h>
#include <components/VobClasses.h>
#include <entry/input.h>
#include <ui/PrintScreenMessages.h>

using namespace World;

WorldInstance::WorldInstance()
	: m_WorldMesh(*this),
      m_ScriptEngine(*this),
      m_PhysicsSystem(*this),
      m_Sky(*this),
      m_DialogManager(*this),
      m_PrintScreenMessageView(nullptr)
{
	
}

void WorldInstance::init(Engine::BaseEngine& engine)
{
	m_Allocators.m_LevelTextureAllocator.setVDFSIndex(&engine.getVDFSIndex());
    m_Allocators.m_LevelStaticMeshAllocator.setVDFSIndex(&engine.getVDFSIndex());
    m_Allocators.m_LevelSkeletalMeshAllocator.setVDFSIndex(&engine.getVDFSIndex());
    m_Allocators.m_AnimationAllocator.setVDFSIndex(&engine.getVDFSIndex());
    m_AudioEngine.setVDFSIndex(&engine.getVDFSIndex());

    m_pEngine = &engine;

    // Create static-collision shape beforehand
    m_StaticWorldObjectCollsionShape = m_PhysicsSystem.makeCompoundCollisionShape(Physics::CollisionShape::CT_Object);
    m_StaticWorldMeshCollsionShape = m_PhysicsSystem.makeCompoundCollisionShape(Physics::CollisionShape::CT_WorldMesh);
    m_StaticWorldObjectPhysicsObject = m_PhysicsSystem.makeRigidBody(m_StaticWorldObjectCollsionShape, Math::Matrix::CreateIdentity());
    m_StaticWorldMeshPhysicsObject = m_PhysicsSystem.makeRigidBody(m_StaticWorldMeshCollsionShape, Math::Matrix::CreateIdentity());

    // Create UI-Views
    m_PrintScreenMessageView = new UI::PrintScreenMessages();
    getEngine()->getRootUIView().addChild(m_PrintScreenMessageView);
}

void WorldInstance::init(Engine::BaseEngine& engine, const std::string& zen)
{
    m_ZenFile = zen;

	// Call other overload
	init(engine);

	// Init daedalus-vm
	std::string datPath = "/_work/data/Scripts/_compiled/GOTHIC.DAT";
	std::string datFile = Utils::getCaseSensitivePath(datPath, m_pEngine->getEngineArgs().gameBaseDirectory);

	if(Utils::fileExists(datFile))
	{
		m_ScriptEngine.loadDAT(datFile);
	} else
	{
		LogError() << "Failed to find GOTHIC.DAT at: " << datFile;
	}

	// Load world
    if(!zen.empty())
    {
        // Load ZEN
        ZenLoad::ZenParser parser(zen, engine.getVDFSIndex());

        parser.readHeader();
        ZenLoad::oCWorldData world;
		parser.readWorld(world);

        ZenLoad::zCMesh *worldMesh = parser.getWorldMesh();

        LogInfo() << "Postprocessing worldmesh...";

        ZenLoad::PackedMesh packedWorldMesh;
        worldMesh->packMesh(packedWorldMesh, 0.01f);

        // Init worldmesh-wrapper
        m_WorldMesh.load(packedWorldMesh);

        Handle::MeshHandle worldMeshHandle = getStaticMeshAllocator().loadFromPacked(packedWorldMesh);
        Meshes::WorldStaticMesh &worldMeshData = getStaticMeshAllocator().getMesh(worldMeshHandle);

        // TODO: Put these into a compound-component or something
        std::vector<Handle::EntityHandle> ents = Content::entitifyMesh(*this, worldMeshHandle, worldMeshData.mesh);

        // If we haven't already, create an instancebuffer for this mesh
        //if(worldMeshData.instanceDataBufferIndex == (uint32_t)-1)
        //    worldMeshData.instanceDataBufferIndex = ((Engine::GameEngine*)m_pEngine)->getDefaultRenderSystem().requestInstanceDataBuffer();

        // Create collisionmesh for the world
        if(!ents.empty())
        {
            LogInfo() << "Generating world collision mesh...";

            // Create world-object using the static collision-shape
            Components::PhysicsComponent& phys = Components::Actions::initComponent<Components::PhysicsComponent>(getComponentAllocator(), ents.front());

            phys.m_PhysicsObject = m_StaticWorldMeshCollsionShape;
            phys.m_IsStatic = true;

            // Create triangle-array
            std::vector<Math::float3> triangles;
            triangles.reserve(packedWorldMesh.vertices.size()); // Note: there are likely more

            for(auto& tri : packedWorldMesh.triangles)
            {
                triangles.push_back(tri.vertices[0].Position.v);
                triangles.push_back(tri.vertices[1].Position.v);
                triangles.push_back(tri.vertices[2].Position.v);

                for(int i=0;i<3;i++)
                {
                    if(Math::float3(tri.vertices[i].Position.v).length() < 100)
                        tri.vertices[i].Color = 0x00000000;
                }
            }

            // Add world-mesh collision
            Handle::CollisionShapeHandle wmch = m_PhysicsSystem.makeCollisionShapeFromMesh(triangles, Physics::CollisionShape::CT_WorldMesh);
            m_PhysicsSystem.compoundShapeAddChild(m_StaticWorldMeshCollsionShape, wmch);
        }

        // Make sure static collision is initialized before adding the VOBs
        m_PhysicsSystem.postProcessLoad();

        for (Handle::EntityHandle e : ents)
        {
            // Init positions
            Components::EntityComponent &entity = getEntity<Components::EntityComponent>(e);
            Components::addComponent<Components::PositionComponent>(entity);

            // Copy world-matrix (These are all identiy on the worldmesh)
            Components::PositionComponent &pos = getEntity<Components::PositionComponent>(e);
            pos.m_WorldMatrix = Math::Matrix::CreateIdentity();
            pos.m_WorldMatrix.Translation(pos.m_WorldMatrix.Translation() * (1.0f / 100.0f));
            pos.m_DrawDistanceFactor = -1.0f; // Always draw the worldmesh

            Components::StaticMeshComponent& sm = getEntity<Components::StaticMeshComponent>(e);
            sm.m_InstanceDataIndex = (uint32_t)-2; // Disable instancing
        }

		// TODO: Refractor. Make a map of all vobs by classes or something.
		ZenLoad::zCVobData startPoint;

        std::function<void(const std::vector<ZenLoad::zCVobData>)> vobLoad = [&](
                const std::vector<ZenLoad::zCVobData> &vobs) {
            for (const ZenLoad::zCVobData &v : vobs)
            {
                vobLoad(v.childVobs);

				// Check for special vobs // FIXME: Should be somewhere else
				Vob::VobInformation vob;
				Handle::EntityHandle e;
				if(v.objectClass == "oCItem:zCVob")
				{
					// Get item instance
					if(getScriptEngine().hasSymbol(v.oCItem.instanceName))
					{
						Daedalus::GameState::ItemHandle h = getScriptEngine().getGameState().insertItem(v.oCItem.instanceName);

						e = VobTypes::initItemFromScript(*this, h);
						vob = Vob::asVob(*this, e);
					}
					else{
						LogWarn() << "Invalid item instance: " << v.oCItem.instanceName;
					}
				}else if(v.objectClass.find("oCMobInter:oCMOB") != std::string::npos)
                {
                    e = VobTypes::createMob(*this, v);
                    vob = Vob::asVob(*this, e);
                }
				else {
					// Normal zCVob or not implemented subclass
					e = Vob::constructVob(*this);
					vob = Vob::asVob(*this, e);
				}

				if(!vob.isValid())
					continue;

				// Setup
				if(!v.vobName.empty())
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
				if(v.visual.empty())
				{
					// Check for startingpoint
					if(v.objectClass == "zCVobStartpoint:zCVob")
					{
						startPoint = v;
					}

                    // Check for freepoint
                    if(v.objectClass == "zCVobSpot:zCVob")
                    {
                        // Register freepoint
                        Handle::EntityHandle h = addEntity(Components::ObjectComponent::MASK | Components::SpotComponent::MASK | Components::PositionComponent::MASK);
                        getEntity<Components::ObjectComponent>(h).m_Name = v.vobName;
                        m_FreePoints[v.vobName] = h;
                    }
				}
				else
				{
					// Set whether we want collision with this vob
					Vob::setCollisionEnabled(vob, v.cdDyn);

                    Utils::BBox3D bbox = {Math::float3(v.bbox[0].v) * (1.0f / 100.0f),
                                          Math::float3(v.bbox[1].v) * (1.0f / 100.0f)};

                    //LogInfo() << "Vobsize (" << v.visual << "): " << (bbox.max - bbox.min).length() / 10.0f;
                    vob.position->m_DrawDistanceFactor = std::max(0.12f, std::min(1.0f, (bbox.max - bbox.min).length() / 10.0f));
                    //LogInfo() << "DistanceFactor (" << v.visual << "): " << vob.position->m_DrawDistanceFactor;

					Vob::setVisual(vob, v.visual);

					//if(!vob.visual)
					//    LogInfo() << "No visual for: " << v.visual;



					Vob::setBBox(vob, Math::float3(v.bbox[0].v) * (1.0f / 100.0f) - m.Translation(),
						Math::float3(v.bbox[1].v) * (1.0f / 100.0f) - m.Translation(),
						0 /*vob.visual ? 0 : 0xFF00AA00*/);



					// Trace down from this vob to get the shadow-value from the worldmesh
					Math::float3 traceStart = Math::float3(m.Translation().x, v.bbox[1].y * (1.0f / 100.0f) , m.Translation().z);
					Math::float3 traceEnd = Math::float3(m.Translation().x, (v.bbox[0].y * (1.0f / 100.0f)) - 5.0f, m.Translation().z);
					Physics::RayTestResult hit = m_PhysicsSystem.raytrace(traceStart, traceEnd,
						Physics::CollisionShape::CT_WorldMesh); // FIXME: Use boundingbox for this

					if(hit.hasHit)
					{
						float shadow = m_WorldMesh.interpolateTriangleShadowValue(hit.hitTriangleIndex, hit.hitPosition);

						if(Vob::getVisual(vob))
							Vob::getVisual(vob)->setShadowValue(shadow);

					}else
                    {
                        if(Vob::getVisual(vob))
                            Vob::getVisual(vob)->setShadowValue(0.6);
                    }
				}
            }
        };

        LogInfo() << "Inserting vobs...";
        vobLoad(world.rootVobs);

        // Make sure static collision is initialized before adding the NPCs
        m_PhysicsSystem.postProcessLoad();

        // Load waynet
        m_Waynet = Waynet::makeWaynetFromZen(world);

		// Insert startpoint as a waypoint with the name zCVobStartpoint:zCVob.
		if(!startPoint.objectClass.empty())
		{
			Waynet::Waypoint startWP;
			startWP.classname = startPoint.objectClass;
			startWP.direction = Math::float3(startPoint.rotationMatrix.Forward().v);
			startWP.name = startPoint.objectClass;
			startWP.position = (1.0f / 100.0f) * Math::float3(startPoint.position.v);
			startWP.underWater = false;
			startWP.waterDepth = 0;
			Waynet::addWaypoint(m_Waynet, startWP);
		}

        // Init script-engine
        initializeScriptEngineForZenWorld(zen.substr(0, zen.find('.')));


        /*Handle::EntityHandle testEntity = Vob::constructVob(*this);

        auto& ph = Components::Actions::initComponent<Components::PhysicsComponent>(getComponentAllocator(), testEntity);
        ph.m_RigidBody.initPhysics(&m_PhysicsSystem, *m_PhysicsSystem.makeBoxCollisionShape(Math::float3(2.5f, 2.5f, 2.5f)));

        Components::Actions::Physics::setRigidBodyPosition(ph, Math::float3(0.0f, 10.0f, 0.0f));*/
	}
	else
	{
		// Dump a list of possible zens
		auto& files = m_pEngine->getVDFSIndex().getKnownFiles();
		std::vector<std::string> zenFiles;
		for(auto& f : files)
		{
			if(f.fileName.find(".ZEN") != std::string::npos)
			{
				zenFiles.push_back(f.fileName);
			}
		}

		LogInfo() << "ZEN-Files found in the currently loaded Archives: " << zenFiles;

		initializeScriptEngineForZenWorld("");
	}

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
	if(!worldName.empty())
	{
		LogInfo() << "Initializing scripts for world: " << worldName;
		m_ScriptEngine.initForWorld(worldName);
	}

    // Initialize dialog manager
    m_DialogManager.init();
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
    entity.m_ThisEntity = h;

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
    // Set frametime in worldinfo
    m_WorldInfo.lastFrameDeltaTime = deltaTime;
    m_WorldInfo.time += deltaTime;

    // Tell script engine the frame started
    m_ScriptEngine.onFrameStart();

    // Update physics
    m_PhysicsSystem.update(deltaTime);

    // Update sky
    m_Sky.interpolate(deltaTime);

    size_t num = getComponentAllocator().getNumObtainedElements();
    const auto& ctuple = getComponentDataBundle().m_Data;

    Components::EntityComponent* ents = std::get<Components::EntityComponent*>(ctuple);
    Components::StaticMeshComponent* sms = std::get<Components::StaticMeshComponent*>(ctuple);
    Components::LogicComponent* logics = std::get<Components::LogicComponent*>(ctuple);
    Components::AnimationComponent* anims = std::get<Components::AnimationComponent*>(ctuple);
    Components::PositionComponent* positions = std::get<Components::PositionComponent*>(ctuple);

    //#pragma omp parallel for
    for (size_t i = 0; i<num; i++)
    {
        // Simple distance-check // TODO: Frustum/Occlusion-Culling
        if(Components::hasComponent<Components::PositionComponent>(ents[i]))
        {
            if ((positions[i].m_WorldMatrix.Translation() - cameraWorld.Translation()).lengthSquared() >
                    updateRangeSquared * positions[i].m_DrawDistanceFactor )
                continue;
        }

        Components::ComponentMask mask = ents[i].m_ComponentMask;
        if(Components::hasComponent<Components::LogicComponent>(ents[i]))
        {
            if(logics[i].m_pLogicController)
            {
                logics[i].m_pLogicController->onUpdate(deltaTime);
            }
        }

        // Update animations, only if there isn't a valid parent registered
        if(Components::hasComponent<Components::AnimationComponent>(ents[i])
            && !anims[i].m_ParentAnimHandler.isValid())
        {
            anims[i].m_AnimHandler.updateAnimations(deltaTime);
        }
    }

    // TODO: Move this somewhere else, where other game-logic is!
    // TODO: Must be done before the main-camera gets updated, actually
    if(m_ScriptEngine.getPlayerEntity().isValid())
    {
        VobTypes::NpcVobInformation player = VobTypes::asNpcVob(*this, m_ScriptEngine.getPlayerEntity());

        if(player.playerController)
            player.playerController->onUpdateByInput(deltaTime);
    }

    // Update dialogs
    m_DialogManager.update(deltaTime);

    // Tell script engine the frame ended
    m_ScriptEngine.onFrameEnd();

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

std::vector<size_t> WorldInstance::findStartPoints()
{
    std::vector<size_t> pts;

	// General

	auto it = m_Waynet.waypointsByName.find("zCVobStartpoint:zCVob");
	if(it != m_Waynet.waypointsByName.end())
		pts.push_back((*it).second);

	if(!pts.empty())
		return pts;

    // Gothic 1
    it = m_Waynet.waypointsByName.find("WP_INTRO_SHORE");
    if(it != m_Waynet.waypointsByName.end())
        pts.push_back((*it).second);

    for(size_t i=0;i<m_Waynet.waypoints.size();i++)
    {
        // From spacer-doc:
        // Der Name eines Startpoints muss den Instanznamen des Spielers enthalten, der an diesem Punkt starten soll.
        // Format: Beispiel: START_PC_HERO für die Instanz "PC_HERO" oder START_OLD_MAN ...
        // => Name of a startpoint always starts with "START_"

        // Doesn't actually seem the case, tho!

        // Take anything with START for now
        if(m_Waynet.waypoints[i].name.find("START") != std::string::npos)
            pts.push_back(i);
    }

    return pts;
}

std::vector<Handle::EntityHandle>
WorldInstance::getFreepointsInRange(const Math::float3& center, float distance, const std::string& name, bool closestOnly,
                                    Handle::EntityHandle inst)
{
    std::vector<Handle::EntityHandle> m;

	// FIXME: This is too slow! (And not used anyways)
	return m;

    Handle::EntityHandle closestFP;

    float closest2 = FLT_MAX;
    float distance2 = distance * distance;
    std::vector<Handle::EntityHandle> fps = getFreepoints(name);
    for(auto& fp : fps)
    {
        Components::ObjectComponent& obj = getEntity<Components::ObjectComponent>(fp);
        if(name.empty() || obj.m_Name == name)
        {
            Components::SpotComponent& sp = getEntity<Components::SpotComponent>(fp);
            Components::PositionComponent& pos = getEntity<Components::PositionComponent>(fp);

            if((!sp.m_UsingEntity.isValid() || sp.m_UseEndTime < m_WorldInfo.time)
               && (!inst.isValid() || sp.m_UsingEntity != inst))
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
    }

    if(closestOnly)
        return closestFP.isValid() ? std::vector<Handle::EntityHandle>( {closestFP} ) : std::vector<Handle::EntityHandle>();
    else
        return m;
}

std::vector<Handle::EntityHandle> WorldInstance::getFreepoints(const std::string& tag)
{
    std::vector<Handle::EntityHandle> mp;

	// FIXME: This is too slow!
	/*
    for(auto& fp : m_FreePoints)
    {
        if(fp.first.substr(0, tag.size()) == tag)
            mp.push_back(fp.second);
    }
	*/

    return mp;
}

EGameType WorldInstance::getBasicGameType()
{
    std::map<std::string, EGameType> m = {{"newworld.zen", GT_Gothic2},
                                          {"oldworld.zen", GT_Gothic2},
                                          {"addonworld.zen", GT_Gothic2},
                                          {"world.zen", GT_Gothic1}};

    std::string lower;
    std::transform(m_ZenFile.begin(), m_ZenFile.end(), std::back_inserter(lower), ::tolower);

    if(m.find(lower) != m.end())
        return m[lower];

    // Default to gothic 2
    return GT_Gothic2;
}







