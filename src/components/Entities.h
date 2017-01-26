#pragma once
#include <utils/Utils.h>
#include <math/mathlib.h>
#include <handle/Handle.h>
#include <handle/HandleDef.h>
#include <content/StaticLevelMesh.h>
#include <content/VertexTypes.h>
#include <memory/AllocatorBundle.h>
#include <memory/Config.h>
#include <engine/WorldTypes.h>
#include "AnimHandler.h"

/**
 * List of all available components
 */
#define ALL_COMPONENTS   EntityComponent,\
                         LogicComponent,\
                         PositionComponent,\
                         NBBoxComponent,\
                         BBoxComponent,\
                         StaticMeshComponent,\
                         CompoundComponent,\
                         ObjectComponent,\
                         VisualComponent,\
                         AnimationComponent,\
                         PhysicsComponent,\
                         SpotComponent,\
                         PfxComponent

namespace Logic
{
    class Controller;
    class VisualController;
}

namespace Components
{
    typedef uint32_t ComponentMask;

	struct Component : public Handle::HandleTypeDescriptor<Handle::EntityHandle>
	{
        ~Component(){};
	};

    /**
     * Component which can be expected to be valid on all entities.
     * This stores, which components are valid for that entity.
     * Since every entity creates all components in the background, a component being "valid"
     * only means, that it's flag was registered inside m_ComponentMask. The memory would be there in all cases,
     * however accessing a component without it being registered is not supported and could lead to undefined behavior
     * (or just 'nothing', in most cases really)
     */
    struct EntityComponent : public Component
    {
        ComponentMask m_ComponentMask;

        // Handle of this entity-component
        Handle::EntityHandle m_ThisEntity;

        static void init(EntityComponent& c)
        {

        }
    };

    struct PositionComponent : public Component
    {
        enum { MASK = 1 << 1 };
        Math::Matrix m_WorldMatrix;


        /*+
         * Factor to apply to global drawing distance before applying the check
         */
        float m_DrawDistanceFactor;

        static void init(PositionComponent& c)
        {
            c.m_WorldMatrix = Math::Matrix::CreateIdentity();
            c.m_DrawDistanceFactor = 1.0f;
        }
    };

    /**
     * Entity with one or more BBoxes.
     * Beware of the cache-miss when accessing these!
     */
    struct NBBoxComponent : public Component
    {
        enum { MASK = 1 << 2 };

        std::vector<Utils::BBox3D> m_BBox3D;

        static void init(NBBoxComponent& c)
        {
        }
    };

    /**
     * Entitiy with only one BBox
     */
    struct BBoxComponent : public Component
    {
        enum { MASK = 1 << 3 };

        Utils::BBox3D m_BBox3D;
        uint32_t m_DebugColor;

        static void init(BBoxComponent& c)
        {
            c.m_BBox3D.min = Math::float3(0,0,0);
            c.m_BBox3D.max = Math::float3(0,0,0);
            c.m_DebugColor = 0;
        }
    };

    /**
     * Entity with a static mesh
     */
    struct StaticMeshComponent : public Component
    {
        enum { MASK = 1 << 4 };

        /**
         * Handle to the mesh to render.
         * Note: You must find the right allocator to this handle yourself!
         * TODO: Maybe give an enum or something to help to find the allocator
         */
        Handle::MeshHandle m_StaticMeshVisual;
		Meshes::SubmeshVxInfo m_SubmeshInfo;
        uint32_t m_SubmeshIdx;
		Handle::TextureHandle m_Texture; // TODO: Put this into a material container!
        uint32_t m_Color;

        /**
         * Index of the instance-buffer used at render time
         * Special values:
         *  -1 no index value placed yet
         *  -2 completely disable instancing on this
         *
         *  // FIXME: This has been moved to the mesh itself. This field is only used for instancing enabled/disabled.
         */
        uint32_t m_InstanceDataIndex;

        static void init(StaticMeshComponent& c)
        {
            c.m_Color = 0xFFFFFFFF;
            c.m_InstanceDataIndex = (uint32_t)-1;
        }
    };

    /**
     * Generic logic component, can execute scripts, etc
     */
    struct LogicComponent : public Component
    {
        enum { MASK = 1 << 5 };

        /**
         * This is a pointer to the logic-controller of the world this lives in.
         * Since these are imlplemented by using OOP, this is a direct pointer to the controller instance.
         * It is important, that this pointer is NOT STORED across frames or even frame-sections!
         * Also you have to take care when creating/freeing this component
         */
        Logic::Controller* m_pLogicController;

        static void init(LogicComponent& c)
        {
            c.m_pLogicController = nullptr;
        }
    };

    struct VisualComponent : public Component
    {
        enum { MASK = 1 << 6 };

