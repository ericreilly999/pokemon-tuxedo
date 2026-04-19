#include "global.h"
#include "level_scaler.h"
#include "pokemon_tuxedo.h"
#include "region_manager.h"
#include "pokemon.h"

/**
 * Calculate wild Pokemon level range based on badge count and region
 *
 * Requirements:
 * - 2.1: Minimum level = Badge_Count x 5
 * - 2.2: Maximum level = (Badge_Count x 5) + 10
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

    /* Special case: badge_count == 0 gives levels 2-10 (Requirement 2.3) */
    if (badge_count == 0)
    {
        range.min_level = 2;
        range.max_level = 10;
    }
    else
    {
        /* General formula (Requirements 2.1, 2.2, 2.6):
           min = (badge_count * 5) + region_starting_level
           max = min + 10 */
        range.min_level = (badge_count * 5) + region_starting_level;
        range.max_level = range.min_level + 10;
    }

    /* Apply Elite Four bonus if defeated (Requirement 2.7) */
    if (elite_four_defeated)
    {
        range.min_level += 10;
        range.max_level += 10;
    }

    /* Clamp to valid Pokemon level range (1-100) */
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

    /* Ace bonus: 3 to 5 levels above average.
       For simplicity, use a fixed bonus of 4 levels.
       (Could be randomized or based on gym_leader_id for variety.) */
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

    /* Ace bonus: 3 to 5 levels above average.
       For simplicity, use a fixed bonus of 4 levels.
       (Could be randomized or based on elite_four_member_id for variety.) */
    ace_bonus = 4;

    /* Elite Four ace = average + 3 to 5 */
    return average_level + ace_bonus;
}

/**
 * Returns the integer average of the 3 highest-level non-egg Pokemon in
 * the player's party. Fainted Pokemon count. Eggs are excluded.
 * If fewer than 3 non-egg Pokemon exist, averages over however many there are.
 * Returns 1 if party is empty.
 *
 * Req 22: adaptive rival formula uses player party average at encounter init.
 */
u8 GetPlayerTop3Average(void)
{
    u8 levels[PARTY_SIZE];
    u8 count;
    u8 i;
    u8 j;
    u8 tmp;
    u16 sum;
    u8 top_count;

    count = 0;
    for (i = 0; i < PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &gPlayerParty[i];
        u16 species = GetMonData(mon, MON_DATA_SPECIES, NULL);
        if (species == SPECIES_NONE)
            continue;
        if (GetMonData(mon, MON_DATA_IS_EGG, NULL))
            continue;
        levels[count] = (u8)GetMonData(mon, MON_DATA_LEVEL, NULL);
        count++;
    }

    if (count == 0)
        return 1;

    /* Simple insertion sort descending */
    for (i = 1; i < count; i++)
    {
        tmp = levels[i];
        j = i;
        while (j > 0 && levels[j - 1] < tmp)
        {
            levels[j] = levels[j - 1];
            j--;
        }
        levels[j] = tmp;
    }

    /* Average the top 3 (or fewer if party is small) */
    top_count = (count < 3) ? count : 3;
    sum = 0;
    for (i = 0; i < top_count; i++)
        sum += levels[i];

    return (u8)(sum / top_count);
}

/**
 * Calculate rival average Pokemon level (Req 22 dual-formula dispatch).
 *
 * Champion slot (is_champion_slot == TRUE):
 *   average = wild_max + 15  (Champion formula)
 *
 * Rival encounter slot (is_champion_slot == FALSE):
 *   average = player top-3 party average; minimum floor of 5.
 *   This minimum also covers the 0-badge first-encounter override.
 */
u8 GetRivalAverageLevel(u8 badge_count, u8 region_id, bool8 is_champion_slot)
{
    struct LevelRange range;
    u8 player_avg;
    u16 result;

    if (is_champion_slot)
    {
        /* Champion formula: wild_max + 15 */
        range = GetWildPokemonLevelRange(badge_count, region_id, FALSE);
        result = (u16)range.max_level + 15;
        if (result < 1) result = 1;
        if (result > 100) result = 100;
        return (u8)result;
    }
    else
    {
        /* Hard override: first encounter (0 badges) always spawns rival at level 5.
           The soft floor below is insufficient when the starter is already above
           level 5 (e.g. level 9 after grinding). */
        if (badge_count == 0)
            return 5;

        /* Rival adaptive formula: player top-3 average */
        player_avg = GetPlayerTop3Average();
        if (player_avg < 5)
            return 5; /* minimum floor for subsequent encounters */
        return player_avg;
    }
}

/**
 * Calculate rival ace Pokemon level (Req 22 / Req 23).
 * Formula: rival average + 4, clamped to 1-100.
 */
