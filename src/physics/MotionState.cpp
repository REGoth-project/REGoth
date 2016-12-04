#include "MotionState.h"

// Workaround for initializer-list
static btTransform getFromFloat(const Math::Matrix& transform)
{
    btTransform t;
    t.setFromOpenGLMatrix(transform.mv);

    return t;
}

Physics::MotionState::MotionState(const Math::Matrix& transform) :
        btDefaultMotionState(getFromFloat(transform))
{
}

Math::float3 Physics::MotionState::getPosition()
{
    btTransform trans;
    getWorldTransform(trans);
    btVector3 origin = trans.getOrigin();
    return Math::float3(origin.x(), origin.y(), origin.z());
}

Math::float4 Physics::MotionState::getRotationQuat()
{
    btTransform trans;
    getWorldTransform(trans);
    btQuaternion rotation = trans.getRotation();
    return Math::float4(rotation.x(), rotation.y(), rotation.z(), rotation.w());
}

void Physics::MotionState::getOpenGLMatrix(float *m)
{
    btTransform trans;
    getWorldTransform(trans);
    trans.getOpenGLMatrix(m);
}

void Physics::MotionState::getWorldTransform(btTransform &centerOfMassWorldTrans) const
{
    //std::shared_lock<std::shared_timed_mutex> shared(m_Mutex, std::defer_lock);
    btDefaultMotionState::getWorldTransform(centerOfMassWorldTrans);
}

void Physics::MotionState::setWorldTransform(const btTransform &centerOfMassWorldTrans)
{
    //std::unique_lock<std::shared_timed_mutex> exclusive(m_Mutex, std::defer_lock);
    btDefaultMotionState::setWorldTransform(centerOfMassWorldTrans);
}

void Physics::MotionState::setWorldTransform(const Math::Matrix &centerOfMassWorldTrans)
{
    btTransform t;
    t.setFromOpenGLMatrix(centerOfMassWorldTrans.mv);

    setWorldTransform(t);
}
