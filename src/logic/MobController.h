#pragma once
#include <daedalus/DaedalusGameState.h>
#include <set>
#include "Controller.h"
#include "LogicDef.h"

namespace Logic
{
    class ModelVisual;
    class MobController;

    /**
     * Position to interact with a mob from
     */
    struct InteractPosition
    {
        // Target position/rotation in the world
        Math::Matrix transform;

        // Whether only distance matters. If true, transform will be ignored
        bool distance;

        // NPC currently in this position
        Handle::EntityHandle usedByNpc;

        // Name of the model-node this was built after
        std::string nodeName;
    };

    /**
     * Base class for all Mob-types (Chests, doors, etc)
     */
    class MobCore
    {
    public:
        MobCore(World::WorldInstance& world, Handle::EntityHandle entity)
            : m_Entity(entity)
            , m_World(world)
        {
            m_StateNum = 0;
        }
        virtual ~MobCore() {}
        /**
         * Sets the base animation-scheme to use
         * @param name New scheme
         */
        void setSchemeName(const std::string& name) { m_Scheme = name; }
        /**
         * Called when this mob was activated
         * @param npc NPC who activated the mob
         */
        virtual void startInteraction(Handle::EntityHandle npc) {}
        /**
         *
         * @param npc
         * @param from Current state
         * @param to State to go to
         */
        virtual void onBeginStateChange(Handle::EntityHandle npc, int from, int to) {}
        /**
         *
         * @param npc
         * @param from Current state
         * @param to State to go to
         */
        virtual void onEndStateChange(Handle::EntityHandle npc, int from, int to) { m_StateNum = to; }
        /**
         * Current animation-scheme to use.
         * Example:
         *  Animations for front and backside of a bed
         * @return Current animation sceme name
         */
        virtual std::string getSchemeName() { return m_Scheme; }
        /**
         * Called right after the MobController found a new position for a new npc asking to use this mob
         * @param npc NPC asking
         * @param pos Position found
         */
        virtual void onFreePositionFound(Handle::EntityHandle npc, InteractPosition* pos) {}
        /**
         * @return Pointer to the mob-controller of this very core
         */
        MobController* getMobController();

        /**
         * @return State this mob is currently in
         */
        int getState() { return m_StateNum; }
        /**
         * Saves this cores settings to the given JSON-object
         */
        virtual void exportCore(json& j);
        virtual void importCore(const json& j);

    protected:
        Handle::EntityHandle m_Entity;
        World::WorldInstance& m_World;

        // Animationsceme this mob should use
        std::string m_Scheme;

        // State this mob is currently in
        int m_StateNum;
    };

    /**
     * Reimplementation of oCMOBInter-class. Base class oCMOB is not used anywhere else.
     * Each further class, being a base of oCMOBInter, is then implemented as a "MobCore", which is a smaller Object
     * managed by this controller.
     */
    class MobController : public Controller
    {
    public:
        /**
         * @param world World of the underlaying entity
         * @param entity Entity owning this controller
         */
        MobController(World::WorldInstance& world, Handle::EntityHandle entity);
        virtual ~MobController();

        /**
         * Starts an interaction from the given npc. This is the method you should use to let
         * your npcs interact with this mob
         * @param npc NPC to start interacting
         * @param target Target-state
         */
        void useMobToState(Handle::EntityHandle npc, int target);

        /**
         * Increments/Decrements the current state from the currently set one
         * @param npc NPC interacting with this/To interact with this
         * @param direction Direction of the state-transition to go. Ie. from sitting on a chest to open it.
         */
        enum EDirection
        {
            D_Forward,
            D_Backward,
            D_Left,
            D_Right
        };
        void useMobIncState(Handle::EntityHandle npc, EDirection direction);

        /**
         * Called on game-tick
         */
        virtual void onUpdate(float deltaTime);

        /**
         * @return The type of this class. If you are adding a new base controller, be sure to add it to ControllerTypes.h
         */
        virtual EControllerType getControllerType() { return EControllerType::MobController; }
        /**
         * Initializes this mob from a vob-descriptor loaded by zenlib
         */
        void initFromVobDescriptor(const ZenLoad::zCVobData& vob);
        void initFromJSONDescriptor(const json& j);

