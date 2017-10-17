#include "NpcAnimationHandler.h"
#include <ZenLib/utils/logger.h>
#include <components/Entities.h>
#include <engine/BaseEngine.h>
#include <engine/World.h>
#include <logic/PlayerController.h>
#include <logic/visuals/ModelVisual.h>

using namespace Logic;

NpcAnimationHandler::NpcAnimationHandler(World::WorldInstance& world, Handle::EntityHandle hostVob)
        : m_World(world)
        , m_HostVob(hostVob)
{
    m_WalkMode = WalkMode::Run;
}

NpcAnimationHandler::~NpcAnimationHandler()
{
}

PlayerController& NpcAnimationHandler::getController() const
{
    return *reinterpret_cast<Logic::PlayerController*>(m_World.getEntity<Components::LogicComponent>(m_HostVob).m_pLogicController);
}

Components::AnimHandler& NpcAnimationHandler::getAnimHandler() const
{
    return *m_World.getEntity<Components::AnimationComponent>(m_HostVob).m_AnimHandler;
}

Animations::AnimationLibrary& NpcAnimationHandler::getAnimLib() const
{
    return m_World.getAnimationLibrary();
}

bool NpcAnimationHandler::Action_GoForward()
{
    /*// TODO: Check if there is enough space for the player
    Math::float3 traceFrom = getController().getEntityTransform().Translation();
    Math::float3 traceTo = traceFrom + getController().getDirection() * 0.5f;
    Physics::RayTestResult result = m_World.getPhysicsSystem().raytrace(traceFrom, traceTo);

    if(result.hasHit)
    {
        return false;
    }*/

    stopTurningAnimations();
    startAni_Forward();

    return true;
}

void NpcAnimationHandler::Action_GoBackward()
{
    // TODO: Check if there is enough space for the player
    stopTurningAnimations();
    startAni_Backward();
}

void NpcAnimationHandler::Action_StrafeLeft()
{
    // TODO: Check if there is enough space for the player
    startAni_StrafeLeft();
}

void NpcAnimationHandler::Action_StrafeRight()
{
    // TODO: Check if there is enough space for the player
    startAni_StrafeRight();
}

void NpcAnimationHandler::Action_TurnLeft()
{
    // TODO: Check if there is enough space for the player
    StartAni_Turn(-1.0f);
}

void NpcAnimationHandler::Action_TurnRight()
{
    // TODO: Check if there is enough space for the player
    StartAni_Turn(1.0f);
}

void NpcAnimationHandler::Action_Stand(bool force, bool allowTurning)
{
    startAni_Stand(force, allowTurning);
}

void NpcAnimationHandler::Action_DrawWeapon(int part)
{
    startAni_DrawWeapon(part);
}

void NpcAnimationHandler::Action_UndrawWeapon()
{
    startAni_UndrawWeapon();
}

void NpcAnimationHandler::Action_FightForward()
{
    startAni_FightForward();
}
void NpcAnimationHandler::Action_FightLeft()
{
    startAni_FightLeft();
}
void NpcAnimationHandler::Action_FightRight()
{
    startAni_FightRight();
}

bool NpcAnimationHandler::playAnimationTrans(const std::string& anim)
{
    Handle::AnimationHandle transAni;
    Components::AnimHandler& h = getAnimHandler();
    Logic::ModelVisual* model = getController().getModelVisual();

    if (!model)
        return false;

    // Try to find a matching transition-file
    if (h.getActiveAnimationPtr())
    {
        // Strip the "s_" from the animation names
        std::string activeName = h.getActiveAnimationPtr()->m_Name.substr(2);
        std::string queryName = anim.substr(2);

        // Build potential transition animation name
        std::string transAniName = "T_" + activeName + "_2_" + queryName;

        // Try to find it in the global library
        transAni = getAnimLib().getAnimation(h.getMeshLibName(), h.getOverlay(), transAniName);
    }

    if (transAni.isValid())
    {
        model->playAnimation(transAni);
        return true;
    }
    else
    {
        // Try to fallback to target animation
        Handle::AnimationHandle fallbackAni = getAnimLib().getAnimation(h.getMeshLibName(), h.getOverlay(), anim);

        // Cancel if the animation really doesn't exist
        if(!fallbackAni.isValid())
            return false;

        model->playAnimation(fallbackAni);
        return true;
    }
}

