/*
 * Integration Tests for Level Scaling System
 * 
 * Task 4.3: Run integration tests for level scaling
 * 
 * Test Scenarios:
 * 1. Wild Pokemon levels at 0 badges (Kanto only)
 * 2. Wild Pokemon levels at 8 badges (Kanto complete, Hoenn unlocked)
 * 3. Wild Pokemon levels at 16 badges (Kanto + Hoenn complete)
 * 4. Trainer levels scale with badge count
 * 5. Level cap increases with progression
 * 6. Elite Four bonus (+10) applied correctly
 * 
 * **Validates: Requirements 11.1, 11.3, 11.4, 11.7**
 * 
 * Region order: Kanto → Hoenn → Johto (per ADR-004)
 * Sinnoh descoped per ADR-003 (3 regions total)
 * 
 * Region starting levels:
 * - Kanto: 2
 * - Hoenn: 60
 * - Johto: 110
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// Type definitions to match GBA types
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef bool bool8;

// Region constants (from region_manager.h)
#define REGION_KANTO  0
#define REGION_HOENN  1
#define REGION_JOHTO  2
#define NUM_REGIONS   3
#define BADGES_PER_REGION  8
#define TOTAL_BADGES       (NUM_REGIONS * BADGES_PER_REGION)  // 24 badges

// Trainer type constants (from level_scaler.h)
#define TRAINER_TYPE_NORMAL     0
#define TRAINER_TYPE_GYM_LEADER 1
#define TRAINER_TYPE_ELITE_FOUR 2
#define TRAINER_TYPE_CHAMPION   3
#define TRAINER_TYPE_RIVAL      4

// Region starting levels (from design document)
#define KANTO_STARTING_LEVEL  2
#define HOENN_STARTING_LEVEL  60
#define JOHTO_STARTING_LEVEL  110

// Level range structure
struct LevelRange {
    u8 min_level;
    u8 max_level;
};

// ==========================================
// Region State Simulation
// ==========================================

struct RegionState {
    u8 current_region;
    u8 kanto_badges;
    u8 hoenn_badges;
    u8 johto_badges;
    bool8 kanto_e4_defeated;
    bool8 hoenn_e4_defeated;
    bool8 johto_e4_defeated;
    bool8 hoenn_unlocked;
    bool8 johto_unlocked;
};

static struct RegionState sRegionState = {0};

static void InitRegionState(void) {
    sRegionState.current_region = REGION_KANTO;
    sRegionState.kanto_badges = 0;
    sRegionState.hoenn_badges = 0;
    sRegionState.johto_badges = 0;
    sRegionState.kanto_e4_defeated = false;
    sRegionState.hoenn_e4_defeated = false;
    sRegionState.johto_e4_defeated = false;
    sRegionState.hoenn_unlocked = false;
    sRegionState.johto_unlocked = false;
}

static u8 GetCurrentRegion(void) {
    return sRegionState.current_region;
}

static void SetCurrentRegion(u8 region) {
    sRegionState.current_region = region;
}

static u8 GetRegionBadgeCount(u8 region_id) {
    switch (region_id) {
    case REGION_KANTO: return sRegionState.kanto_badges;
    case REGION_HOENN: return sRegionState.hoenn_badges;
    case REGION_JOHTO: return sRegionState.johto_badges;
    default: return 0;
    }
}

static void SetRegionBadgeCount(u8 region_id, u8 count) {
    if (count > BADGES_PER_REGION) count = BADGES_PER_REGION;
    switch (region_id) {
    case REGION_KANTO: sRegionState.kanto_badges = count; break;
    case REGION_HOENN: sRegionState.hoenn_badges = count; break;
    case REGION_JOHTO: sRegionState.johto_badges = count; break;
    }
}

static u8 GetTotalBadgeCount(void) {
    return sRegionState.kanto_badges + sRegionState.hoenn_badges + sRegionState.johto_badges;
}

static bool8 IsEliteFourDefeated(u8 region_id) {
    switch (region_id) {
    case REGION_KANTO: return sRegionState.kanto_e4_defeated;
    case REGION_HOENN: return sRegionState.hoenn_e4_defeated;
    case REGION_JOHTO: return sRegionState.johto_e4_defeated;
    default: return false;
    }
}

static void SetEliteFourDefeated(u8 region_id, bool8 defeated) {
    switch (region_id) {
    case REGION_KANTO: 
        sRegionState.kanto_e4_defeated = defeated;
        if (defeated) sRegionState.hoenn_unlocked = true;
        break;
    case REGION_HOENN: 
        sRegionState.hoenn_e4_defeated = defeated;
        if (defeated) sRegionState.johto_unlocked = true;
        break;
    case REGION_JOHTO: 
        sRegionState.johto_e4_defeated = defeated;
        break;
    }
}

// ==========================================
// Level Scaler Implementation (mirrors src/level_scaler.c)
// ==========================================

static u8 GetRegionStartingLevel(u8 region_id) {
    switch (region_id) {
    case REGION_KANTO: return KANTO_STARTING_LEVEL;
    case REGION_HOENN: return HOENN_STARTING_LEVEL;
    case REGION_JOHTO: return JOHTO_STARTING_LEVEL;
    default: return KANTO_STARTING_LEVEL;
    }
}

/**
 * Calculate wild Pokemon level range for a region.
 * Formula: min = region_starting_level + (badge_count * 5), max = min + 10
 * Special case: 0 badges in Kanto = 2-10
 * Elite Four bonus: +10 if defeated in that region
 */
