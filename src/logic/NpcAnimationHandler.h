#pragma once
#include "LogicDef.h"
#include <components/AnimHandler.h>
#include <content/Animation.h>
#include <content/AnimationLibrary.h>
#include <handle/HandleDef.h>

namespace World
{
    class WorldInstance;
}

namespace Logic
{
    class PlayerController;
    class NpcAnimationHandler
    {
    public:
        NpcAnimationHandler(World::WorldInstance& world, Handle::EntityHandle hostVob);
        ~NpcAnimationHandler();

        /**
         * Initializes all animations for the currently loaded model
         */
        void initAnimations();

        /**
         * Makes the model play the go-forward animation (with transitions)
         */
        bool Action_GoForward();

        /**
         * Makes the model play the go-backward animation (with transitions)
         */
        void Action_GoBackward();

        /**
         * Makes the model play the strafe-left animation (with transitions)
         */
        void Action_StrafeLeft();

        /**
         * Makes the model play the strafe-right animation (with transitions)
         */
        void Action_StrafeRight();

        /**
         * Makes the model play the turn-left animation (with transitions)
         */
        void Action_TurnLeft();

        /**
         * Makes the model play the turn-right animation (with transitions)
         */
        void Action_TurnRight();

        /**
         * Plays the pull-weapon animation (with transitions)
         * @param part Animation is segmented into two parts
         */
        void Action_DrawWeapon(int part);

        /**
         * Plays the remove-weapon animation (with transitions)
         */
        void Action_UndrawWeapon();

        /**
         * Plays the fight-forward animation (with transitions)
         */
        void Action_FightForward();

        /**
         * Goes back to the standing-state, when possible
         */
        void Action_Stand(bool force = false, bool allowTurning = false);

        /**
         * Stops any of the turning-animations, if they are currently playing
         */
        void stopTurningAnimations();

        /**
         * Checks whether the given animation is currently active
         * @param anim Animation to look for
         * @return Whether the given animation is currently being played
         */
        bool isAnimationActive(Handle::AnimationHandle anim);
        bool isAnimationActive(const std::string& anim);

        /**
         * @return Whether the model is currently standing around
         */
        bool isStanding(bool allowTurning = false);

        /**
         * @return Controller this is attached to
         */
        PlayerController& getController() const;

        /**
         * @return This players animation handler
         */
        Components::AnimHandler& getAnimHandler() const;

        /**
         * @return Global animation library
         */
        Animations::AnimationLibrary& getAnimLib() const;

        /**
         * @return String used for naming animations from the given type (ie. 1H, 2H, CBOW)
         */
        std::string getWeaponAniTag(EWeaponMode weapon);

    protected:
        /**
         * Starts playing the animation for going in a certain direction
         */
        void startAni_Forward();
        void startAni_Backward();
        void startAni_StrafeLeft();
        void startAni_StrafeRight();
        void startAni_Stand(bool force = false, bool allowTurning = false);
        void startAni_TurnLeft();
        void startAni_TurnRight();
        void startAni_DrawWeapon(int part);
        void startAni_UndrawWeapon();
        void startAni_FightForward();

        /**
         * Starts the turning-animations and does the physical turn
         * @param direction negative for left, positive for right
         */
        void StartAni_Turn(float direction);

        /**
         * Plays an animation, but tries to find the regarding transition-files as well.
         * If a transition-file was found, that one is used and a value of true is returned.
         * Otherwise the animation is played straight away and a value of false is returned.
         * @param anim Animation to start playing
         * @return True, if a transition-file has been found, false otherwise. Animation will play anyways
         */
        bool playAnimationTrans(const std::string& anim);

        /**
         * Plays the given animation on the model
         * @param anim Animation to play
         */
        void playAnimation(Handle::AnimationHandle anim);
        void playAnimation(const std::string& anim);

        /**
         * Goblins use the "fist"-modes like all other monsters, even though they are holding a 1h-weapon.
         * Maybe someone can come up with a nicer solution than this when we completely figured that one out.
         * TODO: Completely figure that one out.
         *
         * @param targetMode Mode that is requested
         * @return actual mode that should be used
         */
        EWeaponMode patchGoblinWeaponMode(EWeaponMode targetMode);

        /**
         * Checks whether crucial animations exist within this weapon-mode
         * @param weaponMode Weaponmode to check the animations for
         * @return Whether this is okay to use for a real monster
         */
        bool isAnimationSetUsable(EWeaponMode weaponMode);

        struct AnimationSet
        {
            /* ---- run ---- */
            Handle::AnimationHandle s_run;
            Handle::AnimationHandle s_runl, t_run_2_runl, t_runl_2_run;
            Handle::AnimationHandle s_runr, t_run_2_runr, t_runr_2_run;
            Handle::AnimationHandle t_runr_2_runl, t_runl_2_runr;

            Handle::AnimationHandle s_runbl, t_run_2_runbl, t_runbl_2_run;
            Handle::AnimationHandle s_runbr, t_runbr_2_run;
            Handle::AnimationHandle t_runbr_2_runbl, t_runbl_2_runbr;

            Handle::AnimationHandle t_jumpb;
            Handle::AnimationHandle t_runturnL, t_runturnR;
            Handle::AnimationHandle t_runstrafeL, t_runstrafeR;

            /* ---- walk ---- */
            Handle::AnimationHandle s_walk;
            Handle::AnimationHandle s_walkl, t_walk_2_walkl, t_walkl_2_walk;
            Handle::AnimationHandle s_walkr, t_walk_2_walkr, t_walkr_2_walk;
            Handle::AnimationHandle t_walkr_2_walkl, t_walkl_2_walkr;

            Handle::AnimationHandle s_walkbl, t_walk_2_walkbl, t_walkbl_2_walk;
            Handle::AnimationHandle s_walkbr, t_walkbr_2_walk;
            Handle::AnimationHandle t_walkbr_2_walkbl, t_walkbl_2_walkbr;

            Handle::AnimationHandle t_walkturnL, t_walkturnR;
            Handle::AnimationHandle t_walkstrafeL, t_walkstrafeR;

            /* ---- Mode-transition ---- */
            Handle::AnimationHandle t_run_2_walk, t_walk_2_run, t_run_2_sneak, t_sneak_2_run;

            /* ---- Jump while running ---- */
            Handle::AnimationHandle t_runl_2_jump, t_runr_2_jump, t_jump_2_runl;
        };

        /**
         * @return The animation set matching the current weapon-state
         */
        const AnimationSet& getActiveSet();

        enum class EFootSide
        {
            FS_None,
            FS_Left,
            FS_Right
        };
        /**
         * @return Foot currently placed in front
         */
        EFootSide getFootFront();

        /**
         * World this resides in
         */
        World::WorldInstance& m_World;

        /**
         * Vob (PlayerController) this belongs to
         */
        Handle::EntityHandle m_HostVob;

        /**
         * Animations based on which type of weapon the model is holding
         */
        AnimationSet m_Anims[(int)EWeaponMode::NUM_WEAPON_MODES];
    };
}
