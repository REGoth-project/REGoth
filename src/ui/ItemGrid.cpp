//
// Created by desktop on 16.12.16.
//

#include "ItemGrid.h"
#include "ImageView.h"
#include <content/Texture.h>
#include <engine/BaseEngine.h>
#include <logic/Inventory.h>
#include <logic/PlayerController.h>
#include <logic/visuals/StaticMeshVisual.h>
#include <imgui/imgui.h>

extern bgfx::ProgramHandle imguiGetImageProgram(uint8_t _mip);

UI::ItemGrid::ItemGrid(Engine::BaseEngine& e)
    : View(e),
      m_SlotSize(100)
{
    Textures::TextureAllocator& texAlloc = m_Engine.getEngineTextureAlloc();
    m_TexSlot = texAlloc.loadTextureVDF("INV_SLOT.TGA");
    m_TexSlotHighlighted = texAlloc.loadTextureVDF("INV_SLOT_HIGHLIGHTED.TGA");
    m_TexInvBack.invalidate();
    //m_Engine.getBasicGameType();
    //if(m_Engine.getBasicGameType() == Daedalus::GameType::GT_Gothic1)
        //m_TexInvBack = texAlloc.loadTextureVDF("DLG_AMBIENT.TGA");
    //else
    //    m_TexInvBack = texAlloc.loadTextureVDF("INV_BACK.TGA");
}

UI::ItemGrid::~ItemGrid()
{
}

void UI::ItemGrid::probeGridSize(int &rows, int &columns, uint32_t width, uint32_t height)
{
    // The grid has always a size of at least 1x1
    columns = std::max(1, Math::ifloor(m_Size.x / m_SlotSize));
    rows = std::max(1, Math::ifloor(m_Size.y / m_SlotSize));
    m_Columns = columns;
    m_Rows = rows;
}

void UI::ItemGrid::getGridSize(int &rows, int &columns)
{
    rows = m_Rows;
    columns = m_Columns;
}

void UI::ItemGrid::calculateAlignedOrigin(int &xOrigin, int &yOrigin, int numRows, int numColumns)
{
    Math::float2 origin = m_Translation - getAlignOffset(m_Alignment, m_Size.x, m_Size.y)
            + getAlignOffset(m_Alignment, m_SlotSize * numColumns, m_SlotSize * numRows);

    // TODO consider using floats instead of ints
    xOrigin = Math::ifloor(origin.x);
    yOrigin = Math::ifloor(origin.y);
}

