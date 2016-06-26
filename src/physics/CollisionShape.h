#pragma once
#include <btBulletCollisionCommon.h>

namespace Physics
{
    enum ECollisionType
    {
        CT_Any = -1,
        CT_WorldMesh = (1 << 1),
        CT_Object = (1 << 2),
    };


    /**
     * @brief Wrapper around a single collisionshape to be used by multiple rigid bodies
     */
    class CollisionShape
    {
        friend class RigidBody;
    public:

        /**
         * @brief Construct from btCollisionShape
         */
        CollisionShape(btCollisionShape *pCollisionShape) : m_pShape(pCollisionShape)
        {
            m_pShape->setUserPointer(this);
        }

        btCollisionShape *shape()
        {
            return m_pShape;
        }

    private:


        /**
         * @brief Internal bulletphysics shape
         */
        btCollisionShape *m_pShape;
    };
}