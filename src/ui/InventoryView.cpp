#include "InventoryView.h"
#include <engine/BaseEngine.h>
#include <logic/Inventory.h>
#include <logic/PlayerController.h>
#include <logic/visuals/StaticMeshVisual.h>
#include <logic/visuals/ModelVisual.h>
#include <logic/ScriptEngine.h>
#include <logic/ItemController.h>
#include <components/Vob.h>
#include <components/VobClasses.h>
#include <content/SkeletalMeshAllocator.h>


#include <bitset>

const float FARPLANE = 1000.0f;

UI::InventoryView::InventoryView(Engine::BaseEngine& e)
    : View(e)
{
    m_State = State::Disabled;

    m_SelectedItemSymbol = -1;
    m_SelectedItemRotation = 0.0f;

    Textures::TextureAllocator& texAlloc = m_Engine.getEngineTextureAlloc();
    m_TexSlot = texAlloc.loadTextureVDF("INV_SLOT.TGA");
    m_TexSlotHighlighted = texAlloc.loadTextureVDF("INV_SLOT_HIGHLIGHTED.TGA");
    m_TexSlotEquipped = texAlloc.loadTextureVDF("INV_SLOT_EQUIPPED.TGA");
    // this texture depends on the game and gets loaded when we know whether g1
    // or g2 is being played
    m_TexInvBack.invalidate();

    m_ThisCursorState.reset();
    m_OtherCursorState.reset();

    //m_SelectedSlot = 0;
    m_DeltaRows = 0;
    m_DeltaColumns = 0;

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
                if(itemData.instanceSymbol == m_ThisCursorState.selectedItemSymbol)
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

            // VobTypes::NpcVobInformation vob = VobTypes::asNpcVob(m_World, e);

            for(auto &itItem : itemHandleList)
            {
                Daedalus::GEngineClasses::C_Item itemData = world.getScriptEngine().getGameState().getItem(itItem);
                if(itemData.instanceSymbol == m_ThisCursorState.selectedItemSymbol)
                {
                    const auto &equippedItems = playerController->getEquippedItems();
                    if( equippedItems.find(itItem) != equippedItems.end() )
                        playerController->unequipItem(itItem);
                    else
                    {
                        if(playerController->useItem(itItem))
                            playerInv.removeItem(itItem);
                    }
                    // If the item was removed we have to stop the iteration otherwise we will crash
                    break;
                    /*
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
                    */
                }
            }
        }
    });
    m_InputUse.getAction().setEnabled(false);
}

UI::InventoryView::~InventoryView() = default;

