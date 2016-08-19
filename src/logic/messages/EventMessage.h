#pragma once

#include <handle/HandleDef.h>
#include <engine/Waynet.h>
#include "../LogicDef.h"

namespace Logic
{
    namespace EventMessages
    {
        enum class EventMessageType
        {
            Event,
            Npc,
            Damage,
            Weapon,
            Movement,
            Attack,
            UseItem,
            State,
            Manipulate,
            Conversation,
            Magic,
        };

        /**
         * Basic event-message. Contains type.
         */
        struct EventMessage
        {
            EventMessage()
            {
                subType = 0;
                messageType = EventMessageType::Event;
                isJob = false;
                deleted = false;
                isHighPriority = false;
                inUse = false;
                isOverlay = false;
            }

            /**
             * Type of class this can be casted to
             */
            EventMessageType messageType;

            /**
             * Vob this was targeted at
             */
            Handle::EntityHandle targetVob;

            /**
             * Vob this came from
             */
            Handle::EntityHandle sourceVob;

            /**
             * Message subtype defined by the child-classes
             */
            unsigned int subType;

            /**
             * Whether this should wait for other messages to complete before triggering
             */
            bool isJob;

            /**
             * Flag used for cleanup
             */
            bool deleted;

            /**
             * Whether this should overwrite existing messages
             */
            bool isHighPriority;

            /**
             * Whether this is currently playing
             */
            bool inUse;

            /**
             * Whether this message should not block further execution of the message-queue
             */
            bool isOverlay;
        };

        struct NpcMessage : public EventMessage
        {
            NpcMessage()
            {
                messageType = EventMessageType::Npc;
                isJob = true;
            }

            /**
             * Name of the vob this message targets. This could be the attack target or a waypoint to go to.
             */
            std::string targetVobName;
        };

        struct DamageMessage : public NpcMessage
        {
            enum class DamageSubType
            {
                Once,
                PerFrame,
                Max
            };

            DamageMessage()
            {
                messageType = EventMessageType::Damage;
            }

            // TODO: Implement DamageDescriptor!
        };

        struct WeaponMessage : public NpcMessage
        {
            enum WeaponSubType
            {
                ST_DrawWeapon,
                ST_DrawWeapon1,
                ST_DrawWeapon2,
                ST_RemoveWeapon,
                ST_RemoveWeapon1,
                ST_RemoveWeapon2,
                ST_ChooseWeapon,
                ST_ForceRemoveWeapon,
                ST_Attack,
                ST_EquipBestWeapon,
                ST_EquipBestArmor,
                ST_UnequipWeapons,
                ST_UnequipArmor,
                ST_EquipArmor,
                ST_WeaponMax
            };

            WeaponMessage()
            {
                messageType = EventMessageType::Weapon;
            }

            /**
             * Type of weapon to go to
             */
            EWeaponMode targetMode;

            // TODO: There is an animation-field here, find out what it does!
        };

        struct MovementMessage : public NpcMessage
        {
            enum MovementSubType
            {
                ST_RobustTrace,
                ST_GotoPos,
                ST_GotoVob,
                ST_GoRoute,
                ST_Turn,
                ST_TurnToPos,
                ST_TurnToVob,
                ST_TurnAway,
                ST_Jump,
                ST_SetWalkMode,
                ST_WhirlAround,
                ST_Standup,
                ST_CanSeeNpc,
                ST_Strafe,
                ST_GotoFP,
                ST_Dodge,
                ST_BeamTo,
                ST_AlignToFP,
                ST_MoveMax
            };

            enum class WalkMode
            {
                Run,
                Walk,
                Sneak,
                Water,
                Swim,
                Dive
            };

            MovementMessage()
            {
                messageType = EventMessageType::Movement;
                walkMode = WalkMode::Run;
            }

            /**
             * Route the NPC has to go if this is a ST_GoRoute.
             * If empty, targetPos is used.
             */
            std::vector <World::Waynet::WaypointIndex> route;

            /**
             * Vob we are going to if this is a ST_GotoVob
             */
            Handle::EntityHandle targetVob;

            /**
             * Position we shall go to, if this is a ST_GotoPos or route and name are emtpy
             */
            Math::float3 targetPosition;

            /**
             * How we should go to our position
             */
            WalkMode walkMode;
        };

