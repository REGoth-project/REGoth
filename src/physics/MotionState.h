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

        void getWorldTransform(btTransform& centerOfMassWorldTrans) const override;
        void setWorldTransform(const btTransform& centerOfMassWorldTrans) override;
        void setWorldTransform(const Math::Matrix& centerOfMassWorldTrans);

    private:
    };
}