#pragma once
#include <inttypes.h>
#include <bgfx/bgfx.h>
#include <zenload/zTypes.h>

namespace Meshes
{
    struct float3
    {
        union
        {
            struct{ float x,y,z; };
            float v[3];
        };
    };
    
    struct float2
    {
        union
        {
            struct{ float x,y; };
            float v[2];
        };
    };
    
    struct PositionVertex
    {
        float3 Position;

		static void init()
		{
			ms_decl
				.begin()
				.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
				.end();
		};

		static bgfx::VertexDecl ms_decl;
    };

	struct PositionColorVertex
	{
		float3 Position;
		uint32_t Color;

		static void init()
		{
			ms_decl
				.begin()
				.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
				.end();
		};

		static bgfx::VertexDecl ms_decl;
	};
    
    struct UVVertex
    {
        float3 Position;
        float2 TexCoord;

		static void init()
		{
			ms_decl
				.begin()
				.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
				.end();
		};

		static bgfx::VertexDecl ms_decl;
    };
    
    struct UVNormVertex
    {
        float3 Position;
        float2 TexCoord;
        float3 Normal;

		static void init()
		{
			ms_decl
				.begin()
				.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
				.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
				.end();
		};

		static bgfx::VertexDecl ms_decl;
    };
    
	struct UVNormColorVertex
	{
		float3 Position;
		float2 TexCoord;
		float3 Normal;
		uint32_t Color;

		static void init()
		{
			ms_decl
				.begin()
				.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
				.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
				.end();
		};

		static bgfx::VertexDecl ms_decl;
	};

	struct PositionUVVertex
	{
		float3 Position;
		float2 TexCoord;

		static void init()
		{
			ms_decl
				.begin()
				.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
				.end();
		};

		static bgfx::VertexDecl ms_decl;
	};

	template<typename V>
	V vertexCast(const ZenLoad::WorldVertex& in)
	{
		return V();
	}

	template<>
	inline PositionVertex vertexCast<PositionVertex>(const ZenLoad::WorldVertex& in)
	{
		return { in.Position.x, in.Position.y, in.Position.z };
	}

	template<>
	inline PositionColorVertex vertexCast<PositionColorVertex>(const ZenLoad::WorldVertex& in)
	{
		return { 
			in.Position.x, in.Position.y, in.Position.z,
			in.Color 
		};
	}

	template<>
	inline UVVertex vertexCast<UVVertex>(const ZenLoad::WorldVertex& in)
	{
		return { 
			in.Position.x, in.Position.y, in.Position.z,
			in.TexCoord.x, in.TexCoord.y 
		};
	}

	template<>
	inline UVNormVertex vertexCast<UVNormVertex>(const ZenLoad::WorldVertex& in)
	{
		return { 
			in.Position.x, in.Position.y, in.Position.z,
			in.TexCoord.x, in.TexCoord.y,
			in.Normal.x, in.Normal.y, in.Normal.z
		};
	}

	template<>
	inline UVNormColorVertex vertexCast<UVNormColorVertex>(const ZenLoad::WorldVertex& in)
	{
		return{
			in.Position.x, in.Position.y, in.Position.z,
			in.TexCoord.x, in.TexCoord.y,
			in.Normal.x, in.Normal.y, in.Normal.z,
			in.Color
		};
	}

	template<>
	inline PositionUVVertex vertexCast<PositionUVVertex>(const ZenLoad::WorldVertex& in)
	{
		return{
			in.Position.x, in.Position.y, in.Position.z,
			in.TexCoord.x, in.TexCoord.y,
		};
	}


	
	
}