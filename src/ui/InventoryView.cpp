#include "InventoryView.h"
#include <engine/BaseEngine.h>
#include <logic/Inventory.h>
#include <logic/PlayerController.h>
#include <logic/visuals/StaticMeshVisual.h>
#include <logic/ScriptEngine.h>

const float FARPLANE = 1000.0f;

UI::InventoryView::InventoryView(Engine::BaseEngine& e)
    : View(e)
{
    Textures::TextureAllocator& texAlloc = m_Engine.getEngineTextureAlloc();
    m_TexSlot = texAlloc.loadTextureVDF("INV_SLOT.TGA");
    m_TexSlotHighlighted = texAlloc.loadTextureVDF("INV_SLOT_HIGHLIGHTED.TGA");
    m_TexInvBack.invalidate();

    m_SelectedItemID = 0;

    m_LastTimeSelectedItemSlot = 0;
    //m_SelectedSlot = 0;
    m_DeltaRows = 0;
    m_DeltaColumns = 0;
    m_DisplayedRowsStart = 0;

    m_InputUp = Engine::Input::RegisterAction(Engine::ActionType::Inventory_Up, [this](bool triggered, float)
    {
        if (triggered)
            m_DeltaRows -= 1;
    });
    m_InputUp.getAction().setEnabled(false);
    m_InputDown = Engine::Input::RegisterAction(Engine::ActionType::Inventory_Down, [this](bool triggered, float)
    {
        if (triggered)
            m_DeltaRows += 1;
    });
    m_InputDown.getAction().setEnabled(false);
    m_InputLeft = Engine::Input::RegisterAction(Engine::ActionType::Inventory_Left, [this](bool triggered, float)
    {
        if (triggered)
            m_DeltaColumns -= 1;
    });
    m_InputLeft.getAction().setEnabled(false);
    m_InputRight = Engine::Input::RegisterAction(Engine::ActionType::Inventory_Right, [this](bool triggered, float)
    {
        if (triggered)
            m_DeltaColumns += 1;
    });
    m_InputRight.getAction().setEnabled(false);
    m_InputUse = Engine::Input::RegisterAction(Engine::ActionType::Inventory_Use, [this](bool triggered, float)
    {
        if (triggered)
        {
            World::WorldInstance &world = m_Engine.getMainWorld().get();
            Logic::PlayerController* playerController = reinterpret_cast<Logic::PlayerController*>(
                        world.getEntity<Components::LogicComponent>(world.getScriptEngine().getPlayerEntity()).m_pLogicController);

            Logic::Inventory &playerInv = playerController->getInventory();
            const std::list<Daedalus::GameState::ItemHandle> &itemHandleList = playerInv.getItems();
            /*
            if( 0 < itemHandleList.size() && m_SelectedSlot < itemHandleList.size())
            {
                // Removes one item
                auto it = itemHandleList.begin();
                std::advance(it, m_SelectedSlot);
                playerInv.removeItem(*it);
            }
            */
        }
    });
    m_InputUse.getAction().setEnabled(false);
}

UI::InventoryView::~InventoryView()
{
}

// TODO: optimize
Math::Matrix UI::InventoryView::applyRotationsAndScale(Math::float3 centerPos,
                                    const Daedalus::GEngineClasses::C_Item &itemData,
                                    bool selected)
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

