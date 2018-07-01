//
// Created by andre on 28.06.18.
//

#pragma once

#include <array>
#include <ZenLib/daedalus/DaedalusGameState.h>
#include <tl/optional.hpp>
#include <handle/HandleDef.h>
#include <logic/visuals/ModelVisual.h>

namespace World
{
    class WorldInstance;
}

namespace Logic
{
    class PlayerController;
    /**
     * Contains information about what a character currently has equipped.
     * This encapsulates logic like whether the characters attributes are high enough
     * and how many rings can be worn at once.
     */
    class CharacterEquipment
    {
    public:
        using ItemHandle = Daedalus::GameState::ItemHandle;
        using ItemData = Daedalus::GEngineClasses::C_Item;

        enum class Slot : size_t
        {
            MELEE,
            BOW,  // + Crossbow
            MAGIC_0,
            MAGIC_1,
            MAGIC_2,
            MAGIC_3,
            MAGIC_4,
            MAGIC_5,
            MAGIC_6,
            MAGIC_7,
            MAGIC_8,
            MAGIC_9,
            RING_LEFT,
            RING_RIGHT,
            AMULET,
            BELT,
            ARMOR,
            COUNT,
        };

        enum class Kind : size_t
        {
            MELEE,
            BOW,
            AMULET,
            RING,
            MAGIC,
            BELT,
            ARMOR,
            OTHER,
        };

        CharacterEquipment(World::WorldInstance& world, Handle::EntityHandle characterEntity);

        /**
         * Tries to equip the given item to the given slot.
         * Returns true on success.
         * Returns false if not possible (attributes, wrong kind of slot).
         */
        bool equipItemToSlot(ItemHandle item, Slot slot);

        /**
         * Equips the item to any free slot avilable for it.
         * @return True, if a slot has been found and the item could be equipped
         *         False, if all possible slots are full or the attributes aren't correct
         */
        bool equipItem(ItemHandle item);

        /**
         * Takes of the item currently in the given slot
         */
        void unequipItemInSlot(Slot slot);

        /**
         * Unequips the given item, if it is currently equipped
         */
        void unequipItem(ItemHandle item);

        /**
         * Finds a slot the given item could potentially be equipped to.
         * For magic and rings the first slot is returned, regardless of
         * whether it's empty or not.
         * This function also does not check whether the attributes of the
         * underlaying character allow equipment.
         */
        tl::optional<Slot> getCorrectSlotForItem(ItemHandle item) const;

        /**
         * Figures out which kind of item the given handle represents.
         * Every kind except "OTHER" can be equipped.
         */
        Kind getKindOfItem(ItemHandle item) const;

        /**
         * Some types of items can be equipped to multiple slots. These functions
         * return the first free one they can find.
         */
        tl::optional<CharacterEquipment::Slot> findAnyFreeMagicSlot() const;
        tl::optional<CharacterEquipment::Slot> findAnyFreeRingSlot() const;

        /**
         * Finds the slot the given item was equipped to
         */
        tl::optional<CharacterEquipment::Slot> findSlotItemWasEquippedTo(ItemHandle item) const;

        /**
         * Status queries
         */
        bool hasMeleeWeaponEquipped() const;
        bool hasBowEquipped() const;
        bool hasItemEquipped(ItemHandle item) const;

        /**
         * Whether the given item could be equipped into the given slot.
         * Ie. A ring cannot be equipped to the slot meant for melee-weapons.
         */
        bool isItemTypeCorrectForSlot(ItemHandle item, Slot slot) const;

        /**
         * @return Whether the given item is of the given kind
         */
        bool isItemOfKind(ItemHandle item, Kind kind) const;

        /**
         * @return Whether this item is possible equippable (not checking attributes)
         */
        bool isItemEquipable(ItemHandle item) const;

        /**
         * Whether the underlaying characters attributes are high enough to
         * equip or use the given item.
         *
         * This can also be called on non-equipable items such as food where it makes sense!
         */
        bool doAttributesAllowUse(ItemHandle item) const;

        /**
         * @return the item currently equipped in the slot
         */
        tl::optional<ItemData&> getItemDataInSlot(Slot slot) const;
        ItemHandle getItemInSlot(Slot slot) const;

    protected:

        /**
         * Assigns the given item to the given slot without any checks
         * or anything else
         */
        void setItemInSlot(ItemHandle item, Slot slot);

        std::array<ItemHandle, (size_t)Slot::COUNT> m_ItemsBySlot;

        /**
         * These functions perform the actual equipment.
         * For these functions to succeed, the target slot needs to be
         * empty!
         *
         * @return True, if equipping was possible
         *         False otherwise (ie. attributes too low)
         */
        bool equipMelee(ItemHandle item);
        bool equipBow(ItemHandle item);
        bool equipAmulet(ItemHandle item);
        bool equipRing(ItemHandle item, Slot slot);
        bool equipMagic(ItemHandle item, Slot slot);
        bool equipBelt(ItemHandle item);
        bool equipArmor(ItemHandle item);

        /**
         * Sets the visual attachment on the character model
         */
        void setCharacterModelAttachment(const std::string& visual, EModelNode node);
        void removeCharacterModelAttachment(EModelNode node);

        /**
         * Switches to the default no-armor body mesh
         */
        void switchToDefaultCharacterModel();

        /**
         * Switches to the given armor-visual
         */
        void switchCharacterModelArmor(const std::string& visual);

        /**
         * @return handle to the underlaying character
         */
        PlayerController& getController() const;

        /**
         * @return Data of the given item
         */
        tl::optional<ItemData&> getDataOfItem(ItemHandle item) const;


        /**
         * @return The visual for this item set by the scripts
         */
        std::string getItemVisual(ItemHandle item) const;

        World::WorldInstance& m_World;
        Handle::EntityHandle m_CharacterEntity;
    };

}  // namespace Logic
