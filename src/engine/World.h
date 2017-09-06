#pragma once

#include "BspTree.h"
#include "Waynet.h"
#include "WorldMesh.h"
#include <json.hpp>
#include "audio/AudioWorld.h"
#include <components/Entities.h>
#include <components/Vob.h>
#include <content/AnimationAllocator.h>
#include <content/AnimationLibrary.h>
#include <content/SkeletalMeshAllocator.h>
#include <content/Sky.h>
#include <content/StaticMeshAllocator.h>
#include <content/Texture.h>
#include <content/VertexTypes.h>
#include <logic/CameraController.h>
#include <logic/DialogManager.h>
#include <logic/LogManager.h>
#include <logic/PfxManager.h>
#include <logic/ScriptEngine.h>
#include <memory/AllocatorBundle.h>
#include <memory/Config.h>
#include <memory/StaticReferencedAllocator.h>
#include <physics/PhysicsSystem.h>

using json = nlohmann::json;

namespace ZenLoad
{
    class ZenParser;
}

namespace Engine
{
    class BaseEngine;
}

namespace World
{
    struct WorldAllocators
    {
        WorldAllocators(Engine::BaseEngine& engine)
            : m_LevelTextureAllocator(engine)
            , m_LevelSkeletalMeshAllocator(engine)
            , m_LevelStaticMeshAllocator(engine)
        {
        }
        template <typename V, typename I>
        using MeshAllocator = Memory::StaticReferencedAllocator<
            Meshes::WorldStaticMesh,
            Config::MAX_NUM_LEVEL_MESHES>;

        typedef MeshAllocator<Meshes::UVNormColorVertex, uint32_t> WorldMeshAllocator;
        typedef Memory::StaticReferencedAllocator<
            Materials::TexturedMaterial,
            Config::MAX_NUM_LEVEL_MATERIALS>
            MaterialAllocator;

        Components::ComponentAllocator m_ComponentAllocator;
        Textures::TextureAllocator m_LevelTextureAllocator;
        Meshes::StaticMeshAllocator m_LevelStaticMeshAllocator;
        Meshes::SkeletalMeshAllocator m_LevelSkeletalMeshAllocator;
        Animations::AnimationAllocator m_AnimationAllocator;
        Animations::AnimationDataAllocator m_AnimationDataAllocator;

        // TODO: Refractor this one into StaticMeshAllocator
        WorldMeshAllocator m_WorldMeshAllocator;
        MaterialAllocator m_MaterialAllocator;
    };

    /**
     * All information inside this struct are only valid at a certain time of the frame, where no entities can be
     * (de)allocated or moved around in any way. The indices stored inside the vectors are a direct mapping to the
     * Elements inside the allocator, which are to be seen as invalid in the next frame!
     */
    struct TransientEntityFeatures
    {
        std::vector<size_t> m_VisibleEntities;
    };

    class WorldInstance : public Handle::HandleTypeDescriptor<Handle::WorldHandle>
    {
    public:
        WorldInstance(Engine::BaseEngine& engine);
        ~WorldInstance();

        /**
         * @param zen filename of world
         * @param worldJson may be empty
         * @param scriptEngine may be empty
         * @param dialogManager may be empty
         * @param logManager may be empty
         */
        bool init(const std::string& zen,
                  const json& worldJson = json(),
                  const json& scriptEngine = json(),
                  const json& dialogManager = json(),
                  const json& logManager = json());

        /**
         * Creates an entity with the given components and returns its handle
         */
        Components::ComponentAllocator::Handle addEntity(Components::ComponentMask components = 0);

        /**
         * creates camera and returns it
         */
        Handle::EntityHandle createCamera();

        /**
         * @return this world's camera
         */
        Handle::EntityHandle getCamera();

        /**
         * @return The given component of the world's camera
         */
        template <typename T>
        T& getCameraComp()
        {
            return getComponentAllocator().getElement<T>(m_Camera);
        }

        /**
         * @return Camera-Controller of the camera
         */
        Logic::CameraController* getCameraController()
        {
            Logic::Controller* ptr = getCameraComp<Components::LogicComponent>().m_pLogicController;
            return dynamic_cast<Logic::CameraController*>(ptr);
        }

        /**
         * Checks whether the passed entity handle points to a valid location
         * @param e Handle to check
         * @return Whether that entity exists
         */
        bool isEntityValid(Handle::EntityHandle e);