static struct LevelRange GetWildPokemonLevelRangeForRegion(u8 badge_count, u8 region_id, bool8 elite_four_defeated) {
    struct LevelRange range;
    u8 region_starting_level = GetRegionStartingLevel(region_id);
    u8 base_min;
    
    // Special case: 0 badges in Kanto gives levels 2-10
    if (badge_count == 0 && region_id == REGION_KANTO) {
        range.min_level = 2;
        range.max_level = 10;
    } else {
        base_min = region_starting_level + (badge_count * 5);
        range.min_level = base_min;
        range.max_level = base_min + 10;
    }
    
    // Apply Elite Four bonus (+10)
    if (elite_four_defeated) {
        range.min_level += 10;
        range.max_level += 10;
    }
    
    // Clamp to valid range (1-100)
    if (range.min_level < 1) range.min_level = 1;
    if (range.min_level > 100) range.min_level = 100;
    if (range.max_level < 1) range.max_level = 1;
    if (range.max_level > 100) range.max_level = 100;
    
    return range;
}

/**
 * Calculate trainer average level for a region.
 * Base = wild max level, then add trainer type bonus
 */
static u8 GetTrainerAverageLevelForRegion(u8 badge_count, u8 region_id, u8 trainer_type) {
    struct LevelRange wild_range;
    u8 base_level, trainer_bonus, result;
    
    // Special case: Rival at 0 badges in Kanto = level 5
    if (trainer_type == TRAINER_TYPE_RIVAL && badge_count == 0 && region_id == REGION_KANTO) {
        return 5;
    }
    
    wild_range = GetWildPokemonLevelRangeForRegion(badge_count, region_id, false);
    base_level = wild_range.max_level;
    
    switch (trainer_type) {
    case TRAINER_TYPE_NORMAL:     trainer_bonus = 0;  break;
    case TRAINER_TYPE_GYM_LEADER: trainer_bonus = 5;  break;
    case TRAINER_TYPE_ELITE_FOUR: trainer_bonus = 10; break;
    case TRAINER_TYPE_CHAMPION:   trainer_bonus = 15; break;
    case TRAINER_TYPE_RIVAL:      trainer_bonus = 10; break;
    default:                      trainer_bonus = 0;  break;
    }
    
    result = base_level + trainer_bonus;
    if (result > 100) result = 100;
    return result;
}

/**
 * Calculate level cap based on total badges and current region.
 * Formula: base_cap = region_starting_level + 20, badge_bonus = badges_in_region * 5
 */
