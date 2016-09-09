#include "WorldRender.h"
#include <engine/World.h>
#include <bgfx/bgfx.h>
#include "bgfx_utils.h"
#include "common.h"
#include "RenderSystem.h"
#include <engine/Waynet.h>
#include <debugdraw/debugdraw.h>
#include <utils/logger.h>

#include <logic/Controller.h>

namespace Render
{
    /**
     * Sets sky and fog related uniforms
     * @param world World to take the parameters from
     */
    void setupSky(World::WorldInstance& world, const RenderConfig& config)
    {
        const Content::Sky::SkyState& masterState = world.getSky().getMasterState();

        world.getSky().setFarPlane(config.state.farPlane);

        // Set up sky-LUT
        bgfx::setUniform(config.uniforms.skyCLUT, world.getSky().getLUTPtr(), 256);


        // Set fog constants
        Math::float3 fogColor;
        float fogNear, fogFar;
        world.getSky().getFogValues(config.state.cameraWorld.Translation(),
                                    fogNear, fogFar, fogColor);

        // Put into float4 to convert to 32-bit RGBA
        Math::float4 fogColorRGBA = Math::float4(fogColor.x,
                                                 fogColor.y,
                                                 fogColor.z,
                                                 1.0f);

        Math::float4 fogNearFar = Math::float4(fogNear, fogFar, 0,0);

        bgfx::setUniform(config.uniforms.fogColor, fogColorRGBA.v);
        bgfx::setUniform(config.uniforms.fogNearFar, fogNearFar.v);

        bgfx::dbgTextPrintf(0, 10, 0x0f, "Time: %f", world.getSky().getTimeOfDay());
        bgfx::dbgTextPrintf(0, 11, 0x0f, "FogNear: %f", fogNear);
        bgfx::dbgTextPrintf(0, 12, 0x0f, "FogFar : %f", fogFar);

        // Set sky-color
        bgfx::setViewClear(0
                , BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
                , fogColorRGBA.toRGBA8()
                , 1.0f
                , 0
        );

		// Don't complain about setting uniforms twice when not actually drawing anything
		bgfx::touch(0);
    }