bool NpcAnimationHandler::isAnimationActive(Handle::AnimationHandle anim)
{
    return getAnimHandler().getAcitveAnimation() == anim;
}

bool NpcAnimationHandler::isAnimationActive(const std::string& anim)
{
    if (getAnimHandler().getActiveAnimationPtr())
    {
        return Utils::toUpper(getAnimHandler().getActiveAnimationPtr()->m_Name) == Utils::toUpper(anim);
    }

    return false;
}

bool NpcAnimationHandler::isStanding(bool allowTurning)
{
    const auto allWalkModes = {
            WalkMode::Run,
            WalkMode::Walk,
            WalkMode::Sneak,
            WalkMode::Water,
            WalkMode::Swim,
            WalkMode::Dive
    };

    for(WalkMode w : allWalkModes)
    {
        const std::string standAniName = buildStateAnimationNameBasedOnWeapon(getWalkModeTag(w));

        if(isAnimationActive(standAniName))
            return true;
    }

    if(isAnimationActive(Handle::AnimationHandle::makeInvalidHandle()))
        return true;

    if(allowTurning)
    {
        if(isTurningAnimationPlaying())
            return true;
    }

    return false;
}

std::string NpcAnimationHandler::getWeaponAniTag(EWeaponMode weapon)
{
    std::string str;
    switch (weapon)
    {
        case EWeaponMode::WeaponNone:
            str = "";
            break;
        case EWeaponMode::Weapon1h:
            str = "1H";
            break;
        case EWeaponMode::Weapon2h:
            str = "2H";
            break;
        case EWeaponMode::WeaponBow:
            str = "BOW";
            break;
        case EWeaponMode::WeaponCrossBow:
            str = "CBOW";
            break;
        case EWeaponMode::WeaponMagic:
            str = "MAG";
            break;
        case EWeaponMode::WeaponFist:
            str = "FIST";
            break;
        default:
            str = "";
            break;
    }

    return str;
}

