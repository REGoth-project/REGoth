//
// Created by andre on 28.06.18.
//

#include "CharacterEquipment.h"
#include <engine/World.h>
#include <logic/PlayerController.h>
#include <logic/ScriptEngine.h>

using namespace Logic;

CharacterEquipment::CharacterEquipment(World::WorldInstance& world, Handle::EntityHandle characterEntity)
    : m_CharacterEntity(characterEntity)
    , m_World(world)
{
}

bool CharacterEquipment::equipItemToSlot(ItemHandle item, Slot slot)
{
    if (!isItemTypeCorrectForSlot(item, slot))
        return false;

    switch (getKindOfItem(item))
    {
        case Kind::MELEE:
            return equipMelee(item);

        case Kind::BOW:
            return equipBow(item);

        case Kind::AMULET:
            return equipAmulet(item);

        case Kind::RING:
            return equipRing(item, slot);

        case Kind::MAGIC:
            return equipMagic(item, slot);

        case Kind::BELT:
            return equipBelt(item);

        case Kind::OTHER:
        default:
            return false;
    }

    return true;
}

bool Logic::CharacterEquipment::equipMelee(ItemHandle item)
{
    setItemInSlot(item, Slot::MELEE);
    return true;
}

bool Logic::CharacterEquipment::equipBow(ItemHandle item)
{
    setItemInSlot(item, Slot::BOW);
    return true;
}

bool Logic::CharacterEquipment::equipAmulet(ItemHandle item)
{
    setItemInSlot(item, Slot::AMULET);
    return true;
}

bool Logic::CharacterEquipment::equipRing(ItemHandle item, Slot slot)
{
    setItemInSlot(item, slot);
    return true;
}

bool Logic::CharacterEquipment::equipMagic(ItemHandle item, Slot slot)
{
    setItemInSlot(item, slot);
    return true;
}

bool Logic::CharacterEquipment::equipBelt(ItemHandle item)
{
    setItemInSlot(item, Slot::BELT);
    return true;
}

tl::optional<CharacterEquipment::Slot> CharacterEquipment::getCorrectSlotForItem(ItemHandle item) const
{
    switch (getKindOfItem(item))
    {
        case Kind::MELEE:
            return Slot::MELEE;
        case Kind::BOW:
            return Slot::BOW;
        case Kind::AMULET:
            return Slot::AMULET;
        case Kind::RING:
            return Slot::RING_LEFT;
        case Kind::MAGIC:
            return Slot::MAGIC_0;
        case Kind::BELT:
            return Slot::BELT;
        case Kind::OTHER:
        default:
            return tl::nullopt;
    }
}

CharacterEquipment::Kind CharacterEquipment::getKindOfItem(ItemHandle item) const
{
    using Daedalus::GEngineClasses::C_Item;
    auto itemDataOpt = getDataOfItem(item);

    if (!itemDataOpt)
        return Kind::OTHER;

    auto& itemData = *itemDataOpt;

    // Magic can only be identified through the mainflags
    if ((itemData.mainflag & C_Item::ITM_CAT_RUNE) != 0)
        return Kind::MAGIC;

    if ((itemData.mainflag & C_Item::ITM_CAT_MAGIC) != 0)
        return Kind::MAGIC;

    std::pair<C_Item::Flags, Kind> pairs[] = {
        {C_Item::Flags::ITEM_DAG, Kind::MELEE},
        {C_Item::Flags::ITEM_SWD, Kind::MELEE},
        {C_Item::Flags::ITEM_AXE, Kind::MELEE},
        {C_Item::Flags::ITEM_2HD_SWD, Kind::MELEE},
        {C_Item::Flags::ITEM_2HD_AXE, Kind::MELEE},
        {C_Item::Flags::ITEM_BOW, Kind::BOW},
        {C_Item::Flags::ITEM_CROSSBOW, Kind::BOW},
        {C_Item::Flags::ITEM_AMULET, Kind::AMULET},
        {C_Item::Flags::ITEM_RING, Kind::RING},
        {C_Item::Flags::ITEM_BELT, Kind::BELT},
        {C_Item::Flags::ITEM_MISSION, Kind::OTHER},
    };

    for (auto& p : pairs)
    {
        if (itemData.flags & p.first)
            return p.second;
    }

    return Kind::OTHER;
}

