#include "global.h"
#include "map_accessibility.h"
#include "region_manager.h"

/*
 * Map Accessibility System
 * 
 * Requirements:
 * - 14.1: Maintain story-based progression for Kanto
 * - 14.2: Unlock all maps in regions 2-4 upon entry
 * - 14.3: Allow gym challenges in any order (regions 2-4)
 * - 14.4: Kanto maintains linear progression
 */

static bool8 sRegionMapsUnlocked[4] = {FALSE, FALSE, FALSE, FALSE};

void UnlockRegionMaps(u8 region_id)
{
    if (region_id <= REGION_SINNOH)
        sRegionMapsUnlocked[region_id] = TRUE;
}

bool8 IsMapAccessible(u16 map_id)
{
    u8 current_region = GetCurrentRegion();
    
    // Kanto maintains story-based progression
    if (current_region == REGION_KANTO)
    {
        // Check story progression flags
        // This would integrate with existing story progression system
        return TRUE;  // Simplified for stub
    }
    
    // Other regions: all maps accessible once region is unlocked
    return sRegionMapsUnlocked[current_region];
}

bool8 CanChallengeGymsInAnyOrder(u8 region_id)
{
    // Kanto: story-based progression (linear)
    if (region_id == REGION_KANTO)
        return FALSE;
    
    // Johto, Hoenn, Sinnoh: any order
    return TRUE;
}