        /**
         * Removes the entitiy with the given handle
         */
        void removeEntity(Handle::EntityHandle h);

        /**
         * Updates this world instances entities
         */
        void onFrameUpdate(double deltaTime, float updateRangeSquared, const Math::Matrix& cameraWorld);

        /**
         * @return The component associated with the given handle
         */
        template <typename C>
        C& getEntity(Handle::EntityHandle h)
        {
            return m_Allocators.m_ComponentAllocator.getElement<C>(h);
        }

        /**
         * @return The vob-entity of a vob using the given name
         */
        Handle::EntityHandle getVobEntityByName(const std::string& name)
        {
            auto it = m_VobsByNames.find(name);
            if (it == m_VobsByNames.end())
                return Handle::EntityHandle::makeInvalidHandle();

            return (*it).second;
        }

        /**
         * Goes through the list of all vobs and returns a list of the startwaypoint-indices
         * @return List indices of the waypoints which are used as starting-position
         */
        std::vector<size_t> findStartPoints();

        /**
         * @return Basic gametype this is. Needed for sky configuration, for example
         */
        Daedalus::GameType getBasicGameType();

        /**
         * Data access
         */
        Components::ComponentAllocator::DataBundle getComponentDataBundle()
        {
            return m_Allocators.m_ComponentAllocator.getDataBundle();
        }
        WorldAllocators& getAllocators()
        {
            return m_Allocators;
        }
        WorldAllocators::WorldMeshAllocator& getWorldMeshAllocator()
        {
            return m_Allocators.m_WorldMeshAllocator;
        }
        Textures::TextureAllocator& getTextureAllocator()
        {
            return m_Allocators.m_LevelTextureAllocator;
        }
        Components::ComponentAllocator& getComponentAllocator()
        {
            return m_Allocators.m_ComponentAllocator;
        }
        Meshes::StaticMeshAllocator& getStaticMeshAllocator()
        {
            return m_Allocators.m_LevelStaticMeshAllocator;
        }
        Meshes::SkeletalMeshAllocator& getSkeletalMeshAllocator()
        {
            return m_Allocators.m_LevelSkeletalMeshAllocator;
        }
        Animations::AnimationAllocator& getAnimationAllocator()
        {
            return m_Allocators.m_AnimationAllocator;
        }
        Animations::AnimationDataAllocator& getAnimationDataAllocator()
        {
            return m_Allocators.m_AnimationDataAllocator;
        }

        // TODO: Depricated, remove
        WorldAllocators::MaterialAllocator& getMaterialAllocator()
        {
            return m_Allocators.m_MaterialAllocator;
        }
        const Waynet::WaynetInstance& getWaynet()
        {
            return m_Waynet;
        }
        Logic::ScriptEngine& getScriptEngine()
        {
            return m_ScriptEngine;
        }
        Handle::WorldHandle getMyHandle()
        {
            return Handle::WorldHandle(this);
        }
        Engine::BaseEngine* getEngine()
        {
            return m_pEngine;
        }
        Physics::PhysicsSystem& getPhysicsSystem()
        {
            return m_PhysicsSystem;
        }
        Handle::CollisionShapeHandle getStaticObjectCollisionShape()
        {
            return m_StaticWorldObjectCollsionShape;
        }
        WorldMesh& getWorldMesh()
        {
            return m_WorldMesh;
        }
        Content::Sky& getSky()
        {
            return m_Sky;
        }
        Logic::DialogManager& getDialogManager()
        {
            return m_DialogManager;
        }
        Logic::LogManager& getLogManager()
        {
            return m_LogManager;
        }
        World::AudioWorld& getAudioWorld()
        {
            return *m_AudioWorld;
        }

        Logic::PfxManager& getPfxManager()
        {
            return m_PfxManager;
        }

        Animations::AnimationLibrary& getAnimationLibrary()
        {
            return m_AnimationLibrary;
        }

        /**
         * HUD's print-screen manager
         */
        UI::PrintScreenMessages& getPrintScreenManager();

        /**
         * @return Map of freepoints
         */
        std::vector<Handle::EntityHandle> getFreepoints(const std::string& tag);

