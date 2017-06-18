#pragma once

#include <handle/HandleDef.h>
#include <zenload/zCModelAni.h>
#include <zenload/zCModelScript.h>

namespace Animations
{
    // represents shared keyframe data (the contents of the .MAN files)
    struct AnimationData : public Handle::HandleTypeDescriptor<Handle::AnimationDataHandle>
    {
        ZenLoad::zCModelAniHeader               m_Header;
        std::vector<ZenLoad::zCModelAniSample>  m_Samples;
        std::vector<uint32_t>                   m_NodeIndexList;
    };

    enum EEffectType
    {
        EFFECT_NONE,
        EFFECT_SFX
    };

    struct Effect
    {
        EEffectType                             type;
    };

    enum
    {
        MAX_ANIM_EFFECTS                        = 4
    };

    // run-time rep of an animation, combines base + overlay animations and contains all
    // information in a model script
    struct Animation : public Handle::HandleTypeDescriptor<Handle::AnimationHandle>
    {
        enum EModelScriptAniFlags : uint32_t
        {
            MSB_FLAG_NONE = 0,
            /// Animation moves model in world space
            MSB_FLAG_MOVE_MODEL = 0x00000001,

            /// Animation rotates model in world space
            MSB_FLAG_ROTATE_MODEL = 0x00000002,

            /// Animation is queued after the current any on layer instead of started immediately
            MSB_FLAG_QUEUE_ANI = 0x00000004,

            /// Don't stick to ground
            MSB_FLAG_FLY = 0x00000008,

            /// Idle animation
            MSB_FLAG_IDLE = 0x00000010,
        };

        // different  values pulled here for quick access and normalization

        // PERF: make this a fixed length string and make sure that no temps are created on compare
        std::string                             m_Name;
        Handle::AnimationDataHandle             m_Data;
        uint32_t                                m_Layer = 0;
        Handle::AnimationHandle                 m_Next;
        // required to look up the handle, next can't be resolved until all animations are loaded
        // FIXME: could be removed when building an index of animations
        std::string                             m_NextName;
        float                                   m_BlendIn = 0;
        float                                   m_BlendOut = 0;
        ZenLoad::EModelScriptAniDir                      m_Dir = ZenLoad::EModelScriptAniDir::MSB_FORWARD;
        //Handle::AnimationHandle                 m_Alias;
        // required to look up the handle, alias can't be resolved until all animations are loaded
        // FIXME: could be removed when building an index of animations
        //std::string                             m_AliasName;
        EModelScriptAniFlags                    m_Flags = MSB_FLAG_NONE;
        float                                   m_FpsRate = 0.0f;
        // TODO: obsolete
        unsigned                                m_FrameCount = 0;
        unsigned                                m_FirstFrame = 0;
        unsigned                                m_LastFrame = 0;
        Effect                                  m_Effects[MAX_ANIM_EFFECTS];

        std::vector<ZenLoad::zCModelScriptEventSfx>      m_EventsSFX;
    };
}
