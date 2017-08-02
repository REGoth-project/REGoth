#pragma once
#include <functional>
#include "RenderSystem.h"
#include <bgfx/bgfx.h>
#include <engine/World.h>

namespace World
{
    class WorldInstance;
    namespace Waynet
    {
        struct WaynetInstance;
    }
}

namespace Render
{
    /**
     * @brief Draws the main renderpass of the given world
     */
    void drawWorld(World::WorldInstance& world, const RenderConfig& config, RenderSystem& system);

    /**
     * @brief Renders the waynet of the given world
     */
    void debugDrawWaynet(const World::Waynet::WaynetInstance& waynet);

    /**
     * @brief Renders the path passed on the given waynet
     */
    void debugDrawPath(const World::Waynet::WaynetInstance& waynet, const std::vector<size_t>& path);

    /**
     * Updates the rendering related fields of a ParticleEffect-Component and renders it
     * @param World the component resides in
     * @param pfx Pfx-component to draw
     * @param config Current renderingconfig
     */
    void drawPfx(World::WorldInstance& world, Components::PfxComponent& pfx, const RenderConfig& config);
}