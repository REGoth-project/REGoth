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
      m_SlotSize(100),
      m_Position(0.6, 0.2),
      m_Size(0.4, 0.7)
{
    // Initially hide this view element
    m_IsHidden = true;

    Textures::TextureAllocator& texAlloc = m_Engine.getEngineTextureAlloc();
    m_TexSlot = texAlloc.loadTextureVDF("INV_SLOT.TGA");
}

UI::ItemGrid::~ItemGrid()
{
}

void UI::ItemGrid::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    if(m_IsHidden)
        return;

    World::WorldInstance &world = m_Engine.getMainWorld().get();
    Logic::PlayerController* playerController = reinterpret_cast<Logic::PlayerController*>(
                world.getEntity<Components::LogicComponent>(world.getScriptEngine().getPlayerEntity()).m_pLogicController);

    const std::list<Daedalus::GameState::ItemHandle> &itemList = playerController->getInventory().getItems();


    // Just some debug things
    static bool isFirstTime = true;
    if(isFirstTime)
    {
        isFirstTime = false;
        m_Engine.getConsole().submitCommand("giveitem ALTESSCHWERT");
        m_Engine.getConsole().submitCommand("giveitem HEILTRANK");
        m_Engine.getConsole().submitCommand("giveitem CRONOS_BRIEF");
        m_Engine.getConsole().submitCommand("giveitem CORDS_SPALTER");
        m_Engine.getConsole().submitCommand("giveitem ITAMARROW");
        m_Engine.getConsole().submitCommand("giveitem HEALTHWATER");
        m_Engine.getConsole().submitCommand("giveitem ASTRONOMIE");
        m_Engine.getConsole().submitCommand("giveitem ITARRUNEFIREBOLT");
        m_Engine.getConsole().submitCommand("giveitem ITFO_POTION_MANA_02");
    }


    // Probably not necessary to call this each frame
    bgfx::setViewClear(RenderViewList::INVENTORY, BGFX_CLEAR_DEPTH, 0x00000000);

    const float farPlane = 1000.0f;
    Math::Matrix projection;

    bx::mtxOrtho(projection.mv, 0.0f, (float)config.state.viewWidth, (float)config.state.viewHeight, 0.0f, 0.0f, farPlane);

    // View matrix is the identity
    bgfx::setViewTransform(RenderViewList::INVENTORY, nullptr, projection.m);

    bgfx::setViewRect(RenderViewList::INVENTORY, 0, 0, uint16_t(config.state.viewWidth), uint16_t(config.state.viewHeight));

    int numColumns = (m_Size.x * config.state.viewWidth) / m_SlotSize;
    int numRows = (m_Size.y * config.state.viewHeight) / m_SlotSize;
    int numSlots = numColumns * numRows;

    // Right align the inventory
    int xOrigin = (m_Position.x + m_Size.x) * config.state.viewWidth - numColumns * m_SlotSize;
    int yOrigin = m_Position.y * config.state.viewHeight;

    Textures::Texture& texSlot = m_Engine.getEngineTextureAlloc().getTexture(m_TexSlot);

    std::list<Daedalus::GameState::ItemHandle>::const_iterator itItem = itemList.begin();
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

        // we draw all slots but we can't draw more items than there are in the inventory
        if(slot >= itemList.size())
            continue;

        Daedalus::GameState::ItemHandle itemHandle = *itItem;
        ++itItem;
        Daedalus::GEngineClasses::C_Item &itemData = world.getScriptEngine().getGameState().getItem(itemHandle);
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

        /* Commentary by Nico Bendlin @nicodex about the positioning of items in the original game
        G2A: width and height is derived from BBox (half), corrected in Magic category for Amulet -4.0,
        Ring -0.5, Belt -2.0; Distance is scaled by C_Item.inv_zbias * 0.01 if not 0.0, ELSE (most items)
        135% for Meele, Ranged, Munition, Docs, 145% for Potions, Misc, and 150% for Food, Runes; Finally
        scaled by (INI) ENGINE.zInventoryItemsDistanceScale if not 0.0.
        And there is a hack for the burning torch (no visual, but VobTree), the view is based on the first
        child Vob and the distance is scaled by 200%.
        Fianlly, distance for selected items is 80%
        */

        float scale = 0.9 * m_SlotSize / itemSize;
        Math::float3 translation(xPos + (float)m_SlotSize / 2, yPos + (float)m_SlotSize / 2, farPlane / 2);

        Math::Matrix transform = Math::Matrix::CreateTranslation(-1.0f * itemCenter);
        /*
        transform = Math::Matrix::CreateRotationX(itemData.inv_rotx) * transform;
        transform = Math::Matrix::CreateRotationY(itemData.inv_roty) * transform;
        transform = Math::Matrix::CreateRotationZ(itemData.inv_rotz) * transform;
        */
        transform = Math::Matrix::CreateRotationZ(Math::PI) * transform;
        transform = Math::Matrix::CreateScale(Math::float3(scale, scale, scale)) * transform;
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

            if (itemMesh.mesh.m_IndexBufferHandle.idx != bgfx::invalidHandle)
            {
                bgfx::setVertexBuffer(itemMesh.mesh.m_VertexBufferHandle);
                bgfx::setIndexBuffer(itemMesh.mesh.m_IndexBufferHandle,
                                     itemMesh.mesh.m_SubmeshStarts[subMesh].m_StartIndex,
                                     itemMesh.mesh.m_SubmeshStarts[subMesh].m_NumIndices);
            }
            else
            {
                bgfx::setVertexBuffer(itemMesh.mesh.m_VertexBufferHandle,
                                      itemMesh.mesh.m_SubmeshStarts[subMesh].m_StartIndex,
                                      itemMesh.mesh.m_SubmeshStarts[subMesh].m_NumIndices);
            }

            // TODO changing fog uniform. should be fixed in shader
            Math::float4 fogNearFar = Math::float4(FLT_MAX, FLT_MAX, 0, 0);
            bgfx::setUniform(config.uniforms.fogNearFar, fogNearFar.v);

            bgfx::submit(RenderViewList::INVENTORY, config.programs.mainWorldProgram);
        }

    }

    View::update(dt, mstate, config);
}