void UI::InventoryView::setState(State state, Handle::EntityHandle other)
{
    m_State = state;
    m_Other = other;
    m_IsHidden = m_State == State::Disabled;
    if(!m_IsHidden)
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
        if(C_Item::Flags::ITEM_BELT & itemData.flags)
            centerPos.y -= 0.02f;
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
            // Use REGoth custom rotation instead
            // transform = Matrix::CreateRotationX(degreeToRadians(360)) * transform;
            transform = Matrix::CreateRotationY(degreeToRadians(180)) * transform;
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
                                 Math::float2 position, float size, bool selected, float rotationY)
{
    World::WorldInstance &world = m_Engine.getMainWorld().get();
    bool isArmor = 0 != (itemData.mainflag & Daedalus::GEngineClasses::C_Item::ITM_CAT_ARMOR);

    if(!isArmor) // Ordinary items
    {
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
        // Apply additional rotation for nice effect
        transform = Math::Matrix::CreateRotationY(rotationY) * transform;
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
            bgfx::setState(BGFX_STATE_DEFAULT);

            // I don't know what this uniform does but just passing white seems to work
            Math::float4 color;
            color.fromRGBA8(0xFFFFFFFF);
            bgfx::setUniform(config.uniforms.objectColor, color.v);

            bgfx::setTexture(0, config.uniforms.diffuseTexture, texture.m_TextureHandle, BGFX_TEXTURE_MIN_ANISOTROPIC | BGFX_TEXTURE_MAG_ANISOTROPIC);

            // TODO changing fog uniform. should be fixed in shader
            Math::float4 fogNearFar = Math::float4(FLT_MAX, FLT_MAX, 0, 0);
            bgfx::setUniform(config.uniforms.fogNearFar, fogNearFar.v);

            bgfx::setTransform(transform.m);
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

            bgfx::submit(RenderViewList::INVENTORY, config.programs.mainWorldProgram);
        }
    }
    else // armor
    {
        VobTypes::NpcVobInformation playerVobInfo = VobTypes::asNpcVob(world, world.getScriptEngine().getPlayerEntity());
        Logic::PlayerController *playerController = playerVobInfo.playerController;
        Logic::ModelVisual *playerModelVisual = playerController->getModelVisual();
        const Logic::ModelVisual::BodyState &bodyState = playerModelVisual->getBodyState();
        //drawText(bodyState.bodyVisual, 250, 100, EAlign::A_TopLeft, config);
        //drawText(bodyState.headVisual, 250, 120, EAlign::A_TopLeft, config);

        std::string mdmVisual = itemData.visual_change.substr(0, itemData.visual_change.size() - 4) + ".MDM";
        Handle::MeshHandle itemMeshHandleSkeletal = world.getSkeletalMeshAllocator().loadMeshVDF(Utils::toUpper(mdmVisual));
        Meshes::WorldSkeletalMesh& itemMeshSkeletal = world.getSkeletalMeshAllocator().getMesh(itemMeshHandleSkeletal);

        // Hard coded values for armor. This works because all humans (and all armor) have essentially the same size
        Math::float3 itemCenter = Math::float3(0, -0.05f, 0);
        const float itemSize = 2.8;

        // Scale required for the bounding box to exactly fit into the slot
        float scale = size / itemSize;
        // Translation to the slot center
        Math::float3 translation(position.x + size / 2, position.y + size / 2, FARPLANE / 2);

        // Apply rotations for gothic 2 inventory
        Math::Matrix transform = applyRotationsAndScale(itemCenter, itemData, selected);
        transform = Math::Matrix::CreateRotationY(rotationY) * transform;
        // Apply bounding box scale. Bias in x-Direction for armor because otherwise it is harder to see
        transform = Math::Matrix::CreateScale(Math::float3(1.4f * scale, scale, scale)) * transform;
        // Apply slot translation
        transform = Math::Matrix::CreateTranslation(translation) * transform;

        Components::AnimHandler animHandler;
        animHandler.setWorld(world);
        animHandler.loadMeshLibFromVDF("HUMANS", m_Engine.getVDFSIndex());
        animHandler.playAnimation(Logic::ModelVisual::getAnimationName(Logic::ModelVisual::EModelAnimType::Idle));

        Math::Matrix nodeMat[ZenLoad::MAX_NUM_SKELETAL_NODES + 1];
        animHandler.updateSkeletalMeshInfo(nodeMat + 1, ZenLoad::MAX_NUM_SKELETAL_NODES);
        nodeMat[0] = transform;

        for( size_t subMesh = 0; subMesh < itemMeshSkeletal.m_SubmeshMaterialNames.size(); ++subMesh)
        {
            std::string materialName = itemMeshSkeletal.m_SubmeshMaterialNames[subMesh];
            if(materialName.find("_BODY") != std::string::npos)
            {
                if(bodyState.bodyTextureIdx != 0)
                    Utils::replace(materialName, "_V0", "_V" + std::to_string(bodyState.bodyTextureIdx));
                if(bodyState.bodySkinColorIdx != 0)
                    Utils::replace(materialName, "_C0", "_C" + std::to_string(bodyState.bodySkinColorIdx));
            }
            Handle::TextureHandle textureHandle = world.getTextureAllocator().loadTextureVDF(materialName);
            Textures::Texture &texture = world.getTextureAllocator().getTexture(textureHandle);

            // Following render code is essentially copy & paste from render/WorldRender.cpp
            bgfx::setState(BGFX_STATE_DEFAULT);

            // I don't know what this uniform does but just passing white seems to work
            Math::float4 color;
            color.fromRGBA8(0xFFFFFFFF);
            bgfx::setUniform(config.uniforms.objectColor, color.v);

            bgfx::setTexture(0, config.uniforms.diffuseTexture, texture.m_TextureHandle, BGFX_TEXTURE_MIN_ANISOTROPIC | BGFX_TEXTURE_MAG_ANISOTROPIC);

            // TODO changing fog uniform. should be fixed in shader
            Math::float4 fogNearFar = Math::float4(FLT_MAX, FLT_MAX, 0, 0);
            bgfx::setUniform(config.uniforms.fogNearFar, fogNearFar.v);

            bgfx::setTransform(nodeMat, static_cast<uint16_t>(animHandler.getNumNodes() + 1));

            bgfx::setVertexBuffer(0, itemMeshSkeletal.m_VertexBufferHandle);
            bgfx::setIndexBuffer(itemMeshSkeletal.m_IndexBufferHandle,
                                 itemMeshSkeletal.m_SubmeshStarts[subMesh].m_StartIndex,
                                 itemMeshSkeletal.m_SubmeshStarts[subMesh].m_NumIndices);

            bgfx::submit(RenderViewList::INVENTORY, config.programs.mainSkinnedMeshProgram);
        }

        // Head rendering

        Handle::MeshHandle headMeshHandle = world.getStaticMeshAllocator().loadMeshVDF(Utils::toUpper(bodyState.headVisual));
        Meshes::WorldStaticMesh &headMesh = world.getStaticMeshAllocator().getMesh(headMeshHandle);
        // I do not know why the helmet node is required rather than the HEAD node but it works.
        Math::Matrix headTransform = transform * nodeMat[playerModelVisual->findNodeIndex("ZS_HELMET")];

        // Get head-texture of current character
        // TODO also correct mouth and teeth textures
        std::vector<std::string> headTxParts = Utils::split(headMesh.mesh.m_SubmeshMaterialNames[0], "0123456789");
        std::string correctedHeadTexture;

        // TODO ugly code, improve it!
        if(headTxParts.size() == 3)
            correctedHeadTexture = headTxParts[0] + std::to_string(bodyState.headTextureIdx) + headTxParts[1] + std::to_string(bodyState.bodySkinColorIdx) + headTxParts[2];
        else if(headTxParts.size() == 2)
            correctedHeadTexture = headTxParts[0] + std::to_string(bodyState.headTextureIdx) + headTxParts[1];
        else if(headTxParts.size() == 1)
            correctedHeadTexture = headTxParts[0];
        else // There is no meaningful head to render
            return;

        for( size_t subMesh = 0; subMesh < headMesh.mesh.m_SubmeshMaterialNames.size(); ++subMesh)
        {
            const std::string &materialName = (subMesh == 0 ? correctedHeadTexture : headMesh.mesh.m_SubmeshMaterialNames[subMesh]);
            Handle::TextureHandle textureHandle = world.getTextureAllocator().loadTextureVDF(materialName);
            Textures::Texture &texture = world.getTextureAllocator().getTexture(textureHandle);

            // Following render code is essentially copy & paste from render/WorldRender.cpp
            bgfx::setState(BGFX_STATE_DEFAULT);

            // I don't know what this uniform does but just passing white seems to work
            Math::float4 color;
            color.fromRGBA8(0xFFFFFFFF);
            bgfx::setUniform(config.uniforms.objectColor, color.v);

            bgfx::setTexture(0, config.uniforms.diffuseTexture, texture.m_TextureHandle, BGFX_TEXTURE_MIN_ANISOTROPIC | BGFX_TEXTURE_MAG_ANISOTROPIC);

            // TODO changing fog uniform. should be fixed in shader
            Math::float4 fogNearFar = Math::float4(FLT_MAX, FLT_MAX, 0, 0);
            bgfx::setUniform(config.uniforms.fogNearFar, fogNearFar.v);

            bgfx::setTransform(headTransform.m);
            if (headMesh.mesh.m_IndexBufferHandle.idx != bgfx::kInvalidHandle)
            {
                bgfx::setVertexBuffer(0, headMesh.mesh.m_VertexBufferHandle);
                bgfx::setIndexBuffer(headMesh.mesh.m_IndexBufferHandle,
                                     headMesh.mesh.m_SubmeshStarts[subMesh].m_StartIndex,
                                     headMesh.mesh.m_SubmeshStarts[subMesh].m_NumIndices);
            }
            else
            {
                bgfx::setVertexBuffer(0, headMesh.mesh.m_VertexBufferHandle,
                                      headMesh.mesh.m_SubmeshStarts[subMesh].m_StartIndex,
                                      headMesh.mesh.m_SubmeshStarts[subMesh].m_NumIndices);
            }

            bgfx::submit(RenderViewList::INVENTORY, config.programs.mainWorldProgram);
        }
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

    if(m_Engine.getBasicGameType() == Daedalus::GameType::GT_Gothic2)
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

        drawItem(config, itemData, Math::float2(xPos, yPos), slotSize, isSelected, 0.0f);
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
    drawItem(config, item, Math::float2(position.x + size.x - 200.0f, position.y + (size.y - itemSize) / 2), itemSize, false, m_SelectedItemRotation);

    /*
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
    */
    /*
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
    */

    //drawText(std::bitset<32>(item.mainflag).to_string(), 200, 200, EAlign::A_TopLeft, config);
    //drawText(std::bitset<32>(item.flags).to_string(), 200, 250, EAlign::A_TopLeft, config);
}