void NpcAnimationHandler::initAnimations()
{
    Animations::AnimationLibrary& lib = getAnimLib();
    std::string meshLib = getAnimHandler().getMeshLibName();
    std::string n;

    // Loop through all available weapon-modes
    for (int i = 0; i < (int)EWeaponMode::NUM_WEAPON_MODES; i++)
    {
        // Get string representation of the current mode
        std::string weapon = getWeaponAniTag((EWeaponMode)i);

        /****** Movement animations ******/
        n = weapon + "RUN";

        // Running
        m_Anims[i].s_run = lib.getAnimation(meshLib, meshLib, "S_" + n);
        m_Anims[i].t_run_2_runl = lib.getAnimation(meshLib, meshLib, "T_" + n + "_2_" + n + "L");
        m_Anims[i].t_runl_2_run = lib.getAnimation(meshLib, meshLib, "T_" + n + "L_2_" + n);
        m_Anims[i].s_runl = lib.getAnimation(meshLib, meshLib, "S_" + n + "L");
        m_Anims[i].s_runr = lib.getAnimation(meshLib, meshLib, "S_" + n + "R");
        m_Anims[i].t_run_2_runr = lib.getAnimation(meshLib, meshLib, "T_" + n + "_2_" + n + "R");
        m_Anims[i].t_runr_2_run = lib.getAnimation(meshLib, meshLib, "T_" + n + "R_2_" + n);
        m_Anims[i].t_runr_2_runl = lib.getAnimation(meshLib, meshLib, "T_" + n + "R_2_" + n + "L");
        m_Anims[i].t_runl_2_runr = lib.getAnimation(meshLib, meshLib, "T_" + n + "L_2_" + n + "R");

        m_Anims[i].t_run_2_runbl = lib.getAnimation(meshLib, meshLib, "T_" + n + "_2_" + n + "BL");
        m_Anims[i].t_runbl_2_run = lib.getAnimation(meshLib, meshLib, "T_" + n + "_2_" + n + "BL");
        m_Anims[i].s_runbl = lib.getAnimation(meshLib, meshLib, "S_" + n + "BL");
        m_Anims[i].s_runbr = lib.getAnimation(meshLib, meshLib, "S_" + n + "BR");
        m_Anims[i].t_runbr_2_run = lib.getAnimation(meshLib, meshLib, "T_" + n + "BR_2_" + n);
        m_Anims[i].t_runbr_2_runbl = lib.getAnimation(meshLib, meshLib, "T_" + n + "BR_2_" + n + "BL");
        m_Anims[i].t_runbl_2_runbr = lib.getAnimation(meshLib, meshLib, "T_" + n + "BL_2_" + n + "BR");

        m_Anims[i].t_jumpb = lib.getAnimation(meshLib, meshLib, "T_" + weapon + "JUMPB");
        m_Anims[i].t_runturnL = lib.getAnimation(meshLib, meshLib, "T_" + n + "TURNL");
        m_Anims[i].t_runturnR = lib.getAnimation(meshLib, meshLib, "T_" + n + "TURNR");
        m_Anims[i].t_runstrafeL = lib.getAnimation(meshLib, meshLib, "T_" + n + "STRAFEL");
        m_Anims[i].t_runstrafeR = lib.getAnimation(meshLib, meshLib, "T_" + n + "STRAFER");

        // Walking
        n = weapon + "WALK";

        m_Anims[i].s_walk = lib.getAnimation(meshLib, meshLib, "S_" + n);
        m_Anims[i].t_walk_2_walkl = lib.getAnimation(meshLib, meshLib, "T_" + n + "_2_" + n + "L");
        m_Anims[i].t_walkl_2_walk = lib.getAnimation(meshLib, meshLib, "T_" + n + "L_2_" + n + "");
        m_Anims[i].s_walkl = lib.getAnimation(meshLib, meshLib, "S_" + n + "L");
        m_Anims[i].s_walkr = lib.getAnimation(meshLib, meshLib, "S_" + n + "R");
        m_Anims[i].t_walk_2_walkr = lib.getAnimation(meshLib, meshLib, "T_" + n + "_2_" + n + "R");
        m_Anims[i].t_walkr_2_walk = lib.getAnimation(meshLib, meshLib, "T_" + n + "R_2_" + n);
        m_Anims[i].t_walkr_2_walkl = lib.getAnimation(meshLib, meshLib, "T_" + n + "R_2_" + n + "L");
        m_Anims[i].t_walkl_2_walkr = lib.getAnimation(meshLib, meshLib, "T_" + n + "L_2_" + n + "R");

        m_Anims[i].t_walk_2_walkbl = lib.getAnimation(meshLib, meshLib, "T_" + n + "_2_" + n + "BL");
        m_Anims[i].t_walkbl_2_walk = lib.getAnimation(meshLib, meshLib, "T_" + n + "_2_" + n + "BL");
        m_Anims[i].s_walkbl = lib.getAnimation(meshLib, meshLib, "S_" + n + "BL");
        m_Anims[i].s_walkbr = lib.getAnimation(meshLib, meshLib, "S_" + n + "BR");
        m_Anims[i].t_walkbr_2_walk = lib.getAnimation(meshLib, meshLib, "T_" + n + "BR_2_" + n);
        m_Anims[i].t_walkbr_2_walkbl = lib.getAnimation(meshLib, meshLib, "T_" + n + "BR_2_" + n + "BL");
        m_Anims[i].t_walkbl_2_walkbr = lib.getAnimation(meshLib, meshLib, "T_" + n + "BL_2_" + n + "BR");

        m_Anims[i].t_walkturnL = lib.getAnimation(meshLib, meshLib, "T_" + n + "TURNL");
        m_Anims[i].t_walkturnR = lib.getAnimation(meshLib, meshLib, "T_" + n + "TURNR");
        m_Anims[i].t_walkstrafeL = lib.getAnimation(meshLib, meshLib, "T_" + n + "STRAFEL");
        m_Anims[i].t_walkstrafeR = lib.getAnimation(meshLib, meshLib, "T_" + n + "STRAFER");

        // TODO: Sneak

        // Jump while running
        m_Anims[i].t_runl_2_jump = lib.getAnimation(meshLib, meshLib, "T_" + weapon + "RUNL_2_JUMP");
        m_Anims[i].t_runr_2_jump = lib.getAnimation(meshLib, meshLib, "T_" + weapon + "RUNR_2_JUMP");
        m_Anims[i].t_jump_2_runl = lib.getAnimation(meshLib, meshLib, "T_JUMP_2_" + weapon + "RUNL");

        // Transition
        m_Anims[i].t_run_2_walk = lib.getAnimation(meshLib, meshLib, "T_" + weapon + "RUN_2_" + weapon + "WALK");
        m_Anims[i].t_walk_2_run = lib.getAnimation(meshLib, meshLib, "T_" + weapon + "WALK_2_" + weapon + "RUN");
        m_Anims[i].t_run_2_sneak = lib.getAnimation(meshLib, meshLib, "T_" + weapon + "RUN_2_" + weapon + "SNEAK");
        m_Anims[i].t_sneak_2_run = lib.getAnimation(meshLib, meshLib, "T_" + weapon + "SNEAK_2_" + weapon + "RUN");
    }
}

