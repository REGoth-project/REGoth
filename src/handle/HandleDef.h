#pragma once
#include <cstdint>
#include <memory/StaticReferencedAllocator.h>
#include <bgfx/bgfx.h>
#include "Handle.h"

namespace World
{
	class WorldInstance;
}

/**
 * Various handle-definitions
 */
namespace Handle
{
    typedef Memory::GenericHandle<16 ,16, 1> MaterialHandle;
    typedef Memory::GenericHandle<16, 16, 2> TextureHandle;
    typedef Memory::GenericHandle<16, 16, 3> VBHandle;
    typedef Memory::GenericHandle<16, 16, 4> IBHandle;
    typedef Memory::GenericHandle<24, 8,  5> EntityHandle;
	typedef Memory::GenericHandle<16, 16, 6> MeshHandle;
	typedef Memory::GenericHandle<16, 16, 8> LogicHandle;
	typedef Memory::GenericHandle<16, 16, 9> AnimationHandle;
	typedef Memory::GenericHandle<16, 16, 10> PhysicsObjectHandle;
	typedef Memory::GenericHandle<16, 16, 10> CollisionShapeHandle; // TODO: Should not be the same as PhysicsObjectHandle
	typedef Memory::GenericHandle<16, 16, 12> AudioHandle;
	typedef PtrHandle<World::WorldInstance> WorldHandle;

    // Internal handle-types (API specific)
    typedef bgfx::TextureHandle InternalTextureHandle;
    typedef bgfx::VertexBufferHandle InternalVBHandle;
    typedef bgfx::IndexBufferHandle InternalIBHandle;
}