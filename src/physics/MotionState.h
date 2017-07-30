#pragma once

#include <shared_mutex>
#include <LinearMath/btDefaultMotionState.h>
#include <math/mathlib.h>

namespace Physics
{
    class MotionState : public btDefaultMotionState
    {
    public:
        MotionState(const Math::Matrix& transform = Math::Matrix::CreateIdentity());

        Math::float3 getPosition();
        Math::float4 getRotationQuat();
        void getOpenGLMatrix(float* m);

        virtual void getWorldTransform(btTransform& centerOfMassWorldTrans) const;
        virtual void setWorldTransform(const btTransform& centerOfMassWorldTrans);
        void setWorldTransform(const Math::Matrix& centerOfMassWorldTrans);

    private:
    };
}