void NpcAnimationHandler::startAni_Forward()
{
    std::string anim = buildStateAnimationNameBasedOnWeapon(getWalkModeTag(m_WalkMode) + "L");

    if(!isStateAnimationPlaying())
        return;

    // Some use "F" as postfix for some reason
    if(!doesAnimationExist(anim))
        anim = buildStateAnimationNameBasedOnWeapon(getWalkModeTag(m_WalkMode) + "F");

    if(!isAnimationActive(anim))
        playAnimationTrans(anim);
}

void NpcAnimationHandler::startAni_Backward()
{
    std::string anim = buildStateAnimationNameBasedOnWeapon(getWalkModeTag(m_WalkMode) + "B");

    if(!isStateAnimationPlaying())
        return;

    if(!doesAnimationExist(anim))
        anim = buildStateAnimationNameBasedOnWeapon(getWalkModeTag(m_WalkMode) + "BL");

    // Fall back to 'bump-back' animation if a proper animation doesn't exist
    if(!doesAnimationExist(anim))
        anim = buildSubStateAnimationNameBasedOnWeapon("JUMPB");

    if(!isAnimationActive(anim))
        playAnimationTrans(anim);
}

void NpcAnimationHandler::startAni_StrafeLeft()
{
    std::string anim = buildSubStateAnimationNameBasedOnWeapon(getWalkModeTag(m_WalkMode) + "STRAFEL");

    if(!isStateAnimationPlaying())
        return;

    if(!isAnimationActive(anim))
        playAnimationTrans(anim);
}

void NpcAnimationHandler::startAni_StrafeRight()
{
    std::string anim = buildSubStateAnimationNameBasedOnWeapon(getWalkModeTag(m_WalkMode) + "STRAFER");

    if(!isStateAnimationPlaying())
        return;

    if(!isAnimationActive(anim))
        playAnimationTrans(anim);
}

void NpcAnimationHandler::startAni_TurnLeft()
{
    std::string anim = buildSubStateAnimationNameBasedOnWeapon(getWalkModeTag(m_WalkMode) + "TURNL");

    if(!isStateAnimationPlaying())
        return;

    if(!isAnimationActive(anim))
        playAnimationTrans(anim);
}

void NpcAnimationHandler::startAni_TurnRight()
{
    std::string anim = buildSubStateAnimationNameBasedOnWeapon(getWalkModeTag(m_WalkMode) + "TURNR");

    if(!isStateAnimationPlaying())
        return;

    if(!isAnimationActive(anim))
        playAnimationTrans(anim);
}

