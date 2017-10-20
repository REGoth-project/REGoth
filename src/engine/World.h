#pragma once

#include <json.hpp>
#include <set>
#include <ZenLib/daedalus/DaedalusStdlib.h>
#include <handle/HandleDef.h>
#include <math/mathlib.h>
#include <components/Entities.h>

using json = nlohmann::json;

namespace ZenLoad
{
    class ZenParser;
}

namespace Engine
{
    class BaseEngine;
}

namespace Physics
{
    class PhysicsSystem;
}

namespace Content
{
    class Sky;
}

namespace Logic
{
    class DialogManager;
    class PfxManager;
    class CameraController;
    class ScriptEngine;
}

namespace Animations
{
    class AnimationLibrary;
}

namespace UI
{
    class PrintScreenMessages;
}

namespace Meshes
{
    class StaticMeshAllocator;
    class SkeletalMeshAllocator;
}

namespace Animations
{
    class AnimationAllocator;
    class AnimationDataAllocator;
}



namespace World
{
    class AudioWorld;
    class WorldMesh;
    class WorldAllocators;

    namespace Waynet
    {
        struct WaynetInstance;
    }

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
        Handle::EntityHandle addEntity(uint32_t components = 0);

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
        Logic::CameraController* getCameraController();

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
            return getComponentAllocator().getElement<C>(h);
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
        WorldAllocators& getAllocators();
        Components::ComponentAllocator::DataBundle getComponentDataBundle();
        Textures::TextureAllocator& getTextureAllocator();
        Components::ComponentAllocator& getComponentAllocator();
        Meshes::StaticMeshAllocator& getStaticMeshAllocator();
        Meshes::SkeletalMeshAllocator& getSkeletalMeshAllocator();
        Animations::AnimationAllocator& getAnimationAllocator();
        Animations::AnimationDataAllocator& getAnimationDataAllocator();

        Handle::WorldHandle getMyHandle()
        {
            return Handle::WorldHandle(this);
        }
        Engine::BaseEngine* getEngine()
        {
            return m_pEngine;
        }

        Handle::CollisionShapeHandle getStaticObjectCollisionShape()
        {
            return m_StaticWorldObjectCollsionShape;
        }

        const Waynet::WaynetInstance& getWaynet();
        Logic::ScriptEngine& getScriptEngine();



        Physics::PhysicsSystem& getPhysicsSystem();


        WorldMesh& getWorldMesh();
        Content::Sky& getSky();
        Logic::DialogManager& getDialogManager();
        World::AudioWorld& getAudioWorld();
        Logic::PfxManager& getPfxManager();
        Animations::AnimationLibrary& getAnimationLibrary();

        /**
         * HUD's print-screen manager
         */
        UI::PrintScreenMessages& getPrintScreenManager();

        /**
         * @return freepoints with this tag
         */
        std::vector<Handle::EntityHandle> getFreepoints(const std::string& tag);

        /**
         * @return Map of all freepoints
         */
        const std::map<std::string, Handle::EntityHandle>& getFreepoints() const;

        /**
         * Checks whether a freepoint with the given exact name exists
         * @param name Name to look for
         * @return Whether such a freepoint exists
         */
        bool doesFreepointExist(const std::string& name);

        /**
         * @return Freepoint handle with the given exact name
         */
        Handle::EntityHandle getFreepoint(const std::string& name);

        /**
         * @return Position of the freepoint given with the exact name. (0,0,0) if not found.
         */
        Math::float3 getFreepointPosition(const std::string& name);

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
         * Marks the given Freepoint as "occupied" for the next occupiedForSeconds
         */
        void markFreepointOccupied(Handle::EntityHandle freepoint, Handle::EntityHandle usingEntity, float occupiedForSeconds);

        /**
         * @return Whether the freepoint is currently occupied
         */
        bool isFreepointOccupied(Handle::EntityHandle freepoint);

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
         * @return Name of the loaded ZEN-File (without extension)
         */
        std::string getWorldName();

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
         * sets the controlled entity
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
        void initializeScriptEngineForZenWorld(const std::string& worldName, bool firstStart = true);


        TransientEntityFeatures m_TransientEntityFeatures;

        /**
         * Loaded zen-file
         */
        std::string m_ZenFile;



        /**
         * Engine-instance
         */
        Engine::BaseEngine* m_pEngine;

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
         * Usually freepoints are named like "FP_GUARD_XXX", where "FP_GUARD" is the 'tag' of
         * a freepoint. To save us from going through the whole freepoint list every time we need a
         * freepoint with a specific tag, we cache the searches here
         */
        std::map<std::string, std::vector<Handle::EntityHandle>> m_FreePointTagCache;

        /**
         * NPCs in this world
         */
        std::set<Handle::EntityHandle> m_NPCEntities;

        /**
         * Players camera entity
         */
        Handle::EntityHandle m_Camera;

        class ClassContents;

        /**
         * Other classes referenced here (pimple-like)
         * m_Allocators must be initialized before m_ClassContents
         */
        std::unique_ptr<WorldAllocators> m_Allocators;
        std::unique_ptr<ClassContents> m_ClassContents;
    };
}