        /**
         * Returns a map of freepoints in the given range of the center
         * @param center Center of distance search
         * @param distance Max distance to check for
         * @param name Name-filter
         * @param closestOnly Put only the closest one into the result
         * @param inst Entity that want's a new freepoint, aka, should not be on any of the returned ones
         * @return Vector of closest freepoints
         */
        std::vector<Handle::EntityHandle> getFreepointsInRange(const Math::float3& center,
                                                               float distance,
                                                               const std::string& name = "",
                                                               bool closestOnly = false,
                                                               Handle::EntityHandle inst = Handle::EntityHandle::makeInvalidHandle());

        /**
         * Exports this world into a json-object
         * @param j json-object to write into
         * @param skip entities which shall be excluded from export
         */
        void exportWorld(json& j, std::set<Handle::EntityHandle> skip = {});

        /**
         * Exports the given controllers to a json-object
         * @param logicController may be nullptr
         * @param visualController may be nullptr
         * @param j json-object to write into
         */
        void exportControllers(Logic::Controller* logicController, Logic::VisualController* visualController, json& j);

        /**
         * @return exported npc as json object
         */
        json exportNPC(Handle::EntityHandle entityHandle);

        /**
         * export npc (i.e. player) and remove him from world
         */
        json exportAndRemoveNPC(Handle::EntityHandle handle);

        /**
         * @return world-file this is built after
         */
        const std::string& getZenFile() { return m_ZenFile; }

        /**
         * Imports vobs from a json-object
         * @param j
         */
        void importVobs(const json& j);

        /**
         * Imports a single vob from a json-object
         * @return entity handle if successfull, else invalid handle
         */
        Handle::EntityHandle importSingleVob(const json& j);

        /**
         * import single vob from json and take control over it
         * @return entity handle if successfull, else invalid handle
         */
        Handle::EntityHandle importVobAndTakeControl(const json& j);

        /**
         * take control over the given entity (camera, movement(key bindings))
         */
        void takeControlOver(Handle::EntityHandle entityHandle);

    private:
        /**
         * copying a world is not allowed and results in a compile error
         */
        WorldInstance(const WorldInstance& other) = delete;

    protected:
        /**
         * Initializes the Script-Engine for a ZEN-World.
         * Will load the .DAT-Files and setup the VM.
         * @param firstStart Whether this is the initial load. If true, all NPCs will be put in here.
         */
        bool initializeScriptEngineForZenWorld(const std::string& worldName, bool firstStart = true);

        WorldAllocators m_Allocators;
        TransientEntityFeatures m_TransientEntityFeatures;

        /**
         * Loaded zen-file
         */
        std::string m_ZenFile;

        /**
         * Worldmesh-data
         */
        WorldMesh m_WorldMesh;

        /**
         * BSP-Tree representing this world
         */
        BspTree m_BspTree;

        /**
         * Waynet-data
         */
        Waynet::WaynetInstance m_Waynet;

        /**
         * Engine-instance
         */
        Engine::BaseEngine* m_pEngine;

        /**
         * Scripting-Engine of this world
         */
        Logic::ScriptEngine m_ScriptEngine;

        /**
         * This worlds physics system
         */
        Physics::PhysicsSystem m_PhysicsSystem;

        World::AudioWorld* m_AudioWorld;

        Animations::AnimationLibrary m_AnimationLibrary;

        /**
         * Sky of this world
         */
        Content::Sky m_Sky;

        /**
         * Static collision-shape for the world
         */
        Handle::CollisionShapeHandle m_StaticWorldObjectCollsionShape;
        Handle::CollisionShapeHandle m_StaticWorldMeshCollsionShape;
        Handle::PhysicsObjectHandle m_StaticWorldMeshPhysicsObject;
        Handle::PhysicsObjectHandle m_StaticWorldObjectPhysicsObject;

        /**
         * Handle of this world-instance
         */
        Handle::WorldHandle m_MyHandle;

        /**
         * Map of vobs by their names (If they have one)
         */
        std::unordered_map<std::string, Handle::EntityHandle> m_VobsByNames;

        /**
         * List of freepoints
         */
        std::map<std::string, Handle::EntityHandle> m_FreePoints;

        /**
         * NPCs in this world
         */
        std::set<Handle::EntityHandle> m_NPCEntities;

        /**
         * This worlds dialog-manager
         */
        Logic::DialogManager m_DialogManager;

        /**
         * The log-manager
         */
        Logic::LogManager m_LogManager;

        /**
         * Pfx-cache
         */
        Logic::PfxManager m_PfxManager;

        /**
         * Players camera entity
         */
        Handle::EntityHandle m_Camera;
    };
}
