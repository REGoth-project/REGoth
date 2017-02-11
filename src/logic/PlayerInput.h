#pragma once
#include <math/mathlib.h>

namespace Logic
{
    /**
     * Player-input since the last frame
     */
    struct PlayerInput
    {
        // Continous information
        Math::float2 direction; // Normalized direction on the xz-plane, like a joystick
    
        // Calculated from 'direction' because it's often needed
        bool isForward;
        bool isLeft;
        bool isRight;
        bool isBackward;

        float turnAngle;        // Where to turn to, in radians

        float debugSpeedMod;    // Makes the player move faster

        bool operator==(const PlayerInput& other)
        {
            return memcmp(this, &other, sizeof(PlayerInput)) == 0;
        }

        bool operator!=(const PlayerInput& other)
        {
            return !(*this == other);
        }
    };
}