        struct AttackMessage : public NpcMessage
        {
            enum AttackSubType
            {
                ST_AttackForward,
                ST_AttackLeft,
                ST_AttackRight,
                ST_AttackRun,
                ST_AttackFinish,
                ST_Parade,
                ST_AimAt,
                ST_ShootAt,
                ST_StopAim,
                ST_Defend,
                ST_AttackBow,
                ST_AttackMagic,
                ST_AttackMax
            };

            AttackMessage()
            {
                messageType = EventMessageType::Attack;
                startFrame = 0.0f;
                animationIndex = static_cast<size_t>(-1);
            }

            /**
             * How far we are in our combo
             */
            int combo;

            /**
             * Vob we are attacking
             */
            Handle::EntityHandle targetVob;

            /**
             * Animation to use when attacking, -1 if none.
             */
            size_t animationIndex;

            /**
             * Frame to skip the animation to
             */
            float startFrame;
        };

        struct UseItemMessage : public NpcMessage
        {
            enum UseItemSubType
            {
                EV_Drink,
                EV_EquipItem,
                EV_UnequipItem,
                EV_UseItemMax
            };

            UseItemMessage()
            {
                messageType = EventMessageType::UseItem;
            }

            // TODO: We have no actual object for these right now, as they are explicitly in the inventory.
            // TODO: Use the itemhandle here or something
        };

        struct StateMessage : public NpcMessage
        {
            enum TStateSubType
            {
                EV_StartState,
                EV_Wait,
                EV_SetNpcsToState,
                EV_SetTime,
                EV_ApplyTimedOverlay,
                EV_StateMax
            };

            StateMessage()
            {
                messageType = EventMessageType::State;
            }

            // TODO: Implement
        };

        class ManipulateMessage : public NpcMessage
        {
            enum ManipulateSubType
            {
                ST_TakeVob = 0,
                ST_DropVob,
                ST_ThrowVob,
                ST_Exchange,
                ST_UseMob,
                ST_UseItem,
                ST_InsertInteractitem,
                ST_RemoveInteractitem,
                ST_CreateInteractitem,
                ST_DestroyInteractitem,
                ST_PlaceInteractitem,
                ST_ExchangeInteractitem,
                ST_UseMobWithItem,
                ST_CallScript,
                ST_EquipItem,
                ST_UseItemToState,
                ST_TakeMob,
                ST_DropMob,
                ST_ManipMax
            };

            ManipulateMessage()
            {
                messageType = EventMessageType::Manipulate;
            }

            // TODO: Implement
        };

        struct ConversationMessage : public NpcMessage
        {
            enum ConversationSubType
            {
                ST_PlayAniSound,
                ST_PlayAni,
                ST_PlaySound,
                ST_LookAt,
                ST_Output,
                ST_OutputSVM,
                ST_Cutscene,
                ST_WaitTillEnd,
                ST_Ask,
                ST_WaitForQuestion,
                ST_StopLookAt,
                ST_StopPointAt,

                ST_PointAt,
                ST_QuickLook,
                ST_PlayAni_NoOverlay,
                ST_PlayAni_Face,

                ST_ProcessInfos,
                ST_StopProcessInfos,
                ST_OutputSVM_Overlay,
                ST_SndPlay,
                ST_SndPlay3d,
                ST_PrintScreen,
                ST_StartFx,
                ST_StopFx,
                ST_ConvMax
            };

            ConversationMessage()
            {
                messageType = EventMessageType::Conversation;
            }

            /**
             * Text to be displayed in the subtitle box
             */
            std::string text;

            /**
             * Name of the .wav to play
             */
            std::string name;

            /**
             * Target-vob to look at
             */
            Handle::EntityHandle target;

            /**
             * Target-pos to look at
             */
            Math::float3 targetPosition;

            /**
             * Animation to play. -1 if none
             */
            size_t animationIndex;

            /**
             * Animation name to be used. If empty, index will be checked.
             */
            std::string animation;
        };

        struct MagicMessage : public NpcMessage
        {
            enum ConversationSubType
            {
                ST_Open,
                ST_Close,
                ST_Move,
                ST_Invest,
                ST_Cast,
                ST_SetLevel,
                ST_ShowSymbol,
                ST_SetFrontSpell,
                ST_CastSpell,
                ST_Ready,
                ST_Unready,
                ST_MagicMax
            };

            MagicMessage()
            {
                messageType = EventMessageType::Magic;
            }

            // TODO: Implement
        };
    }
}