void UI::ItemGrid::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    if(m_IsHidden)
        return;

    View::update(dt, mstate, config);
}
void UI::ItemGrid::drawInventory(double dt, Render::RenderConfig& config, const std::vector<Daedalus::GEngineClasses::C_Item> &itemList,
                                 const std::vector<int> &indices, int selected)
{
    if(!m_TexInvBack.isValid())
    {
        Textures::TextureAllocator& texAlloc = m_Engine.getEngineTextureAlloc();
        if(m_Engine.getBasicGameType() == Daedalus::GameType::GT_Gothic2)
            m_TexInvBack = texAlloc.loadTextureVDF("INV_BACK.TGA");
        //else if(m_Engine.getBasicGameType() == Daedalus::GameType::GT_Gothic2)
        //    m_TexInvBack = texAlloc.loadTextureVDF("DLG_AMBIENT.TGA");
    }

    World::WorldInstance &world = m_Engine.getMainWorld().get();

    // Probably not necessary to call this each frame
    bgfx::setViewClear(RenderViewList::INVENTORY, BGFX_CLEAR_DEPTH, 0x00000000);

    const float farPlane = 10000.0f;
    Math::Matrix projection;
    bx::mtxOrtho(projection.mv, 0.0f, (float)config.state.viewWidth, (float)config.state.viewHeight, 0.0f, farPlane, 0.0f, 0.0f, false);
    // View matrix is the identity
    bgfx::setViewTransform(RenderViewList::INVENTORY, nullptr, projection.m);
    bgfx::setViewRect(RenderViewList::INVENTORY, 0, 0, uint16_t(config.state.viewWidth), uint16_t(config.state.viewHeight));

    int numColumns, numRows;
    probeGridSize(numRows, numColumns, config.state.viewWidth, config.state.viewHeight);
    int numSlots = numColumns * numRows;
    int xOrigin, yOrigin;
    calculateAlignedOrigin(xOrigin, yOrigin, numRows, numColumns);

    // slot background texture
    Textures::Texture& texSlot = m_Engine.getEngineTextureAlloc().getTexture(m_TexSlot);
    Textures::Texture& texSlotHighlighted = m_Engine.getEngineTextureAlloc().getTexture(m_TexSlotHighlighted);

    if(m_TexInvBack.isValid())
    {
        Textures::Texture &texInvBack = m_Engine.getEngineTextureAlloc().getTexture(m_TexInvBack);
        drawTexture(RenderViewList::INVENTORY_BG, xOrigin, yOrigin, m_SlotSize * numColumns, m_SlotSize * numRows,
                    config.state.viewWidth, config.state.viewHeight, texInvBack.m_TextureHandle,
                    config.programs.imageProgram, config.uniforms.diffuseTexture);
    }

    for(int slot = 0; slot < numSlots; ++slot)
    {
        int y = slot / numColumns;
        int x = slot % numColumns;
        if(y > numRows)
            break;

        // position of the slot (top left corner)
        int xPos = xOrigin + x * m_SlotSize;
        int yPos = yOrigin + y * m_SlotSize;

        drawTexture(RenderViewList::INVENTORY_BG, xPos, yPos, m_SlotSize, m_SlotSize,
                    config.state.viewWidth, config.state.viewHeight, texSlot.m_TextureHandle,
                    config.programs.imageProgram, config.uniforms.diffuseTexture);

        if(slot >= indices.size())
            continue;

        if(slot == selected)
        {
            drawTexture(RenderViewList::UI, xPos, yPos, m_SlotSize, m_SlotSize,
                        config.state.viewWidth, config.state.viewHeight, texSlotHighlighted.m_TextureHandle,
                        config.programs.imageProgram, config.uniforms.diffuseTexture);
        }

        const Daedalus::GEngineClasses::C_Item &itemData = itemList[indices[slot]];
        Handle::MeshHandle itemMeshHandle = world.getStaticMeshAllocator().loadMeshVDF(Utils::toUpper(itemData.visual));
        Meshes::WorldStaticMesh &itemMesh = world.getStaticMeshAllocator().getMesh(itemMeshHandle);

        // Draw the item amount number in the lower right corner
        drawText(std::to_string(itemData.amount), xPos + m_SlotSize - 15, yPos + m_SlotSize - 5,
                 A_BottomRight, config, UI::DEFAULT_FONT_LARGE);

        // TODO do this once when mesh is loaded and not on every frame
        // Before we can do any reasonable rendering we have to first
        // determine a approximate size of the item to render
        // Let's calculate the axis aligned bounding box and take the biggest
        // among all three axis as the item's size
        Math::float3 bbmin = { FLT_MAX, FLT_MAX, FLT_MAX };
        Math::float3 bbmax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
        std::vector<Meshes::UVNormColorVertex> &verts = itemMesh.mesh.m_Vertices;
        for(const auto &vertex : verts)
        {
            const Math::float3 &vpos = vertex.Position;
            bbmin.x = std::min(bbmin.x, vpos.x);
            bbmin.y = std::min(bbmin.y, vpos.y);
            bbmin.z = std::min(bbmin.z, vpos.z);
            bbmax.x = std::max(bbmax.x, vpos.x);
            bbmax.y = std::max(bbmax.y, vpos.y);
            bbmax.z = std::max(bbmax.z, vpos.z);
        }
        float itemSize = std::max(bbmax.x - bbmin.x, std::max(bbmax.y - bbmin.y, bbmax.z - bbmin.z));
        // The origin of some item meshes are too far away from their actual center
        Math::float3 itemCenter = (bbmin + bbmax) / 2.0f;

        // Scale required for the bounding box to exactly fit into the slot
        float scale = m_SlotSize / itemSize;
        // Translation to the slot center
        Math::float3 translation(xPos + (float)m_SlotSize / 2, yPos + (float)m_SlotSize / 2, farPlane / 2);

        // Apply rotations for gothic 2 inventory
        Math::Matrix transform = applyRotationsAnsScale(itemCenter, itemData, slot == selected);
        // Apply bounding box scale
        transform = Math::Matrix::CreateScale(Math::float3(scale, scale, scale)) * transform;
        // Apply slot translation
        transform = Math::Matrix::CreateTranslation(translation) * transform;

        for( int subMesh = 0; subMesh < itemMesh.mesh.m_SubmeshMaterialNames.size(); ++subMesh)
        {
            std::string &materialName = itemMesh.mesh.m_SubmeshMaterialNames[subMesh];
            Handle::TextureHandle textureHandle = world.getTextureAllocator().loadTextureVDF(materialName);
            Textures::Texture &texture = world.getTextureAllocator().getTexture(textureHandle);

            // Following render code is essentially copy & paste from render/WorldRender.cpp
            bgfx::setTransform(transform.m);
            bgfx::setState(BGFX_STATE_DEFAULT);

            // I don't know what this uniform does but just passing white seems to work
            Math::float4 color;
            color.fromRGBA8(0xFFFFFFFF);
            bgfx::setUniform(config.uniforms.objectColor, color.v);

            bgfx::setTexture(0, config.uniforms.diffuseTexture, texture.m_TextureHandle, BGFX_TEXTURE_MIN_ANISOTROPIC | BGFX_TEXTURE_MAG_ANISOTROPIC);

            if (itemMesh.mesh.m_IndexBufferHandle.idx != bgfx::kInvalidHandle)
            {
                bgfx::setVertexBuffer(0, itemMesh.mesh.m_VertexBufferHandle);
                bgfx::setIndexBuffer(itemMesh.mesh.m_IndexBufferHandle,
                                     itemMesh.mesh.m_SubmeshStarts[subMesh].m_StartIndex,
                                     itemMesh.mesh.m_SubmeshStarts[subMesh].m_NumIndices);
            }
            else
            {
                bgfx::setVertexBuffer(0, itemMesh.mesh.m_VertexBufferHandle,
                                      itemMesh.mesh.m_SubmeshStarts[subMesh].m_StartIndex,
                                      itemMesh.mesh.m_SubmeshStarts[subMesh].m_NumIndices);
            }

            // TODO changing fog uniform. should be fixed in shader
            Math::float4 fogNearFar = Math::float4(FLT_MAX, FLT_MAX, 0, 0);
            bgfx::setUniform(config.uniforms.fogNearFar, fogNearFar.v);

            bgfx::submit(RenderViewList::INVENTORY, config.programs.mainWorldProgram);
        }

    }
}

