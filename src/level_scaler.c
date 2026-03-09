#include "global.h"
#include "level_scaler.h"
#include "pokemon_tuxedo.h"
#include "region_manager.h"

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
    struct LevelRange wild_range;
    bool8 elite_four_defeated;
    
    /* Get Elite Four status for current region */
    elite_four_defeated = IsEliteFourDefeated(region_id);
    
    /* Get wild Pokemon level range */
    wild_range = GetWildPokemonLevelRange(badge_count, region_id, elite_four_defeated);
    
    /* Gym leader average = wild_max + 5 */
    return wild_range.max_level + 5;
}

/**
 * Calculate gym leader ace Pokemon level
 * Formula: average + 3 to 5
 */
u8 GetGymLeaderAceLevel(u8 gym_leader_id)
{
    u8 badge_count;
    u8 region_id;
    u8 average_level;
    u8 ace_bonus;
    
    /* Get current progression state */
    badge_count = GetBadgeCount();
    region_id = GetCurrentRegion();
    
    /* Get gym leader's average level */
    average_level = GetGymLeaderAverageLevel(badge_count, region_id);
    
    /* Ace bonus: 3 to 5 levels above average */
    /* For simplicity, use a fixed bonus of 4 levels */
    /* (Could be randomized or based on gym_leader_id for variety) */
    ace_bonus = 4;
    
    /* Gym leader ace = average + 3 to 5 */
    return average_level + ace_bonus;
}

/**
 * Calculate Elite Four average Pokemon level
 * Formula: wild_max + 10
 */
u8 GetEliteFourAverageLevel(u8 badge_count, u8 region_id)
{
    struct LevelRange wild_range;
    bool8 elite_four_defeated;
    
    /* Get Elite Four status for current region */
    elite_four_defeated = IsEliteFourDefeated(region_id);
    
    /* Get wild Pokemon level range */
    wild_range = GetWildPokemonLevelRange(badge_count, region_id, elite_four_defeated);
    
    /* Elite Four average = wild_max + 10 */
    return wild_range.max_level + 10;
}

/**
 * Calculate Elite Four ace Pokemon level
 * Formula: average + 3 to 5
 */
u8 GetEliteFourAceLevel(u8 elite_four_member_id)
{
    u8 badge_count;
    u8 region_id;
    u8 average_level;
    u8 ace_bonus;
    
    /* Get current progression state */
    badge_count = GetBadgeCount();
    region_id = GetCurrentRegion();
    
    /* Get Elite Four's average level */
    average_level = GetEliteFourAverageLevel(badge_count, region_id);
    
    /* Ace bonus: 3 to 5 levels above average */
    /* For simplicity, use a fixed bonus of 4 levels */
    /* (Could be randomized or based on elite_four_member_id for variety) */
    ace_bonus = 4;
    
    /* Elite Four ace = average + 3 to 5 */
    return average_level + ace_bonus;
}

/**
 * Calculate rival average Pokemon level
 * Formula: last_gym_average + 10
 */
u8 GetRivalAverageLevel(u8 badge_count, u8 region_id)
{
    u8 last_gym_average;

    /* Special case: First rival battle (0 badges) - rival should be at starter level */
    if (badge_count == 0)
    {
        return 5;
    }

    /* Calculate last gym leader's average level */
    last_gym_average = GetGymLeaderAverageLevel(badge_count, region_id);

    /* Rival average = last gym average + 10 */
    return last_gym_average + 10;
}


/**
 * Calculate rival ace Pokemon level
 * Formula: average + 7 to 10
 */
u8 GetRivalAceLevel(u8 rival_encounter_id)
{
    u8 badge_count;
    u8 region_id;
    u8 average_level;
    u8 ace_bonus;

    /* Get current progression state */
    badge_count = GetBadgeCount();
    region_id = GetCurrentRegion();

    /* Get rival's average level */
    average_level = GetRivalAverageLevel(badge_count, region_id);

    /* Special case: First rival battle (0 badges) - ace should also be level 5 */
    if (badge_count == 0)
    {
        return 5;
    }

    /* Ace bonus: 7 to 10 levels above average */
    /* For simplicity, use a fixed bonus of 8 levels */
    /* (Could be randomized or based on rival_encounter_id for variety) */
    ace_bonus = 8;

    /* Rival ace = average + 7 to 10 */
    return average_level + ace_bonus;
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