tl::optional<CharacterEquipment::Slot> CharacterEquipment::findAnyFreeMagicSlot() const
{
    Slot possibleSlots[] = {
        Slot::MAGIC_0,
        Slot::MAGIC_1,
        Slot::MAGIC_2,
        Slot::MAGIC_3,
        Slot::MAGIC_4,
        Slot::MAGIC_5,
        Slot::MAGIC_6,
        Slot::MAGIC_7,
        Slot::MAGIC_8};

    for (Slot s : possibleSlots)
    {
        if (!getItemInSlot(s).isValid())
            return s;
    }

    return tl::nullopt;
}

tl::optional<CharacterEquipment::Slot> CharacterEquipment::findAnyFreeRingSlot() const
{
    Slot possibleSlots[] = {
        Slot::RING_LEFT,
        Slot::RING_RIGHT,
    };

    for (Slot s : possibleSlots)
    {
        if (!getItemInSlot(s).isValid())
            return s;
    }

    return tl::nullopt;
}

bool CharacterEquipment::hasMeleeWeaponEquipped() const
{
    return getItemInSlot(Slot::MELEE).isValid();
}

bool CharacterEquipment::hasBowEquipped() const
{
    return getItemInSlot(Slot::BOW).isValid();
}

bool Logic::CharacterEquipment::hasItemEquipped(ItemHandle item) const
{
    for(auto h: m_ItemsBySlot)
    {
        if(h == item)
            return true;
    }

    return false;
}

bool CharacterEquipment::isItemTypeCorrectForSlot(ItemHandle item, Slot slot) const
{
    Kind kind = getKindOfItem(item);

    switch (kind)
    {
        case Kind::MELEE:
            return slot == Slot::MELEE;

        case Kind::BOW:
            return slot == Slot::BOW;

        case Kind::AMULET:
            return slot == Slot::AMULET;

        case Kind::RING:
        {
            if (slot == Slot::RING_LEFT)
                return true;

            if (slot == Slot::RING_RIGHT)
                return true;

            return false;
        }

        case Kind::MAGIC:
        {
            Slot possibleSlots[] = {
                Slot::MAGIC_0,
                Slot::MAGIC_1,
                Slot::MAGIC_2,
                Slot::MAGIC_3,
                Slot::MAGIC_4,
                Slot::MAGIC_5,
                Slot::MAGIC_6,
                Slot::MAGIC_7,
                Slot::MAGIC_8};

            for (Slot s : possibleSlots)
            {
                if (slot == s)
                    return true;
            }

            return false;
        }

        case Kind::BELT:
            return slot == Slot::BELT;

        case Kind::OTHER:
        default:
            return false;
    }
}

bool CharacterEquipment::doAttributesAllowUse(ItemHandle item) const
{
    // Check attributes
    Daedalus::GEngineClasses::C_Item& data = m_World.getScriptEngine().getGameState().getItem(item);
    Daedalus::GEngineClasses::C_Npc& npc = getController().getScriptInstance();
    ScriptEngine& s = m_World.getScriptEngine();

    for (size_t i = 0; i < Daedalus::GEngineClasses::C_Item::COND_ATR_MAX; i++)
    {
        // Why is 0 not allowed? That's how gothic is doing it though, as it seems...
        if (data.cond_atr[i] > 0)
        {
            assert(data.cond_atr[i] < Daedalus::GEngineClasses::C_Npc::EATR_MAX);

            // Check for enough strength, etc.
            if (npc.attribute[data.cond_atr[i]] < data.cond_value[i])
            {
                return false;
            }
        }
    }

    return true;
}

tl::optional<CharacterEquipment::ItemData&> CharacterEquipment::getItemDataInSlot(Slot slot) const
{
    return getDataOfItem(getItemInSlot(slot));
}

CharacterEquipment::ItemHandle CharacterEquipment::getItemInSlot(Slot slot) const
{
    return m_ItemsBySlot[(size_t)slot];
}

void Logic::CharacterEquipment::setItemInSlot(ItemHandle item, Slot slot)
{
    m_ItemsBySlot[(size_t)slot] = item;
}

PlayerController& CharacterEquipment::getController() const
{
    auto logic = m_World.getEntity<Components::LogicComponent>(m_CharacterEntity).m_pLogicController;
    assert(logic != nullptr);
    return *reinterpret_cast<Logic::PlayerController*>(logic);
}

tl::optional<CharacterEquipment::ItemData&> Logic::CharacterEquipment::getDataOfItem(ItemHandle item) const
{
    if (!item.isValid())
        return tl::nullopt;

    return m_World.getScriptEngine().getGameState().getItem(item);
}