Math::Matrix UI::ItemGrid::applyRotationsAnsScale(Math::float3 centerPos, const Daedalus::GEngineClasses::C_Item &itemData, bool selected)
{
    using Daedalus::GEngineClasses::C_Item;
    using namespace Math;

    // Corrections to the center
    if(itemData.mainflag == C_Item::Categories::ITM_CAT_MAGIC)
    {
        if(C_Item::Flags::ITEM_AMULET & itemData.flags)
            centerPos.y -= 0.04f;
        if(C_Item::Flags::ITEM_RING & itemData.flags)
            centerPos.y -= 0.005f;
        //if(C_Item::Flags::ITEM_BELT & itemData.flags)
        //    centerPos.y -= 0.02f;
    }

    // Move the mesh to it's designated center
    Matrix transform = Matrix::CreateTranslation(-1.0f * centerPos);

    // Scale item based on it's type or, if given, by a bias set in the scripts
    float scale = 1.0f;
    if (itemData.inv_zbias != 0)
        scale = 1.0f / (0.01f * itemData.inv_zbias);
    else
    {
        switch(itemData.mainflag)
        {
        case C_Item::Categories::ITM_CAT_NF:
        case C_Item::Categories::ITM_CAT_FF:
        case C_Item::Categories::ITM_CAT_MUN:
        case C_Item::Categories::ITM_CAT_DOCS:
            scale = 1.0f/1.35f;
            break;

        case C_Item::Categories::ITM_CAT_NONE:
        case C_Item::Categories::ITM_CAT_POTION:
            scale = 1.0f/1.45f;
            break;

        case C_Item::Categories::ITM_CAT_FOOD:
        case C_Item::Categories::ITM_CAT_RUNE:
            scale = 1.0f/1.5f;
            break;

        case C_Item::Categories::ITM_CAT_MAGIC:
            if(C_Item::Flags::ITEM_RING)
                scale = 1.0f/2.0f;
            break;
        }
    }

    if(selected)
        scale *= 1.2f;

    // Apply scale
    transform = Math::Matrix::CreateScale(scale, scale, scale) * transform;

    // Base rotation
    transform = Matrix::CreateRotationX(degreeToRadians(180)) * transform;

    if (itemData.inv_rotx != 0 || itemData.inv_roty != 0 || itemData.inv_rotz != 0)
    {
        transform = Matrix::CreateRotationX(degreeToRadians(itemData.inv_rotx)) * transform;
        transform = Matrix::CreateRotationY(degreeToRadians(-itemData.inv_roty)) * transform;
        transform = Matrix::CreateRotationZ(degreeToRadians(-itemData.inv_rotz)) * transform;
    }
    else
    {
        switch (itemData.mainflag)
        {
        case C_Item::Categories::ITM_CAT_FF:
            transform = Matrix::CreateRotationX(degreeToRadians(90)) * transform;
            transform = Matrix::CreateRotationZ(degreeToRadians(-45)) * transform;
            break;
        case C_Item::Categories::ITM_CAT_NF:
            // if flag & ITM_FLAG_SHIELD: X -90, Z -90 break;
            transform = Matrix::CreateRotationX(degreeToRadians(-90)) * transform;
            // if flag & ITM_FLAG_DAG: Z -45 else:
            transform = Matrix::CreateRotationZ(degreeToRadians(135)) * transform;
            break;

        case C_Item::Categories::ITM_CAT_ARMOR:
            transform = Matrix::CreateRotationX(degreeToRadians(360)) * transform;
            break;

        case C_Item::Categories::ITM_CAT_MUN:
            transform = Matrix::CreateRotationZ(degreeToRadians(135)) * transform;
            break;

        case C_Item::Categories::ITM_CAT_DOCS:
            transform = Matrix::CreateRotationX(degreeToRadians(90)) * transform;
            break;

        case C_Item::Categories::ITM_CAT_POTION:
            transform = Matrix::CreateRotationY(degreeToRadians(-90)) * transform;
            break;

        case C_Item::Categories::ITM_CAT_LIGHT:
            break;

        case C_Item::Categories::ITM_CAT_FOOD:
            transform = Matrix::CreateRotationY(degreeToRadians(90)) * transform;
            transform = Matrix::CreateRotationX(degreeToRadians(-35)) * transform;
            break;

        case C_Item::Categories::ITM_CAT_MAGIC:
            transform = Matrix::CreateRotationY(degreeToRadians(90)) * transform;
            transform = Matrix::CreateRotationX(degreeToRadians(-35)) * transform;
            break;

        case C_Item::Categories::ITM_CAT_RUNE:
            transform = Matrix::CreateRotationX(degreeToRadians(-90)) * transform;
            break;

        case C_Item::Categories::ITM_CAT_NONE:
            transform = Matrix::CreateRotationX(degreeToRadians(-25)) * transform;
            transform = Matrix::CreateRotationZ(degreeToRadians(45)) * transform;
            break;
        }
    }
    return transform;
}
