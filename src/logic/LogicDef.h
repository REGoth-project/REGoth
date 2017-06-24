#pragma once

namespace Logic
{
    enum class EWeaponMode
    {
        WeaponNone = 0,
        Weapon1h,
        Weapon2h,
        WeaponBow,
        WeaponCrossBow,
        WeaponMagic,
        WeaponFist,
        NUM_WEAPON_MODES
    };

    enum class EWeaponDrawType
    {
        Weapon_NF,
        Weapon_FF,
        Weapon_Magic
    };
}