void UI::InventoryView::update(double dt, Engine::Input::MouseState &mstate, Render::RenderConfig &config)
{
    if(m_IsHidden)
        return;

    /*
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
            m_Engine.getConsole().submitCommand("giveitem GRD_ARMOR_L");
        }
        else if( Daedalus::GameType::GT_Gothic2 == m_Engine.getBasicGameType())
        {
            auto &console = m_Engine.getConsole();
            for(size_t i = 0; i < g2Items.size(); ++i)
            {
                const std::string startsWith = "ITAR";
                std::string itemName = Utils::toUpper(g2Items[i]);
                if(true || itemName.compare(0, startsWith.length(), startsWith) == 0)
                {
                    std::string command = std::string("giveitem ") + itemName;
                    console.submitCommand(command);
                }
            }


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
    */

    World::WorldInstance &world = m_Engine.getMainWorld().get();
    VobTypes::NpcVobInformation playerVobInfo = VobTypes::asNpcVob(world, world.getScriptEngine().getPlayerEntity());
    Logic::PlayerController *playerController = playerVobInfo.playerController;
    const std::list<Daedalus::GameState::ItemHandle> &itemHandleList = playerController->getInventory().getItems();
    const std::set<Daedalus::GameState::ItemHandle> &equippedItems = playerController->getEquippedItems();
    std::vector<Daedalus::GEngineClasses::C_Item> itemList(itemHandleList.size());
    std::vector<bool> isItemEquipped(itemList.size());
    size_t goldAmount = 0; // The amount of gold (ore) the player has
    std::string goldName; // The localized name of gold (ore)
    std::list<Daedalus::GameState::ItemHandle>::const_iterator itItem = itemHandleList.begin();
    for(uint32_t index = 0; index < itemList.size(); ++index)
    {
        //TODO copying the item object is not necessary
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
    std::sort(indices.begin(), indices.end(), [this, &itemList](const int &a, const int &b)
    {
        return compareItems(itemList[a], itemList[b]);
    });

    const float slotSize = 100.0f;
    Math::float2 inventoryPosition = Math::float2(0.7f * config.state.viewWidth, 100);
    Math::float2 inventorySize = Math::float2(0.28f * config.state.viewWidth, config.state.viewHeight - 150);
    Math::float2 otherInventoryPosition = Math::float2(0.02f * config.state.viewWidth, 100);
    Math::float2 otherInventorySize = inventorySize;

    calculateCursorState(m_ThisCursorState, slotSize, inventorySize, itemList, indices);

    if(m_ThisCursorState.selectedItemSymbol == m_SelectedItemSymbol)
        m_SelectedItemRotation += dt;
    else
    {
        m_SelectedItemRotation = 0.0f;
        m_SelectedItemSymbol = m_ThisCursorState.selectedItemSymbol;
    }

    if(m_State == State::Loot && m_Other.isValid())
    {
        VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(world, m_Other);
        Logic::PlayerController *npcController = npc.playerController;

        const std::list<Daedalus::GameState::ItemHandle> &otherItemHandleList = npcController->getInventory().getItems();
        std::vector<Daedalus::GEngineClasses::C_Item> otherItemList(otherItemHandleList.size());
        std::list<Daedalus::GameState::ItemHandle>::const_iterator itOtherItem = otherItemHandleList.begin();
        for(size_t index = 0; index < otherItemList.size(); ++index)
        {
            otherItemList[index] = world.getScriptEngine().getGameState().getItem(*itOtherItem);
            ++itOtherItem;
        }

        std::vector<int> otherIndices(otherItemList.size());
        std::iota(otherIndices.begin(), otherIndices.end(), 0);

        std::sort(otherIndices.begin(), otherIndices.end(), [this, &otherItemList](const int &a, const int &b)
        {
            return compareItems(otherItemList[a], otherItemList[b]);
        });

        calculateCursorState(m_OtherCursorState, slotSize, otherInventorySize, otherItemList, otherIndices);

        std::vector<ItemDrawState> otherItemsToDraw(m_OtherCursorState.numVisibleItems);
        for(int i = 0; i < otherItemsToDraw.size(); ++i)
        {
            otherItemsToDraw[i].index = otherIndices[i + m_OtherCursorState.offsetItems];
            otherItemsToDraw[i].flags = 0;
        }
        if(otherItemsToDraw.size() > 0)
            otherItemsToDraw[m_OtherCursorState.itemIndex - m_OtherCursorState.offsetItems].flags |= ItemDrawFlags::Selected;

        drawItemGrid(config, otherInventoryPosition, otherInventorySize, EAlign::A_TopLeft,
                     slotSize, otherItemList, otherItemsToDraw);
    }

    // only display visible items
    std::vector<ItemDrawState> itemsToDraw(m_ThisCursorState.numVisibleItems);
    for(int i = 0; i < itemsToDraw.size(); ++i)
    {
        itemsToDraw[i].index = indices[i + m_ThisCursorState.offsetItems];
        itemsToDraw[i].flags = 0;
        if( isItemEquipped[itemsToDraw[i].index] )
            itemsToDraw[i].flags |= ItemDrawFlags::Equipped;
    }
    if(itemsToDraw.size() > 0)
        itemsToDraw[m_ThisCursorState.itemIndex - m_ThisCursorState.offsetItems].flags |= ItemDrawFlags::Selected;

    drawItemGrid(config, inventoryPosition, inventorySize, EAlign::A_TopRight,
                 slotSize, itemList, itemsToDraw);

    drawText(goldName + ": " + std::to_string(goldAmount), inventoryPosition.x + inventorySize.x - slotSize, inventoryPosition.y - 15,
             EAlign::A_BottomCenter, config);
    Textures::Texture &texSlot = m_Engine.getEngineTextureAlloc().getTexture(m_TexSlot);

    if(m_Engine.getBasicGameType() == Daedalus::GameType::GT_Gothic2)
    {
        Textures::Texture &texInvBack = m_Engine.getEngineTextureAlloc().getTexture(m_TexInvBack);
        drawTexture(RenderViewList::INVENTORY_BG, inventoryPosition.x + inventorySize.x - 2 * slotSize,
                    inventoryPosition.y - 40, 2 * slotSize, 35,
                    config.state.viewWidth, config.state.viewHeight, texInvBack.m_TextureHandle,
                    config.programs.imageProgram, config.uniforms.diffuseTexture);
    }
    drawTexture(RenderViewList::INVENTORY_BG, inventoryPosition.x + inventorySize.x - 2 * slotSize,
                inventoryPosition.y - 40, 2 * slotSize, 35,
                config.state.viewWidth, config.state.viewHeight, texSlot.m_TextureHandle,
                config.programs.imageProgram, config.uniforms.diffuseTexture);

    if(indices.size() > 0)
        drawItemInfobox(config, itemList[indices[m_ThisCursorState.itemIndex]],
                Math::float2(0.5f * config.state.viewWidth - 300, config.state.viewHeight - 140 - 10),
                Math::float2(600, 140));

    View::update(dt, mstate, config);
}

void UI::InventoryView::calculateCursorState(CursorState &cursorState, float slotSize, Math::float2 inventorySize,
                                             const std::vector<Daedalus::GEngineClasses::C_Item> &itemList, const std::vector<int> &indices)
{
    int selectedSlot = -1;
    // Search the sorted list for the selected item
    for( int index = 0; index < indices.size(); ++index)
    {
        if(itemList[indices[index]].instanceSymbol == cursorState.selectedItemSymbol)
        {
            selectedSlot = index;
            break;
        }
    }
    if(selectedSlot < 0)
        selectedSlot = std::max(0, std::min(cursorState.lastTimeSelectedItemSlot, int(indices.size())));

    cursorState.lastTimeSelectedItemSlot = selectedSlot;

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
    cursorState.itemIndex = std::max(0, std::min(currentRow * numColumns + currentColumn + m_DeltaColumns, int(itemList.size()) - 1));
    if(itemList.size() > 0)
        cursorState.selectedItemSymbol = itemList[indices[cursorState.itemIndex]].instanceSymbol;
    else
        cursorState.selectedItemSymbol = -1;
    // Reset for next input
    m_DeltaRows = 0;
    m_DeltaColumns = 0;

    int itemRow = cursorState.itemIndex / numColumns;
    // The item is above the rows we are currently showing
    if (itemRow < cursorState.displayedRowsStart)
        cursorState.displayedRowsStart = itemRow;
    // The item is below the rows we are currently showing
    if (itemRow >= cursorState.displayedRowsStart + numRows)
        cursorState.displayedRowsStart = itemRow - numRows + 1;
    // If there is enough space anyway, simply display all items
    if (itemList.size() <= numSlots)
        cursorState.displayedRowsStart = 0;

    cursorState.offsetItems = cursorState.displayedRowsStart * numColumns;
    cursorState.numVisibleItems = std::min(numSlots, int(itemList.size() - cursorState.offsetItems));
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
                return l.flags > r.flags;
        }
    case Daedalus::GEngineClasses::C_Item::ITM_CAT_NONE:
        bool lMI = (l.flags & Daedalus::GEngineClasses::C_Item::ITEM_MISSION) != 0;
        bool rMI = (r.flags & Daedalus::GEngineClasses::C_Item::ITEM_MISSION) != 0;
        if(lMI != rMI)
            return lMI > rMI;
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