u8 GetRivalAceLevel(u8 badge_count, u8 region_id, bool8 is_champion_slot)
{
    u8 avg;
    u16 result;

    avg = GetRivalAverageLevel(badge_count, region_id, is_champion_slot);
    result = (u16)avg + 4;
    if (result > 100) result = 100;
    return (u8)result;
}

/**
 * Get the current level cap
 */
u8 GetCurrentLevelCap(void)
{
    u8 total_badges = GetTotalBadgeCount();
    u8 current_region = GetCurrentRegion();
    return GetLevelCapForProgression(total_badges, current_region);
}

/**
 * Update the level cap based on progression
 */
void UpdateLevelCap(u8 badge_count, u8 region_id)
{
    /* Level cap is now calculated dynamically via GetLevelCapForProgression.
       This function is kept for backward compatibility but does nothing. */
    (void)badge_count;
    (void)region_id;
}

/**
 * Calculate wild Pokemon level range for any region with cross-region scaling.
 *
 * This function extends GetWildPokemonLevelRange to support the full
 * Kanto -> Hoenn -> Johto progression (ADR-004).
 *
 * Level calculation formula:
 * - Base level = region_starting_level + (badges_in_region * 5)
 * - For regions after Kanto, previous region completion adds bonus
 * - Elite Four defeat in current region adds +10 levels
 *
 * Region starting levels (from design):
 * - Kanto: 2
 * - Hoenn: 60 (unlocked after Kanto E4)
 * - Johto: 110 (unlocked after Hoenn E4)
 *
 * Requirements: 11.1, 11.3, 11.4, 11.7
 * Decision: ADR-004 (Kanto -> Hoenn -> Johto progression)
 *
 * @param badge_count Number of badges earned in the target region (0-8)
 * @param region_id Target region (REGION_KANTO, REGION_HOENN, REGION_JOHTO)
 * @param elite_four_defeated Whether Elite Four defeated in target region
 * @return LevelRange structure with min and max levels
 */
struct LevelRange GetWildPokemonLevelRangeForRegion(u8 badge_count, u8 region_id, bool8 elite_four_defeated)
{
    struct LevelRange range;
    u8 region_starting_level;
    u8 base_min;

    /* Get the starting level for this region */
    region_starting_level = GetRegionStartingLevel(region_id);

    /* Special case: 0 badges in Kanto gives levels 2-10 (starter area) */
    if (badge_count == 0 && region_id == REGION_KANTO)
    {
        range.min_level = 2;
        range.max_level = 10;
    }
    else
    {
        /* General formula for cross-region scaling:
           min = region_starting_level + (badge_count * 5)
           max = min + 10 */
        base_min = region_starting_level + (badge_count * 5);
        range.min_level = base_min;
        range.max_level = base_min + 10;
    }

    /* Apply Elite Four bonus if defeated in this region (+10 levels).
       This represents post-game content being harder.
       Requirement 11.7: Recognize Elite Four completion for scaling. */
    if (elite_four_defeated)
    {
        range.min_level += 10;
        range.max_level += 10;
    }

