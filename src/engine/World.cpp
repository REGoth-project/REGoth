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
#include <logic/MobController.h>
#include <stdlib.h>
#include <iterator>

#include <engine/GameEngine.h>
#include <debugdraw/debugdraw.h>
#include <components/Vob.h>
#include <components/VobClasses.h>
#include <entry/input.h>
#include <ui/PrintScreenMessages.h>
#include <ZenLib/zenload/zTypes.h>
#include <ui/Hud.h>

using namespace World;

WorldInstance::WorldInstance(Engine::BaseEngine& engine)
	: m_pEngine(&engine),
      m_WorldMesh(*this),
      m_ScriptEngine(*this),
      m_PhysicsSystem(*this),
      m_Sky(*this),
      m_DialogManager(*this),
      m_PrintScreenMessageView(nullptr),
      m_BspTree(*this),
      m_PfxManager(*this),
      m_AudioWorld(nullptr)
{
}

WorldInstance::~WorldInstance()
{
    // kick out player if he is still in this world (clears key bindings)
    auto player = getScriptEngine().getPlayerEntity();
    if (player.isValid())
        exportAndRemoveNPC(player);

    if (m_PrintScreenMessageView && getEngine())
        getEngine()->getRootUIView().removeChild(m_PrintScreenMessageView);

    delete m_PrintScreenMessageView;

    delete m_AudioWorld;
}

