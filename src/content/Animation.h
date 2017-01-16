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
        // different  values pulled here for quick access and normalization

        // PERF: make this a fixed length string and make sure that no temps are created on compare
        std::string                             m_Name;
        Handle::AnimationDataHandle             m_Data;
        uint32_t                                m_Layer = 0;
        Handle::AnimationHandle                 m_Next;
        float                                   m_BlendIn = 0;
        float                                   m_BlendOut = 0;
        Handle::AnimationHandle                 m_Alias;
        uint32_t                                m_Flags = 0;
        float                                   m_FpsRate = 0.0f;
        unsigned                                m_FrameCount = 0;
        Effect                                  m_Effects[MAX_ANIM_EFFECTS];
    };
}