void UI::InventoryView::drawItem(Render::RenderConfig& config,
                                 const Daedalus::GEngineClasses::C_Item &itemData,
                                 Math::float2 position, float size, bool selected)
{
    World::WorldInstance &world = m_Engine.getMainWorld().get();
    Handle::MeshHandle itemMeshHandle = world.getStaticMeshAllocator().loadMeshVDF(Utils::toUpper(itemData.visual));
    Meshes::WorldStaticMesh &itemMesh = world.getStaticMeshAllocator().getMesh(itemMeshHandle);

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
    float scale = size / itemSize;
    // Translation to the slot center
    Math::float3 translation(position.x + size / 2, position.y + size / 2, FARPLANE / 2);

    // Apply rotations for gothic 2 inventory
    Math::Matrix transform = applyRotationsAndScale(itemCenter, itemData, selected);
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

void UI::InventoryView::drawItemGrid(Render::RenderConfig& config, Math::float2 position,
                                     Math::float2 size, UI::EAlign alignment, float slotSize,
                  const std::vector<Daedalus::GEngineClasses::C_Item> &itemList,
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

    // Probably not necessary to call this each frame
    bgfx::setViewClear(RenderViewList::INVENTORY, BGFX_CLEAR_DEPTH, 0x00000000);

    const float farPlane = 10000.0f;
    Math::Matrix projection;
    bx::mtxOrtho(projection.mv, 0.0f, (float)config.state.viewWidth, (float)config.state.viewHeight, 0.0f, farPlane, 0.0f, 0.0f, false);
    // View matrix is the identity
    bgfx::setViewTransform(RenderViewList::INVENTORY, nullptr, projection.m);
    bgfx::setViewRect(RenderViewList::INVENTORY, 0, 0, uint16_t(config.state.viewWidth), uint16_t(config.state.viewHeight));

    int numColumns = std::max(1, Math::ifloor(size.x / slotSize));
    int numRows = std::max(1, Math::ifloor(size.y / slotSize));
    int numSlots = numColumns * numRows;
    // calculate origin which account for the actual grid size and the grid's alignment
    Math::float2 origin = position - getAlignOffset(alignment, size.x, size.y)
            + getAlignOffset(alignment, numColumns * slotSize, numRows * slotSize);
    // slot background texture
    Textures::Texture& texSlot = m_Engine.getEngineTextureAlloc().getTexture(m_TexSlot);
    Textures::Texture& texSlotHighlighted = m_Engine.getEngineTextureAlloc().getTexture(m_TexSlotHighlighted);

    if(m_TexInvBack.isValid())
    {
        Textures::Texture &texInvBack = m_Engine.getEngineTextureAlloc().getTexture(m_TexInvBack);
        drawTexture(RenderViewList::INVENTORY_BG, origin.x, origin.y, slotSize * numColumns, slotSize * numRows,
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
        int xPos = origin.x + x * slotSize;
        int yPos = origin.y + y * slotSize;

        drawTexture(RenderViewList::INVENTORY_BG, xPos, yPos, slotSize, slotSize,
                    config.state.viewWidth, config.state.viewHeight, texSlot.m_TextureHandle,
                    config.programs.imageProgram, config.uniforms.diffuseTexture);

        if(slot >= indices.size())
            continue;

        if(slot == selected)
        {
            drawTexture(RenderViewList::UI, xPos, yPos, slotSize, slotSize,
                        config.state.viewWidth, config.state.viewHeight, texSlotHighlighted.m_TextureHandle,
                        config.programs.imageProgram, config.uniforms.diffuseTexture);
        }

        const Daedalus::GEngineClasses::C_Item &itemData = itemList[indices[slot]];

        // Draw the item amount number in the lower right corner
        drawText(std::to_string(itemData.amount), xPos + slotSize - 15, yPos + slotSize - 5,
                 A_BottomRight, config, UI::DEFAULT_FONT_LARGE);

        drawItem(config, itemData, Math::float2(xPos, yPos), slotSize, slot == selected);
    }

}

void UI::InventoryView::drawItemInfobox(Render::RenderConfig &config, const Daedalus::GEngineClasses::C_Item &item,
                                        Math::float2 position, Math::float2 size)
{
    Textures::Texture &texInvBack = m_Engine.getEngineTextureAlloc().getTexture(m_TexInvBack);

    drawTexture(RenderViewList::INVENTORY_BG, Math::ifloor(position.x) - 10, Math::ifloor(position.y) - 10,
                Math::ifloor(size.x) + 20, Math::ifloor(size.y) + 20,
                config.state.viewWidth, config.state.viewHeight, texInvBack.m_TextureHandle,
                config.programs.imageProgram, config.uniforms.diffuseTexture);

    const std::string &descriptionString = item.description.empty() ? item.name : item.description;
    drawText(descriptionString, Math::ifloor(position.x + size.x / 2), Math::ifloor(position.y),
             EAlign::A_TopCenter, config);

    for(int i = 0; i < Daedalus::GEngineClasses::ITM_TEXT_MAX; ++i)
    {
        drawText(item.text[i], Math::ifloor(position.x), Math::ifloor(32.0f + position.y + i * 16.0f),
                 EAlign::A_TopLeft, config);
        // Test if the count is unequal to zero otherwise we would print zeros next to purely informatic text
        // ITM_TEXT_MAX - 1 is an exception because this is the "Value" attribute and here zero is possible
        if(item.count[i] != 0 || i == (Daedalus::GEngineClasses::ITM_TEXT_MAX - 1))
        {
            drawText(std::to_string(item.count[i]), Math::ifloor(position.x + size.x), Math::ifloor(32.0f + position.y + i * 16.0f),
                     EAlign::A_TopRight, config);
        }
    }
    float itemSize = 150.0f;
    drawItem(config, item, Math::float2(position.x + size.x - 200.0f, position.y + (size.y - itemSize) / 2), itemSize, false);
}

void UI::InventoryView::update(double dt, Engine::Input::MouseState &mstate, Render::RenderConfig &config)
{
    if(m_IsHidden)
        return;

    // Just some debug things
    static bool isFirstTime = true;
    if(isFirstTime)
    {
        isFirstTime = false;

        if( Daedalus::GameType::GT_Gothic1 == m_Engine.getBasicGameType())
        {
            m_Engine.getConsole().submitCommand("giveitem ALTESSCHWERT");
            m_Engine.getConsole().submitCommand("giveitem HEILTRANK");
            m_Engine.getConsole().submitCommand("giveitem CRONOS_BRIEF");
            m_Engine.getConsole().submitCommand("giveitem CORDS_SPALTER");
            m_Engine.getConsole().submitCommand("giveitem ITAMARROW");
            m_Engine.getConsole().submitCommand("giveitem HEALTHWATER");
            m_Engine.getConsole().submitCommand("giveitem ASTRONOMIE");
            m_Engine.getConsole().submitCommand("giveitem ITARRUNEFIREBOLT");
        }
        else if( Daedalus::GameType::GT_Gothic2 == m_Engine.getBasicGameType())
        {
            m_Engine.getConsole().submitCommand("giveitem ITFP_POTION_MANA_02");
            m_Engine.getConsole().submitCommand("giveitem ITRI_ADDON_HEALTH_01");
            m_Engine.getConsole().submitCommand("giveitem ITAM_ADDON_HEALTH");
            m_Engine.getConsole().submitCommand("giveitem ITWR_STONEPLATECOMMON_ADDON");
            m_Engine.getConsole().submitCommand("giveitem ITMW_ADDON_PIR1HAXE");
            m_Engine.getConsole().submitCommand("giveitem ITFO_BEER");
            m_Engine.getConsole().submitCommand("giveitem ITRU_ARMYOFDARKNESS");
            m_Engine.getConsole().submitCommand("giveitem ITPO_HEALTH_01");
            m_Engine.getConsole().submitCommand("giveitem ITFO_APPLE");
            m_Engine.getConsole().submitCommand("giveitem ITKE_BROMOR");
            m_Engine.getConsole().submitCommand("giveitem ITPL_MUSHROOM_01");
            m_Engine.getConsole().submitCommand("giveitem ITPL_MUSHROOM_02");
            m_Engine.getConsole().submitCommand("giveitem ITFO_CHEESE");
            m_Engine.getConsole().submitCommand("giveitem ITKE_LOCKPICK");
            m_Engine.getConsole().submitCommand("giveitem ITWR_MAP_NEWWORLD_CITY");
            m_Engine.getConsole().submitCommand("giveitem ITMI_PANFULL");
            m_Engine.getConsole().submitCommand("giveitem ITAT_WARANFIRETONGUE");
            m_Engine.getConsole().submitCommand("giveitem ITLSTORCH");
            m_Engine.getConsole().submitCommand("giveitem ITAR_PAL_H");
            m_Engine.getConsole().submitCommand("giveitem ITPL_STRENGTH_HERB_01");
            m_Engine.getConsole().submitCommand("giveitem ITMI_GOLD");
            m_Engine.getConsole().submitCommand("giveitem ITRI_DEX_01");
        }
    }

    World::WorldInstance &world = m_Engine.getMainWorld().get();
    Logic::PlayerController* playerController = reinterpret_cast<Logic::PlayerController*>(
                world.getEntity<Components::LogicComponent>(world.getScriptEngine().getPlayerEntity()).m_pLogicController);

    const std::list<Daedalus::GameState::ItemHandle> &itemHandleList = playerController->getInventory().getItems();
    std::vector<Daedalus::GEngineClasses::C_Item> itemList(itemHandleList.size());
    std::list<Daedalus::GameState::ItemHandle>::const_iterator itItem = itemHandleList.begin();
    for(uint32_t index = 0; index < itemList.size(); ++index)
    {
        itemList[index] = world.getScriptEngine().getGameState().getItem(*itItem);
        ++itItem;
    }

    // Provide the render function a list of indices into the itemList array
    std::vector<int> indices(itemList.size());
    std::iota(indices.begin(), indices.end(), 0);

    // Sort the indices
    const auto &constItemList = itemList;
    std::sort(indices.begin(), indices.end(), [&constItemList](const int &a, const int &b)
    {
        return constItemList[a].value > constItemList[b].value;
    });

    int selectedSlot = -1;
    // Search the sorted list for the selected item
    for( int index = 0; index < indices.size(); ++index)
    {
        if(itemList[indices[index]].instanceSymbol == m_SelectedItemID)
        {
            selectedSlot = index;
            break;
        }
    }
    if(selectedSlot < 0)
        selectedSlot = std::max(0, std::min(m_LastTimeSelectedItemSlot, int(indices.size())));

    Math::float2 inventoryPosition = Math::float2(0.7f * config.state.viewWidth, 0.1f * config.state.viewHeight);
    Math::float2 inventorySize = Math::float2(0.3f * config.state.viewWidth, 0.8f * config.state.viewHeight);

    float slotSize = 100.0f;
    int numColumns = std::max(1, Math::ifloor(inventorySize.x / slotSize));
    int numRows = std::max(1, Math::ifloor(inventorySize.y / slotSize));
    int numSlots = numRows * numColumns;

    int maxRowsByItemCount = itemList.size() / numColumns;
    // Get old row and column
    int currentRow = selectedSlot / numColumns;
    int currentColumn = selectedSlot % numColumns;
    // Adjust row by input
    currentRow = std::max(0, std::min(currentRow + m_DeltaRows, maxRowsByItemCount));
    // and adjusts column by input
    // This is the actual index for the item in itemList which we want to select
    int itemIndex = std::max(0, std::min(currentRow * numColumns + currentColumn + m_DeltaColumns, int(itemList.size()) - 1));
    m_SelectedItemID = itemList[indices[itemIndex]].instanceSymbol;
    //m_SelectedSlot = itemIndex;
    // Reset for next input
    m_DeltaRows = 0;
    m_DeltaColumns = 0;

    int itemRow = itemIndex / numColumns;
    // The item is above the rows we are currently showing
    if (itemRow < m_DisplayedRowsStart)
        m_DisplayedRowsStart = itemRow;
    // The item is below the rows we are currently showing
    if (itemRow >= m_DisplayedRowsStart + numRows)
        m_DisplayedRowsStart = itemRow - numRows + 1;
    // If there is enough space anyway, simply display all items
    if (itemList.size() <= numSlots)
        m_DisplayedRowsStart = 0;

    int offsetSlots = m_DisplayedRowsStart * numColumns;
    int numVisibleItems = std::min(numSlots, int(itemList.size() - offsetSlots));

    // only display visible items
    std::vector<int> displayIndices(indices.begin() + offsetSlots, indices.begin() + offsetSlots + numVisibleItems);

    drawItemGrid(config, inventoryPosition, inventorySize, EAlign::A_TopRight,
                 slotSize, itemList, displayIndices, itemIndex - offsetSlots);

    if(indices.size() > 0)
        drawItemInfobox(config, itemList[indices[itemIndex]], Math::float2(0.5f * config.state.viewWidth - 300, config.state.viewHeight - 140 - 10),
                Math::float2(600, 140));

    View::update(dt, mstate, config);
}

void UI::InventoryView::setEnabled(bool enabled)
{
    m_IsHidden = !enabled;
    if(enabled)
    {
        m_InputUp.getAction().setEnabled(true);
        m_InputDown.getAction().setEnabled(true);
        m_InputLeft.getAction().setEnabled(true);
        m_InputRight.getAction().setEnabled(true);
        m_InputUse.getAction().setEnabled(true);
    }
    else
    {
        m_InputUp.getAction().setEnabled(false);
        m_InputDown.getAction().setEnabled(false);
        m_InputLeft.getAction().setEnabled(false);
        m_InputRight.getAction().setEnabled(false);
        m_InputUse.getAction().setEnabled(false);
    }
}
