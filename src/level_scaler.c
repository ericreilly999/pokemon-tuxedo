#include "global.h"
#include "level_scaler.h"
#include "pokemon_tuxedo.h"

// Region starting levels as defined in requirements
static const u8 sRegionStartingLevels[4] = {
    2,   // REGION_KANTO
    60,  // REGION_JOHTO
    110, // REGION_HOENN
    160  // REGION_SINNOH
};

/**
 * Get the starting level for a given region
 * 
 * @param region_id The region ID (0-3)
 * @return The starting level for that region
 */
u8 GetRegionStartingLevel(u8 region_id)
{
    if (region_id >= 4)
        return 2; // Default to Kanto starting level if invalid
    
    return sRegionStartingLevels[region_id];
}

/**
 * Calculate wild Pokemon level range based on badge count and region
 * 
 * Requirements:
 * - 2.1: Minimum level = Badge_Count × 5
 * - 2.2: Maximum level = (Badge_Count × 5) + 10
 * - 2.3: When Badge_Count is 0, levels should be 2-10
 * - 2.6: Account for region starting level
 * - 2.7: Add 10 levels if Elite Four defeated in that region
 * 
 * @param badge_count Total number of badges (0-32)
 * @param region_id Current region (0-3)
 * @param elite_four_defeated Whether Elite Four defeated in current region
 * @return LevelRange structure with min and max levels
 */
struct LevelRange GetWildPokemonLevelRange(u8 badge_count, u8 region_id, bool8 elite_four_defeated)
{
    struct LevelRange range;
    u8 region_starting_level = GetRegionStartingLevel(region_id);
    
    // Special case: badge_count == 0 gives levels 2-10 (Requirement 2.3)
    if (badge_count == 0)
    {
        range.min_level = 2;
        range.max_level = 10;
    }
    else
    {
        // General formula (Requirements 2.1, 2.2, 2.6):
        // min = (badge_count * 5) + region_starting_level
        // max = min + 10
        range.min_level = (badge_count * 5) + region_starting_level;
        range.max_level = range.min_level + 10;
    }
    
    // Apply Elite Four bonus if defeated (Requirement 2.7)
    if (elite_four_defeated)
    {
        range.min_level += 10;
        range.max_level += 10;
    }
    
    // Clamp to valid Pokemon level range (1-100)
    if (range.min_level < 1)
        range.min_level = 1;
    if (range.min_level > 100)
        range.min_level = 100;
    
    if (range.max_level < 1)
        range.max_level = 1;
    if (range.max_level > 100)
        range.max_level = 100;
    
    return range;
}

/**
 * Calculate gym leader average Pokemon level
 * Formula: wild_max + 5
 */
u8 GetGymLeaderAverageLevel(u8 badge_count, u8 region_id)
{
    // TODO: Implement in task 3.2
    return 0;
}

/**
 * Calculate gym leader ace Pokemon level
 * Formula: average + 3 to 5
 */
u8 GetGymLeaderAceLevel(u8 gym_leader_id)
{
    // TODO: Implement in task 3.2
    return 0;
}

/**
 * Calculate Elite Four average Pokemon level
 * Formula: wild_max + 10
 */
u8 GetEliteFourAverageLevel(u8 badge_count, u8 region_id)
{
    // TODO: Implement in task 3.3
    return 0;
}

/**
 * Calculate Elite Four ace Pokemon level
 * Formula: average + 3 to 5
 */
u8 GetEliteFourAceLevel(u8 elite_four_member_id)
{
    // TODO: Implement in task 3.3
    return 0;
}

/**
 * Calculate rival average Pokemon level
 * Formula: last_gym_average + 10
 */
u8 GetRivalAverageLevel(u8 badge_count, u8 region_id)
{
    // TODO: Implement in task 3.4
    return 0;
}

/**
 * Calculate rival ace Pokemon level
 * Formula: average + 7 to 10
 */
u8 GetRivalAceLevel(u8 rival_encounter_id)
{
    // TODO: Implement in task 3.4
    return 0;
}

/**
 * Get the current level cap
 */
u8 GetCurrentLevelCap(void)
{
    // TODO: Implement in task 4.2
    return 100;
}

/**
 * Update the level cap based on progression
 */
void UpdateLevelCap(u8 badge_count, u8 region_id)
{
    // TODO: Implement in task 4.2
}
