#pragma once
#include <bgfx/bgfx.h>
#include <functional>
#include <engine/World.h>
#include "RenderSystem.h"

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
}