void NpcAnimationHandler::startAni_Stand(bool force, bool allowTurning)
{
    // Always overwrite an invalid animation ( = none plays )
    if(!isAnimationActive(Handle::AnimationHandle::makeInvalidHandle()))
    {
        if (!force)
        {
            if (allowTurning && isTurningAnimationPlaying())
                return;

            if (!isStateAnimationPlaying())
                return; // Can't do anything if we're currently inside a transition
        }

        if (isStanding(false)) // Set allowTurning false here, since we already filtered that out before
            return;
    }

    // Contains "S_RUN", "S_WALK", "S_DIVE", etc
    std::string standAniName = buildStateAnimationNameBasedOnWeapon(getWalkModeTag(m_WalkMode));

    if(!doesAnimationExist(standAniName))
        standAniName = getDefaultStandAniName();

    // Check if the general transition exists. Otherwise, some animations use "STAND" as target
    std::string transition = buildTransitionAnimationFromCurrentToGiven(getWalkModeTag(m_WalkMode));

    if(!doesAnimationExist(transition))
    {
        if(!playAnimationTrans("S_STAND")) // This animation does not exist, but the transition may
        {
            playAnimation(standAniName); // Snap straight to the target animation
        }
    }
    else
    {
        playAnimationTrans(standAniName);
    }

/*
    if (isAnimationActive(Handle::AnimationHandle::makeInvalidHandle()))
    {
        // Default to idle-animation here
        playAnimation(standAniName);
    }
    else if (isAnimationActive(getActiveSet().s_walk))  // Standing in walk-mode
    {
    }
    else if (isAnimationActive(getActiveSet().s_walkl))  // Forward in walk-mode
    {
        playAnimation(getActiveSet().t_walkl_2_walk);
    }
    else if (isAnimationActive(getActiveSet().s_walkbl))  // Backward in walk-mode
    {
        playAnimation(getActiveSet().t_walkbl_2_walk);
    }
    else if (isAnimationActive(getActiveSet().s_run))  // Standing in run-mode
    {
    }
    else if (isAnimationActive(getActiveSet().s_runl))  // Forward in run-mode
    {
        playAnimation(getActiveSet().t_runl_2_run);
    }
    else if (!allowTurning &&
             (isAnimationActive(getActiveSet().t_runturnL) || isAnimationActive(getActiveSet().t_runturnR) || isAnimationActive(getActiveSet().t_runstrafeL) || isAnimationActive(getActiveSet().t_runstrafeR)))
    {
        playAnimation(getActiveSet().s_run);
    }
    else if (!allowTurning &&
             (isAnimationActive(getActiveSet().t_walkturnL) || isAnimationActive(getActiveSet().t_walkturnR) || isAnimationActive(getActiveSet().t_walkstrafeL) || isAnimationActive(getActiveSet().t_walkstrafeR)))
    {
        playAnimation(getActiveSet().s_walk);
    }
    else if (force)
    {
        switch(m_WalkMode)
        {
            case WalkMode::Run:   playAnimation(getActiveSet().s_run);  break;
            case WalkMode::Walk:  playAnimation(getActiveSet().s_walk); break;
            case WalkMode::Sneak: playAnimation(getActiveSet().s_walk); break; // FIXME: Correct walk->sneak
            case WalkMode::Water: playAnimation(getActiveSet().s_walk); break; // FIXME: Correct walk->water
            case WalkMode::Swim:  playAnimation(getActiveSet().s_walk); break; // FIXME: Correct walk->swim
            case WalkMode::Dive:  playAnimation(getActiveSet().s_walk); break; // FIXME: Correct walk-dive
            default:              playAnimation(getActiveSet().s_run);  break;
        }

    }*/
}

void NpcAnimationHandler::StartAni_Turn(float direction)
{
    // Normalize
    direction = direction < 0 ? -1 : 1;

    const bool allowTurning = false;
    if(isStanding(allowTurning))
    {
        // Start regarding animation, only if we're standing and it hasn't been started
        if (direction < 0)
        {
            startAni_TurnLeft();
        } else
        {
            startAni_TurnRight();
        }
    }

    // Get angles
    float turnSpeed = 1.5f;  // TODO: Calculate this somehow
    float turnRad = direction * turnSpeed * (float)m_World.getEngine()->getGameClock().getLastDt();

    // Apply
    getController().applyRotationY(turnRad);
}

void NpcAnimationHandler::startAni_DrawWeapon(int part)
{
    // FIXME: This are more conditions than i'd like to have...
    // Check on the weapon-mode and play the corresponding animation
    EWeaponMode mode = getController().getWeaponMode();
    std::string first = "T_RUN_2_" + getWeaponAniTag(mode);
    std::string second = "T_" + getWeaponAniTag(mode) + "_2_" + getWeaponAniTag(mode) + "RUN";

    if (!isStanding())
    {
        if (!isAnimationActive(first) && !isAnimationActive(second) && !isAnimationActive("S_" + getWeaponAniTag(mode)))  // This is played when the part is done
        {
            playAnimation(first);
        }

        if (!isAnimationActive(first) && !isAnimationActive(second) && !isStanding())  // This is played when the part is done
        {
            playAnimation(second);
        }
    }
}

