#pragma once
#include <inttypes.h>

/**
 * Explicit drawing-orders or views used for special effects must be set here
 */

namespace RenderViewList
{
    const uint8_t DEFAULT = 0;  // Opaque
    const uint8_t ALPHA_1 = 1;  // Draws smoke, etc
    const uint8_t ALPHA_2 = 2;  // Draws fire above the smoke
    const uint8_t INVENTORY_BG = 252; // Draws inventory background (slots)
    const uint8_t INVENTORY = 253; // Draws inventory items
    const uint8_t UI = 254;
}
