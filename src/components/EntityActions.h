#pragma once
#include "Entities.h"
#include <content/StaticLevelMesh.h>
#include <logic/Controller.h>

namespace Components
{

    namespace Actions
    {
        /**
         * @brief Initializes a component
         * @param alloc Allocator to lookup the handle
         * @param h Handle of the component to modify
         */
        template<typename T>
        inline T& initComponent(Components::ComponentAllocator& alloc,
                                Handle::EntityHandle h)
        {
			auto& c = alloc.getElement<Components::EntityComponent>(h);
			if((c.m_ComponentMask & T::MASK) == 0)
				T::init(alloc.getElement<T>(h));

            c.m_ComponentMask |= T::MASK;
            return alloc.getElement<T>(h);
        }

        namespace BBox
        {
            /**
             * @param bxs Array of BBoxComponents
             * @param checkIdx Array of indices to the BBoxComponents to check
             * @param numCheckIdx Number of indices inside the checkIdx-array
             * @param outIndices [out] Array of the same size as checkIdx to contain indices to the visible entities
             * @param numVisible [out] Number of valid indices inside outIndices
             * TODO: Add actual camera-information
             */
            inline void frustumVisibilityCheck(const BBoxComponent* bxs, const size_t* checkIdx, const size_t numCheckIdx, size_t* outIndices, size_t& numVisible)
            {
                size_t nv = 0;
                for(size_t i=0;i<numCheckIdx;i++)
                {
                    size_t c = checkIdx[i];

                    //std::cout << "Dist: " << Math::float3::distanceSquared((bxs[c].m_BBox3D.min + bxs[c].m_BBox3D.max) * 0.5f, Math::float3(0.0f,0.0f,0.0f)) << std::endl;

                    /*const float dist = 1000.0f * 1000.0f;
                    if(Math::float3::distanceSquared((bxs[c].m_BBox3D.min + bxs[c].m_BBox3D.max) * 0.5f, Math::float3(0.0f,0.0f,0.0f)) < dist
                       && Math::float3::distanceSquared((bxs[c].m_BBox3D.min + bxs[c].m_BBox3D.max) * 0.5f, Math::float3(1.0f,0.0f,0.0f)) < dist
                       && Math::float3::distanceSquared((bxs[c].m_BBox3D.min + bxs[c].m_BBox3D.max) * 0.5f, Math::float3(2.0f,0.0f,0.0f)) < dist
                       && Math::float3::distanceSquared((bxs[c].m_BBox3D.min + bxs[c].m_BBox3D.max) * 0.5f, Math::float3(3.0f,0.0f,0.0f)) < dist
                       && Math::float3::distanceSquared((bxs[c].m_BBox3D.min + bxs[c].m_BBox3D.max) * 0.5f, Math::float3(4.0f,0.0f,0.0f)) < dist
                       && Math::float3::distanceSquared((bxs[c].m_BBox3D.min + bxs[c].m_BBox3D.max) * 0.5f, Math::float3(5.0f,0.0f,0.0f)) < dist)
                    {
                        outIndices[nv] = c;
                        nv++;
                    }*/
                }

                numVisible = nv;
            }
        }

		namespace StaticMesh
		{
			/**
			 * @brief Initializes a static-mesh component using the given mesh-data and entity handle
			 * @param alloc Allocator to lookup the handle
			 * @param h Handle of the component to modify
			 * @param msh Mesh handle to set
			 */
            inline void initComponent(Components::ComponentAllocator& alloc,
				Handle::EntityHandle h, 
				Handle::MeshHandle msh)
			{
				alloc.getElement<Components::StaticMeshComponent>(h).m_StaticMeshVisual = msh;
			}
		}

        namespace Position
        {
            /**
             * @brief Creates a camera view-matrix from the given entity
             * @param alloc Allocator to lookup the handle
             * @param e Entity handle to get the matrix from
             * @return computed view-matrix from the given entity
             */
            inline Math::Matrix makeViewMatrixFrom(Components::ComponentAllocator& alloc,
                                    Handle::EntityHandle e)
            {
                assert(alloc.getElement<Components::EntityComponent>(e).m_ComponentMask & Components::PositionComponent::MASK);
                return alloc.getElement<Components::PositionComponent>(e).m_WorldMatrix.Invert();
            }

            /**
             * @brief Sets the world-matrix for the given entity from a view-matrix
             * @param alloc Allocator to lookup the handle
             * @param e Entity handle to set the matrix to
             * @param view View-Matrix to convert
             */
            inline void setTransformFromView(Components::ComponentAllocator& alloc,
                                      Handle::EntityHandle e,
                                      const Math::Matrix& view)
            {
                assert(alloc.getElement<Components::EntityComponent>(e).m_ComponentMask & Components::PositionComponent::MASK);
                alloc.getElement<Components::PositionComponent>(e).m_WorldMatrix = view.Invert();
            }
        }

        namespace Logic
        {
            void destroyLogicComponent(LogicComponent& c);
            void destroyVisualComponent(VisualComponent& c);
        }

        namespace Physics
        {
            /**
             * Returns the transformation matrix of the rigid-bodys state
             * @param c Physicsscomponent to use
             * @return Transform of the rigid-bodys state
             */
            inline Math::Matrix getRigidBodyTransform(Components::PhysicsComponent& c)
            {
                Math::Matrix m;
                if(c.m_RigidBody.getMotionState())
                    c.m_RigidBody.getMotionState()->getOpenGLMatrix(m.mv);
                else
                    m = Math::Matrix::CreateIdentity();

                return m;
            }

            /**
             * Sets the transform of the rigid-bodys state
             * @param c Physicsscomponent to use
             * @param Transform of the rigid-bodys state
             */
            inline void setRigidBodyPosition(Components::PhysicsComponent& c, const Math::float3& position)
            {
                Math::Matrix m = Math::Matrix::CreateTranslation(position);

                c.m_RigidBody.setBodyTransform(m);
            }
        }

        /**
         * Cleans created resources from the components
         */
        template<typename T>
        inline void destroyComponent(T& c)
        {
            // Dummy function for components which don't need this kind of functionality
        }

        template<>
        inline void destroyComponent<LogicComponent>(LogicComponent& c)
        {
            Logic::destroyLogicComponent(c);
        }

        template<>
        inline void destroyComponent<VisualComponent>(VisualComponent& c)
        {
            Logic::destroyVisualComponent(c);
        }

        template<typename F>
        inline void forAllComponents(Components::ComponentAllocator& alloc,
                                     Handle::EntityHandle e,
                                     F f)
        {
            std::tuple<ALL_COMPONENTS> t;
            Utils::for_each_in_tuple(t, [&](auto& v){
               f(alloc.getElement<typename std::remove_reference<decltype(v)>::type>(e));
            });
        }
    }
}