        /**
         * This is a pointer to the visual-controller of the world this lives in.
         * Since these are imlplemented by using OOP, this is a direct pointer to the controller instance.
         * It is important, that this pointer is NOT STORED across frames or even frame-sections!
         * Also you have to take care when creating/freeing this component
         */
        Logic::VisualController* m_pVisualController;

        static void init(VisualComponent& c)
        {
            c.m_pVisualController = nullptr;
        }
    };

    struct CompoundComponent : public Component
    {
        enum { MASK = 1 << 7 };

        /**
         * Entities belonging to this one.
         * Note: This simply is an std::vector beacause accessing attachments like
         *       this won't happen too often and won't be a performance issue, most likely.
         */
        std::vector<Handle::EntityHandle> m_Attachments;

        /**
         * If this entitiy is part of a compound, then this will be set to the entity containing the compound.
         */
        Handle::EntityHandle m_Parent;

        static void init(CompoundComponent& c)
        {
        }
    };

    struct ObjectComponent : public Component
    {
        enum { MASK = 1 << 8 };

        enum EObjectType
        {
            Vob,
            NPC,
            Item,
            Other
        };

        /**
         * Name of this object
         */
        std::string m_Name;

        /**
         * Object-type
         */
        EObjectType m_Type;

        /**
         * Whether collision should be enabled on this
         */
        bool m_EnableCollision;


        static void init(ObjectComponent& c)
        {
            c.m_Type = Other;
            c.m_EnableCollision = false;
        }
    };

    /**
	 * Handles current animations for the entity. Stores the results inside the instance-data-
	 */
    struct AnimationComponent : public Component
    {
        enum { MASK = 1 << 9 };

        /**
         * Storage for animations of this model
         */
        AnimHandler* m_AnimHandler;

        /**
         * If this is set to something valid, the anim-handler of this will be ignored and the one of the
         * set entity will be used
         */
        Handle::EntityHandle m_ParentAnimHandler;

        AnimHandler& getAnimHandler(){ return *m_AnimHandler; }

        static void init(AnimationComponent& c)
        {
            c.m_AnimHandler = new AnimHandler;
        }
    };

    /**
     * Handles collision and collision response
     */
    struct PhysicsComponent : public Component
    {
        enum { MASK = 1 << 10 };

        /**
         * The rigid-Body of this component
         */
        Handle::PhysicsObjectHandle m_PhysicsObject;

        /**
         * Whether this physics-object will move by it's own
         */
        bool m_IsStatic;

        static void init(PhysicsComponent& c)
        {
            c.m_IsStatic = true;
        }
    };

    struct SpotComponent : public Component
    {
        enum { MASK = 1 << 11 };

        /**
         * Entity currently on this spot
         */
        Handle::EntityHandle m_UsingEntity;

        /**
         * Time when this spot will be free again
         */
        float m_UseEndTime;

        static void init(SpotComponent& c)
        {
            c.m_UseEndTime = 0.0f;
        }
    };

    struct PfxComponent : public Component
    {
        enum { MASK = 1 << 12 };

        /**
         * Single particle
         */
        struct Particle
        {
            Math::float3 position;
            Math::float3 velocity;
            float lifetime;
            float texAniFrame;
            float alphaVel;
            float alpha;
            Math::float2 size;
            Math::float2 sizeVel;
            Math::float3 color;
            Math::float3 colorVel;
            uint32_t particleColorU8; // Actual color for this frame
        };

        bgfx::DynamicVertexBufferHandle m_ParticleVB;
        Handle::TextureHandle m_Texture;
        uint64_t m_bgfxRenderState;

        std::vector<Particle> m_Particles;

        static void init(PfxComponent& c)
        {
            c.m_bgfxRenderState = BGFX_STATE_DEFAULT | BGFX_STATE_BLEND_ADD;
            c.m_ParticleVB.idx = BGFX_INVALID_HANDLE;
        }
    };

    /**
     * Adds a component to the given Entity-Component
     */
    template<typename T>
    void addComponent(EntityComponent& e)
    {
        // See "EntityComponent" for further information
        e.m_ComponentMask |= T::MASK;
    }

    /**
     * Removes a component from the given Entity-Component
     */
    template<typename T>
    void removeComponent(EntityComponent& e)
    {
        // See "EntityComponent" for further information
        e.m_ComponentMask &= ~T::MASK;
    }

    /**
     * Checks if the given component is present in the entity
     */
    template<typename T>
    bool hasComponent(const EntityComponent& e)
    {
        // See "EntityComponent" for further information
        return (e.m_ComponentMask & T::MASK) != 0;
    }

    /**
     * Creates a ComponentMask from uint32_t, for cleaner access
     */
    static ComponentMask makeEntityMask(uint32_t mask)
    {
        return static_cast<ComponentMask>(mask);
    }

    /**
     * Default allocator-type
     */
    typedef Memory::AllocatorBundle<Config::MAX_NUM_LEVEL_ENTITIES, EntityComponent::HandleType, ALL_COMPONENTS> ComponentAllocator;
}