#pragma once
#include <inttypes.h>

/**
 * Explicit drawing-orders or views used for special effects must be set here
 */

namespace RenderViewList
{
    const uint8_t PRE_WORLD_SKY = 0;
    const uint8_t DEFAULT = 1;  // Opaque
    const uint8_t ALPHA_1 = 2;  // Draws smoke, etc
    const uint8_t ALPHA_2 = 3;  // Draws fire above the smoke

    const uint8_t INVENTORY_BG = 252; // Draws inventory background (slots)
	const uint8_t INVENTORY = 253; // Draws inventory items
    
    const uint8_t UI = 254;
}