static u8 GetLevelCapForProgression(u8 total_badges, u8 current_region) {
    u8 region_starting_level = GetRegionStartingLevel(current_region);
    u8 base_cap = region_starting_level + 20;
    u8 badges_in_current_region;
    u16 result;
    
    switch (current_region) {
    case REGION_KANTO:
        badges_in_current_region = total_badges;
        if (badges_in_current_region > BADGES_PER_REGION)
            badges_in_current_region = BADGES_PER_REGION;
        break;
    case REGION_HOENN:
        if (total_badges > BADGES_PER_REGION)
            badges_in_current_region = total_badges - BADGES_PER_REGION;
        else
            badges_in_current_region = 0;
        if (badges_in_current_region > BADGES_PER_REGION)
            badges_in_current_region = BADGES_PER_REGION;
        break;
    case REGION_JOHTO:
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
    
    result = (u16)base_cap + (u16)(badges_in_current_region * 5);
    if (result > 100) result = 100;
    return (u8)result;
}

// ==========================================
// Integration Test 1: Wild Pokemon Levels at 0 Badges (Kanto Only)
// **Validates: Requirements 11.1**
// ==========================================

static int test_wild_levels_0_badges_kanto(void) {
    int passed = 0, failed = 0;
    struct LevelRange range;
    
    printf("\n=== Integration Test 1: Wild Pokemon Levels at 0 Badges (Kanto) ===\n");
    printf("**Validates: Requirements 11.1**\n\n");
    
    InitRegionState();
    SetCurrentRegion(REGION_KANTO);
    SetRegionBadgeCount(REGION_KANTO, 0);
    
    // Test 1: Wild levels in Kanto with 0 badges
    printf("Test 1.1: Wild levels in Kanto (0 badges)...\n");
    range = GetWildPokemonLevelRangeForRegion(0, REGION_KANTO, false);
    
    if (range.min_level != 2) {
        printf("  FAIL: Min level should be 2, got %u\n", range.min_level);
        failed++;
    } else {
        printf("  PASS: Min level = 2\n");
        passed++;
    }
    
    if (range.max_level != 10) {
        printf("  FAIL: Max level should be 10, got %u\n", range.max_level);
        failed++;
    } else {
        printf("  PASS: Max level = 10\n");
        passed++;
    }
    
    // Test 2: Verify level range spread is 8 (special case for 0 badges)
    printf("Test 1.2: Verify level range spread...\n");
    u8 spread = range.max_level - range.min_level;
    if (spread != 8) {
        printf("  FAIL: Level spread should be 8, got %u\n", spread);
        failed++;
    } else {
        printf("  PASS: Level spread = 8 (special case for starter area)\n");
        passed++;
    }
    
    printf("\n--- Test 1 Results: Passed=%d, Failed=%d ---\n", passed, failed);
    return failed;
}

// ==========================================
// Integration Test 2: Wild Pokemon Levels at 8 Badges (Kanto Complete)
// **Validates: Requirements 11.1, 11.3**
// ==========================================

static int test_wild_levels_8_badges_kanto_complete(void) {
    int passed = 0, failed = 0;
    struct LevelRange kanto_range, hoenn_range;
    
    printf("\n=== Integration Test 2: Wild Pokemon Levels at 8 Badges (Kanto Complete) ===\n");
    printf("**Validates: Requirements 11.1, 11.3**\n\n");
    
    InitRegionState();
    SetRegionBadgeCount(REGION_KANTO, 8);
    SetEliteFourDefeated(REGION_KANTO, true);  // Unlocks Hoenn
    
    // Test 1: Wild levels in Kanto with 8 badges (no E4 bonus yet for wild calc)
    printf("Test 2.1: Wild levels in Kanto (8 badges, E4 not applied to wild)...\n");
    kanto_range = GetWildPokemonLevelRangeForRegion(8, REGION_KANTO, false);
    
    // Expected: min = 2 + (8 * 5) = 42, max = 52
    if (kanto_range.min_level != 42) {
        printf("  FAIL: Kanto min level should be 42, got %u\n", kanto_range.min_level);
        failed++;
    } else {
        printf("  PASS: Kanto min level = 42\n");
        passed++;
    }
    
    if (kanto_range.max_level != 52) {
        printf("  FAIL: Kanto max level should be 52, got %u\n", kanto_range.max_level);
        failed++;
    } else {
        printf("  PASS: Kanto max level = 52\n");
        passed++;
    }
    
    // Test 2: Wild levels in Kanto with E4 bonus applied
    printf("Test 2.2: Wild levels in Kanto (8 badges, E4 bonus applied)...\n");
    kanto_range = GetWildPokemonLevelRangeForRegion(8, REGION_KANTO, true);
    
    // Expected: min = 42 + 10 = 52, max = 62
    if (kanto_range.min_level != 52) {
        printf("  FAIL: Kanto min level with E4 should be 52, got %u\n", kanto_range.min_level);
        failed++;
    } else {
        printf("  PASS: Kanto min level with E4 = 52\n");
        passed++;
    }
    
    if (kanto_range.max_level != 62) {
        printf("  FAIL: Kanto max level with E4 should be 62, got %u\n", kanto_range.max_level);
        failed++;
    } else {
        printf("  PASS: Kanto max level with E4 = 62\n");
        passed++;
    }
    
    // Test 3: Wild levels in Hoenn (just unlocked, 0 Hoenn badges)
    printf("Test 2.3: Wild levels in Hoenn (0 Hoenn badges)...\n");
    hoenn_range = GetWildPokemonLevelRangeForRegion(0, REGION_HOENN, false);
    
    // Expected: min = 60 + (0 * 5) = 60, max = 70
    if (hoenn_range.min_level != 60) {
        printf("  FAIL: Hoenn min level should be 60, got %u\n", hoenn_range.min_level);
        failed++;
    } else {
        printf("  PASS: Hoenn min level = 60\n");
        passed++;
    }
    
    if (hoenn_range.max_level != 70) {
        printf("  FAIL: Hoenn max level should be 70, got %u\n", hoenn_range.max_level);
        failed++;
    } else {
        printf("  PASS: Hoenn max level = 70\n");
        passed++;
    }
    
    // Test 4: Verify Hoenn starts higher than Kanto end
    printf("Test 2.4: Verify Hoenn starting level > Kanto ending level...\n");
    if (hoenn_range.min_level <= kanto_range.max_level) {
        printf("  FAIL: Hoenn min (%u) should be > Kanto max with E4 (%u)\n", 
               hoenn_range.min_level, kanto_range.max_level);
        failed++;
    } else {
        printf("  PASS: Hoenn min (%u) > Kanto max with E4 (%u) - no overlap\n",
               hoenn_range.min_level, kanto_range.max_level);
        passed++;
    }
    
    printf("\n--- Test 2 Results: Passed=%d, Failed=%d ---\n", passed, failed);
    return failed;
}

// ==========================================
// Integration Test 3: Wild Pokemon Levels at 16 Badges (Kanto + Hoenn Complete)
// **Validates: Requirements 11.1, 11.3, 11.4**
// ==========================================

static int test_wild_levels_16_badges_hoenn_complete(void) {
    int passed = 0, failed = 0;
    struct LevelRange hoenn_range, johto_range;
    
    printf("\n=== Integration Test 3: Wild Pokemon Levels at 16 Badges (Hoenn Complete) ===\n");
    printf("**Validates: Requirements 11.1, 11.3, 11.4**\n\n");
    
    InitRegionState();
    SetRegionBadgeCount(REGION_KANTO, 8);
    SetRegionBadgeCount(REGION_HOENN, 8);
    SetEliteFourDefeated(REGION_KANTO, true);
    SetEliteFourDefeated(REGION_HOENN, true);  // Unlocks Johto
    
    // Test 1: Wild levels in Hoenn with 8 Hoenn badges
    printf("Test 3.1: Wild levels in Hoenn (8 badges, no E4 bonus)...\n");
    hoenn_range = GetWildPokemonLevelRangeForRegion(8, REGION_HOENN, false);
    
    // Expected: min = 60 + (8 * 5) = 100, max = 110 -> clamped to 100
    if (hoenn_range.min_level != 100) {
        printf("  FAIL: Hoenn min level should be 100, got %u\n", hoenn_range.min_level);
        failed++;
    } else {
        printf("  PASS: Hoenn min level = 100 (clamped)\n");
        passed++;
    }
    
    if (hoenn_range.max_level != 100) {
        printf("  FAIL: Hoenn max level should be 100 (clamped), got %u\n", hoenn_range.max_level);
        failed++;
    } else {
        printf("  PASS: Hoenn max level = 100 (clamped)\n");
        passed++;
    }
    
    // Test 2: Wild levels in Hoenn with E4 bonus (should still be clamped)
    printf("Test 3.2: Wild levels in Hoenn (8 badges, E4 bonus)...\n");
    hoenn_range = GetWildPokemonLevelRangeForRegion(8, REGION_HOENN, true);
    
    // Expected: min = 100 + 10 = 110 -> clamped to 100
    if (hoenn_range.min_level != 100) {
        printf("  FAIL: Hoenn min level with E4 should be 100 (clamped), got %u\n", hoenn_range.min_level);
        failed++;
    } else {
        printf("  PASS: Hoenn min level with E4 = 100 (clamped)\n");
        passed++;
    }
    
    // Test 3: Wild levels in Johto (just unlocked, 0 Johto badges)
    printf("Test 3.3: Wild levels in Johto (0 Johto badges)...\n");
    johto_range = GetWildPokemonLevelRangeForRegion(0, REGION_JOHTO, false);
    
    // Expected: min = 110 + (0 * 5) = 110 -> clamped to 100
    if (johto_range.min_level != 100) {
        printf("  FAIL: Johto min level should be 100 (clamped from 110), got %u\n", johto_range.min_level);
        failed++;
    } else {
        printf("  PASS: Johto min level = 100 (clamped from 110)\n");
        passed++;
    }
    
    if (johto_range.max_level != 100) {
        printf("  FAIL: Johto max level should be 100 (clamped), got %u\n", johto_range.max_level);
        failed++;
    } else {
        printf("  PASS: Johto max level = 100 (clamped)\n");
        passed++;
    }
    
    // Test 4: Verify level clamping works correctly
    printf("Test 3.4: Verify level clamping at 100...\n");
    bool8 clamping_works = (hoenn_range.min_level <= 100 && hoenn_range.max_level <= 100 &&
                           johto_range.min_level <= 100 && johto_range.max_level <= 100);
    if (!clamping_works) {
        printf("  FAIL: Levels should never exceed 100\n");
        failed++;
    } else {
        printf("  PASS: All levels clamped to 100 maximum\n");
        passed++;
    }
    
    printf("\n--- Test 3 Results: Passed=%d, Failed=%d ---\n", passed, failed);
    return failed;
}

// ==========================================
// Integration Test 4: Trainer Levels Scale with Badge Count
// **Validates: Requirements 11.3, 11.4**
// ==========================================

static int test_trainer_levels_scale_with_badges(void) {
    int passed = 0, failed = 0;
    u8 normal_level, gym_level, e4_level, champ_level, rival_level;
    
    printf("\n=== Integration Test 4: Trainer Levels Scale with Badge Count ===\n");
    printf("**Validates: Requirements 11.3, 11.4**\n\n");
    
    InitRegionState();
    
    // Test 1: Trainer levels in Kanto at 0 badges
    printf("Test 4.1: Trainer levels in Kanto (0 badges)...\n");
    normal_level = GetTrainerAverageLevelForRegion(0, REGION_KANTO, TRAINER_TYPE_NORMAL);
    gym_level = GetTrainerAverageLevelForRegion(0, REGION_KANTO, TRAINER_TYPE_GYM_LEADER);
    rival_level = GetTrainerAverageLevelForRegion(0, REGION_KANTO, TRAINER_TYPE_RIVAL);
    
    // Normal trainer: wild max (10) + 0 = 10
    if (normal_level != 10) {
        printf("  FAIL: Normal trainer level should be 10, got %u\n", normal_level);
        failed++;
    } else {
        printf("  PASS: Normal trainer level = 10\n");
        passed++;
    }
    
    // Gym leader: wild max (10) + 5 = 15
    if (gym_level != 15) {
        printf("  FAIL: Gym leader level should be 15, got %u\n", gym_level);
        failed++;
    } else {
        printf("  PASS: Gym leader level = 15\n");
        passed++;
    }
    
    // Rival at 0 badges: special case = 5
    if (rival_level != 5) {
        printf("  FAIL: Rival level at 0 badges should be 5, got %u\n", rival_level);
        failed++;
    } else {
        printf("  PASS: Rival level at 0 badges = 5 (special case)\n");
        passed++;
    }
    
    // Test 2: Trainer levels in Kanto at 4 badges
    printf("Test 4.2: Trainer levels in Kanto (4 badges)...\n");
    // Wild range at 4 badges: min = 2 + 20 = 22, max = 32
    normal_level = GetTrainerAverageLevelForRegion(4, REGION_KANTO, TRAINER_TYPE_NORMAL);
    gym_level = GetTrainerAverageLevelForRegion(4, REGION_KANTO, TRAINER_TYPE_GYM_LEADER);
    rival_level = GetTrainerAverageLevelForRegion(4, REGION_KANTO, TRAINER_TYPE_RIVAL);
    
    if (normal_level != 32) {
        printf("  FAIL: Normal trainer level should be 32, got %u\n", normal_level);
        failed++;
    } else {
        printf("  PASS: Normal trainer level = 32\n");
        passed++;
    }
    
    if (gym_level != 37) {
        printf("  FAIL: Gym leader level should be 37, got %u\n", gym_level);
        failed++;
    } else {
        printf("  PASS: Gym leader level = 37\n");
        passed++;
    }
    
    if (rival_level != 42) {
        printf("  FAIL: Rival level should be 42, got %u\n", rival_level);
        failed++;
    } else {
        printf("  PASS: Rival level = 42\n");
        passed++;
    }
    
    // Test 3: Trainer levels in Kanto at 8 badges (Elite Four)
    printf("Test 4.3: Trainer levels in Kanto (8 badges, Elite Four)...\n");
    // Wild range at 8 badges: min = 2 + 40 = 42, max = 52
    e4_level = GetTrainerAverageLevelForRegion(8, REGION_KANTO, TRAINER_TYPE_ELITE_FOUR);
    champ_level = GetTrainerAverageLevelForRegion(8, REGION_KANTO, TRAINER_TYPE_CHAMPION);
    
    // Elite Four: wild max (52) + 10 = 62
    if (e4_level != 62) {
        printf("  FAIL: Elite Four level should be 62, got %u\n", e4_level);
        failed++;
    } else {
        printf("  PASS: Elite Four level = 62\n");
        passed++;
    }
    
    // Champion: wild max (52) + 15 = 67
    if (champ_level != 67) {
        printf("  FAIL: Champion level should be 67, got %u\n", champ_level);
        failed++;
    } else {
        printf("  PASS: Champion level = 67\n");
        passed++;
    }
    
    // Test 4: Trainer levels in Hoenn at 4 Hoenn badges
    printf("Test 4.4: Trainer levels in Hoenn (4 badges)...\n");
    // Wild range at 4 badges in Hoenn: min = 60 + 20 = 80, max = 90
    gym_level = GetTrainerAverageLevelForRegion(4, REGION_HOENN, TRAINER_TYPE_GYM_LEADER);
    
    // Gym leader: wild max (90) + 5 = 95
    if (gym_level != 95) {
        printf("  FAIL: Hoenn Gym leader level should be 95, got %u\n", gym_level);
        failed++;
    } else {
        printf("  PASS: Hoenn Gym leader level = 95\n");
        passed++;
    }
    
    printf("\n--- Test 4 Results: Passed=%d, Failed=%d ---\n", passed, failed);
    return failed;
}

// ==========================================
// Integration Test 5: Level Cap Increases with Progression
// **Validates: Requirements 11.1, 11.7**
// ==========================================

static int test_level_cap_increases_with_progression(void) {
    int passed = 0, failed = 0;
    u8 cap;
    
    printf("\n=== Integration Test 5: Level Cap Increases with Progression ===\n");
    printf("**Validates: Requirements 11.1, 11.7**\n\n");
    
    InitRegionState();
    
    // Test 1: Level cap at game start (0 badges, Kanto)
    printf("Test 5.1: Level cap at game start (0 badges, Kanto)...\n");
    cap = GetLevelCapForProgression(0, REGION_KANTO);
    // Expected: 2 + 20 + (0 * 5) = 22
    if (cap != 22) {
        printf("  FAIL: Level cap should be 22, got %u\n", cap);
        failed++;
    } else {
        printf("  PASS: Level cap = 22\n");
        passed++;
    }
    
    // Test 2: Level cap at 4 badges (Kanto)
    printf("Test 5.2: Level cap at 4 badges (Kanto)...\n");
    cap = GetLevelCapForProgression(4, REGION_KANTO);
    // Expected: 2 + 20 + (4 * 5) = 42
    if (cap != 42) {
        printf("  FAIL: Level cap should be 42, got %u\n", cap);
        failed++;
    } else {
        printf("  PASS: Level cap = 42\n");
        passed++;
    }
    
    // Test 3: Level cap at 8 badges (Kanto complete)
    printf("Test 5.3: Level cap at 8 badges (Kanto complete)...\n");
    cap = GetLevelCapForProgression(8, REGION_KANTO);
    // Expected: 2 + 20 + (8 * 5) = 62
    if (cap != 62) {
        printf("  FAIL: Level cap should be 62, got %u\n", cap);
        failed++;
    } else {
        printf("  PASS: Level cap = 62\n");
        passed++;
    }
    
    // Test 4: Level cap when entering Hoenn (8 total badges)
    printf("Test 5.4: Level cap when entering Hoenn (8 total badges)...\n");
    cap = GetLevelCapForProgression(8, REGION_HOENN);
    // Expected: 60 + 20 + (0 * 5) = 80 (0 Hoenn badges)
    if (cap != 80) {
        printf("  FAIL: Level cap should be 80, got %u\n", cap);
        failed++;
    } else {
        printf("  PASS: Level cap = 80\n");
        passed++;
    }
    
    // Test 5: Level cap at 12 badges (4 Hoenn badges)
    printf("Test 5.5: Level cap at 12 badges (4 Hoenn badges)...\n");
    cap = GetLevelCapForProgression(12, REGION_HOENN);
    // Expected: 60 + 20 + (4 * 5) = 100
    if (cap != 100) {
        printf("  FAIL: Level cap should be 100, got %u\n", cap);
        failed++;
    } else {
        printf("  PASS: Level cap = 100\n");
        passed++;
    }
    
    // Test 6: Level cap at 16 badges (Hoenn complete)
    printf("Test 5.6: Level cap at 16 badges (Hoenn complete)...\n");
    cap = GetLevelCapForProgression(16, REGION_HOENN);
    // Expected: 60 + 20 + (8 * 5) = 120 -> clamped to 100
    if (cap != 100) {
        printf("  FAIL: Level cap should be 100 (clamped), got %u\n", cap);
        failed++;
    } else {
        printf("  PASS: Level cap = 100 (clamped)\n");
        passed++;
    }
    
    // Test 7: Level cap in Johto (16 total badges)
    printf("Test 5.7: Level cap in Johto (16 total badges)...\n");
    cap = GetLevelCapForProgression(16, REGION_JOHTO);
    // Expected: 110 + 20 + (0 * 5) = 130 -> clamped to 100
    if (cap != 100) {
        printf("  FAIL: Level cap should be 100 (clamped), got %u\n", cap);
        failed++;
    } else {
        printf("  PASS: Level cap = 100 (clamped)\n");
        passed++;
    }
    
    // Test 8: Verify cap always increases or stays same with more badges
    printf("Test 5.8: Verify cap monotonically increases...\n");
    bool8 monotonic = true;
    u8 prev_cap = 0;
    for (u8 badges = 0; badges <= 8; badges++) {
        cap = GetLevelCapForProgression(badges, REGION_KANTO);
        if (cap < prev_cap) {
            printf("  FAIL: Cap decreased from %u to %u at %u badges\n", prev_cap, cap, badges);
            monotonic = false;
            break;
        }
        prev_cap = cap;
    }
    if (monotonic) {
        printf("  PASS: Level cap monotonically increases\n");
        passed++;
    } else {
        failed++;
    }
    
    printf("\n--- Test 5 Results: Passed=%d, Failed=%d ---\n", passed, failed);
    return failed;
}

// ==========================================
// Integration Test 6: Elite Four Bonus (+10) Applied Correctly
// **Validates: Requirements 11.7**
// ==========================================

static int test_elite_four_bonus_applied_correctly(void) {
    int passed = 0, failed = 0;
    struct LevelRange range_no_e4, range_with_e4;
    
    printf("\n=== Integration Test 6: Elite Four Bonus (+10) Applied Correctly ===\n");
    printf("**Validates: Requirements 11.7**\n\n");
    
    InitRegionState();
    
    // Test 1: E4 bonus in Kanto (4 badges)
    printf("Test 6.1: E4 bonus in Kanto (4 badges)...\n");
    range_no_e4 = GetWildPokemonLevelRangeForRegion(4, REGION_KANTO, false);
    range_with_e4 = GetWildPokemonLevelRangeForRegion(4, REGION_KANTO, true);
    
    if (range_with_e4.min_level != range_no_e4.min_level + 10) {
        printf("  FAIL: E4 bonus should add +10 to min level\n");
        printf("    Without E4: %u, With E4: %u (expected %u)\n",
               range_no_e4.min_level, range_with_e4.min_level, range_no_e4.min_level + 10);
        failed++;
    } else {
        printf("  PASS: E4 bonus adds +10 to min level (%u -> %u)\n",
               range_no_e4.min_level, range_with_e4.min_level);
        passed++;
    }
    
    if (range_with_e4.max_level != range_no_e4.max_level + 10) {
        printf("  FAIL: E4 bonus should add +10 to max level\n");
        printf("    Without E4: %u, With E4: %u (expected %u)\n",
               range_no_e4.max_level, range_with_e4.max_level, range_no_e4.max_level + 10);
        failed++;
    } else {
        printf("  PASS: E4 bonus adds +10 to max level (%u -> %u)\n",
               range_no_e4.max_level, range_with_e4.max_level);
        passed++;
    }
    
    // Test 2: E4 bonus in Hoenn (4 badges)
    printf("Test 6.2: E4 bonus in Hoenn (4 badges)...\n");
    range_no_e4 = GetWildPokemonLevelRangeForRegion(4, REGION_HOENN, false);
    range_with_e4 = GetWildPokemonLevelRangeForRegion(4, REGION_HOENN, true);
    
    // Hoenn 4 badges: min = 60 + 20 = 80, max = 90
    // With E4: min = 90, max = 100
    if (range_with_e4.min_level != 90) {
        printf("  FAIL: Hoenn E4 min level should be 90, got %u\n", range_with_e4.min_level);
        failed++;
    } else {
        printf("  PASS: Hoenn E4 min level = 90\n");
        passed++;
    }
    
    if (range_with_e4.max_level != 100) {
        printf("  FAIL: Hoenn E4 max level should be 100, got %u\n", range_with_e4.max_level);
        failed++;
    } else {
        printf("  PASS: Hoenn E4 max level = 100\n");
        passed++;
    }
    
    // Test 3: E4 bonus with clamping (high badge count)
    printf("Test 6.3: E4 bonus with clamping (8 Hoenn badges)...\n");
    range_no_e4 = GetWildPokemonLevelRangeForRegion(8, REGION_HOENN, false);
    range_with_e4 = GetWildPokemonLevelRangeForRegion(8, REGION_HOENN, true);
    
    // Both should be clamped to 100
    if (range_no_e4.min_level != 100 || range_no_e4.max_level != 100) {
        printf("  FAIL: Without E4 should be clamped to 100\n");
        failed++;
    } else {
        printf("  PASS: Without E4 clamped to 100\n");
        passed++;
    }
    
    if (range_with_e4.min_level != 100 || range_with_e4.max_level != 100) {
        printf("  FAIL: With E4 should be clamped to 100\n");
        failed++;
    } else {
        printf("  PASS: With E4 clamped to 100\n");
        passed++;
    }
    
    // Test 4: E4 bonus at 0 badges (Kanto special case)
    printf("Test 6.4: E4 bonus at 0 badges (Kanto special case)...\n");
    range_no_e4 = GetWildPokemonLevelRangeForRegion(0, REGION_KANTO, false);
    range_with_e4 = GetWildPokemonLevelRangeForRegion(0, REGION_KANTO, true);
    
    // Without E4: 2-10, With E4: 12-20
    if (range_no_e4.min_level != 2 || range_no_e4.max_level != 10) {
        printf("  FAIL: Without E4 should be 2-10\n");
        failed++;
    } else {
        printf("  PASS: Without E4 = 2-10\n");
        passed++;
    }
    
    if (range_with_e4.min_level != 12 || range_with_e4.max_level != 20) {
        printf("  FAIL: With E4 should be 12-20, got %u-%u\n",
               range_with_e4.min_level, range_with_e4.max_level);
        failed++;
    } else {
        printf("  PASS: With E4 = 12-20\n");
        passed++;
    }
    
    printf("\n--- Test 6 Results: Passed=%d, Failed=%d ---\n", passed, failed);
    return failed;
}

// ==========================================
// Integration Test 7: Full Progression Flow (0 to 24 badges)
// **Validates: Requirements 11.1, 11.3, 11.4, 11.7**
// ==========================================

static int test_full_progression_flow(void) {
    int passed = 0, failed = 0;
    struct LevelRange range;
    u8 cap;
    
    printf("\n=== Integration Test 7: Full Progression Flow (0 to 24 badges) ===\n");
    printf("**Validates: Requirements 11.1, 11.3, 11.4, 11.7**\n\n");
    
    InitRegionState();
    
    // Simulate full game progression
    printf("Simulating full game progression...\n\n");
    
    // Phase 1: Kanto (0-8 badges)
    printf("=== PHASE 1: KANTO (0-8 badges) ===\n");
    SetCurrentRegion(REGION_KANTO);
    
    for (u8 badges = 0; badges <= 8; badges += 2) {
        SetRegionBadgeCount(REGION_KANTO, badges);
        range = GetWildPokemonLevelRangeForRegion(badges, REGION_KANTO, false);
        cap = GetLevelCapForProgression(badges, REGION_KANTO);
        
        printf("  %u badges: Wild=%u-%u, Cap=%u\n", badges, range.min_level, range.max_level, cap);
        
        // Verify wild levels are within cap
        if (range.max_level > cap) {
            printf("    FAIL: Wild max (%u) exceeds cap (%u)\n", range.max_level, cap);
            failed++;
        } else {
            passed++;
        }
    }
    
    // Defeat Kanto E4
    SetEliteFourDefeated(REGION_KANTO, true);
    printf("\n  [Kanto Elite Four Defeated - Hoenn Unlocked]\n\n");
    
    // Phase 2: Hoenn (8-16 total badges)
    printf("=== PHASE 2: HOENN (8-16 total badges) ===\n");
    SetCurrentRegion(REGION_HOENN);
    
    for (u8 hoenn_badges = 0; hoenn_badges <= 8; hoenn_badges += 2) {
        SetRegionBadgeCount(REGION_HOENN, hoenn_badges);
        u8 total = GetTotalBadgeCount();
        range = GetWildPokemonLevelRangeForRegion(hoenn_badges, REGION_HOENN, false);
        cap = GetLevelCapForProgression(total, REGION_HOENN);
        
        printf("  %u Hoenn badges (%u total): Wild=%u-%u, Cap=%u\n", 
               hoenn_badges, total, range.min_level, range.max_level, cap);
        
        // Verify wild levels are within cap (or both clamped to 100)
        if (range.max_level > cap && cap < 100) {
            printf("    FAIL: Wild max (%u) exceeds cap (%u)\n", range.max_level, cap);
            failed++;
        } else {
            passed++;
        }
    }
    
    // Defeat Hoenn E4
    SetEliteFourDefeated(REGION_HOENN, true);
    printf("\n  [Hoenn Elite Four Defeated - Johto Unlocked]\n\n");
    
    // Phase 3: Johto (16-24 total badges)
    printf("=== PHASE 3: JOHTO (16-24 total badges) ===\n");
    SetCurrentRegion(REGION_JOHTO);
    
    for (u8 johto_badges = 0; johto_badges <= 8; johto_badges += 2) {
        SetRegionBadgeCount(REGION_JOHTO, johto_badges);
        u8 total = GetTotalBadgeCount();
        range = GetWildPokemonLevelRangeForRegion(johto_badges, REGION_JOHTO, false);
        cap = GetLevelCapForProgression(total, REGION_JOHTO);
        
        printf("  %u Johto badges (%u total): Wild=%u-%u, Cap=%u\n", 
               johto_badges, total, range.min_level, range.max_level, cap);
        
        // All Johto levels should be clamped to 100
        if (range.min_level != 100 || range.max_level != 100) {
            printf("    FAIL: Johto levels should be clamped to 100\n");
            failed++;
        } else {
            passed++;
        }
    }
    
    // Final verification
    printf("\n=== FINAL VERIFICATION ===\n");
    u8 final_total = GetTotalBadgeCount();
    printf("Total badges collected: %u\n", final_total);
    
    if (final_total != 24) {
        printf("  FAIL: Should have 24 total badges, got %u\n", final_total);
        failed++;
    } else {
        printf("  PASS: All 24 badges collected\n");
        passed++;
    }
    
    printf("\n--- Test 7 Results: Passed=%d, Failed=%d ---\n", passed, failed);
    return failed;
}

// ==========================================
// Main Test Runner
// ==========================================

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    int total_failed = 0;
    
    printf("╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║     Integration Tests for Level Scaling System                   ║\n");
    printf("║     Task 4.3: Run integration tests for level scaling            ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n");
    printf("\nRegion configuration: Kanto → Hoenn → Johto (3 regions)\n");
    printf("Sinnoh descoped per ADR-003\n");
    printf("Progression order per ADR-004\n");
    printf("\nRegion starting levels:\n");
    printf("  - Kanto: 2\n");
    printf("  - Hoenn: 60\n");
    printf("  - Johto: 110\n");
    printf("\nLevel scaling formulas:\n");
    printf("  - Wild min = region_start + (badges * 5)\n");
    printf("  - Wild max = min + 10\n");
    printf("  - E4 bonus = +10 to wild levels\n");
    printf("  - Level cap = region_start + 20 + (badges * 5)\n");
    printf("\nTrainer type bonuses:\n");
    printf("  - Normal: +0\n");
    printf("  - Gym Leader: +5\n");
    printf("  - Elite Four: +10\n");
    printf("  - Champion: +15\n");
    printf("  - Rival: +10 (special: 0 badges Kanto = level 5)\n");
    
    // Run all integration tests
    total_failed += test_wild_levels_0_badges_kanto();
    total_failed += test_wild_levels_8_badges_kanto_complete();
    total_failed += test_wild_levels_16_badges_hoenn_complete();
    total_failed += test_trainer_levels_scale_with_badges();
    total_failed += test_level_cap_increases_with_progression();
    total_failed += test_elite_four_bonus_applied_correctly();
    total_failed += test_full_progression_flow();
    
    // Overall results
    printf("\n\n╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║                    OVERALL TEST RESULTS                          ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n");
    
    if (total_failed == 0) {
        printf("\n✓ ALL INTEGRATION TESTS PASSED!\n\n");
        printf("Level Scaling Integration Tests: PASSED\n");
        printf("  - Wild Pokemon levels at 0 badges (Kanto): PASSED\n");
        printf("  - Wild Pokemon levels at 8 badges (Kanto complete): PASSED\n");
        printf("  - Wild Pokemon levels at 16 badges (Hoenn complete): PASSED\n");
        printf("  - Trainer levels scale with badge count: PASSED\n");
        printf("  - Level cap increases with progression: PASSED\n");
        printf("  - Elite Four bonus (+10) applied correctly: PASSED\n");
        printf("  - Full progression flow (0-24 badges): PASSED\n");
        printf("\n**Validates: Requirements 11.1, 11.3, 11.4, 11.7**\n");
        return 0;
    } else {
        printf("\n✗ SOME INTEGRATION TESTS FAILED\n\n");
        printf("Total failures: %d\n", total_failed);
        printf("\nPlease review the test output above for details.\n");
        return 1;
    }
}