void NpcAnimationHandler::startAni_UndrawWeapon()
{
    // Check on the weapon-mode and play the corresponding animation
    //EWeaponMode mode = getController().getWeaponMode();

    // FIXME: Find the right animation to play...
    if (!isAnimationActive(buildStateAnimationNameBasedOnWeapon("RUN")))
    {
        playAnimation(buildStateAnimationNameBasedOnWeapon("RUN"));
    }
}

const NpcAnimationHandler::AnimationSet& NpcAnimationHandler::getActiveSet()
{
    EWeaponMode mode = getController().getWeaponMode();

    // See patchGoblinWeaponMode() why this is needed
    mode = patchGoblinWeaponMode(mode);

    return m_Anims[(int)mode];
}

EWeaponMode NpcAnimationHandler::patchGoblinWeaponMode(EWeaponMode targetMode)
{
    if(targetMode == EWeaponMode::Weapon1h)
    {
        // Goblins are using the "fist"-set, even though they are holding a 1h-weapon.
        if(!isAnimationSetUsable(targetMode) && isAnimationSetUsable(EWeaponMode::WeaponFist))
        {
            return EWeaponMode::WeaponFist;
        }
    }

    return targetMode;
}

void NpcAnimationHandler::playAnimation(Handle::AnimationHandle anim)
{
    getAnimHandler().playAnimation(anim);
}

void NpcAnimationHandler::playAnimation(const std::string& anim)
{
    getAnimHandler().playAnimation(anim);
}

NpcAnimationHandler::EFootSide NpcAnimationHandler::getFootFront()
{
    if (isAnimationActive(getActiveSet().s_walk) || isAnimationActive(getActiveSet().s_runl))
    {
        float p = getAnimHandler().getActiveAnimationProgress();

        if (getController().getWeaponMode() == EWeaponMode::WeaponNone)
        {
            // These values are tied to the original animations
            if (p > 0.85f || p < 0.15f)
                return EFootSide::FS_Left;
            else if (p > 0.35f && p < 0.65f)
                return EFootSide::FS_Right;
        }
        else
        {
            // Never return "none" when fighting, making stopping faster
            if (p > 0.75f || p < 0.25f)
                return EFootSide::FS_Left;
            else
                return EFootSide::FS_Right;
        }
    }
    else if (isAnimationActive(getActiveSet().s_walk))
    {
        float p = getAnimHandler().getActiveAnimationProgress();

        if (p > 0.90f || p < 0.10f)
            return EFootSide::FS_Left;
        else if (p > 0.40f && p < 0.60f)
            return EFootSide::FS_Right;
    }

    return EFootSide::FS_None;
}

void NpcAnimationHandler::stopTurningAnimations()
{
    if (isTurningAnimationPlaying())
    {
        startAni_Stand();
    }
}

void NpcAnimationHandler::startAni_FightForward()
{
    std::string anim = buildStateAnimationNameBasedOnWeapon("ATTACK");
    if (!isAnimationActive(anim))
    {
        playAnimation(anim);
    }
}

void NpcAnimationHandler::startAni_FightLeft()
{
    std::string anim = "T_"+ getWeaponAniTag(getController().getWeaponMode()) + "ATTACKL";
    if(!isAnimationActive(anim))
    {
        playAnimation(anim);
    }
}
void NpcAnimationHandler::startAni_FightRight()
{
    std::string anim = "T_"+ getWeaponAniTag(getController().getWeaponMode()) + "ATTACKR";
    if(!isAnimationActive(anim))
    {
        playAnimation(anim);
    }
}

bool NpcAnimationHandler::isAnimationSetUsable(EWeaponMode weaponMode)
{
    AnimationSet& set = m_Anims[(int)weaponMode];

    bool valid = false;

    valid = valid || set.s_run.isValid();
    valid = valid || set.s_runl.isValid();
    valid = valid || set.s_walk.isValid();
    valid = valid || set.s_walkl.isValid();

    return valid;
}

void NpcAnimationHandler::setWalkMode(NpcAnimationHandler::WalkMode walkMode)
{
    m_WalkMode = walkMode;
}

