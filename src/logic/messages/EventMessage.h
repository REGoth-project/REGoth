#pragma once

#include <handle/HandleDef.h>
#include <engine/Waynet.h>
#include <list>
#include <ZenLib/daedalus/DaedalusGameState.h>
#include "../LogicDef.h"
#include <audio/AudioWorld.h>

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
            Mob,

            SndPlay,
            SndStarted,
            SndStopped
        };

        /**
         * Basic event-message. Contains type.
         */
        struct EventMessage
        {
            using SharedEMessage = std::shared_ptr<EventMessage>;

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
            virtual ~EventMessage() {}
            /**
             * Export as JSON-String
             */
            virtual std::string exportPart();

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

            /**
             * Adds a callback, triggered when this message has been successfully and completely processed
             * @param hostVob Vob which the callback is set from
             * @param callback Callback function
             */
            void addDoneCallback(Handle::EntityHandle hostVob, std::function<void(Handle::EntityHandle hostVob, SharedEMessage)> callback)
            {
                onMessageDone.push_back(std::make_pair(hostVob, callback));
            }

            /**
             * External callbacks to trigger if this message gets processed. Must also store the waiting entity.
             */
            std::list<std::pair<Handle::EntityHandle, std::function<void(Handle::EntityHandle, SharedEMessage)>>> onMessageDone;
        };

        struct NpcMessage : public EventMessage
        {
            NpcMessage()
            {
                messageType = EventMessageType::Npc;
                isJob = true;
            }
            virtual ~NpcMessage() {}
            /**
             * Export as JSON-String
             */
            virtual std::string exportPart();

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

            /**
             * Export as JSON-String
             */
            virtual std::string exportPart();

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
             * Export as JSON-String
             */
            virtual std::string exportPart();

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
                Run = 0,
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
                targetMode = 0;
            }

            /**
             * Export as JSON-String
             */
            virtual std::string exportPart();

            /**
             * Route the NPC has to go if this is a ST_GoRoute.
             * If empty, targetPos is used.
             */
            std::vector<World::Waynet::WaypointIndex> route;

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

            /**
             * General purpose mode
             */
            int targetMode;
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
             * Export as JSON-String
             */
            virtual std::string exportPart();

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

            /**
             * Export as JSON-String
             */
            virtual std::string exportPart();

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
            } ConversationMessage;

            StateMessage()
            {
                messageType = EventMessageType::State;
                isPrgState = false;
                isRoutineState = false;
            }

            /**
             * Export as JSON-String
             */
            virtual std::string exportPart();

            /**
             * Symbol to the Setupfunction for this state (ZS_...)
             */
            size_t functionSymbol;

            /**
             * Whether the old state should be ended properly (true), or just interrupted (false)
             */
            bool endOldState;

            /**
             * Symbols for other and victim
             */
            Daedalus::GameState::NpcHandle other;
            Daedalus::GameState::NpcHandle victim;

            /**
             * Whether this belongs to the daily routine
             */
            bool isRoutineState;

            /**
             * Whether this is a program-managed state. List of states in NpcScriptState.h
             */
            bool isPrgState;

            /**
             * If this is a wait-message, this is how long we should wait
             */
            float waitTime;

            /**
             * Waypoint name to got to, in case the state needs that
             */
            std::string wpname;
        };

        class ManipulateMessage : public NpcMessage
        {
        public:
            enum ManipulateSubType
            {
                ST_TakeVob = 0,
                ST_DropVob,
                ST_ThrowVob,
                ST_Exchange,
                ST_UseMob,
                ST_UseItem,
                ST_InsertInteractItem,
                ST_RemoveInteractItem,
                ST_CreateInteractItem,
                ST_DestroyInteractItem,
                ST_PlaceInteractItem,
                ST_ExchangeInteractItem,
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
                flag = false;
                pickupAniY = 0.0f;
                targetState = 0;
                symIdx = static_cast<size_t>(-1);
            }

            /**
             * Export as JSON-String
             */
            virtual std::string exportPart();

            /**
             * Symbol of the item to use. If no item with this symbol can be found in the inventory, nothing happens
             * Alternatively, you can set "targetItem" to a valid handle.
             */
            size_t symIdx;

            /**
             * Handle of the item to use. Alternatively you could set "name" to something found in the inventory
             */
            Daedalus::GameState::ItemHandle targetItem;

            /**
             * Slot the item should be put in, in case this is a ST_*InteractItem
             */
            std::string slot;

            /**
             * General purpose flag
             */
            bool flag;

            /**
             * Y-coord of the the pickup animation?
             */
            float pickupAniY;

            /**
             * State to go to when using the item
             */
            int targetState;

            /**
             * Animation to play, for example on pickup
             */
            std::string animation;
        };

        struct ConversationMessage : public NpcMessage
        {
            // These need to be compatible with scripts
            enum ConversationSubType
            {
                ST_PlayAniSound = 0,
                ST_PlayAni,
                ST_PlaySound,
                ST_LookAt,
                ST_Output,
                ST_OutputMonolog,
                ST_OutputEnd,
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

            enum class Status
            {
                INIT,  // never seen by PlayerController yet
                PLAYING,
                FADING_OUT
            };

            ConversationMessage()
            {
                messageType = EventMessageType::Conversation;
                status = Status::INIT;
                canceled = false;
                waitIdentifier = nullptr;
            }

            /**
             * Shortcut-function to play animations
             */
            static ConversationMessage playAnimation(const std::string& anim)
            {
                ConversationMessage msg;
                msg.subType = ST_PlayAni;
                msg.animation = anim;

                return msg;
            }

            /**
             * Export as JSON-String
             */
            virtual std::string exportPart();

            /**
             * Text to be displayed in the subtitle box
             */
            std::string text;

            /**
             * Name of the SFX to play
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

            /**
             * SharedPointer to the message we are waiting for. Currently not used. Useful for debug purpose
             */
            SharedEMessage waitIdentifier;

            /**
             * stores the state of the message, to handle multiple stages/states inside the Playercontroller
             */
            Status status;

            /**
             * Whether this message has been kindly canceled by the ouside (i.e. DialogManager).
             */
            bool canceled;

            /**
             * Ticket. Can be used to ask AudioWorld if sound is playing.
             */
            Utils::Ticket<World::AudioWorld> soundTicket;
        };

        struct MagicMessage : public NpcMessage
        {
            enum MagicSubType
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

            /**
             * Export as JSON-String
             */
            virtual std::string exportPart();

            // TODO: Implement
        };

        struct MobMessage : public EventMessage
        {
            enum MobSubType
            {
                ST_STARTINTERACTION = 0,
                ST_STARTSTATECHANGE,
                ST_ENDINTERACTION,
                ST_UNLOCK,
                ST_LOCK,
                ST_CALLSCRIPT
            };

            MobMessage()
            {
                messageType = EventMessageType::Mob;
                stateFrom = 0;
                stateTo = 0;
            }

            /**
             * Export as JSON-String
             */
            virtual std::string exportPart();

            // NPC this message is from
            Handle::EntityHandle npc;

            // State to change from
            int stateFrom;

            // State to change to
            int stateTo;

            // Whether to play an animation
            bool playAnimation;
        };
    }
}
