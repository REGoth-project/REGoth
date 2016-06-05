#pragma once

#ifndef _MSC_VER
#include <sys/time.h>
#else

#endif
#include <math/mathlib.h>

namespace Utils
{
    struct BBox3D
    {
        Math::float3 min;
        Math::float3 max;
    };

    /**
     * @return The current timestamp since some time in milliseconds
     */
    static long long currentTimestamp() {
#ifndef _MSC_VER
        struct timeval te;
        gettimeofday(&te, nullptr); // get current time
        long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // caculate milliseconds

        return milliseconds;
#else
		return 0;
#endif
    }
}