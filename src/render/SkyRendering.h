#pragma once

namespace World
{
    class WorldInstance;
}

namespace Render
{
    struct RenderConfig;
    namespace Sky
    {
        void drawSkyOf(World::WorldInstance& world, const RenderConfig& renderConfig);
    }
}



