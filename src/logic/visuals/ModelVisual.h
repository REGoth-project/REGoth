#pragma once
#include "../VisualController.h"
#include <handle/HandleDef.h>

namespace Logic
{
    enum class EModelNode
    {
		None,
        Righthand,
        Lefthand,
        Sword,
        Longsword,
        Bow,
        Crossbow,
        Shield,
        Helmet,
        Jaws,
        Torso,
    };

    class ModelVisual : public VisualController
    {
    public:

        enum EModelAnimType
        {
			// Normal
			Walk,
			Run,
			Idle,
			Backpedal,
			StrafeLeft,
			StrafeRight,
			Jump,
			TurnLeft,
			TurnRight,

			// 1h
			Walk1h,
			Run1h,
			Idle1h,
			Backpedal1h,
			StrafeLeft1h,
			StrafeRight1h,
			Attack1h,
			Attack1h_L,
			Attack1h_R,
			Attack1h_Move,
			Attack1h_Finish,
			Parade1h,
			Parade1h_Back,
			Draw1h,

			// 2h
			Walk2h,
			Run2h,
			Idle2h,
			Backpedal2h,
			StrafeLeft2h,
			StrafeRight2h,
			Attack2h,
			Attack2h_L,
			Attack2h_R,
			Attack2h_Move,
			Attack2h_Finish,
			Parade2h,
			Parade2h_Back,
			Draw2h,

			// Bow
			WalkBow,
			RunBow,
			IdleBow,
			BackpedalBow,
			StrafeLeftBow,
			StrafeRightBow,
			AttackBow,
			DrawBow,

			// Cbow
			WalkCBow,
			RunCBow,
			IdleCBow,
			BackpedalCBow,
			StrafeLeftCBow,
			StrafeRightCBow,
			AttackCBow,
			DrawCBow,

			// Fist
			WalkFist,
			RunFist,
			IdleFist,
			BackpedalFist,
			StrafeLeftFist,
			StrafeRightFist,
			AttackFist,
			ParadeFist,
			ParadeFist_Back,
			DrawFist,

         // Swimming
         Swim,
         SwimB,
         SwimF,
         SwimTurnLeft,
         SwimTurnRight,
         SwimToDive,
         SwimToHang, // No idea what that is, can't remember it from the original game
         Wade,
         // Transitions
			// TODO: Transitions, Running attacks, weapon walking, sneaking, backwards walking, attack while running, magic, other

			NUM_ANIMATIONS
        };

        struct BodyState
        {
            BodyState()
            {
                headTextureIdx = 0;
                teethTextureIdx = 0;
                bodySkinColorIdx = 0;
                bodyTextureIdx = 0;
            }

            std::string headVisual;
            std::string bodyVisual;
            int headTextureIdx;
            int teethTextureIdx;
            int bodySkinColorIdx;
            int bodyTextureIdx;
        };

        /**
         * @param world World of the underlaying entity
         * @param entity Entity owning this controller
         */
        ModelVisual(World::WorldInstance& world, Handle::EntityHandle entity);
        virtual ~ModelVisual();

		/**
         * @return Classtype of this visual
         */
		virtual EVisualType getVisualType(){ return EVisualType::Model; }

        /**
         * Loads the visual from the given name from the worlds VDFS
         * Note: The type must match this class
         * Note2: This should not be used directly (Only while creating the NPC), use setBodyState instead!
         * @param visual Filename of the visual. Uncompiled-version of the name!
         */
        virtual bool load(const std::string& visual);

        /**
         * Sets the headmesh for this model
         * Note: Will do an implicit setBodyState!
         * @param head File of the mesh to use as head
         */
        void setHeadMesh(const std::string& head, int headTextureIdx=0, int teethTextureIdx=0);

        /**
         * Attaches a visual to the given node
         * @param visual The visual to load and attach
         * @param node to attach the visual to
         */
        Handle::EntityHandle setNodeVisual(const std::string& visual, const std::string& nodeName);
        Handle::EntityHandle setNodeVisual(const std::string& visual, EModelNode node);