    /* Clamp to valid Pokemon level range (1-100) */
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
 * Calculate trainer average level for any region with cross-region scaling.
 *
 * This function provides unified trainer level calculation across all regions
 * and trainer types for the Kanto -> Hoenn -> Johto progression (ADR-004).
 *
 * Trainer type bonuses (added to wild Pokemon max level):
 * - Normal trainers: +0 (same as wild Pokemon max)
 * - Gym Leaders: +5
 * - Elite Four: +10
 * - Champion: +15
 * - Rival: +10 (scales with player progression)
 *
 * Requirements: 11.1, 11.3, 11.4, 11.7
 * Decision: ADR-004 (Kanto -> Hoenn -> Johto progression)
 *
 * @param badge_count Number of badges earned in the target region (0-8)
 * @param region_id Target region (REGION_KANTO, REGION_HOENN, REGION_JOHTO)
 * @param trainer_type Type of trainer (TRAINER_TYPE_NORMAL, TRAINER_TYPE_GYM_LEADER, etc.)
 * @return Average level for trainer's Pokemon
 */
u8 GetTrainerAverageLevelForRegion(u8 badge_count, u8 region_id, u8 trainer_type)
{
    struct LevelRange wild_range;
    bool8 elite_four_defeated;
    u8 base_level;
    u8 trainer_bonus;
    u8 result;

    /* Get Elite Four status for the target region */
    elite_four_defeated = IsEliteFourDefeated(region_id);

    /* Get wild Pokemon level range for this region */
    wild_range = GetWildPokemonLevelRangeForRegion(badge_count, region_id, elite_four_defeated);

    /* Base trainer level is the wild Pokemon max level */
    base_level = wild_range.max_level;

    /* Apply trainer type bonus */
    switch (trainer_type)
    {
    case TRAINER_TYPE_NORMAL:
        /* Normal trainers match wild Pokemon levels */
        trainer_bonus = 0;
        break;
    case TRAINER_TYPE_GYM_LEADER:
        /* Gym leaders are 5 levels above wild Pokemon */
        trainer_bonus = 5;
        break;
    case TRAINER_TYPE_ELITE_FOUR:
        /* Elite Four are 10 levels above wild Pokemon */
        trainer_bonus = 10;
        break;
    case TRAINER_TYPE_CHAMPION:
        /* Champion is 15 levels above wild Pokemon */
        trainer_bonus = 15;
        break;
    case TRAINER_TYPE_RIVAL:
        /* Rival scales with player, 10 levels above wild Pokemon.
           Special case: First rival battle (0 badges in Kanto) is level 5. */
        if (badge_count == 0 && region_id == REGION_KANTO)
        {
            return 5;
        }
        trainer_bonus = 10;
        break;
    default:
        /* Unknown trainer type, use normal bonus */
        trainer_bonus = 0;
        break;
    }

    /* Calculate final level */
    result = base_level + trainer_bonus;

    /* Clamp to valid Pokemon level range (1-100) */
    if (result < 1)
        result = 1;
    if (result > 100)
        result = 100;

    return result;
}

/**
 * Calculate level cap based on total badge count and current region.
 *
 * The level cap increases with progression through the game to prevent
 * over-leveling while still allowing growth. The cap is based on:
 * - Total badges across all regions (0-24)
 * - Current region (affects base cap)
 *
 * Level cap formula:
 * - Base cap = region_starting_level + 20
 * - Per badge bonus = +5 per badge
 * - Maximum cap = 100
 *
 * Example progression (Kanto -> Hoenn -> Johto):
 * - 0 badges (Kanto start): cap = 2 + 20 = 22
 * - 8 badges (Kanto complete): cap = 2 + 20 + (8 * 5) = 62
 * - 8 badges (Hoenn start): cap = 60 + 20 = 80
 * - 16 badges (Hoenn complete): cap = 60 + 20 + (8 * 5) = 120 -> clamped to 100
 * - 16 badges (Johto start): cap = 100 (already at max)
 *
 * Requirements: 11.1, 11.3, 11.4, 11.7
 * Decision: ADR-004 (Kanto -> Hoenn -> Johto progression)
 *
 * @param total_badges Total badges across all regions (0-24)
 * @param current_region Current region (REGION_KANTO, REGION_HOENN, REGION_JOHTO)
 * @return Level cap for the current progression state
 */
u8 GetLevelCapForProgression(u8 total_badges, u8 current_region)
{
    u8 region_starting_level;
    u8 base_cap;
    u8 badge_bonus;
    u8 badges_in_current_region;
    u16 result;

    /* Get the starting level for current region */
    region_starting_level = GetRegionStartingLevel(current_region);

    /* Base cap is region starting level + 20.
       This gives room to grow within the region. */
    base_cap = region_starting_level + 20;

    /* Calculate badges earned in current region
       (total badges minus badges from previous regions) */
    switch (current_region)
    {
    case REGION_KANTO:
        /* In Kanto, all badges are Kanto badges */
        badges_in_current_region = total_badges;
        if (badges_in_current_region > BADGES_PER_REGION)
            badges_in_current_region = BADGES_PER_REGION;
        break;
    case REGION_HOENN:
        /* In Hoenn, subtract Kanto badges (8) */
        if (total_badges > BADGES_PER_REGION)
            badges_in_current_region = total_badges - BADGES_PER_REGION;
        else
            badges_in_current_region = 0;
        if (badges_in_current_region > BADGES_PER_REGION)
            badges_in_current_region = BADGES_PER_REGION;
        break;
    case REGION_JOHTO:
        /* In Johto, subtract Kanto + Hoenn badges (16) */
        if (total_badges > (BADGES_PER_REGION * 2))
            badges_in_current_region = total_badges - (BADGES_PER_REGION * 2);
        else
            badges_in_current_region = 0;
        if (badges_in_current_region > BADGES_PER_REGION)
            badges_in_current_region = BADGES_PER_REGION;
        break;
    default:
        badges_in_current_region = 0;
        break;
    }

    /* Badge bonus: +5 per badge in current region */
    badge_bonus = badges_in_current_region * 5;

    /* Calculate final cap (use u16 to avoid overflow) */
    result = (u16)base_cap + (u16)badge_bonus;

    /* Clamp to maximum level (100) */
    if (result > 100)
        result = 100;

    return (u8)result;
}
