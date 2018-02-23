#pragma once
#include <inttypes.h>
#include <bgfx/bgfx.h>
#include <math/mathlib.h>
#include <zenload/zTypes.h>

namespace Meshes
{
#pragma pack(push, 4)
    struct PositionVertex
    {
        Math::float3 Position;

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
        Math::float3 Position;
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
        Math::float3 Position;
        Math::float2 TexCoord;

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
        Math::float3 Position;
        Math::float2 TexCoord;
        Math::float3 Normal;

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
        Math::float3 Position;
        Math::float2 TexCoord;
        Math::float3 Normal;
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
        Math::float3 Position;
        Math::float2 TexCoord;

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

    struct PositionUVVertex2D
    {
        Math::float2 Position;
        Math::float2 TexCoord;

        static void init()
        {
            ms_decl
                .begin()
                .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                .end();
        };

        static bgfx::VertexDecl ms_decl;
    };

    struct SkeletalVertex
    {
        Math::float3 Normal;
        Math::float2 TexCoord;
        uint32_t Color;
        std::array<Math::float3, 4> localPositions;
        std::array<float, 4> boneIndices;
        std::array<float, 4> weights;

        static void init()
        {
            ms_decl
                .begin()
                .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
                .add(bgfx::Attrib::TexCoord1, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord2, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord3, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord4, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord5, 4, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord6, 4, bgfx::AttribType::Float)
                .end();
        };

        static bgfx::VertexDecl ms_decl;
    };

#pragma pack(pop)

    template <typename V>
    V vertexCast(const ZenLoad::WorldVertex& in)
    {
        return V();
    }

    template <>
    inline PositionVertex vertexCast<PositionVertex>(const ZenLoad::WorldVertex& in)
    {
        PositionVertex v;
        v.Position = Math::float3(in.Position.x, in.Position.y, in.Position.z);

        return v;
    }

    template <>
    inline PositionUVVertex vertexCast<PositionUVVertex>(const ZenLoad::WorldVertex& in)
    {
        PositionUVVertex v;
        v.Position = Math::float3(in.Position.x, in.Position.y, in.Position.z);
        v.TexCoord = Math::float2(in.TexCoord.x, in.TexCoord.y);

        return v;
    }

    template <>
    inline PositionColorVertex vertexCast<PositionColorVertex>(const ZenLoad::WorldVertex& in)
    {
        PositionColorVertex v;
        v.Position = Math::float3(in.Position.x, in.Position.y, in.Position.z);
        v.Color = in.Color;

        return v;
    }

    template <>
    inline UVVertex vertexCast<UVVertex>(const ZenLoad::WorldVertex& in)
    {
        UVVertex v;
        v.Position = Math::float3(in.Position.x, in.Position.y, in.Position.z);
        v.TexCoord = Math::float2(in.TexCoord.x, in.TexCoord.y);

        return v;
    }

    template <>
    inline UVNormVertex vertexCast<UVNormVertex>(const ZenLoad::WorldVertex& in)
    {
        UVNormVertex v;
        v.Position = Math::float3(in.Position.x, in.Position.y, in.Position.z);
        v.TexCoord = Math::float2(in.TexCoord.x, in.TexCoord.y);
        v.Normal = Math::float3(in.Normal.x, in.Normal.y, in.Normal.z);

        return v;
    }

    template <>
    inline UVNormColorVertex vertexCast<UVNormColorVertex>(const ZenLoad::WorldVertex& in)
    {
        UVNormColorVertex v;
        v.Position = Math::float3(in.Position.x, in.Position.y, in.Position.z);
        v.TexCoord = Math::float2(in.TexCoord.x, in.TexCoord.y);
        v.Normal = Math::float3(in.Normal.x, in.Normal.y, in.Normal.z);
        v.Color = in.Color;

        return v;
    }
}