std::string NpcAnimationHandler::getWalkModeTag(NpcAnimationHandler::WalkMode walkMode)
{
    switch(walkMode)
    {
        case WalkMode::Run:   return "RUN";
        case WalkMode::Walk:  return "WALK";
        case WalkMode::Sneak: return "SNEAK";
        case WalkMode::Water: return "WATER";
        case WalkMode::Swim:  return "SWIM";
        case WalkMode::Dive:  return "DIVE";
        default:              return "INVALID_WALKMODE";
    }

}

std::string NpcAnimationHandler::buildStateAnimationNameBasedOnWeapon(const std::string& state)
{
    const std::string weaponAniTag = getWeaponAniTag(getController().getWeaponMode());

    return "S_" + weaponAniTag + state;
}

std::string NpcAnimationHandler::buildTransitionAnimationNameBasedOnWeapon(const std::string& stateFrom,
                                                                           const std::string& stateTo)
{
    const std::string weaponAniTag = getWeaponAniTag(getController().getWeaponMode());

    std::string transition = "T_" + weaponAniTag + stateFrom + "_2_" + weaponAniTag + stateTo;

    if(!doesAnimationExist(transition))
        return "S_" + stateTo;

    return transition;
}

bool NpcAnimationHandler::doesAnimationExist(const std::string& animName)
{
    Components::AnimHandler& h = getAnimHandler();
    Handle::AnimationHandle hanim = getAnimLib().getAnimation(h.getMeshLibName(), h.getOverlay(), animName);

    return hanim.isValid();
}

std::string NpcAnimationHandler::buildTransitionAnimationFromCurrentToGiven(const std::string& stateTo)
{
    Handle::AnimationHandle hcurrent = getAnimHandler().getAcitveAnimation();

    if(!hcurrent.isValid())
        return "S_" + stateTo;

    std::string current = getAnimHandler().getAnimation(hcurrent).m_Name;

    if(current.substr(0, 2) != "S_")
        return "S_" + stateTo;

    // Strip "S_"-prefix
    current = current.substr(2);

    return buildTransitionAnimationNameBasedOnWeapon(current, stateTo);
}

bool NpcAnimationHandler::isStateAnimationPlaying()
{
    Handle::AnimationHandle hcurrent = getAnimHandler().getAcitveAnimation();

    if(!hcurrent.isValid())
        return true;

    std::string current = getAnimHandler().getAnimation(hcurrent).m_Name;

    if(current.substr(0, 2) == "S_")
        return true;

    // Animations like "T_RUNTURNL" also count as states
    if(isSubStateAnimationPlaying())
        return true;

    return false;
}

std::string NpcAnimationHandler::buildSubStateAnimationNameBasedOnWeapon(const std::string& state)
{
    const std::string weaponAniTag = getWeaponAniTag(getController().getWeaponMode());

    return "T_" + weaponAniTag + state;
}

bool NpcAnimationHandler::isTurningAnimationPlaying()
{
    Handle::AnimationHandle hcurrent = getAnimHandler().getAcitveAnimation();

    if(!hcurrent.isValid())
        return false;

    std::string current = getAnimHandler().getAnimation(hcurrent).m_Name;

    if(current.substr(0, 2) != "T_" )
        return false;

    if(current.find("TURN") == std::string::npos)
        return false;

    // Cannot be a transition animation
    if(current.find("_2_") != std::string::npos)
        return false;

    return true;
}

std::string NpcAnimationHandler::getDefaultStandAniName()
{
    std::string weaponBased = buildStateAnimationNameBasedOnWeapon("RUN");

    if(doesAnimationExist(weaponBased))
        return weaponBased;

    return "S_RUN";
}

bool NpcAnimationHandler::isSubStateAnimationPlaying()
{
    if(isAnimationActive(buildSubStateAnimationNameBasedOnWeapon(getWalkModeTag(m_WalkMode) + "TURNL")))
        return true;

    if(isAnimationActive(buildSubStateAnimationNameBasedOnWeapon(getWalkModeTag(m_WalkMode) + "TURNR")))
        return true;

    if(isAnimationActive(buildSubStateAnimationNameBasedOnWeapon(getWalkModeTag(m_WalkMode) + "STRAFEL")))
        return true;

    if(isAnimationActive(buildSubStateAnimationNameBasedOnWeapon(getWalkModeTag(m_WalkMode) + "STRAFER")))
        return true;

    if(isAnimationActive("T_JUMPB"))
        return true;

    return false;
}