        /**
         * @brief Sets the currently playing animation. Empty string for none
         */
        void setAnimation(const std::string& anim, bool loop = true);

        /**
         * @brief Sets the closest animation to the specified type. A goblin uses FistWalk instead of Run, for example.
         */
		void setAnimation(EModelAnimType type, bool loop = true);

		/**
		 * @brief Applys the given animation overlay
		 */
		void applyOverlay(const std::string& mds);

		/**
		 * Plays the given animation exactly one time. Has more priority than setAnimation.
		 * @param type Animation to play
		 */
		void playAnimation(EModelAnimType type);

		/**
		 * Stops all playing animations
		 */
		void stopAnimations();

		/**
		 * @return Current name of the given animation type (Reacts to overlays)
		 */
		static const char* getAnimationName(EModelAnimType type);


        /**
         * @brief Updates the attachments transforms using the given object-space transforms
         */
        void updateAttachmentTransforms(const std::vector<Math::Matrix>& transforms);
        void updateAttachmentTransforms();

        /**
         * @brief Returns the underlaying animation-handler of this model
         */
        Components::AnimHandler& getAnimationHandler();

        /**
         * @brief To be called every frame. You have to take care of this by yourself if you are using the model-visual!
         */
        void onFrameUpdate(float dt);

        /**
         * @brief Called when something else modified the transform of the underlaying entity
         */
        virtual void onTransformChanged() override;

        /**
         * Searches through the list of nodes and returns the index of the node with the given name
         * @param name Name to look for
         * @return Index of the node with the given name
         */
        size_t findNodeIndex(const std::string& name);

        /**
         * @return Visuals this body uses other than the main one (Head, armor, etc)
         */
        const BodyState& getBodyState(){ return m_BodyState; }

        /**
         * Sets a body-state
         */
        void setBodyState(const BodyState& state);

        /**
         * @return Distance from (0,0,0) to the feet
         */
        void getCollisionBBox(Math::float3 bb[2]);

        /**
         * @return Root position of the model
         */
        Math::float3 getModelRoot();

        /**
         * Sets the shadow-value from the worldmesh for this visual-controller
         */
        virtual void setShadowValue(float shadow);

		/**
         * @return Skeleton-information for the currently set mesh
         */
		const ZenLoad::zCModelMeshLib& getMeshLib();

		/**
		 * @return Whether an animation with the given name is playing
		 */
		 bool isAnimPlaying(const std::string& name);
    protected:

        /**
         * Replaces the current body mesh of this model from the current body-state
         */
        void updateBodyMesh();

        /**
         * Replaces the current headmesh of this model from the current body-state
         */
        void updateHeadMesh();

		/**
		 * Attaches the attachments in m_AttachmentVisualsByNode again
		 */
		void updateAttachmentVisuals();

        /**
         * Rebuilds the main entity list from everything found inside the PartEntityLists
         */
        void rebuildMainEntityList();

        /**
         * Handle to the main-mesh and it's skeleton information
         */
        Handle::MeshHandle m_MainMeshHandle;

        /**
         * Vectors of the entities created for the different parts of a model
         */
        struct
        {
            std::vector<Handle::EntityHandle> mainSkelMeshEntities;
            std::vector<Handle::EntityHandle> headMeshEntities;
            std::vector<Handle::EntityHandle> dynamicAttachments;
            Handle::EntityHandle headEntity;
        }m_PartEntities;

        /**
         * Visuals attached to the models nodes. First vector: Index <-> node
         */
        std::vector<std::vector<Handle::EntityHandle>> m_VisualAttachments;

        /**
         * Bind-pose matrices
         */
        std::vector<Math::Matrix> m_BindPoseTransforms;

        /**
         * Visuals this body uses other than the main one (Head, armor, etc)
         */
        BodyState m_BodyState;

        /**
         * Last known animation state hash from the animation controller
         */
        size_t m_LastKnownAnimationState;

		/**
		 * State of the attachments of the nodes. [Node-name, visual]
		 */
		std::map<std::string, std::string> m_AttachmentVisualsByNode;
    };
}
