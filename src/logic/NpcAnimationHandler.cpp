#include <components/Entities.h>
#include "NpcAnimationHandler.h"
#include <engine/World.h>
#include <logic/PlayerController.h>
#include <logic/visuals/ModelVisual.h>
#include <ZenLib/utils/logger.h>


using namespace Logic;

NpcAnimationHandler::NpcAnimationHandler(World::WorldInstance& world, Handle::EntityHandle hostVob)
    : m_World(world), m_HostVob(hostVob)
{

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

void NpcAnimationHandler::Action_GoForward()
{
    // TODO: Check if there is enough space for the player
    stopTurningAnimations();
    startAni_Forward();
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

void NpcAnimationHandler::Action_Stand(bool force, bool allowTurning )
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


bool NpcAnimationHandler::playAnimationTrans(const std::string& anim)
{
    Handle::AnimationHandle transAni;
    Components::AnimHandler& h = getAnimHandler();
    Logic::ModelVisual* model = getController().getModelVisual();

    if(!model)
        return false;

    // Try to find a matching transition-file
    if(h.getActiveAnimationPtr())
    {
        // Strip the "s_" from the animation names
        std::string activeName = h.getActiveAnimationPtr()->m_Name.substr(2);
        std::string queryName = anim.substr(2);

        // Build potential transition animation name
        std::string transAniName = "T_" + activeName + "_2_" + queryName;

        // Try to find it in the global library
        transAni = getAnimLib().getAnimation(h.getMeshLibName(), h.getOverlay(), transAniName);

        if(transAni.isValid())
            LogInfo() << "Found transition ani: " << transAniName;
        else
            LogInfo() << "Failed to find transition ani: " << transAniName;
    }

    if(transAni.isValid())
    {
        model->playAnimation(transAni);
        return true;
    }else
    {
        model->playAnimation(anim);
        return false;
    }
}

bool NpcAnimationHandler::isAnimationActive(Handle::AnimationHandle anim)
{
    return getAnimHandler().getAcitveAnimation() == anim;
}

bool NpcAnimationHandler::isAnimationActive(const std::string& anim)
{
    if(getAnimHandler().getActiveAnimationPtr())
    {
        return Utils::toUpper(getAnimHandler().getActiveAnimationPtr()->m_Name) == Utils::toUpper(anim);
    }

    return false;
}

bool NpcAnimationHandler::isStanding(bool allowTurning)
{
    bool standing = isAnimationActive(getActiveSet().s_run) || isAnimationActive(getActiveSet().s_walk);

    if(allowTurning)
    {
        standing = standing || isAnimationActive(getActiveSet().t_runturnL)
                            || isAnimationActive(getActiveSet().t_runturnR)
                            || isAnimationActive(getActiveSet().t_walkturnL)
                            || isAnimationActive(getActiveSet().t_walkturnR);
    }

    return standing;
}

std::string NpcAnimationHandler::getWeaponAniTag(EWeaponMode weapon)
{
    std::string str;
    switch(weapon)
    {
        case EWeaponMode::WeaponNone:       str = ""; break;
        case EWeaponMode::Weapon1h:         str = "1H"; break;
        case EWeaponMode::Weapon2h:         str = "2H"; break;
        case EWeaponMode::WeaponBow:        str = "BOW"; break;
        case EWeaponMode::WeaponCrossBow:   str = "CBOW"; break;
        case EWeaponMode::WeaponMagic:      str = "MAG"; break;
        case EWeaponMode::WeaponFist:       str = "FIST"; break;
        default: str = ""; break;
    }

    return str;
}

void NpcAnimationHandler::initAnimations()
{
    Animations::AnimationLibrary& lib = getAnimLib();
    std::string meshLib = getAnimHandler().getMeshLibName();
    std::string n;

    // Loop through all available weapon-modes
    for(int i=0;i<(int)EWeaponMode::NUM_WEAPON_MODES;i++)
    {
        // Get string representation of the current mode
        std::string weapon = getWeaponAniTag((EWeaponMode)i);

        /****** Movement animations ******/
        n = weapon + "RUN";

        // Running
        m_Anims[i].s_run            = lib.getAnimation(meshLib, meshLib, "S_" + n);
        m_Anims[i].t_run_2_runl     = lib.getAnimation(meshLib, meshLib, "T_" + n + "_2_" + n + "L");
        m_Anims[i].t_runl_2_run     = lib.getAnimation(meshLib, meshLib, "T_" + n + "L_2_" + n);
        m_Anims[i].s_runl           = lib.getAnimation(meshLib, meshLib, "S_" + n + "L");
        m_Anims[i].s_runr           = lib.getAnimation(meshLib, meshLib, "S_" + n + "R");
        m_Anims[i].t_run_2_runr     = lib.getAnimation(meshLib, meshLib, "T_" + n + "_2_" + n + "R");
        m_Anims[i].t_runr_2_run     = lib.getAnimation(meshLib, meshLib, "T_" + n + "R_2_" + n);
        m_Anims[i].t_runr_2_runl    = lib.getAnimation(meshLib, meshLib, "T_" + n + "R_2_" + n + "L");
        m_Anims[i].t_runl_2_runr    = lib.getAnimation(meshLib, meshLib, "T_" + n + "L_2_" + n + "R");

        m_Anims[i].t_run_2_runbl    = lib.getAnimation(meshLib, meshLib, "T_" + n + "_2_" + n + "BL");
        m_Anims[i].t_runbl_2_run    = lib.getAnimation(meshLib, meshLib, "T_" + n + "_2_" + n + "BL");
        m_Anims[i].s_runbl          = lib.getAnimation(meshLib, meshLib, "S_" + n + "BL");
        m_Anims[i].s_runbr          = lib.getAnimation(meshLib, meshLib, "S_" + n + "BR");
        m_Anims[i].t_runbr_2_run    = lib.getAnimation(meshLib, meshLib, "T_" + n + "BR_2_" + n);
        m_Anims[i].t_runbr_2_runbl  = lib.getAnimation(meshLib, meshLib, "T_" + n + "BR_2_" + n + "BL");
        m_Anims[i].t_runbl_2_runbr  = lib.getAnimation(meshLib, meshLib, "T_" + n + "BL_2_" + n + "BR");

        m_Anims[i].t_jumpb          = lib.getAnimation(meshLib, meshLib, "T_" + weapon + "JUMPB");
        m_Anims[i].t_runturnL       = lib.getAnimation(meshLib, meshLib, "T_" + n + "TURNL");
        m_Anims[i].t_runturnR       = lib.getAnimation(meshLib, meshLib, "T_" + n + "TURNR");
        m_Anims[i].t_runstrafeL     = lib.getAnimation(meshLib, meshLib, "T_" + n + "STRAFEL");
        m_Anims[i].t_runstrafeR     = lib.getAnimation(meshLib, meshLib, "T_" + n + "STRAFER");

        // Walking
        n = weapon + "WALK";

        m_Anims[i].s_walk            = lib.getAnimation(meshLib, meshLib, "S_" + n);
        m_Anims[i].t_walk_2_walkl    = lib.getAnimation(meshLib, meshLib, "T_" + n + "_2_" + n + "L");
        m_Anims[i].t_walkl_2_walk    = lib.getAnimation(meshLib, meshLib, "T_" + n + "_2_" + n + "L");
        m_Anims[i].s_walkl           = lib.getAnimation(meshLib, meshLib, "S_" + n + "L");
        m_Anims[i].s_walkr           = lib.getAnimation(meshLib, meshLib, "S_" + n + "R");
        m_Anims[i].t_walk_2_walkr    = lib.getAnimation(meshLib, meshLib, "T_" + n + "_2_" + n + "R");
        m_Anims[i].t_walkr_2_walk    = lib.getAnimation(meshLib, meshLib, "T_" + n + "R_2_" + n);
        m_Anims[i].t_walkr_2_walkl   = lib.getAnimation(meshLib, meshLib, "T_" + n + "R_2_" + n + "L");
        m_Anims[i].t_walkl_2_walkr   = lib.getAnimation(meshLib, meshLib, "T_" + n + "L_2_" + n + "R");

        m_Anims[i].t_walk_2_walkbl   = lib.getAnimation(meshLib, meshLib, "T_" + n + "_2_" + n + "BL");
        m_Anims[i].t_walkbl_2_walk   = lib.getAnimation(meshLib, meshLib, "T_" + n + "_2_" + n + "BL");
        m_Anims[i].s_walkbl          = lib.getAnimation(meshLib, meshLib, "S_" + n + "BL");
        m_Anims[i].s_walkbr          = lib.getAnimation(meshLib, meshLib, "S_" + n + "BR");
        m_Anims[i].t_walkbr_2_walk   = lib.getAnimation(meshLib, meshLib, "T_" + n + "BR_2_" + n);
        m_Anims[i].t_walkbr_2_walkbl = lib.getAnimation(meshLib, meshLib, "T_" + n + "BR_2_" + n + "BL");
        m_Anims[i].t_walkbl_2_walkbr = lib.getAnimation(meshLib, meshLib, "T_" + n + "BL_2_" + n + "BR");

        m_Anims[i].t_walkturnL       = lib.getAnimation(meshLib, meshLib, "T_" + n + "TURNL");
        m_Anims[i].t_walkturnR       = lib.getAnimation(meshLib, meshLib, "T_" + n + "TURNR");
        m_Anims[i].t_walkstrafeL     = lib.getAnimation(meshLib, meshLib, "T_" + n + "STRAFEL");
        m_Anims[i].t_walkstrafeR     = lib.getAnimation(meshLib, meshLib, "T_" + n + "STRAFER");

        // TODO: Sneak

        // Jump while running
        m_Anims[i].t_runl_2_jump    = lib.getAnimation(meshLib, meshLib, "T_" + weapon + "RUNL_2_JUMP");
        m_Anims[i].t_runr_2_jump    = lib.getAnimation(meshLib, meshLib, "T_" + weapon + "RUNR_2_JUMP");
        m_Anims[i].t_jump_2_runl    = lib.getAnimation(meshLib, meshLib, "T_JUMP_2_" + weapon + "RUNL");

        // Transition
        m_Anims[i].t_run_2_walk     = lib.getAnimation(meshLib, meshLib, "T_" + weapon + "RUN_2_" + weapon + "WALK");
        m_Anims[i].t_walk_2_run     = lib.getAnimation(meshLib, meshLib, "T_" + weapon + "WALK_2_" + weapon + "RUN");
        m_Anims[i].t_run_2_sneak    = lib.getAnimation(meshLib, meshLib, "T_" + weapon + "RUN_2_" + weapon + "SNEAK");
        m_Anims[i].t_sneak_2_run    = lib.getAnimation(meshLib, meshLib, "T_" + weapon + "SNEAK_2_" + weapon + "RUN");
    }
}

void NpcAnimationHandler::startAni_Forward()
{
    // TODO: Check for swimming, sneaking, etc
    if(isAnimationActive(getActiveSet().s_walk)) // Standing, walk-mode
    {
        // Start walking forward
        playAnimation(getActiveSet().t_walk_2_walkl);
    }else if(isAnimationActive(getActiveSet().s_run)) // Standing, run-mode
    {
        // Start walking forward
        playAnimation(getActiveSet().t_run_2_runl);
    }
}

void NpcAnimationHandler::startAni_Backward()
{
    // TODO: Check for swimming, sneaking, etc
    if(isAnimationActive(getActiveSet().s_walk)) // Standing, walk-mode
    {
        // Start walking backward
        playAnimation(getActiveSet().t_walk_2_walkbl);
    }else if(isAnimationActive(getActiveSet().s_run)) // Standing, run-mode
    {
        // Jump back in running mode
        playAnimation(getActiveSet().t_jumpb);
    }
}

void NpcAnimationHandler::startAni_StrafeLeft()
{
    if(isAnimationActive(getActiveSet().s_walk)) // Standing, walk-mode
    {
        // Start walking backward
        playAnimation(getActiveSet().t_walkstrafeL);
    }else if(isAnimationActive(getActiveSet().s_run)) // Standing, run-mode
    {
        // Jump back in running mode
        playAnimation(getActiveSet().t_runstrafeL);
    }
}

void NpcAnimationHandler::startAni_StrafeRight()
{
    if(isAnimationActive(getActiveSet().s_walk)) // Standing, walk-mode
    {
        // Start walking backward
        playAnimation(getActiveSet().t_walkstrafeR);
    }else if(isAnimationActive(getActiveSet().s_run)) // Standing, run-mode
    {
        // Jump back in running mode
        playAnimation(getActiveSet().t_runstrafeR);
    }
}

void NpcAnimationHandler::startAni_TurnLeft()
{
    if(isAnimationActive(getActiveSet().s_walk) || isAnimationActive(getActiveSet().t_walkturnR)) // Standing, walk-mode
    {
        // Start walking backward
        playAnimation(getActiveSet().t_walkturnL);
    }else if(isAnimationActive(getActiveSet().s_run) || isAnimationActive(getActiveSet().t_runturnR)) // Standing, run-mode
    {
        // Jump back in running mode
        playAnimation(getActiveSet().t_runturnL);
    }
}

void NpcAnimationHandler::startAni_TurnRight()
{
    if(isAnimationActive(getActiveSet().s_walk) || isAnimationActive(getActiveSet().t_walkturnL)) // Standing, walk-mode
    {
        // Start walking backward
        playAnimation(getActiveSet().t_walkturnR);
    }else if(isAnimationActive(getActiveSet().s_run) || isAnimationActive(getActiveSet().t_runturnL)) // Standing, run-mode
    {
        // Jump back in running mode
        playAnimation(getActiveSet().t_runturnR);
    }
}

void NpcAnimationHandler::startAni_Stand(bool force, bool allowTurning )
{
    if(isAnimationActive(Handle::AnimationHandle::makeInvalidHandle()))
    {
        // Default to idle-animation here
        playAnimation(getActiveSet().s_run);
    }else if(isAnimationActive(getActiveSet().s_walk)) // Standing in walk-mode
    {

    }else if(isAnimationActive(getActiveSet().s_walkl)) // Forward in walk-mode
    {
        playAnimation(getActiveSet().t_walkl_2_walk);

    }else if(isAnimationActive(getActiveSet().s_walkbl)) // Backward in walk-mode
    {
        playAnimation(getActiveSet().t_walkbl_2_walk);

    }else if(isAnimationActive(getActiveSet().s_run)) // Standing in run-mode
    {

    }else if(isAnimationActive(getActiveSet().s_runl)) // Forward in run-mode
    {
        playAnimation(getActiveSet().t_runl_2_run);
    }else if( !allowTurning &&
            (  isAnimationActive(getActiveSet().t_runturnL)
            || isAnimationActive(getActiveSet().t_runturnR)
            || isAnimationActive(getActiveSet().t_runstrafeL)
            || isAnimationActive(getActiveSet().t_runstrafeR)))
    {
        playAnimation(getActiveSet().s_run);
    }else if( !allowTurning &&
            (  isAnimationActive(getActiveSet().t_walkturnL)
            || isAnimationActive(getActiveSet().t_walkturnR)
            || isAnimationActive(getActiveSet().t_walkstrafeL)
            || isAnimationActive(getActiveSet().t_walkstrafeR)))
    {
        playAnimation(getActiveSet().s_walk);
    }else if(force)
    {
        // FIXME: Respect walkmode
        playAnimation(getActiveSet().s_run);
    }
}

void NpcAnimationHandler::StartAni_Turn(float direction)
{
    // Normalize
    direction = direction < 0 ? -1 : 1;

    // Start regarding animation
    if(direction < 0)
    {
        startAni_TurnLeft();
    } else
    {
        startAni_TurnRight();
    }

    // Get angles
    float turnSpeed = 1.5f; // TODO: Calculate this somehow
    float turnRad = direction * turnSpeed * (float)m_World.getWorldInfo().m_LastFrameDeltaTime;

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

    if(!isStanding())
    {
        if (!isAnimationActive(first) && !isAnimationActive(second)
            && !isAnimationActive("S_" + getWeaponAniTag(mode))) // This is played when the part is done
        {
            playAnimation(first);
        }

        if (!isAnimationActive(first) && !isAnimationActive(second)
            && !isStanding()) // This is played when the part is done
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
    if(!isAnimationActive(getActiveSet().s_run))
    {
        playAnimation(getActiveSet().s_run);
    }
}

const NpcAnimationHandler::AnimationSet& NpcAnimationHandler::getActiveSet()
{
    return m_Anims[(int)getController().getWeaponMode()];
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
    if(isAnimationActive(getActiveSet().s_walk) || isAnimationActive(getActiveSet().s_runl))
    {
        float p = getAnimHandler().getActiveAnimationProgress();

        if(getController().getWeaponMode() == EWeaponMode::WeaponNone)
        {
            // These values are tied to the original animations
            if (p > 0.85f || p < 0.15f)
                return EFootSide::FS_Left;
            else if (p > 0.35f && p < 0.65f)
                return EFootSide::FS_Right;
        }else
        {
            // Never return "none" when fighting, making stopping faster
            if (p > 0.75f || p < 0.25f)
                return EFootSide::FS_Left;
            else
                return EFootSide::FS_Right;
        }

    }else if(isAnimationActive(getActiveSet().s_walk))
    {
        float p = getAnimHandler().getActiveAnimationProgress();

        if (p>0.90f || p<0.10f)
            return EFootSide::FS_Left;
        else if (p>0.40f && p<0.60f)
            return EFootSide::FS_Right;
    }

    return EFootSide::FS_None;
}

void NpcAnimationHandler::stopTurningAnimations()
{
    if(isAnimationActive(getActiveSet().t_runturnL)
       || isAnimationActive(getActiveSet().t_runturnR)
       || isAnimationActive(getActiveSet().t_walkturnL)
       || isAnimationActive(getActiveSet().t_walkturnR))
    {
        startAni_Stand();
    }
}

void NpcAnimationHandler::startAni_FightForward()
{
    std::string anim = "S_" + getWeaponAniTag(getController().getWeaponMode()) + "ATTACK";
    if(!isAnimationActive(anim))
    {
        playAnimation(anim);
    }
}







