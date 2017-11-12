#include "InventoryView.h"
#include <engine/BaseEngine.h>
#include <logic/Inventory.h>
#include <logic/PlayerController.h>
#include <logic/visuals/StaticMeshVisual.h>
#include <logic/ScriptEngine.h>
#include <logic/ItemController.h>
#include <components/Vob.h>

#include <bitset>

const float FARPLANE = 1000.0f;

UI::InventoryView::InventoryView(Engine::BaseEngine& e)
    : View(e)
{
    Textures::TextureAllocator& texAlloc = m_Engine.getEngineTextureAlloc();
    m_TexSlot = texAlloc.loadTextureVDF("INV_SLOT.TGA");
    m_TexSlotHighlighted = texAlloc.loadTextureVDF("INV_SLOT_HIGHLIGHTED.TGA");
    m_TexSlotEquipped = texAlloc.loadTextureVDF("INV_SLOT_EQUIPPED.TGA");
    // this texture depends on the game and gets loaded when we know whether g1
    // or g2 is being played
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
    m_InputDrop = Engine::Input::RegisterAction(Engine::ActionType::Inventory_Drop, [this](bool triggered, float)
    {
        if (triggered)
        {
            World::WorldInstance &world = m_Engine.getMainWorld().get();
            Logic::PlayerController* playerController = reinterpret_cast<Logic::PlayerController*>(
                        world.getEntity<Components::LogicComponent>(world.getScriptEngine().getPlayerEntity()).m_pLogicController);

            Logic::Inventory &playerInv = playerController->getInventory();
            const std::list<Daedalus::GameState::ItemHandle> &itemHandleList = playerInv.getItems();

            for(auto &itItem : itemHandleList)
            {
                Daedalus::GEngineClasses::C_Item itemData = world.getScriptEngine().getGameState().getItem(itItem);
                if(itemData.instanceSymbol == m_SelectedItemID)
                {
                    //TODO play drop animation, mabye add some gravity
                    playerInv.removeItem(itItem);

                    Handle::EntityHandle e = Vob::constructVob(world);

                    // Setup itemcontroller
                    Components::LogicComponent& logic = world.getEntity<Components::LogicComponent>(e);
                    logic.m_pLogicController = new Logic::ItemController(world, e, itemData.instanceSymbol);

                    Vob::VobInformation vob = Vob::asVob(world, e);

                    Math::Matrix playerPosition = world.getEntity<Components::PositionComponent>(world.getScriptEngine().getPlayerEntity()).m_WorldMatrix;

                    // forward is behind the player for some reason
                    Vob::setPosition(vob, playerPosition.Translation() - 1.0f * playerPosition.Forward());

                    Vob::setVisual(vob, itemData.visual);

                    break;
                }
            }
        }
    });
    m_InputDrop.getAction().setEnabled(false);
    m_InputUse = Engine::Input::RegisterAction(Engine::ActionType::Inventory_Use, [this](bool triggered, float)
    {
        if (triggered)
        {
            World::WorldInstance &world = m_Engine.getMainWorld().get();
            Logic::PlayerController* playerController = reinterpret_cast<Logic::PlayerController*>(
                        world.getEntity<Components::LogicComponent>(world.getScriptEngine().getPlayerEntity()).m_pLogicController);

            Logic::Inventory &playerInv = playerController->getInventory();
            const std::list<Daedalus::GameState::ItemHandle> &itemHandleList = playerInv.getItems();

            for(auto &itItem : itemHandleList)
            {
                Daedalus::GEngineClasses::C_Item itemData = world.getScriptEngine().getGameState().getItem(itItem);
                if(itemData.instanceSymbol == m_SelectedItemID)
                {
                    //TODO play use animation
                    //playerInv.removeItem(itItem);

                    if(Daedalus::GEngineClasses::C_Item::Categories::ITM_CAT_EQUIPABLE & itemData.mainflag)
                    {
                        const std::set<Daedalus::GameState::ItemHandle> &equippedItems = playerController->getEquippedItems();
                        if(equippedItems.find(itItem) != equippedItems.end())
                            playerController->unequipItem(itItem);
                        else
                            playerController->useItem(itItem);
                    }
                    else
                        playerController->useItem(itItem);

                    break;
                }
            }
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

    for( size_t subMesh = 0; subMesh < itemMesh.mesh.m_SubmeshMaterialNames.size(); ++subMesh)
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
                  const std::vector<ItemDrawState> &itemsToDraw)
{
    if(!m_TexInvBack.isValid())
    {
        Textures::TextureAllocator& texAlloc = m_Engine.getEngineTextureAlloc();
        if(m_Engine.getBasicGameType() == Daedalus::GameType::GT_Gothic2)
            m_TexInvBack = texAlloc.loadTextureVDF("INV_BACK.TGA");
        else if(m_Engine.getBasicGameType() == Daedalus::GameType::GT_Gothic1)
            m_TexInvBack = texAlloc.loadTextureVDF("DLG_AMBIENT.TGA");
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
    Textures::Texture& texSlotEquipped = m_Engine.getEngineTextureAlloc().getTexture(m_TexSlotEquipped);

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

        if(slot >= itemsToDraw.size())
            continue;

        bool isSelected = ItemDrawFlags::Selected & itemsToDraw[slot].flags;
        bool isEquipped = ItemDrawFlags::Equipped & itemsToDraw[slot].flags;

        if(isSelected)
            drawTexture(RenderViewList::UI, xPos, yPos, slotSize, slotSize,
                        config.state.viewWidth, config.state.viewHeight, texSlotHighlighted.m_TextureHandle,
                        config.programs.imageProgram, config.uniforms.diffuseTexture);

        if(isEquipped)
            drawTexture(RenderViewList::INVENTORY_BG, xPos, yPos, slotSize, slotSize,
                        config.state.viewWidth, config.state.viewHeight, texSlotEquipped.m_TextureHandle,
                        config.programs.imageProgram, config.uniforms.diffuseTexture);


        const Daedalus::GEngineClasses::C_Item &itemData = itemList[itemsToDraw[slot].index];

        // Draw the item amount number in the lower right corner
        drawText(std::to_string(itemData.amount), xPos + slotSize - 15, yPos + slotSize - 5,
                 A_BottomRight, config, UI::DEFAULT_FONT_LARGE);

        drawItem(config, itemData, Math::float2(xPos, yPos), slotSize, isSelected);
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

    auto simpleDraw = [this, &config](int row, std::string name, std::string value)
    {
        drawText(name, 250, 100 + 15 * row, EAlign::A_TopLeft, config);
        drawText(value, 500, 100 + 15 * row, EAlign::A_TopLeft, config);
    };

    std::string names[32];
    for(int i = 0; i < 32; ++i)
        names[i] = std::string("BIT_") + std::to_string(i);

    names[14] = "SWD";
    names[15] = "AXE";
    names[16] = "2HD_SWD";
    names[17] = "2HD_AXE";
    names[19] = "BOW";
    names[20] = "CROSSBOW";
    names[22] = "AMULET";
    names[11] = "RING";
    names[24] = "BELT";

    std::string flagString = "";
    for(int i = 0; i < 32; ++i)
        if(item.flags & (1 << i))
        {
            if(!flagString.empty())
                flagString += " | ";
            flagString += names[i];
        }

    std::string symbolName = m_Engine.getMainWorld().get().getScriptEngine().getVM().getDATFile().getSymbolByIndex(item.instanceSymbol).name;

    simpleDraw(-2, "symbolName", symbolName);
    simpleDraw(-1, "flagString", flagString);
    simpleDraw(0, "id", std::to_string(item.id));
    simpleDraw(1, "name", item.name);
    simpleDraw(2, "nameID", item.nameID);
    simpleDraw(3, "hp", std::to_string(item.hp));
    simpleDraw(4, "hp_max", std::to_string(item.hp_max));
    simpleDraw(5, "mainflag", std::bitset<32>(item.mainflag).to_string());
    simpleDraw(6, "flags", std::bitset<32>(item.flags).to_string());
    simpleDraw(7, "weight", std::to_string(item.weight));
    simpleDraw(8, "value", std::to_string(item.value));

    simpleDraw(10, "damageType", std::to_string(item.damageType));
    simpleDraw(11, "damageTotal", std::to_string(item.damageTotal));
    for(int i = 0; i < Daedalus::GEngineClasses::DAM_INDEX_MAX; ++i)
        simpleDraw(12 + i, std::string("damage[") + std::to_string(i) + std::string("]"), std::to_string(item.damage[i]));

    simpleDraw(21, "wear", std::to_string(item.wear));
    for(int i = 0; i < Daedalus::GEngineClasses::PROT_INDEX_MAX; ++i)
        simpleDraw(22 + i, "protection[" + std::to_string(i) + "]", std::to_string(item.protection[i]));

    simpleDraw(31, "nutrition", std::to_string(item.nutrition));
    simpleDraw(32, "owner", std::to_string(item.owner));
    simpleDraw(33, "ownerGuild", std::to_string(item.ownerGuild));
    simpleDraw(34, "disguiseGuild", std::to_string(item.disguiseGuild));
    simpleDraw(35, "munition", std::to_string(item.munition));
    simpleDraw(36, "spell", std::to_string(item.spell));
    simpleDraw(37, "range", std::to_string(item.range));
    simpleDraw(38, "mag_circle", std::to_string(item.mag_circle));

    //drawText(std::bitset<32>(item.mainflag).to_string(), 200, 200, EAlign::A_TopLeft, config);
    //drawText(std::bitset<32>(item.flags).to_string(), 200, 250, EAlign::A_TopLeft, config);
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
        const std::vector<std::string> g2Items = {
            "ItAm_Prot_Fire_01",
            "ItAm_Prot_Edge_01",
            "ItAm_Prot_Point_01",
            "ItAm_Prot_Mage_01",
            "ItAm_Prot_Total_01",
            "ItAm_Dex_01",
            "ItAm_Strg_01",
            "ItAm_Hp_01",
            "ItAm_Mana_01",
            "ItAm_Dex_Strg_01",
            "ItAm_Hp_Mana_01",
            "ItSe_ErzFisch",
            "ItSe_GoldFisch",
            "ItSe_Ringfisch",
            "ItSe_LockpickFisch",
            "ItSe_GoldPocket25",
            "ItSe_GoldPocket50",
            "ItSe_GoldPocket100",
            "ItSe_HannasBeutel",
            "ItLsTorch",
            "ItLsTorchburning",
            "ItLsTorchburned",
            "ItLsTorchFirespit",
            "ItRw_Arrow",
            "ItRw_Bolt",
            "ItRw_Mil_Crossbow",
            "ItRw_Sld_Bow",
            "ItRw_Bow_L_01",
            "ItRw_Bow_L_02",
            "ItRw_Bow_L_03",
            "ItRw_Bow_L_04",
            "ItRw_Bow_M_01",
            "ItRw_Bow_M_02",
            "ItRw_Bow_M_03",
            "ItRw_Bow_M_04",
            "ItRw_Bow_H_01",
            "ItRw_Bow_H_02",
            "ItRw_Bow_H_03",
            "ItRw_Bow_H_04",
            "ItRw_Crossbow_L_01",
            "ItRw_Crossbow_L_02",
            "ItRw_Crossbow_M_01",
            "ItRw_Crossbow_M_02",
            "ItRw_Crossbow_H_01",
            "ItRw_Crossbow_H_02",
            "ItKe_Xardas",
            "ItWr_Canthars_KomproBrief_MIS",
            "ItMw_2h_Rod",
            "ItMi_CoragonsSilber",
            "ItMi_TheklasPaket",
            "ItMi_MariasGoldPlate",
            "ItRi_ValentinosRing",
            "ItKe_Dexter",
            "ItWr_Kraeuterliste",
            "ItWr_ManaRezept",
            "ItWr_Passierschein",
            "ItMi_HerbPaket",
            "ItKe_Storage",
            "ItFo_SmellyFish",
            "ItFo_HalvorFish_MIS",
            "ItWr_HalvorMessage",
            "ItFo_HalvorFish",
            "ItMw_AlriksSword_Mis",
            "ItWr_VatrasMessage",
            "ItWr_VatrasMessage_Open",
            "ItKe_Hotel",
            "ItKe_ThiefGuildKey_MIS",
            "ItKe_ThiefGuildKey_Hotel_MIS",
            "ItKe_Innos_MIS",
            "ItKe_KlosterSchatz",
            "ItKe_KlosterStore",
            "ItKe_KDFPlayer",
            "ItKe_KlosterBibliothek",
            "ItFo_Schafswurst",
            "ItPo_Perm_LittleMana",
            "Holy_Hammer_MIS",
            "ItKe_MagicChest",
            "ItWr_Passage_MIS",
            "ItWr_BanditLetter_MIS",
            "ItWr_Poster_MIS",
            "ItKe_Bandit",
            "ItRw_Bow_L_03_MIS",
            "ItRi_Prot_Point_01_MIS",
            "ItMi_EddasStatue",
            "ItKe_EVT_CRYPT_01",
            "ItKe_EVT_CRYPT_02",
            "ItKe_EVT_CRYPT_03",
            "ITAR_PAL_SKEL",
            "ItKe_Valentino",
            "ItKe_Buerger",
            "ItKe_Richter",
            "ItKe_Salandril",
            "ItKe_PaladinTruhe",
            "ItKe_ThiefTreasure",
            "ItKe_Fingers",
            "ItWr_Schuldenbuch",
            "ItPl_Sagitta_Herb_MIS",
            "ITKE_ORLAN_HOTELZIMMER",
            "ItRw_DragomirsArmbrust_MIS",
            "ItMi_StoneOfKnowlegde_MIS",
            "ItWr_PaladinLetter_MIS",
            "ItWr_LetterForGorn_MIS",
            "ItKe_PrisonKey_MIS",
            "ItKe_OC_Store",
            "ITKE_ErzBaronFlur",
            "ITKE_ErzBaronRaum",
            "ItMi_GornsTreasure_MIS",
            "ItWr_Silvestro_MIS",
            "ItAt_ClawLeader",
            "ItSe_Olav",
            "ItMi_GoldPlate_MIS",
            "ItKe_Pass_MIS",
            "ItKe_Bromor",
            "ITKE_RUNE_MIS",
            "ItWr_Bloody_MIS",
            "ItWr_Pfandbrief_MIS",
            "ItWr_Map_OldWorld_Oremines_MIS",
            "ItWr_Manowar",
            "ItWr_KDWLetter",
            "ItWr_GilbertLetter",
            "ItRi_Tengron",
            "ItMi_InnosEye_MIS",
            "ItMi_InnosEye_Discharged_Mis",
            "ItMi_InnosEye_Broken_Mis",
            "ItWr_PermissionToWearInnosEye_MIS",
            "ItWr_XardasBookForPyrokar_Mis",
            "ItKe_CHEST_SEKOB_XARDASBOOK_MIS",
            "ItWr_CorneliusTagebuch_Mis",
            "ITWR_DementorObsessionBook_MIS",
            "ItWr_PyrokarsObsessionList",
            "ItPo_HealHilda_MIS",
            "ItMw_MalethsGehstock_MIS",
            "ItMi_MalethsBanditGold",
            "ItMi_Moleratlubric_MIS",
            "ItWr_BabosLetter_MIS",
            "ItWr_BabosPinUp_MIS",
            "ItWr_BabosDocs_MIS",
            "ItKe_IgarazChest_Mis",
            "ItWr_Astronomy_Mis",
            "ItPo_HealObsession_MIS",
            "ItSe_Golemchest_Mis",
            "ItWr_ShatteredGolem_MIS",
            "ItWr_DiegosLetter_MIS",
            "ItSe_DiegosTreasure_Mis",
            "ItMi_UltharsHolyWater_Mis",
            "ItWr_MinenAnteil_Mis",
            "ItAm_Prot_BlackEye_Mis",
            "ItMi_KarrasBlessedStone_Mis",
            "ItWr_RichterKomproBrief_MIS",
            "ItWr_MorgahardTip",
            "ItWr_Map_Shrine_MIS",
            "ItWr_VinosKellergeister_Mis",
            "ItAm_Mana_Angar_MIS",
            "ItMW_1H_FerrosSword_Mis",
            "ItMi_KerolothsGeldbeutel_MIS",
            "ItMi_KerolothsGeldbeutelLeer_MIS",
            "ItRw_SengrathsArmbrust_MIS",
            "ItAt_TalbinsLurkerSkin",
            "ItAt_DragonEgg_MIS",
            "ItRi_OrcEliteRing",
            "ItPo_DragonEggDrinkNeoras_MIS",
            "ItWr_Map_Orcelite_MIS",
            "ItWr_Map_Caves_MIS",
            "ItWr_XardasLetterToOpenBook_MIS",
            "ItKe_MonastarySecretLibrary_Mis",
            "ItWr_HallsofIrdorath_Mis",
            "ItWr_HallsofIrdorath_Open_Mis",
            "ItWr_XardasSeamapBook_Mis",
            "ItWr_UseLampIdiot_Mis",
            "ItWr_Seamap_Irdorath",
            "ITWr_ForgedShipLetter_MIS",
            "ITKE_OC_MAINGATE_MIS",
            "ITKE_SHIP_LEVELCHANGE_MIS",
            "ItPo_PotionOfDeath_01_Mis",
            "ItPo_PotionOfDeath_02_Mis",
            "ItAm_AmulettOfDeath_Mis",
            "ItPo_HealRandolph_MIS",
            "ItSe_XardasNotfallBeutel_MIS",
            "ItWr_XardasErmahnungFuerIdioten_MIS",
            "ItWr_Krypta_Garon",
            "ItKe_OrkKnastDI_MIS",
            "ItKe_EVT_UNDEAD_01",
            "ItKe_EVT_UNDEAD_02",
            "ItKe_LastDoorToUndeadDrgDI_MIS",
            "ItWr_LastDoorToUndeadDrgDI_MIS",
            "ItKe_ChestMasterDementor_MIS",
            "ItWr_Rezept_MegaDrink_MIS",
            "ItWr_Diary_BlackNovice_MIS",
            "ItWr_ZugBruecke_MIS",
            "ItMi_PowerEye",
            "ItMw_1h_Vlk_Dagger",
            "ItMw_1H_Mace_L_01",
            "ItMw_1h_Bau_Axe",
            "ItMw_1h_Vlk_Mace",
            "ItMw_1H_Mace_L_03",
            "ItMw_1h_Bau_Mace",
            "ItMw_1h_Vlk_Axe",
            "ItMw_1H_Mace_L_04",
            "ItMw_ShortSword1",
            "ItMw_Nagelknueppel",
            "ItMw_1H_Sword_L_03",
            "ItMw_ShortSword2",
            "ItMw_Sense",
            "ItMw_1h_Vlk_Sword",
            "ItMw_1h_Nov_Mace",
            "ItMw_2h_Bau_Axe",
            "ItMw_2H_Axe_L_01",
            "ItMw_1h_MISC_Sword",
            "ItMw_1h_Misc_Axe",
            "ItMw_2H_Sword_M_01",
            "ItMw_1h_Mil_Sword",
            "ItMw_1h_Sld_Axe",
            "ItMw_1h_Sld_Sword",
            "ItMw_2h_Sld_Axe",
            "ItMw_2h_Sld_Sword",
            "ItMw_1h_Pal_Sword",
            "ItMw_2h_Pal_Sword",
            "ItMw_2H_OrcAxe_01",
            "ItMw_2H_OrcAxe_02",
            "ItMw_2H_OrcAxe_03",
            "ItMw_2H_OrcAxe_04",
            "ItMw_2H_OrcSword_01",
            "ItMw_2H_OrcSword_02",
            "ItMw_ShortSword3",
            "ItMw_Nagelkeule",
            "ItMw_ShortSword4",
            "ItMw_Kriegskeule",
            "ItMw_Richtstab",
            "ItMw_ShortSword5",
            "ItMw_Kriegshammer1",
            "ItMw_Hellebarde",
            "ItMw_Nagelkeule2",
            "ItMw_Schiffsaxt",
            "ItMw_Piratensaebel",
            "ItMw_Schwert",
            "ItMw_1H_Common_01",
            "ItMw_Stabkeule",
            "ItMw_Zweihaender1",
            "ItMw_Steinbrecher",
            "ItMw_Spicker",
            "ItMw_Streitaxt1",
            "ItMw_Schwert1",
            "ItMw_Schwert2",
            "ItMw_Doppelaxt",
            "ItMw_Bartaxt",
            "ItMw_Morgenstern",
            "ItMw_Schwert3",
            "ItMw_Schwert4",
            "ItMw_1H_Special_01",
            "ItMw_2H_Special_01",
            "ItMw_Rapier",
            "ItMw_Rubinklinge",
            "ItMw_Streitkolben",
            "ItMw_Zweihaender2",
            "ItMw_Runenschwert",
            "ItMw_Rabenschnabel",
            "ItMw_Schwert5",
            "ItMw_Inquisitor",
            "ItMw_Streitaxt2",
            "ItMw_Zweihaender3",
            "ItMw_1H_Special_02",
            "ItMw_2H_Special_02",
            "ItMw_ElBastardo",
            "ItMw_Kriegshammer2",
            "ItMw_Meisterdegen",
            "ItMw_Folteraxt",
            "ItMw_Orkschlaechter",
            "ItMw_Zweihaender4",
            "ItMw_Schlachtaxt",
            "ItMw_Krummschwert",
            "ItMw_Barbarenstreitaxt",
            "ItMw_Sturmbringer",
            "ItMw_1H_Special_03",
            "ItMw_2H_Special_03",
            "ItMw_Berserkeraxt",
            "ItMw_Drachenschneide",
            "ItMw_1H_Special_04",
            "ItMw_2H_Special_04",
            "ItMw_1H_Blessed_01",
            "ItMw_1H_Blessed_02",
            "ItMw_1H_Blessed_03",
            "ItMw_2H_Blessed_01",
            "ItMw_2H_Blessed_02",
            "ItMw_2H_Blessed_03",
            "ItMw_1H_Sword_L_01",
            "ItMw_1H_Mace_L_02",
            "ItMw_1H_Axe_L_01",
            "ItMw_1H_Sword_L_02",
            "ItMw_1H_Axe_L_02",
            "ItMw_1H_Sword_L_04",
            "ItMw_1H_Axe_L_03",
            "ItMw_1H_Mace_L_05",
            "ItMw_1H_Sword_L_05",
            "ItMw_1H_Sword_L_06",
            "ItMw_1H_Axe_L_04",
            "ItMw_1H_Mace_L_06",
            "ItMw_1H_Sword_L_07",
            "ItMw_1H_Sword_L_08",
            "ItMw_1H_Axe_L_05",
            "ItMw_1H_Mace_L_07",
            "ItMw_1H_Sword_L_09",
            "ItMw_1H_Sword_L_10",
            "ItMw_1H_Axe_L_06",
            "ItMw_1H_Mace_L_08",
            "ItMw_1H_Mace_L_09",
            "ItMw_1H_Sword_M_01",
            "ItMw_1H_Axe_M_01",
            "ItMw_1H_Mace_M_01",
            "ItMw_1H_Mace_M_02",
            "ItMw_1H_Sword_M_02",
            "ItMw_1H_Axe_M_02",
            "ItMw_1H_Mace_M_03",
            "ItMw_1H_Mace_M_04",
            "ItMw_1H_Sword_M_03",
            "ItMw_1H_Axe_M_03",
            "ItMw_1H_Mace_M_05",
            "ItMw_1H_Mace_M_06",
            "ItMw_1H_Sword_M_04",
            "ItMw_1H_Axe_M_04",
            "ItMw_1H_Mace_M_07",
            "ItMw_1H_Sword_M_05",
            "ItMw_1H_Sword_H_01",
            "ItMw_1H_Axe_H_01",
            "ItMw_1H_Mace_H_01",
            "ItMw_1H_Sword_H_02",
            "ItMw_1H_Sword_H_03",
            "ItMw_1H_Axe_H_02",
            "ItMw_1H_Mace_H_02",
            "ItMw_1H_Sword_H_04",
            "ItMw_1H_Sword_H_05",
            "ItMw_1H_Axe_H_03",
            "ItMw_1H_Mace_H_03",
            "ItMw_2H_Axe_L_02",
            "ItMw_2H_Mace_L_01",
            "ItMw_2H_Sword_L_01",
            "ItMw_2H_Axe_L_03",
            "ItMw_2H_Mace_L_02",
            "ItMw_2H_Mace_L_03",
            "ItMw_2H_Sword_L_02",
            "ItMw_2H_Axe_L_04",
            "ItMw_2H_Mace_L_04",
            "ItMw_2H_Axe_M_01",
            "ItMw_2H_Mace_M_01",
            "ItMw_2H_Mace_M_02",
            "ItMw_2H_Sword_M_02",
            "ItMw_2H_Axe_M_02",
            "ItMw_2H_Mace_M_03",
            "ItMw_2H_Sword_M_03",
            "ItMw_2H_Axe_M_03",
            "ItMw_2H_Mace_M_04",
            "ItMw_2H_Sword_M_04",
            "ItMw_2H_Sword_M_05",
            "ItMw_2H_Axe_M_04",
            "ItMw_2H_Sword_M_06",
            "ItMw_2H_Sword_M_07",
            "ItMw_2H_Axe_M_05",
            "ItMw_2H_Mace_M_05",
            "ItMw_2H_Mace_M_06",
            "ItMw_2H_Sword_M_08",
            "ItMw_2H_Axe_M_06",
            "ItMw_2H_SWORD_M_09",
            "ItMw_2H_Sword_H_01",
            "ItMw_2H_Axe_H_01",
            "ItMw_2H_Sword_H_02",
            "ItMw_2H_Sword_H_03",
            "ItMw_2H_Axe_H_02",
            "ItMw_2H_Sword_H_04",
            "ItMw_2H_Sword_H_05",
            "ItMw_2H_Axe_H_03",
            "ItMw_2H_Sword_H_06",
            "ItMw_2H_Sword_H_07",
            "ItMw_2H_Axe_H_04",
            "ItMw_1H_Blessed_01",
            "ItMw_1H_Blessed_02",
            "ItMw_1H_Blessed_03",
            "ItMw_2H_Blessed_01",
            "ItMw_2H_Blessed_02",
            "ItMw_2H_Blessed_03",
            "ItFo_Apple",
            "ItFo_Cheese",
            "ItFo_Bacon",
            "ItFo_Bread",
            "ItFo_Fish",
            "ItFoMuttonRaw",
            "ItFoMutton",
            "ItFo_Stew",
            "ItFo_XPStew",
            "ItFo_CoragonsBeer",
            "ItFo_FishSoup",
            "ItFo_Sausage",
            "ItFo_Honey",
            "ItFo_Water",
            "ItFo_Beer",
            "ItFo_Booze",
            "ItFo_Wine",
            "ItFo_Milk",
            "ItPl_Weed",
            "ItPl_Beet",
            "ItPl_SwampHerb",
            "ItPl_Mana_Herb_01",
            "ItPl_Mana_Herb_02",
            "ItPl_Mana_Herb_03",
            "ItPl_Health_Herb_01",
            "ItPl_Health_Herb_02",
            "ItPl_Health_Herb_03",
            "ItPl_Dex_Herb_01",
            "ItPl_Strength_Herb_01",
            "ItPl_Speed_Herb_01",
            "ItPl_Mushroom_01",
            "ItPl_Mushroom_02",
            "ItPl_Blueplant",
            "ItPl_Forestberry",
            "ItPl_Planeberry",
            "ItPl_Temp_Herb",
            "ItPl_Perm_Herb",
            "ItRi_Prot_Fire_01",
            "ItRi_Prot_Fire_02",
            "ItRi_Prot_Point_01",
            "ItRi_Prot_Point_02",
            "ItRi_Prot_Edge_01",
            "ItRi_Prot_Edge_02",
            "ItRi_Prot_Mage_01",
            "ItRi_Prot_Mage_02",
            "ItRi_Prot_Total_01",
            "ItRi_Prot_Total_02",
            "ItRi_Dex_01",
            "ItRi_Dex_02",
            "ItRi_Hp_01",
            "ItRi_Hp_02",
            "ItRi_Str_01",
            "ItRi_Str_02",
            "ItRi_Mana_01",
            "ItRi_Mana_02",
            "ItRi_Hp_Mana_01",
            "ItRi_Dex_Strg_01",
            "ITAR_Governor",
            "ITAR_JUDGE",
            "ITAR_SMITH",
            "ITAR_BARKEEPER",
            "ITAR_VLK_L",
            "ITAR_VLK_M",
            "ITAR_VLK_H",
            "ITAR_VlkBabe_L",
            "ITAR_VlkBabe_M",
            "ITAR_VlkBabe_H",
            "ITAR_MIL_L",
            "ITAR_MIL_M",
            "ITAR_PAL_M",
            "ITAR_PAL_H",
            "ITAR_BAU_L",
            "ITAR_BAU_M",
            "ITAR_BauBabe_L",
            "ITAR_BauBabe_M",
            "ITAR_SLD_L",
            "ITAR_SLD_M",
            "ITAR_SLD_H",
            "ITAR_DJG_Crawler",
            "ITAR_DJG_L",
            "ITAR_DJG_M",
            "ITAR_DJG_H",
            "ITAR_DJG_BABE",
            "ITAR_NOV_L",
            "ITAR_KDF_L",
            "ITAR_KDF_H",
            "ITAR_Leather_L",
            "ITAR_BDT_M",
            "ITAR_BDT_H",
            "ITAR_XARDAS",
            "ITAR_LESTER",
            "ITAR_Diego",
            "ITAR_CorAngar",
            "ITAR_Dementor",
            "ITAR_KDW_H",
            "ITAR_Prisoner",
            "ItRu_PalLight",
            "ItRu_PalLightHeal",
            "ItRu_PalMediumHeal",
            "ItRu_PalFullHeal",
            "ItRu_PalHolyBolt",
            "ItRu_PalRepelEvil",
            "ItRu_PalDestroyEvil",
            "ItRu_PalTeleportSecret",
            "ItRu_TeleportSeaport",
            "ItRu_TeleportMonastery",
            "ItRu_TeleportFarm",
            "ItRu_TeleportXardas",
            "ItRu_TeleportPassNW",
            "ItRu_TeleportPassOW",
            "ItRu_TeleportOC",
            "ItRu_TeleportOWDemonTower",
            "ItRu_TeleportTaverne",
            "ItRu_Light",
            "ItRu_FireBolt",
            "ItRu_Zap",
            "ItRu_LightHeal",
            "ItRu_SumGobSkel",
            "ItRu_InstantFireball",
            "ItRu_Icebolt",
            "ItRu_SumWolf",
            "ItRu_Windfist",
            "ItRu_Sleep",
            "ItRu_MediumHeal",
            "ItRu_LightningFlash",
            "ItRu_ChargeFireball",
            "ItRu_SumSkel",
            "ItRu_Fear",
            "ItRu_IceCube",
            "ItRu_ThunderBall",
            "ItRu_SumGol",
            "ItRu_HarmUndead",
            "ItRu_Pyrokinesis",
            "ItRu_Firestorm",
            "ItRu_IceWave",
            "ItRu_SumDemon",
            "ItRu_FullHeal",
            "ItRu_Firerain",
            "ItRu_BreathOfDeath",
            "ItRu_MassDeath",
            "ItRu_MasterOfDisaster",
            "ItRu_ArmyOfDarkness",
            "ItRu_Shrink",
            "ItRu_Deathbolt",
            "ItRu_Deathball",
            "ItRu_Concussionbolt",
            "ItKe_Lockpick",
            "ItKe_Key_01",
            "ItKe_Key_02",
            "ItKe_Key_03",
            "ItKe_City_Tower_01",
            "ItKe_City_Tower_02",
            "ItKe_City_Tower_03",
            "ItKe_City_Tower_04",
            "ItKe_City_Tower_05",
            "ItKe_City_Tower_06",
            "ItSc_PalLight",
            "ItSc_PalLightHeal",
            "ItSc_PalHolyBolt",
            "ItSc_PalMediumHeal",
            "ItSc_PalRepelEvil",
            "ItSc_PalFullHeal",
            "ItSc_PalDestroyEvil",
            "ItSc_Light",
            "ItSc_Firebolt",
            "ItSc_Icebolt",
            "ItSc_LightHeal",
            "ItSc_SumGobSkel",
            "ItSc_InstantFireball",
            "ItSc_Zap",
            "ItSc_SumWolf",
            "ItSc_Windfist",
            "ItSc_Sleep",
            "ItSc_Charm",
            "ItSc_MediumHeal",
            "ItSc_LightningFlash",
            "ItSc_ChargeFireball",
            "ItSc_SumSkel",
            "ItSc_Fear",
            "ItSc_IceCube",
            "ItSc_ThunderBall",
            "ItSc_SumGol",
            "ItSc_HarmUndead",
            "ItSc_Pyrokinesis",
            "ItSc_Firestorm",
            "ItSc_IceWave",
            "ItSc_SumDemon",
            "ItSc_FullHeal",
            "ItSc_Firerain",
            "ItSc_BreathOfDeath",
            "ItSc_MassDeath",
            "ItSc_ArmyOfDarkness",
            "ItSc_Shrink",
            "ItSc_TrfSheep",
            "ItSc_TrfScavenger",
            "ItSc_TrfGiantRat",
            "ItSc_TrfGiantBug",
            "ItSc_TrfWolf",
            "ItSc_TrfWaran",
            "ItSc_TrfSnapper",
            "ItSc_TrfWarg",
            "ItSc_TrfFireWaran",
            "ItSc_TrfLurker",
            "ItSc_TrfShadowbeast",
            "ItSc_TrfDragonSnapper",
            "StandardBrief",
            "StandardBuch",
            "ItWr_Map_NewWorld",
            "ItWr_Map_NewWorld_City",
            "ItWr_Map_OldWorld",
            "ItWr_EinhandBuch",
            "ItWr_ZweihandBuch",
            "ItMi_Stomper",
            "ItMi_RuneBlank",
            "ItMi_Pliers",
            "ItMi_Flask",
            "ItMi_Hammer",
            "ItMi_Scoop",
            "ItMi_Pan",
            "ItMi_PanFull",
            "ItMi_Saw",
            "ItMiSwordraw",
            "ItMiSwordrawhot",
            "ItMiSwordbladehot",
            "ItMiSwordblade",
            "ItMi_Broom",
            "ItMi_Lute",
            "ItMi_Brush",
            "ItMi_Joint",
            "ItMi_Alarmhorn",
            "ItMi_Packet",
            "ItMi_Pocket",
            "ItMi_Nugget",
            "ItMi_Gold",
            "ItMi_OldCoin",
            "ItMi_GoldCandleHolder",
            "ItMi_GoldNecklace",
            "ItMi_SilverRing",
            "ItMi_SilverCup",
            "ItMi_SilverPlate",
            "ItMi_GoldPlate",
            "ItMi_GoldCup",
            "ItMi_BloodCup_MIS",
            "ItMi_GoldRing",
            "ItMi_SilverChalice",
            "ItMi_JeweleryChest",
            "ItMi_GoldChalice",
            "ItMi_GoldChest",
            "ItMi_InnosStatue",
            "ItMi_Sextant",
            "ItMi_SilverCandleHolder",
            "ItMi_SilverNecklace",
            "ItMi_Sulfur",
            "ItMi_Quartz",
            "ItMi_Pitch",
            "ItMi_Rockcrystal",
            "ItMi_Aquamarine",
            "ItMi_HolyWater",
            "ItMi_Coal",
            "ItMi_DarkPearl",
            "ItMi_ApfelTabak",
            "ItMi_PilzTabak",
            "ItMi_DoppelTabak",
            "ItMi_Honigtabak",
            "ItMi_SumpfTabak",
            "ItAt_Addon_BCKopf",
            "ItAt_Meatbugflesh",
            "ItAt_SheepFur",
            "ItAt_WolfFur",
            "ItAt_BugMandibles",
            "ItAt_Claw",
            "ItAt_LurkerClaw",
            "ItAt_Teeth",
            "ItAt_CrawlerMandibles",
            "ItAt_Wing",
            "ItAt_Sting",
            "itat_LurkerSkin",
            "ItAt_WargFur",
            "ItAt_Addon_KeilerFur",
            "ItAt_DrgSnapperHorn",
            "ItAt_CrawlerPlate",
            "ItAt_ShadowFur",
            "ItAt_SharkSkin",
            "ItAt_TrollFur",
            "ItAt_TrollBlackFur",
            "ItAt_WaranFiretongue",
            "ItAt_ShadowHorn",
            "ItAt_SharkTeeth",
            "ItAt_TrollTooth",
            "ItAt_StoneGolemHeart",
            "ItAt_FireGolemHeart",
            "ItAt_IceGolemHeart",
            "ItAt_GoblinBone",
            "ItAt_SkeletonBone",
            "ItAt_DemonHeart",
            "ItAt_UndeadDragonSoulStone",
            "ItAt_IcedragonHeart",
            "ItAt_RockdragonHeart",
            "ItAt_SwampdragonHeart",
            "ItAt_FiredragonHeart",
            "ItAt_DragonBlood",
            "ItAt_DragonScale",
            "ItPo_Mana_01",
            "ItPo_Mana_02",
            "ItPo_Mana_03",
            "ItPo_Health_01",
            "ItPo_Health_02",
            "ItPo_Health_03",
            "ItPo_Perm_STR",
            "ItPo_Perm_DEX",
            "ItPo_Perm_Health",
            "ItPo_Perm_Mana",
            "ItPo_Speed",
            "ItPo_MegaDrink",
            "ItAm_Addon_Franco",
            "ItAm_Addon_Health",
            "ItRi_Addon_Health_01",
            "ItRi_Addon_Health_02",
            "ItAm_Addon_MANA",
            "ItRi_Addon_MANA_01",
            "ItRi_Addon_MANA_02",
            "ItAm_Addon_STR",
            "ItRi_Addon_STR_01",
            "ItRi_Addon_STR_02",
            "ItWr_Addon_BookXP250",
            "ItWr_Addon_BookXP500",
            "ItWr_Addon_BookXP1000",
            "ItWr_Addon_BookLP2",
            "ItWr_Addon_BookLP3",
            "ItWr_Addon_BookLP5",
            "ItWr_Addon_BookLP8",
            "ItBE_Addon_Leather_01",
            "ItBE_Addon_SLD_01",
            "ItBE_Addon_NOV_01",
            "ItBE_Addon_MIL_01",
            "ItBE_Addon_KDF_01",
            "ItBE_Addon_MC",
            "ItBe_Addon_STR_5",
            "ItBe_Addon_STR_10",
            "ItBe_Addon_DEX_5",
            "ItBe_Addon_DEX_10",
            "ItBe_Addon_Prot_EDGE",
            "ItBe_Addon_Prot_Point",
            "ItBe_Addon_Prot_MAGIC",
            "ItBe_Addon_Prot_FIRE",
            "ItBe_Addon_Prot_EdgPoi",
            "ItBe_Addon_Prot_TOTAL",
            "ItWr_SaturasFirstMessage_Addon_Sealed",
            "ItWr_SaturasFirstMessage_Addon",
            "ItMi_Ornament_Addon",
            "ItMi_Ornament_Addon_Vatras",
            "ItWr_Map_NewWorld_Ornaments_Addon",
            "ItWr_Map_NewWorld_Dexter",
            "ItRi_Ranger_Lares_Addon",
            "ItRi_Ranger_Addon",
            "ItRi_LanceRing",
            "ItMi_PortalRing_Addon",
            "ItWr_Martin_MilizEmpfehlung_Addon",
            "ItWr_RavensKidnapperMission_Addon",
            "ItWr_Vatras_KDFEmpfehlung_Addon",
            "ItMi_LostInnosStatue_Daron",
            "ItWr_LuciasLoveLetter_Addon",
            "ItMi_Rake",
            "ItRi_Addon_BanditTrader",
            "ItWr_Addon_BanditTrader",
            "ItWr_Vatras2Saturas_FindRaven",
            "ItWr_Vatras2Saturas_FindRaven_opened",
            "ItAm_Addon_WispDetector",
            "ItFo_Addon_Krokofleisch_Mission",
            "ItRi_Addon_MorgansRing_Mission",
            "ItMi_Focus",
            "ItMi_Addon_Steel_Paket",
            "ItWr_StonePlateCommon_Addon",
            "ItMi_Addon_Stone_01",
            "ItMi_Addon_Stone_05",
            "ItMi_Addon_Stone_03",
            "ItMi_Addon_Stone_04",
            "ItMi_Addon_Stone_02",
            "ItMI_Addon_Kompass_Mis",
            "ItSE_Addon_FrancisChest",
            "ITWR_Addon_FrancisAbrechnung_Mis",
            "ITWR_Addon_GregsLogbuch_Mis",
            "ITKE_Addon_Bloodwyn_01",
            "ITKE_Addon_Heiler",
            "ItMi_TempelTorKey",
            "ItMi_Addon_Bloodwyn_Kopf",
            "ItWR_Addon_TreasureMap",
            "ItMi_Addon_GregsTreasureBottle_MIS",
            "itmi_erolskelch",
            "ItMi_Addon_Shell_01",
            "ItMi_Addon_Shell_02",
            "ItFo_Addon_Shellflesh",
            "ItFo_Addon_Rum",
            "ItFo_Addon_Grog",
            "ItFo_Addon_LousHammer",
            "ItFo_Addon_SchlafHammer",
            "ItFo_Addon_SchnellerHering",
            "ItFo_Addon_Pfeffer_01",
            "ItFo_Addon_FireStew",
            "ItFo_Addon_Meatsoup",
            "ITAR_PIR_L_Addon",
            "ITAR_PIR_M_Addon",
            "ITAR_PIR_H_Addon",
            "ITAR_Thorus_Addon",
            "ITAR_Raven_Addon",
            "ITAR_OreBaron_Addon",
            "ITAR_RANGER_Addon",
            "ITAR_Fake_RANGER",
            "ITAR_KDW_L_Addon",
            "ITAR_Bloodwyn_Addon",
            "ITAR_MayaZombie_Addon",
            "ItAr_FireArmor_Addon",
            "ItRu_Thunderstorm",
            "ItRu_Whirlwind",
            "ItRu_Geyser",
            "ItRu_Waterfist",
            "ItRu_Icelance",
            "ItRu_BeliarsRage",
            "ItRu_SuckEnergy",
            "ItRu_GreenTentacle",
            "ItRu_Swarm",
            "ItRu_Skull",
            "ItRu_SummonZombie",
            "ItRu_SummonGuardian",
            "ITWR_Addon_Runemaking_KDW_CIRC1",
            "ITWR_Addon_Runemaking_KDW_CIRC2",
            "ITWR_Addon_Runemaking_KDW_CIRC3",
            "ITWR_Addon_Runemaking_KDW_CIRC4",
            "ITWR_Addon_Runemaking_KDW_CIRC5",
            "ITWR_Addon_Runemaking_KDW_CIRC6",
            "ITWR_Addon_Runemaking_KDF_CIRC1",
            "ITWR_Addon_Runemaking_KDF_CIRC2",
            "ITWR_Addon_Runemaking_KDF_CIRC3",
            "ITWR_Addon_Runemaking_KDF_CIRC4",
            "ITWR_Addon_Runemaking_KDF_CIRC5",
            "ITWR_Addon_Runemaking_KDF_CIRC6",
            "ITKE_PORTALTEMPELWALKTHROUGH_ADDON",
            "ITKE_Greg_ADDON_MIS",
            "ITKE_Addon_Tavern_01",
            "ITKE_Addon_Esteban_01",
            "ITKE_ORLAN_TELEPORTSTATION",
            "ITKE_CANYONLIBRARY_HIERARCHY_BOOKS_ADDON",
            "ITKE_ADDON_BUDDLER_01",
            "ITKE_ADDON_SKINNER",
            "ITKE_Addon_Thorus",
            "ItSc_Geyser",
            "ItSc_Icelance",
            "ItSc_Waterfist",
            "ItSc_Thunderstorm",
            "ItSc_Whirlwind",
            "ITWr_Addon_Hinweis_02",
            "ITWr_Addon_Health_04",
            "ITWr_Addon_Mana_04",
            "ITWr_Addon_Hinweis_01",
            "ITWr_Addon_William_01",
            "ITWr_Addon_MCELIXIER_01",
            "ITWr_Addon_Pirates_01",
            "ITWr_Addon_Joint_01",
            "ITWr_Addon_Lou_Rezept",
            "ITWr_Addon_Lou_Rezept2",
            "ITWr_Addon_Piratentod",
            "Fakescroll_Addon",
            "ItWr_Addon_AxtAnleitung",
            "ItWr_Addon_SUMMONANCIENTGHOST",
            "ItWr_Map_AddonWorld",
            "ItMi_GoldNugget_Addon",
            "ItMi_Addon_WhitePearl",
            "ItMi_Addon_Joint_01",
            "ItMi_BaltramPaket",
            "ItMi_Packet_Baltram4Skip_Addon",
            "ItMi_BromorsGeld_Addon",
            "ItSe_ADDON_CavalornsBeutel",
            "ItMi_Skull",
            "ItMi_IECello",
            "ItMi_IECelloBow",
            "ItMi_IEDrum",
            "ItMi_IEDrumScheit",
            "ItMi_IEDrumStick",
            "ItMi_IEDudelBlau",
            "ItMi_IEDudelGelb",
            "ItMi_IEHarfe",
            "ItMi_IELaute",
            "ItMi_Addon_Lennar_Paket",
            "ItMi_Zeitspalt_Addon",
            "ItWr_StrStonePlate1_Addon",
            "ItWr_StrStonePlate2_Addon",
            "ItWr_StrStonePlate3_Addon",
            "ItWr_DexStonePlate1_Addon",
            "ItWr_DexStonePlate2_Addon",
            "ItWr_DexStonePlate3_Addon",
            "ItWr_HitPointStonePlate1_Addon",
            "ItWr_HitPointStonePlate2_Addon",
            "ItWr_HitPointStonePlate3_Addon",
            "ItWr_ManaStonePlate1_Addon",
            "ItWr_ManaStonePlate2_Addon",
            "ItWr_ManaStonePlate3_Addon",
            "ItWr_OneHStonePlate1_Addon",
            "ItWr_OneHStonePlate2_Addon",
            "ItWr_OneHStonePlate3_Addon",
            "ItWr_TwoHStonePlate1_Addon",
            "ItWr_TwoHStonePlate2_Addon",
            "ItWr_TwoHStonePlate3_Addon",
            "ItWr_BowStonePlate1_Addon",
            "ItWr_BowStonePlate2_Addon",
            "ItWr_BowStonePlate3_Addon",
            "ItWr_CrsBowStonePlate1_Addon",
            "ItWr_CrsBowStonePlate2_Addon",
            "ItWr_CrsBowStonePlate3_Addon",
            "ItPo_Addon_Geist_01",
            "ItPo_Addon_Geist_02",
            "ItPo_Health_Addon_04",
            "ItPo_Mana_Addon_04",
            "ItMW_Addon_Knife01",
            "ItMW_Addon_Stab01",
            "ItMW_Addon_Stab02",
            "ItMW_Addon_Stab03",
            "ItMW_Addon_Stab04",
            "ItMW_Addon_Stab05",
            "ItMW_Addon_Hacker_1h_01",
            "ItMW_Addon_Hacker_1h_02",
            "ItMW_Addon_Hacker_2h_01",
            "ItMW_Addon_Hacker_2h_02",
            "ItMW_Addon_Keule_1h_01",
            "ItMW_Addon_Keule_2h_01",
            "ItMw_FrancisDagger_Mis",
            "ItMw_RangerStaff_Addon",
            "ItMw_Addon_PIR2hAxe",
            "ItMw_Addon_PIR2hSword",
            "ItMw_Addon_PIR1hAxe",
            "ItMw_Addon_PIR1hSword",
            "ItMw_Addon_BanditTrader",
            "ItMw_Addon_Betty",
            "ItRw_Addon_MagicArrow",
            "ItRw_Addon_FireArrow",
            "ItRw_Addon_MagicBow",
            "ItRw_Addon_FireBow",
            "ItRw_Addon_MagicBolt",
            "ItRw_Addon_MagicCrossbow"
        };

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
            /*
            auto &console = m_Engine.getConsole();
            for(size_t i = 0; i < g2Items.size(); ++i)
            {
                std::string command = std::string("giveitem ") + Utils::toUpper(g2Items[i]);
                console.submitCommand(command);
            }
            */
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
            m_Engine.getConsole().submitCommand("giveitem ITMI_GOLD 42");
        }
    }

    World::WorldInstance &world = m_Engine.getMainWorld().get();
    Logic::PlayerController* playerController = reinterpret_cast<Logic::PlayerController*>(
                world.getEntity<Components::LogicComponent>(world.getScriptEngine().getPlayerEntity()).m_pLogicController);

    const std::list<Daedalus::GameState::ItemHandle> &itemHandleList = playerController->getInventory().getItems();
    const std::set<Daedalus::GameState::ItemHandle> &equippedItems = playerController->getEquippedItems();
    std::vector<Daedalus::GEngineClasses::C_Item> itemList(itemHandleList.size());
    std::vector<bool> isItemEquipped(itemList.size());
    std::list<Daedalus::GameState::ItemHandle>::const_iterator itItem = itemHandleList.begin();
    size_t goldAmount = 0; // The amount of gold the player has
    std::string goldName; // The localized name of gold
    for(uint32_t index = 0; index < itemList.size(); ++index)
    {
        itemList[index] = world.getScriptEngine().getGameState().getItem(*itItem);
        if(itemList[index].instanceSymbol == 6929)
        {
            goldAmount = itemList[index].amount;
            goldName = itemList[index].name;
        }
        Daedalus::GEngineClasses::C_Item &debugItem = itemList[index];
        isItemEquipped[index] = equippedItems.find(*itItem) != equippedItems.end();
        ++itItem;
    }

    // Provide the render function a list of indices into the itemList array
    std::vector<int> indices(itemList.size());
    std::iota(indices.begin(), indices.end(), 0);

    // Sort the indices
    const auto &constItemList = itemList;
    std::sort(indices.begin(), indices.end(), [this, &constItemList](const int &a, const int &b)
    {
        const Daedalus::GEngineClasses::C_Item &l = constItemList[a], &r = constItemList[b];
        return compareItems(l, r);
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

    m_LastTimeSelectedItemSlot = selectedSlot;
    Math::float2 inventoryPosition = Math::float2(0.7f * config.state.viewWidth, 100);
    Math::float2 inventorySize = Math::float2(0.28f * config.state.viewWidth, config.state.viewHeight - 100);

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
    if(itemList.size() > 0)
        m_SelectedItemID = itemList[indices[itemIndex]].instanceSymbol;
    else
        m_SelectedItemID = -1;
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
    std::vector<ItemDrawState> itemsToDraw(numVisibleItems);
    for(int i = 0; i < numVisibleItems; ++i)
    {
        itemsToDraw[i].index = indices[i + offsetSlots];
        itemsToDraw[i].flags = 0;
        if( isItemEquipped[itemsToDraw[i].index] )
            itemsToDraw[i].flags |= ItemDrawFlags::Equipped;
    }
    if(itemsToDraw.size() > 0)
        itemsToDraw[itemIndex - offsetSlots].flags |= ItemDrawFlags::Selected;

    drawItemGrid(config, inventoryPosition, inventorySize, EAlign::A_TopRight,
                 slotSize, itemList, itemsToDraw);

    drawText(goldName + ": " + std::to_string(goldAmount), inventoryPosition.x + inventorySize.x - slotSize, inventoryPosition.y - 15,
             EAlign::A_BottomCenter, config);
    Textures::Texture &texInvBack = m_Engine.getEngineTextureAlloc().getTexture(m_TexInvBack);
    Textures::Texture &texSlot = m_Engine.getEngineTextureAlloc().getTexture(m_TexSlot);
    drawTexture(RenderViewList::INVENTORY_BG, inventoryPosition.x + inventorySize.x - 2 * slotSize,
                inventoryPosition.y - 40, 2 * slotSize, 35,
                config.state.viewWidth, config.state.viewHeight, texInvBack.m_TextureHandle,
                config.programs.imageProgram, config.uniforms.diffuseTexture);
    drawTexture(RenderViewList::INVENTORY_BG, inventoryPosition.x + inventorySize.x - 2 * slotSize,
                inventoryPosition.y - 40, 2 * slotSize, 35,
                config.state.viewWidth, config.state.viewHeight, texSlot.m_TextureHandle,
                config.programs.imageProgram, config.uniforms.diffuseTexture);

    if(indices.size() > 0)
        drawItemInfobox(config, itemList[indices[itemIndex]],
                Math::float2(0.5f * config.state.viewWidth - 300, config.state.viewHeight - 140 - 10),
                Math::float2(600, 140));

    View::update(dt, mstate, config);
}

int32_t UI::InventoryView::getPrimarySortValue(const Daedalus::GEngineClasses::C_Item &item)
{
    switch (item.mainflag)
    {
    case Daedalus::GEngineClasses::C_Item::ITM_CAT_NF:
        return 1;
    case Daedalus::GEngineClasses::C_Item::ITM_CAT_FF:
        return 2;
    case Daedalus::GEngineClasses::C_Item::ITM_CAT_MUN:
        return 3;
    case Daedalus::GEngineClasses::C_Item::ITM_CAT_ARMOR:
        return 6;
    case Daedalus::GEngineClasses::C_Item::ITM_CAT_FOOD:
        return 5;
    case Daedalus::GEngineClasses::C_Item::ITM_CAT_DOCS:
        return 9;
    case Daedalus::GEngineClasses::C_Item::ITM_CAT_POTION:
        return 4;
    case Daedalus::GEngineClasses::C_Item::ITM_CAT_RUNE:
        return 8;
    case Daedalus::GEngineClasses::C_Item::ITM_CAT_MAGIC:
        return 7;
    case Daedalus::GEngineClasses::C_Item::ITM_CAT_NONE:
    default:
        return 10;
    }
}

bool UI::InventoryView::compareItems(const Daedalus::GEngineClasses::C_Item &l, const Daedalus::GEngineClasses::C_Item &r)
{
    if(l.mainflag != r.mainflag)
        return getPrimarySortValue(l) < getPrimarySortValue(r);

    switch(l.mainflag) // is the same as r.mainflag at this point
    {
    case Daedalus::GEngineClasses::C_Item::ITM_CAT_NF:
    case Daedalus::GEngineClasses::C_Item::ITM_CAT_FF:
    case Daedalus::GEngineClasses::C_Item::ITM_CAT_MUN:
        if(l.damageTotal != r.damageTotal)
            return l.damageTotal > r.damageTotal;
        break;
    case Daedalus::GEngineClasses::C_Item::ITM_CAT_ARMOR:
        if(l.wear != r.wear)
            return l.wear < r.wear;
        else
        {
            int32_t lSumProtection = 0, rSumProtection = 0;
            for(size_t i = 0; i < Daedalus::GEngineClasses::PROT_INDEX_MAX; ++i)
            {
                lSumProtection += l.protection[i];
                rSumProtection += r.protection[i];
            }
            if(lSumProtection != rSumProtection)
                return lSumProtection > rSumProtection;
        }
        break;
    case Daedalus::GEngineClasses::C_Item::ITM_CAT_FOOD:
        // The last one is gold value
        /*
        for(size_t i = 0; i < Daedalus::GEngineClasses::ITM_TEXT_MAX - 1; ++i)
        {
            if(l.count[i] != r.count[i])
                return l.count[i] > r.count[i];
        }
        */
        break;
    case Daedalus::GEngineClasses::C_Item::ITM_CAT_MAGIC:
        if(l.flags != r.flags)
        {
            if(l.flags & Daedalus::GEngineClasses::C_Item::ITEM_BELT)
                return false;
            else if(r.flags & Daedalus::GEngineClasses::C_Item::ITEM_BELT)
                return true;
            else
                l.flags > r.flags;
        }
        break;
    }

    const std::string &lInstanceName = m_Engine.getMainWorld().get().getScriptEngine().getVM().getDATFile().getSymbolByIndex(l.instanceSymbol).name;
    const std::string &rInstanceName = m_Engine.getMainWorld().get().getScriptEngine().getVM().getDATFile().getSymbolByIndex(r.instanceSymbol).name;
    return lInstanceName < rInstanceName;
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
        m_InputDrop.getAction().setEnabled(true);
    }
    else
    {
        m_InputUp.getAction().setEnabled(false);
        m_InputDown.getAction().setEnabled(false);
        m_InputLeft.getAction().setEnabled(false);
        m_InputRight.getAction().setEnabled(false);
        m_InputUse.getAction().setEnabled(false);
        m_InputDrop.getAction().setEnabled(false);
    }
}
