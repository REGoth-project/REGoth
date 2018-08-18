#pragma once
#include <LinearMath/btIDebugDraw.h>
#include <debugdraw/debugdraw.h>
#include <math/mathlib.h>

namespace Physics
{
    class DebugDrawer : public btIDebugDraw
    {
    public:
        DebugDrawer() = default;
        virtual ~DebugDrawer() = default;
        void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override
        {
            ddPush();
            ddSetTransform(nullptr);

            Math::float4 f(color.x(), color.y(), color.z(), 1.0f);
            ddSetColor(f.toABGR8());

            ddMoveTo(from.x(), from.y() + 0.1f, from.z());
            ddLineTo(to.x(), to.y() + 0.1f, to.z());

            ddPop();
        }

        void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override {};
        void reportErrorWarning(const char* warningString) override {}
        void draw3dText(const btVector3& location, const char* textString) override {}
        void setDebugMode(int debugMode) override { m_debugMode = debugMode; }
        int getDebugMode() const override
        {
            return m_debugMode;
        }

    private:
        int m_debugMode;
    };
}