bool WorldInstance::init(const std::string& zen, const json& j)
{
    m_ZenFile = zen;
    Engine::BaseEngine& engine = *m_pEngine;

    m_Allocators.m_LevelTextureAllocator.setVDFSIndex(&engine.getVDFSIndex());
    m_Allocators.m_LevelStaticMeshAllocator.setVDFSIndex(&engine.getVDFSIndex());
    m_Allocators.m_LevelSkeletalMeshAllocator.setVDFSIndex(&engine.getVDFSIndex());
    m_Allocators.m_AnimationAllocator.setVDFSIndex(&engine.getVDFSIndex());

    // Create static-collision shape beforehand
    m_StaticWorldObjectCollsionShape = m_PhysicsSystem.makeCompoundCollisionShape(Physics::CollisionShape::CT_Object);
    m_StaticWorldMeshCollsionShape = m_PhysicsSystem.makeCompoundCollisionShape(Physics::CollisionShape::CT_WorldMesh);
    m_StaticWorldObjectPhysicsObject = m_PhysicsSystem.makeRigidBody(m_StaticWorldObjectCollsionShape, Math::Matrix::CreateIdentity());
    m_StaticWorldMeshPhysicsObject = m_PhysicsSystem.makeRigidBody(m_StaticWorldMeshCollsionShape, Math::Matrix::CreateIdentity());

    // Create UI-Views
    m_PrintScreenMessageView = new UI::PrintScreenMessages(*m_pEngine);
    getEngine()->getRootUIView().addChild(m_PrintScreenMessageView);

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

        LogInfo() << "Initilizing BSP-Tree...";
        m_BspTree.loadBspTree(world.bspTree);

        LogInfo() << "Postprocessing worldmesh...";

        ZenLoad::PackedMesh packedWorldMesh;
        worldMesh->packMesh(packedWorldMesh, 0.01f, false);

        // Init worldmesh-wrapper
        m_WorldMesh.load(packedWorldMesh);

        for(auto& sm : packedWorldMesh.subMeshes)
        {
            size_t k=0;
            for(auto& lm : sm.triangleLightmapIndices)
            {
                if (lm != -1)
                {
                    packedWorldMesh.vertices[sm.indices[k * 3 + 0]].Color = 0xFFAAAAAA;
                    packedWorldMesh.vertices[sm.indices[k * 3 + 1]].Color = 0xFFAAAAAA;
                    packedWorldMesh.vertices[sm.indices[k * 3 + 2]].Color = 0xFFAAAAAA;
                }

                k++;
            }
        }

        // TODO: Put these into a compound-component or something
        std::vector<Handle::EntityHandle> ents;
        Handle::MeshHandle worldMeshHandle = getStaticMeshAllocator().loadFromPacked(packedWorldMesh, "WORLDMESH.3DS");

        /*Handle::EntityHandle eh = Vob::constructVob(*this);
        Vob::VobInformation wvob = Vob::asVob(*this, eh);
        wvob.position->m_DrawDistanceFactor = 0.0f;
        Vob::setVisual(wvob, "WORLDMESH.3DS");
        Vob::setTransform(wvob, Math::Matrix::CreateIdentity());
*/

        Meshes::WorldStaticMesh &worldMeshData = getStaticMeshAllocator().getMesh(worldMeshHandle);

        for(size_t i=0;i<packedWorldMesh.subMeshes.size();i++)
        {
            Handle::MeshHandle h = getStaticMeshAllocator().loadFromPackedSubmesh(packedWorldMesh, i, "");
            Meshes::WorldStaticMesh &hdata = getStaticMeshAllocator().getMesh(h);

            std::vector<Handle::EntityHandle> subents  = Content::entitifyMesh(*this, h, hdata.mesh);

            ents.insert(ents.end(), subents.begin(), subents.end());
        }

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

                bool allowCollision = true; // FIXME: Hack. Items shouldn't be placed into physicsworld right now

				// Check for special vobs // FIXME: Should be somewhere else
				Vob::VobInformation vob;
				Handle::EntityHandle e;
				if(v.objectClass == "oCItem:zCVob")
				{
					// Get item instance
					if(getScriptEngine().hasSymbol(v.oCItem.instanceName))
					{
                        e = VobTypes::createItem(*this, v.oCItem.instanceName);

						vob = Vob::asVob(*this, e);

                        allowCollision = false;
					}
					else{
						LogWarn() << "Invalid item instance: " << v.oCItem.instanceName;
					}
				}else if(v.objectClass.find("oCMobInter:oCMOB") != std::string::npos)
                {
                    e = VobTypes::createMob(*this);
                    VobTypes::MobVobInformation mob = VobTypes::asMobVob(*this, e);
                    mob.mobController->initFromVobDescriptor(v);

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
					Vob::setCollisionEnabled(vob, v.cdDyn && allowCollision);

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

        if(j.empty())
        {
            // Load vobs from zen (initial load)
            LogInfo() << "Inserting vobs from zen...";
            vobLoad(world.rootVobs);
        } else
        {
            // Load vobs from saved json (Savegame)
            LogInfo() << "Inserting vobs from json...";
            importVobs(j["vobs"]);
        }



        LogInfo() << "Done!";

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

        LogInfo() << "Creating AudioWorld";
        // must create AudioWorld before initializeScriptEngineForZenWorld, because startup_<worldname> calls snd_play
        m_AudioWorld = new World::AudioWorld(*m_pEngine, m_pEngine->getAudioEngine(), engine.getVDFSIndex());

        LogInfo() << "Running startup-scripts";

        // Init script-engine
        if (!initializeScriptEngineForZenWorld(zen.substr(0, zen.find('.')), j.empty()))
        {
            LogInfo() << "Failed to initialize script engine for zen world";
            delete m_AudioWorld;
            m_AudioWorld = nullptr;
            return false;
        }
        //initializeScriptEngineForZenWorld(zen.substr(0, zen.find('.')), false);

        // Load values from savegame, if there is one
        if(!j.empty())
        {
            m_ScriptEngine.importScriptEngine(j["scriptEngine"]);
            m_DialogManager.importDialogManager(j["dialogManager"]);
        }
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

        if (!initializeScriptEngineForZenWorld(""))
        {
            return false;
        }
    }

    // Initialize the sky, so it will get the right values
    m_Sky.fillSkyStates();

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

    if(!getEngine()->getEngineArgs().testVisual.empty())
    {
        LogInfo() << "Testing visual: " << getEngine()->getEngineArgs().testVisual;
        Handle::EntityHandle e = Vob::constructVob(*this);
        Vob::VobInformation vob = Vob::asVob(*this, e);

        Vob::setVisual(vob, getEngine()->getEngineArgs().testVisual);
    }

    // reset gamespeed to default when new world is loaded
    m_pEngine->setGameEngineSpeedFactor(1.0);

    auto& clock = m_pEngine->getGameClock();
    // reset clockspeed to default on world init
    clock.setClockSpeedFactor(1.0);
    // for test purpose make the clock run 7 times faster than usual gameplay
    clock.setClockSpeedFactor(7.0);

    LogInfo() << "Done loading world!";
    return true;
}