	/**
	 * @brief Draws the main renderpass of the given world
	 */
	void drawWorld(World::WorldInstance& world, const RenderConfig& config)
	{
        // Setup sky and fog
        setupSky(world, config);

		// Extract camera position
		const Math::float3 cameraPosition = config.state.cameraWorld.Translation();
		const float drawDistance2 = config.state.drawDistanceSquared;

		// Draw all components
		const auto& ctuple = world.getComponentDataBundle().m_Data;
		size_t num = world.getComponentDataBundle().m_NumElements;

		auto& meshes = world.getStaticMeshAllocator();
		auto& skelmeshes = world.getSkeletalMeshAllocator();
		Components::StaticMeshComponent* sms = std::get<Components::StaticMeshComponent*>(ctuple);
		Components::PositionComponent* psc = std::get<Components::PositionComponent*>(ctuple);
		Components::EntityComponent* ents = std::get<Components::EntityComponent*>(ctuple);
		Components::BBoxComponent* bboxes = std::get<Components::BBoxComponent*>(ctuple);
		Components::LogicComponent* logics = std::get<Components::LogicComponent*>(ctuple);
		Components::AnimationComponent* animations = std::get<Components::AnimationComponent*>(ctuple);
		Components::PhysicsComponent* physics = std::get<Components::PhysicsComponent*>(ctuple);

		size_t numIndices = 0;
		for (size_t i = 0; i<num; i++)
		{
			// Simple distance-check // TODO: Frustum/Occlusion-Culling
			auto& pos = psc[i].m_WorldMatrix;
			float distance2 = (pos.Translation() - cameraPosition).lengthSquared() * psc[i].m_DrawDistanceFactor;

			Components::ComponentMask mask = ents[i].m_ComponentMask;

			// FIXME: Temporary
			/*if((mask & Components::PhysicsComponent::MASK) != 0)
			{
				physics[i].m_RigidBody.setDebugDrawEnabled(psc[i].m_DrawDistanceFactor > 0.0f &&  distance2 < 10.0f * 10.0f);
			}*/

			if(distance2 > drawDistance2)
				continue;

			if ((mask & Components::StaticMeshComponent::MASK) != 0)
			{
				if (!sms[i].m_StaticMeshVisual.isValid())
					continue;

				if ((mask & Components::PositionComponent::MASK) != 0)
				{
					// Set model matrix for rendering.
					bgfx::setTransform(pos.m);
				}

				bgfx::setState(BGFX_STATE_DEFAULT);



				numIndices += sms[i].m_SubmeshInfo.m_NumIndices;

				if(sms[i].m_Texture.isValid())
				{
					Textures::Texture& texture = world.getTextureAllocator().getTexture(sms[i].m_Texture);
					bgfx::setTexture(0, config.uniforms.diffuseTexture, texture.m_TextureHandle, BGFX_TEXTURE_MIN_ANISOTROPIC | BGFX_TEXTURE_MAG_ANISOTROPIC);
				}

				// Set object-color
				Math::float4 color;
				color.fromABGR8(sms[i].m_Color);
				bgfx::setUniform(config.uniforms.objectColor, color.v);

				if((mask & Components::AnimationComponent::MASK) != 0)
				{
					Components::AnimHandler* animHandler = nullptr;
					if(animations[i].m_ParentAnimHandler.isValid())
					{
						Components::AnimationComponent& pac = world.getEntity<Components::AnimationComponent>(animations[i].m_ParentAnimHandler);
						animHandler = &pac.m_AnimHandler;
					} else
					{
						animHandler = &animations[i].m_AnimHandler;
					}

					//animHandler->debugDrawSkeleton(pos);

					// Copy everything to the temporary skeletal instance
                    Math::Matrix nodeMat[ZenLoad::MAX_NUM_SKELETAL_NODES + 1];
					animHandler->updateSkeletalMeshInfo(nodeMat + 1, ZenLoad::MAX_NUM_SKELETAL_NODES);
                    nodeMat[0] = pos;

					// They are vec4 inside the shader, thus numMatrices * 4
                    //bgfx::setUniform(config.uniforms.nodeTransforms, tmpSkelInstance.nodeTransforms,
					//				 4 * ZenLoad::MAX_NUM_SKELETAL_NODES);

                    //float f[] = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
                    //bgfx::setUniform(config.uniforms.nodeTransforms, f, 2);
                    bgfx::setTransform(nodeMat, static_cast<uint16_t>(animHandler->getNumNodes() + 1));

					auto& mesh = skelmeshes.getMesh(sms[i].m_StaticMeshVisual);
					bgfx::setVertexBuffer(mesh.m_VertexBufferHandle);
					bgfx::setIndexBuffer(mesh.m_IndexBufferHandle,
										 sms[i].m_SubmeshInfo.m_StartIndex,
										 sms[i].m_SubmeshInfo.m_NumIndices);

				} else
				{
					auto& mesh = meshes.getMesh(sms[i].m_StaticMeshVisual);
					bgfx::setVertexBuffer(mesh.m_VertexBufferHandle);
					bgfx::setIndexBuffer(mesh.m_IndexBufferHandle,
										 sms[i].m_SubmeshInfo.m_StartIndex,
										 sms[i].m_SubmeshInfo.m_NumIndices);
				}

				// Submit primitive for rendering to view 0.
				if((mask & Components::AnimationComponent::MASK) != 0)
					bgfx::submit(0, config.programs.mainSkinnedMeshProgram);
				else
					bgfx::submit(0, config.programs.mainWorldProgram);
			}

			if ((mask & Components::BBoxComponent::MASK) != 0)
			{
				if(bboxes[i].m_DebugColor != 0)
				{
					Aabb box = {bboxes[i].m_BBox3D.min.x, bboxes[i].m_BBox3D.min.y, bboxes[i].m_BBox3D.min.z,
								bboxes[i].m_BBox3D.max.x, bboxes[i].m_BBox3D.max.y, bboxes[i].m_BBox3D.max.z};

					ddPush();
					Math::Matrix m = Math::Matrix::CreateIdentity();
					m.Translation(pos.Translation());
					ddSetTransform(m.mv);
					ddSetColor(bboxes[i].m_DebugColor);
					ddDraw(box);
					ddPop();
				}
			}

			if((mask & Components::LogicComponent::MASK) != 0)
			{
				if(logics[i].m_pLogicController)
				{
					logics[i].m_pLogicController->onDebugDraw();
				}
			}



		}

		bgfx::dbgTextPrintf(0, 3, 0x0f, "Num Triangles: %d", numIndices/3);


		//world.getPhysicsSystem().debugDraw();

		// TODO: Debugging - remove
		/*if(!world.getWaynet().waypoints.empty())
		{
			debugDrawWaynet(world.getWaynet());


			static std::vector<size_t> path;

			if(path.empty())
				path = World::Waynet::findWay(world.getWaynet(),
											  493,
											  342);

			debugDrawPath(world.getWaynet(), path);
		}*/
	}

	void debugDrawWaynet(const World::Waynet::WaynetInstance& waynet)
	{
		ddPush();

		ddSetTransform(nullptr);
		ddSetColor(0xFF0000FF);
		//ddSetStipple(true, 1.0f);

		const float wpAxisLen = 1.0f;
		for(const World::Waynet::Waypoint& wp : waynet.waypoints)
		{

			ddDrawAxis(wp.position.x, wp.position.y, wp.position.z, wpAxisLen);
			ddDrawAxis(wp.position.x, wp.position.y, wp.position.z, -wpAxisLen);



			// FIXME: These would be drawn twice, but I don't care right now
			for(size_t t : wp.edges)
			{
				const World::Waynet::Waypoint& target = waynet.waypoints[t];

				ddMoveTo(wp.position.x, wp.position.y, wp.position.z);
				ddLineTo(target.position.x, target.position.y, target.position.z);
			}
		}

		ddPop();
	}



}

void Render::debugDrawPath(const World::Waynet::WaynetInstance& waynet, const std::vector<size_t>& path)
{
	if(path.empty())
		return;

	ddPush();

	ddSetTransform(nullptr);
	ddSetColor(0xFF00FFFF);
	//ddSetStipple(true, 1.0f);

	const float wpAxisLen = 1.0f;

	const World::Waynet::Waypoint* start = &waynet.waypoints[path[0]];


	for(size_t t : path)
	{
		const World::Waynet::Waypoint& wp = waynet.waypoints[t];

		ddMoveTo(start->position.x, start->position.y, start->position.z);
		ddLineTo(wp.position.x, wp.position.y, wp.position.z);

		start = &wp;
	}


	ddPop();
}