        /**
         * @return The ModelVisual of the underlaying vob
         */
        ModelVisual* getModelVisual();

        /**
         * Checks if one of the interact-positions can be used by the given npc
         * @param npc NPC to check for
         * @param maxDistance Max distance acceptable
         * @return Free position, or nullptr of none could be found
         */
        InteractPosition* findFreePosition(Handle::EntityHandle npc, float maxDistance = 150.0f);

        /**
         * Calls the script-function for the current mobcore-state
         * @param npc NPC to set as "self"
         * @param state State function to call
         */
        void callOnStateFunc(Handle::EntityHandle npc, int state);

        /**
         * Sends an event message to call callOnStateFunc
         * @param npc NPC to set as "self"
         * @param state State function to call
         */
        void sendCallOnStateFunc(Handle::EntityHandle npc, int state);

        /**
         * Checks all interaction positions for the given NPC
         * @param npc NPC to check
         * @return whether this npc is currently interacting with this mob
         */
        bool isInteractingWith(Handle::EntityHandle npc);

        /**
         * Sets the given NPC to an ideal position and registers it as using the mob
         * @param npc NPC to operate on
         */
        void setIdealPosition(Handle::EntityHandle npc);

        void onMessage(std::shared_ptr<EventMessages::EventMessage> message, Handle::EntityHandle sourceVob) override;

        /**
         * Called when the models visual changed
         */
        virtual void onVisualChanged() override;

        /**
         * @return Focus name the player sees
         */
        const std::string& getFocusName() { return m_FocusName; }
        /**
         * @return Main animation scheme name prefix
         */
        const std::string& getMainSchemeName() { return m_MainSchemeName; }
        /**
         * @return If camera position should be locked by CameraController
         */
        bool isCameraLocked() { return m_lockCamera; }
        /**
         * @return Access to the internal core. Handle with care.
         */
        MobCore* getCore() { return m_MobCore; }
        /**
         * @return Classes which want to get exported on save should return true here
         */
        virtual bool shouldExport() { return true; }
        virtual void importObject(const json& j);

    protected:
        virtual void exportPart(json& j) override;

        /**
         * Starts the interaction using the given npc
         * @param npc
         */
        void startInteraction(Handle::EntityHandle npc);

        /**
         * Starts a state-change between the two given states. This starts the animations, etc
         * @param npc NPC to do the change on
         * @param from State from
         * @param to State to
         */
        void startStateChange(Handle::EntityHandle npc, int from, int to);

        /**
         * Stops the interaction of the given NPC
         * @param npc
         */
        void stopInteraction(Handle::EntityHandle npc);

        /**
         * Checks what the animations between two states are called and outputs them
         * @param stateFrom State to go from
         * @param stateTo State to go to. If this is -1, the npc will go back to standing
         * @param outMobAnimation animation the mob should play
         * @param outNpcAnimation animation the npc should play
         */
        void getAnimationTransitionNames(int stateFrom,
                                         int stateTo,
                                         std::string& outMobAnimation,
                                         std::string& outNpcAnimation);

        /**
         * Scans the current visual for interact-positions
         */
        void findInteractPositions();

        // Max number of npcs which can use this mob at once
        unsigned int m_NumNpcsMax;

        // Number of NPCs currently using this Mob
        unsigned int m_NumNpcsCurrent;

        // Positions this vob can be interacted with from
        std::vector<InteractPosition> m_InteractPositions;

        // Script-function name-base to call on state changes (<name>_S<idx>)
        std::string m_OnStateFuncName;

        // Active Mob-Core
        MobCore* m_MobCore;

        // Focus name (BED, LADDER, ...)
        std::string m_FocusName;

        // Class which this vob had in the old engine
        std::string m_zObjectClass;

        // Main sceme name
        std::string m_MainSchemeName;

        // lock camera position during usage
        bool m_lockCamera;
    };
}
