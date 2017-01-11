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
	void drawWorld(World::WorldInstance& world, const RenderConfig& config, RenderSystem& system)
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
        Components::LightComponent* lights = std::get<Components::LightComponent*>(ctuple);

		// Static mesh instancing
		struct InstanceData
		{
			Math::Matrix world;
			Math::float4 color;
		};

		struct InstanceKind
		{
			std::vector<size_t> entityKinds;
			std::vector<InstanceData> instances;
			uint32_t bufferIndex;
		};

		static std::vector<InstanceKind> instanceKinds;
		size_t instanceKindIdx = 0;

		// Prepare memory already allocated
		for(InstanceKind& k : instanceKinds)
		{
			k.bufferIndex = 0xFFFFFFFF;
			k.instances.clear();
			k.entityKinds.clear();
		}

		size_t numDrawcalls = 0;
		size_t numIndices = 0;
        size_t numSubmeshesDrawn = 0;
		for (size_t i = 0; i<num; i++)
		{
			// Simple distance-check // TODO: Frustum/Occlusion-Culling
			auto& pos = psc[i].m_WorldMatrix;
			float distance2 = (pos.Translation() - cameraPosition).lengthSquared();

            //if(pos.Translation().lengthSquared() < 0.01f && psc[i].m_DrawDistanceFactor > 0)
             //   continue; // FIXME: HACK, against many many drawcalls in the center of the world

			Components::ComponentMask mask = ents[i].m_ComponentMask;

			// FIXME: Temporary
			/*if((mask & Components::PhysicsComponent::MASK) != 0)
			{
				 physics[i].m_RigidBody.setDebugDrawEnabled(psc[i].m_DrawDistanceFactor > 0.0f &&  distance2 < 10.0f * 10.0f);
			}*/

            if(psc[i].m_DrawDistanceFactor >= 0)
            {
                if (distance2 > drawDistance2 * psc[i].m_DrawDistanceFactor)
                    continue;
            }

			if ((mask & Components::StaticMeshComponent::MASK) != 0)
			{
				if (!sms[i].m_StaticMeshVisual.isValid())
					continue;

				if((mask & Components::AnimationComponent::MASK) != 0)
				{
					if ((mask & Components::PositionComponent::MASK) != 0)
					{
						// Set model matrix for rendering.
						bgfx::setTransform(pos.m);
					}

					bgfx::setState(BGFX_STATE_DEFAULT);

					numIndices += sms[i].m_SubmeshInfo.m_NumIndices;
					numDrawcalls++;
                    numSubmeshesDrawn++;

					if(sms[i].m_Texture.isValid())
					{
						Textures::Texture& texture = world.getTextureAllocator().getTexture(sms[i].m_Texture);
						bgfx::setTexture(0, config.uniforms.diffuseTexture, texture.m_TextureHandle, BGFX_TEXTURE_MIN_ANISOTROPIC | BGFX_TEXTURE_MAG_ANISOTROPIC);
					}

					// Set object-color
					Math::float4 color;
					color.fromRGBA8(sms[i].m_Color);
					bgfx::setUniform(config.uniforms.objectColor, color.v);

					Components::AnimHandler* animHandler = nullptr;
					if(animations[i].m_ParentAnimHandler.isValid())
					{
						Components::AnimationComponent& pac = world.getEntity<Components::AnimationComponent>(animations[i].m_ParentAnimHandler);
						animHandler = &pac.getAnimHandler();
					} else
					{
						animHandler = &animations[i].getAnimHandler();
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

					bgfx::submit(0, config.programs.mainSkinnedMeshProgram);
				} else
				{
					auto& mesh = meshes.getMesh(sms[i].m_StaticMeshVisual);

					//if(sms[i].m_InstanceDataIndex != (uint32_t)-2)
                    if(false)
					{
						// Set instance buffer if we haven't had this before
						if (mesh.instanceDataBufferIndex == 0xffffffff)
						{
							if (instanceKindIdx == instanceKinds.size())
							{
								mesh.instanceDataBufferIndex = (uint32_t) instanceKinds.size();
								instanceKinds.emplace_back();
							} else
							{
								mesh.instanceDataBufferIndex = (uint32_t) instanceKindIdx;
							}

							instanceKindIdx++;
							instanceKinds[mesh.instanceDataBufferIndex].bufferIndex = system.requestInstanceDataBuffer();
						}

						InstanceKind& kind = instanceKinds[mesh.instanceDataBufferIndex];
						kind.entityKinds.resize(sms[i].m_SubmeshIdx + 1);
						kind.entityKinds[sms[i].m_SubmeshIdx] = i;

						if (sms[i].m_SubmeshIdx == 0)
						{
							kind.instances.emplace_back();
							kind.instances.back().world = pos;
							kind.instances.back().color.fromRGBA8(sms[i].m_Color);
						}

						//uint32_t idx = mesh.instanceDataBufferIndex;

						/*bgfx::setVertexBuffer(mesh.mesh.m_VertexBufferHandle);
                        bgfx::setIndexBuffer(mesh.mesh.m_IndexBufferHandle,
                                             sms[i].m_SubmeshInfo.m_StartIndex,
                                             sms[i].m_SubmeshInfo.m_NumIndices);*/
					}else
					{
						if ((mask & Components::PositionComponent::MASK) != 0)
						{
							// Set model matrix for rendering.
							bgfx::setTransform(pos.m);
						}

						bgfx::setState(BGFX_STATE_DEFAULT);

						numIndices += sms[i].m_SubmeshInfo.m_NumIndices;
						numDrawcalls++;
                        numSubmeshesDrawn++;

						if(sms[i].m_Texture.isValid())
						{
							Textures::Texture& texture = world.getTextureAllocator().getTexture(sms[i].m_Texture);
							bgfx::setTexture(0, config.uniforms.diffuseTexture, texture.m_TextureHandle, BGFX_TEXTURE_MIN_ANISOTROPIC | BGFX_TEXTURE_MAG_ANISOTROPIC);
						}

						// Set object-color
						Math::float4 color;
						color.fromRGBA8(sms[i].m_Color);
						bgfx::setUniform(config.uniforms.objectColor, color.v);

						auto& mesh = meshes.getMesh(sms[i].m_StaticMeshVisual);

	                    if(mesh.mesh.m_IndexBufferHandle.idx != bgfx::invalidHandle)
                        {
                            bgfx::setVertexBuffer(mesh.mesh.m_VertexBufferHandle);
                            bgfx::setIndexBuffer(mesh.mesh.m_IndexBufferHandle,
                                                 sms[i].m_SubmeshInfo.m_StartIndex,
                                                 sms[i].m_SubmeshInfo.m_NumIndices);
                        }else
                        {
                            bgfx::setVertexBuffer(mesh.mesh.m_VertexBufferHandle,
                                                 sms[i].m_SubmeshInfo.m_StartIndex,
                                                 sms[i].m_SubmeshInfo.m_NumIndices);
                        }
						bgfx::submit(0, config.programs.mainWorldProgram);
					}
				}

				//bgfx::submit(0, config.programs.mainWorldProgram);
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

            if((mask & Components::LightComponent::MASK) != 0)
            {
                drawPointLight(world, psc[i], lights[i], config);
            }



		}

		// Now draw instances
#if 0
		for(size_t i=0;i<instanceKindIdx;i++)
		{
			const InstanceKind& k = instanceKinds[i];

			if(k.entityKinds.empty() || k.instances.empty())
				continue;

			bgfx::DynamicVertexBufferHandle buffer = system.getFrameInstanceDataBuffer(k.bufferIndex);

			// Mark as free now. This buffer won't be used until the next frame
			// FIXME: This breaks in multiple renderpasses! Need to use a different rendersystem? Move instancebuffers to renderconfig?
			system.unregisterInstanceDataBuffer(k.bufferIndex);

			// Update buffer
			bgfx::updateDynamicVertexBuffer(buffer, 0, bgfx::copy(k.instances.data(), sizeof(InstanceData) * k.instances.size()));

			for(size_t e : k.entityKinds)
			{
				auto& mesh = meshes.getMesh(sms[e].m_StaticMeshVisual);
				bgfx::setInstanceDataBuffer(buffer, 0, (uint32_t) k.instances.size());

				bgfx::setVertexBuffer(mesh.mesh.m_VertexBufferHandle);
				bgfx::setIndexBuffer(mesh.mesh.m_IndexBufferHandle,
									 sms[e].m_SubmeshInfo.m_StartIndex,
									 sms[e].m_SubmeshInfo.m_NumIndices);

				if (sms[e].m_Texture.isValid())
				{
					Textures::Texture& texture = world.getTextureAllocator().getTexture(sms[e].m_Texture);
					bgfx::setTexture(0, config.uniforms.diffuseTexture, texture.m_TextureHandle,
									 BGFX_TEXTURE_MIN_ANISOTROPIC | BGFX_TEXTURE_MAG_ANISOTROPIC);
				}

				bgfx::submit(0, config.programs.mainWorldInstancedProgram);

				mesh.instanceDataBufferIndex = 0xFFFFFFFF;
				numDrawcalls++;
                numSubmeshesDrawn += k.instances.size();
				numIndices += sms[e].m_SubmeshInfo.m_NumIndices;
			}

			//bgfx::submit(0, config.programs.mainWorldProgram);
		}
#endif


		/*bgfx::dbgTextPrintf(0, 3, 0x0f, "Num Triangles:    %d", numIndices/3);
        bgfx::dbgTextPrintf(0, 4, 0x0f, "Num Drawcalls:    %d", numDrawcalls);
        bgfx::dbgTextPrintf(0, 5, 0x0f, "Num Meshes drawn: %d", numSubmeshesDrawn);*/


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

void ::Render::collectLightTriangles(World::WorldInstance& world,
                                     const Components::PositionComponent& position,
                                     const Components::LightComponent& light,
									 std::vector<size_t>& litTriangles)
{
	// Make Bounding Box of the given light and get light radius
	float radius = position.m_WorldMatrix.Forward().length() * 5.0f; // FIXME: 5.0 is only for testing

	Utils::BBox3D bb = {Math::float3(-radius,-radius,-radius) + position.m_WorldMatrix.Translation(),
                        Math::float3(radius,radius,radius) + position.m_WorldMatrix.Translation()};
	std::vector<World::NodeIndex> nodes = world.getBspTree().findLeafsOf(bb);

    // Collect all triangles of found nodes
    for(World::NodeIndex i : nodes)
    {
        std::vector<size_t> tris = world.getBspTree().getNode(i).nodeTriangles;
        litTriangles.insert(litTriangles.end(), tris.begin(), tris.end());
    }

    // Throw out duplicates
    std::sort( litTriangles.begin(), litTriangles.end() );
    litTriangles.erase( std::unique( litTriangles.begin(), litTriangles.end() ), litTriangles.end() );
}

void ::Render::drawPointLight(World::WorldInstance& world,
                              const Components::PositionComponent& position,
                              const Components::LightComponent& light,
                              const RenderConfig& config)
{
    // Check which triangles are hit by this light
    std::vector<size_t> litTriangles;
    collectLightTriangles(world, position, light, litTriangles);

    LogInfo() << "Lit triangles: " << litTriangles.size();

    if(litTriangles.empty())
        return; // Nothing to do

    assert((uint32_t)(litTriangles.size() * 3) == litTriangles.size() * 3); // Catch overflows

    // Get light radius
    float radius = position.m_WorldMatrix.Forward().length();
    Math::float3 lightPosition = position.m_WorldMatrix.Translation();

    // Transform light position into viewspace
    lightPosition = config.state.cameraWorld.Invert() * lightPosition;

    const World::WorldMesh& mesh = world.getWorldMesh();

    // Sort by texture
    std::sort(litTriangles.begin(), litTriangles.end(), [&](size_t a, size_t b){
        return mesh.getTriangles()[a].submeshIndex < mesh.getTriangles()[b].submeshIndex;
    });

    // Plug them into a dynamic vertexbuffer
    if (bgfx::checkAvailTransientVertexBuffer((uint32_t)litTriangles.size() * 3, Meshes::WorldStaticMeshVertex::ms_decl) )
    {
        bgfx::TransientVertexBuffer vb;
        bgfx::allocTransientVertexBuffer(&vb, (uint32_t) litTriangles.size() * 3,
                                         Meshes::WorldStaticMeshVertex::ms_decl);
        Meshes::WorldStaticMeshVertex* vertex = (Meshes::WorldStaticMeshVertex*) vb.data;

        // submesh-index, vertex-start, num vertices
        std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> submeshes;
        for (size_t tri : litTriangles)
        {
            const auto& triData = mesh.getTriangles()[tri];

            if(submeshes.empty())
                submeshes.emplace_back(triData.submeshIndex, 0, 0);

            // Add new submesh, if the texture changed
            if (triData.submeshIndex != std::get<0>(submeshes.back()))
                submeshes.emplace_back(triData.submeshIndex,
                                       std::get<1>(submeshes.back()) + std::get<2>(submeshes.back()), 0);

            uint8_t mt;
            // Grab the triangle data
            mesh.getTriangle(tri, vertex, mt);

            vertex += 3;
            std::get<2>(submeshes.back()) += 3;
        }

        // Draw all submeshes
        for (const std::tuple<uint32_t, uint32_t, uint32_t>& t : submeshes)
        {
            LogInfo() << "Submesh #" << std::get<0>(t) << ", Start: " << std::get<1>(t) << ", num: " << std::get<2>(t);

            bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_DEPTH_TEST_EQUAL | BGFX_STATE_BLEND_ADD);

            Math::float4 color;
            color.fromRGBA8(light.m_Color);
            Math::float4 prad = {lightPosition.x,
                                 lightPosition.y,
                                 lightPosition.z,
                                 radius};


            bgfx::setUniform(config.uniforms.lightPositionAndRadius, prad.v);
            bgfx::setUniform(config.uniforms.lightColor, color.v);
            bgfx::setTransform(Math::Matrix::CreateIdentity().mv);

            bgfx::setVertexBuffer(&vb, std::get<1>(t), std::get<2>(t));
            Textures::Texture& tex = world.getTextureAllocator().getTexture(mesh.getSubmeshMatData(std::get<0>(t)));

            bgfx::setTexture(0, config.uniforms.diffuseTexture, tex.m_TextureHandle);
            bgfx::submit(0, config.programs.pointLightProgram);
        }
    }
}