bool WorldInstance::initializeScriptEngineForZenWorld(const std::string& worldName, bool firstStart)
{
	if(!worldName.empty())
	{
		LogInfo() << "Initializing scripts for world: " << worldName;
		m_ScriptEngine.initForWorld(worldName, firstStart);
	}

    LogInfo() << "Initialize dialog manager";
    // Initialize dialog manager
    if (!m_DialogManager.init())
    {
       LogWarn() << "Failed to initialize dialog manager";
       return false;
    }

    LogInfo() << "Script-initialization done!";
    return true;
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
    // Tell script engine the frame started
    m_ScriptEngine.onFrameStart();

    // Update physics
    m_PhysicsSystem.update(deltaTime);

    // Update sky
    m_Sky.interpolate();

    size_t num = getComponentAllocator().getNumObtainedElements();
    const auto& ctuple = getComponentDataBundle().m_Data;

    Components::EntityComponent* ents = std::get<Components::EntityComponent*>(ctuple);
    Components::StaticMeshComponent* sms = std::get<Components::StaticMeshComponent*>(ctuple);
    Components::LogicComponent* logics = std::get<Components::LogicComponent*>(ctuple);
    Components::AnimationComponent* anims = std::get<Components::AnimationComponent*>(ctuple);
    Components::PositionComponent* positions = std::get<Components::PositionComponent*>(ctuple);
    Components::VisualComponent* visuals = std::get<Components::VisualComponent*>(ctuple);

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

        if(Components::hasComponent<Components::VisualComponent>(ents[i]))
        {
            if(visuals[i].m_pVisualController)
            {
                visuals[i].m_pVisualController->onUpdate(deltaTime);
            }
        }

        // Update animations, only if there isn't a valid parent registered
        if(Components::hasComponent<Components::AnimationComponent>(ents[i])
            && !anims[i].m_ParentAnimHandler.isValid())
        {
            anims[i].getAnimHandler().updateAnimations(deltaTime);
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

    // Update hud
    m_pEngine->getHud().setDateTimeDisplay(m_pEngine->getGameClock().getDateTimeFormatted());

    m_BspTree.debugDraw();
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

    // Gothic 1
    auto it = m_Waynet.waypointsByName.find("WP_INTRO_SHORE");
    if(it != m_Waynet.waypointsByName.end())
        pts.push_back((*it).second);

	it = m_Waynet.waypointsByName.find("zCVobStartpoint:zCVob");
	if(it != m_Waynet.waypointsByName.end())
		pts.push_back((*it).second);

	if(!pts.empty())
		return pts;

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

            if((!sp.m_UsingEntity.isValid() || sp.m_UseEndTime < getEngine()->getGameClock().getTime())
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

Daedalus::GameType WorldInstance::getBasicGameType()
{
    return m_pEngine->getBasicGameType();
}

void WorldInstance::exportWorld(json& j)
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

    // Write script-values
    m_ScriptEngine.exportScriptEngine(j["scriptEngine"]);

    // Write dialog-info
    m_DialogManager.exportDialogManager(j["dialogManager"]);
}

Handle::EntityHandle WorldInstance::importSingleVob(const json& j)
{
    // This has a logic and visual controller

    Handle::EntityHandle entity;

    if(j.find("logic") != j.end())
    {
        // TODO: Automate this...
        if(j["logic"]["type"] == "PlayerController")
        {
            entity = Logic::PlayerController::importPlayerController(*this, j["logic"]);
        }else if(j["logic"]["type"] == "MobController")
        {
            entity = VobTypes::createMob(*this);
            VobTypes::MobVobInformation vob = VobTypes::asMobVob(*this, entity);
            vob.mobController->importObject(j["logic"]);
        }else if(j["logic"]["type"] == "ItemController")
        {
            size_t sym = j["logic"]["instanceSymbol"];
            entity = VobTypes::createItem(*this, sym);
        }
    }else
    {
        entity = Vob::constructVob(*this);
    }

    if(!entity.isValid())
        return Handle::EntityHandle::makeInvalidHandle();

    if(j.find("visual") != j.end())
    {
        Vob::VobInformation vob = Vob::asVob(*this, entity);

        // Need to enable collision before setting the visual
        Vob::setCollisionEnabled(vob, j["visual"]["collision"]);

        // Get transform before setting the visual, so staic collision doesn't end up at world-origin
        // TODO: There must be a nicer way of writing this
        auto& jtrans = j["visual"]["transform"];
        Math::Matrix transform;

        // Parse transform from json
        for(int i=0;i<16;i++)
            if(!jtrans[i].is_null())
                transform.mv[i] = jtrans[i];

        Vob::setTransform(vob, transform);

        // Need to set the visual before actually importing the object
        Vob::setVisual(vob, j["visual"]["name"]);
        vob = Vob::asVob(*this, entity);

        // Now proceed with the other values
        if(vob.visual)
            vob.visual->importObject(j["visual"]);
    }
    return entity;
}

void WorldInstance::importVobs(const json& j)
{
    // j must be an array of vobs
    for(const json& vob : j["controllers"])
    {
        if(!vob.is_null())
        {
            importSingleVob(vob);
        }
    }
}

Handle::EntityHandle WorldInstance::createCamera() {
    // Add player-camera
    m_Camera = addEntity(Components::PositionComponent::MASK);

    Math::Matrix lookAt = Math::Matrix::CreateLookAt(Math::float3(0,0,0), Math::float3(1,0,0), Math::float3(0,1,0));
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

Handle::EntityHandle WorldInstance::getCamera() {
    return m_Camera;
}

json WorldInstance::exportAndRemoveNPC(Handle::EntityHandle entityHandle) {
    VobTypes::NpcVobInformation playerVob = VobTypes::asNpcVob(*this, entityHandle);
    json j;
    exportControllers(playerVob.playerController, playerVob.visual, j);

    VobTypes::Wld_RemoveNpc(*this, entityHandle);
    return j;
}

void WorldInstance::exportControllers(Logic::Controller* logicController, Logic::VisualController* visualController, json &j) {
    if (logicController && logicController->shouldExport())
        logicController->exportObject(j["logic"]);
    if (visualController && visualController->shouldExport())
        visualController->exportObject(j["visual"]);
}

void WorldInstance::takeControlOver(Handle::EntityHandle entityHandle) {
    VobTypes::NpcVobInformation npcVob = VobTypes::asNpcVob(*this, entityHandle);

    getScriptEngine().setPlayerEntity(entityHandle);
    getScriptEngine().setInstanceNPC("hero", VobTypes::getScriptHandle(npcVob));
    // reset bindings
    Engine::Input::clearActions();
    npcVob.playerController->setupKeyBindings();
}

Handle::EntityHandle WorldInstance::importVobAndTakeControl(const json &j) {
    auto player = importSingleVob(j);
    takeControlOver(player);
    return player;
}





