#pragma once

#include <LinearMath/btDefaultMotionState.h>
#include <shared_mutex>
#include <math/mathlib.h>

namespace Physics
{
    class MotionState : public btDefaultMotionState
    {
    public:
        MotionState();

        Math::float3 getPosition();
        Math::float4 getRotationQuat();
        void getOpenGLMatrix(float *m);

        virtual void getWorldTransform(btTransform &centerOfMassWorldTrans) const;
        virtual void setWorldTransform(const btTransform &centerOfMassWorldTrans);

    private:
    };
}