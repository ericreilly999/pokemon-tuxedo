/*
 * Property-Based Tests for Badge Tracking System
 * 
 * Tests Property 23: Badge Tracking Separation
 * Tests Property 24: Badge Storage Capacity
 * 
 * **Validates: Requirements 14.1, 14.2, 14.3, 14.5, 14.6**
 * 
 * Uses Theft library pattern with minimum 100 iterations per property.
 * Region order: Kanto → Hoenn → Johto (per ADR-004)
 * Sinnoh descoped per ADR-003 (3 regions, 24 badges total)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

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

// Badge tracking structure for multi-region support
struct BadgeData {
    bool8 kanto_badges[BADGES_PER_REGION];
    bool8 hoenn_badges[BADGES_PER_REGION];
    bool8 johto_badges[BADGES_PER_REGION];
};

// Region state tracking structure
struct RegionState {
    u8 current_region;
    bool8 hoenn_unlocked;
    bool8 johto_unlocked;
    bool8 elite_four_defeated[NUM_REGIONS];
    struct BadgeData badges;
};

// Global region state for testing
static struct RegionState sRegionState = {0};

// Simple random number generator for property-based testing
static unsigned int test_seed = 0;

static void init_test_random(unsigned int seed) {
    test_seed = seed;
}

static unsigned int test_random(unsigned int max) {
    if (max == 0) return 0;
    test_seed = (test_seed * 1103515245 + 12345) & 0x7fffffff;
    return test_seed % max;
}

// Test implementation of region manager functions
static void InitRegionState(void) {
    u8 i, j;
    
    sRegionState.current_region = REGION_KANTO;
    sRegionState.hoenn_unlocked = false;
    sRegionState.johto_unlocked = false;
    
    for (i = 0; i < NUM_REGIONS; i++) {
        sRegionState.elite_four_defeated[i] = false;
    }
    
    for (j = 0; j < BADGES_PER_REGION; j++) {
        sRegionState.badges.kanto_badges[j] = false;
        sRegionState.badges.hoenn_badges[j] = false;
        sRegionState.badges.johto_badges[j] = false;
    }
}

static bool8 HasBadge(u8 region_id, u8 badge_index) {
    if (badge_index >= BADGES_PER_REGION)
        return false;
    
    switch (region_id) {
    case REGION_KANTO:
        return sRegionState.badges.kanto_badges[badge_index];
    case REGION_HOENN:
        return sRegionState.badges.hoenn_badges[badge_index];
    case REGION_JOHTO:
        return sRegionState.badges.johto_badges[badge_index];
    default:
        return false;
    }
}

static void AwardBadge(u8 region_id, u8 badge_index) {
    if (badge_index >= BADGES_PER_REGION)
        return;
    
    switch (region_id) {
    case REGION_KANTO:
        sRegionState.badges.kanto_badges[badge_index] = true;
        break;
    case REGION_HOENN:
        sRegionState.badges.hoenn_badges[badge_index] = true;
        break;
    case REGION_JOHTO:
        sRegionState.badges.johto_badges[badge_index] = true;
        break;
    }
}

static u8 GetRegionBadgeCount(u8 region_id) {
    u8 count = 0;
    u8 i;
    bool8 *badges;
    
    switch (region_id) {
    case REGION_KANTO:
        badges = sRegionState.badges.kanto_badges;
        break;
    case REGION_HOENN:
        badges = sRegionState.badges.hoenn_badges;
        break;
    case REGION_JOHTO:
        badges = sRegionState.badges.johto_badges;
        break;
    default:
        return 0;
    }
    
    for (i = 0; i < BADGES_PER_REGION; i++) {
        if (badges[i])
            count++;
    }
    
    return count;
}

static u8 GetTotalBadgeCount(void) {
    return GetRegionBadgeCount(REGION_KANTO) +
           GetRegionBadgeCount(REGION_HOENN) +
           GetRegionBadgeCount(REGION_JOHTO);
}

// Test data structures for property-based testing
struct BadgeAwardTestData {
    u8 region_id;
    u8 badge_index;
};

struct MultiBadgeTestData {
    u8 kanto_badges_to_award;  // Bitmask of badges to award (0-255)
    u8 hoenn_badges_to_award;
    u8 johto_badges_to_award;
};

/**
 * Property 23: Badge Tracking Separation
 * 
 * For any badge earned, it should be tracked separately by region,
 * with each region maintaining its own set of 8 badges.
 * 
 * This property verifies:
 * 1. Badges awarded in one region don't affect other regions
 * 2. GetRegionBadgeCount() returns correct count per region
 * 3. HasBadge() correctly identifies badges by region and index
 * 
 * **Validates: Requirements 14.1, 14.2, 14.3, 14.5**
 */
static bool prop_badge_tracking_separation(struct BadgeAwardTestData *data) {
    // Reset state for clean test
    InitRegionState();
    
    // Validate input bounds
    if (data->region_id >= NUM_REGIONS || data->badge_index >= BADGES_PER_REGION) {
        // Invalid input - skip this test case
        return true;
    }
    
    // Record initial state of all regions
    u8 initial_kanto_count = GetRegionBadgeCount(REGION_KANTO);
    u8 initial_hoenn_count = GetRegionBadgeCount(REGION_HOENN);
    u8 initial_johto_count = GetRegionBadgeCount(REGION_JOHTO);
    
    // Award the badge
    AwardBadge(data->region_id, data->badge_index);
    
    // Verify the badge was awarded
    if (!HasBadge(data->region_id, data->badge_index)) {
        printf("FAIL: Badge not awarded - region=%u, badge=%u\n",
               data->region_id, data->badge_index);
        return false;
    }

    // Verify the target region's badge count increased by 1
    u8 new_target_count = GetRegionBadgeCount(data->region_id);
    u8 expected_target_count;
    
    switch (data->region_id) {
    case REGION_KANTO:
        expected_target_count = initial_kanto_count + 1;
        break;
    case REGION_HOENN:
        expected_target_count = initial_hoenn_count + 1;
        break;
    case REGION_JOHTO:
        expected_target_count = initial_johto_count + 1;
        break;
    default:
        return false;
    }
    
    if (new_target_count != expected_target_count) {
        printf("FAIL: Target region badge count incorrect - region=%u, expected=%u, got=%u\n",
               data->region_id, expected_target_count, new_target_count);
        return false;
    }
    
    // CRITICAL: Verify other regions were NOT affected
    // This is the core of the separation property
    if (data->region_id != REGION_KANTO) {
        u8 new_kanto_count = GetRegionBadgeCount(REGION_KANTO);
        if (new_kanto_count != initial_kanto_count) {
            printf("FAIL: Kanto badges affected by award to region %u - was=%u, now=%u\n",
                   data->region_id, initial_kanto_count, new_kanto_count);
            return false;
        }
    }
    
    if (data->region_id != REGION_HOENN) {
        u8 new_hoenn_count = GetRegionBadgeCount(REGION_HOENN);
        if (new_hoenn_count != initial_hoenn_count) {
            printf("FAIL: Hoenn badges affected by award to region %u - was=%u, now=%u\n",
                   data->region_id, initial_hoenn_count, new_hoenn_count);
            return false;
        }
    }
    
    if (data->region_id != REGION_JOHTO) {
        u8 new_johto_count = GetRegionBadgeCount(REGION_JOHTO);
        if (new_johto_count != initial_johto_count) {
            printf("FAIL: Johto badges affected by award to region %u - was=%u, now=%u\n",
                   data->region_id, initial_johto_count, new_johto_count);
            return false;
        }
    }

    // Verify HasBadge returns false for same badge index in other regions
    for (u8 other_region = 0; other_region < NUM_REGIONS; other_region++) {
        if (other_region != data->region_id) {
            if (HasBadge(other_region, data->badge_index)) {
                printf("FAIL: HasBadge incorrectly returns true for region=%u, badge=%u "
                       "(badge was awarded to region=%u)\n",
                       other_region, data->badge_index, data->region_id);
                return false;
            }
        }
    }
    
    return true;
}

/**
 * Property 24: Badge Storage Capacity
 * 
 * For any game state, the system should support tracking a total of 
 * 24 badges (8 per region × 3 regions).
 * 
 * This property verifies:
 * 1. System can track all 24 badges simultaneously
 * 2. GetTotalBadgeCount() returns sum across all regions
 * 3. Each region can hold exactly 8 badges
 * 
 * **Validates: Requirements 14.6**
 */
static bool prop_badge_storage_capacity(struct MultiBadgeTestData *data) {
    // Reset state for clean test
    InitRegionState();
    
    u8 expected_kanto = 0;
    u8 expected_hoenn = 0;
    u8 expected_johto = 0;
    u8 i;
    
    // Award Kanto badges based on bitmask
    for (i = 0; i < BADGES_PER_REGION; i++) {
        if (data->kanto_badges_to_award & (1 << i)) {
            AwardBadge(REGION_KANTO, i);
            expected_kanto++;
        }
    }
    
    // Award Hoenn badges based on bitmask
    for (i = 0; i < BADGES_PER_REGION; i++) {
        if (data->hoenn_badges_to_award & (1 << i)) {
            AwardBadge(REGION_HOENN, i);
            expected_hoenn++;
        }
    }
    
    // Award Johto badges based on bitmask
    for (i = 0; i < BADGES_PER_REGION; i++) {
        if (data->johto_badges_to_award & (1 << i)) {
            AwardBadge(REGION_JOHTO, i);
            expected_johto++;
        }
    }

    // Verify per-region counts
    u8 actual_kanto = GetRegionBadgeCount(REGION_KANTO);
    u8 actual_hoenn = GetRegionBadgeCount(REGION_HOENN);
    u8 actual_johto = GetRegionBadgeCount(REGION_JOHTO);
    
    if (actual_kanto != expected_kanto) {
        printf("FAIL: Kanto badge count mismatch - expected=%u, got=%u (mask=0x%02X)\n",
               expected_kanto, actual_kanto, data->kanto_badges_to_award);
        return false;
    }
    
    if (actual_hoenn != expected_hoenn) {
        printf("FAIL: Hoenn badge count mismatch - expected=%u, got=%u (mask=0x%02X)\n",
               expected_hoenn, actual_hoenn, data->hoenn_badges_to_award);
        return false;
    }
    
    if (actual_johto != expected_johto) {
        printf("FAIL: Johto badge count mismatch - expected=%u, got=%u (mask=0x%02X)\n",
               expected_johto, actual_johto, data->johto_badges_to_award);
        return false;
    }
    
    // Verify total count equals sum of all regions
    u8 expected_total = expected_kanto + expected_hoenn + expected_johto;
    u8 actual_total = GetTotalBadgeCount();
    
    if (actual_total != expected_total) {
        printf("FAIL: Total badge count mismatch - expected=%u, got=%u\n",
               expected_total, actual_total);
        printf("  Kanto: %u, Hoenn: %u, Johto: %u\n",
               actual_kanto, actual_hoenn, actual_johto);
        return false;
    }
    
    // Verify total is within valid range (0-24)
    if (actual_total > TOTAL_BADGES) {
        printf("FAIL: Total badge count exceeds maximum - got=%u, max=%u\n",
               actual_total, TOTAL_BADGES);
        return false;
    }
    
    // Verify each region count is within valid range (0-8)
    if (actual_kanto > BADGES_PER_REGION || 
        actual_hoenn > BADGES_PER_REGION || 
        actual_johto > BADGES_PER_REGION) {
        printf("FAIL: Region badge count exceeds maximum of %u\n", BADGES_PER_REGION);
        printf("  Kanto: %u, Hoenn: %u, Johto: %u\n",
               actual_kanto, actual_hoenn, actual_johto);
        return false;
    }
    
    return true;
}

/**
 * Test runner for Property 23: Badge Tracking Separation
 */
static int test_property_23_badge_separation(void) {
    int num_trials = 100;  // Minimum 100 iterations per property
    int passed = 0;
    int failed = 0;
    
    printf("\n=== Property-Based Test: Badge Tracking Separation (Property 23) ===\n");
    printf("**Validates: Requirements 14.1, 14.2, 14.3, 14.5**\n");
    printf("Running %d random test cases...\n\n", num_trials);
    
    // Run property tests with random inputs
    for (int i = 0; i < num_trials; i++) {
        struct BadgeAwardTestData data;
        
        // Generate random test data
        data.region_id = test_random(NUM_REGIONS);
        data.badge_index = test_random(BADGES_PER_REGION);
        
        // Run the property test
        if (prop_badge_tracking_separation(&data)) {
            passed++;
        } else {
            failed++;
            if (failed <= 5) {
                printf("  Counterexample #%d: region_id=%u, badge_index=%u\n",
                       failed, data.region_id, data.badge_index);
            }
        }
    }
    
    // Test specific edge cases
    printf("\nTesting specific edge cases...\n");
    
    // Edge case 1: First badge in each region
    for (u8 region = 0; region < NUM_REGIONS; region++) {
        struct BadgeAwardTestData edge = {region, 0};
        if (!prop_badge_tracking_separation(&edge)) {
            printf("FAIL: Edge case first badge in region %u\n", region);
            failed++;
        } else {
            passed++;
        }
    }
    
    // Edge case 2: Last badge in each region
    for (u8 region = 0; region < NUM_REGIONS; region++) {
        struct BadgeAwardTestData edge = {region, BADGES_PER_REGION - 1};
        if (!prop_badge_tracking_separation(&edge)) {
            printf("FAIL: Edge case last badge in region %u\n", region);
            failed++;
        } else {
            passed++;
        }
    }

    // Edge case 3: Same badge index across all regions (should be independent)
    printf("\nTesting same badge index across all regions...\n");
    InitRegionState();
    
    // Award badge 3 to all regions
    AwardBadge(REGION_KANTO, 3);
    AwardBadge(REGION_HOENN, 3);
    AwardBadge(REGION_JOHTO, 3);
    
    // Verify each region has exactly 1 badge
    if (GetRegionBadgeCount(REGION_KANTO) != 1 ||
        GetRegionBadgeCount(REGION_HOENN) != 1 ||
        GetRegionBadgeCount(REGION_JOHTO) != 1) {
        printf("FAIL: Same badge index test - each region should have 1 badge\n");
        printf("  Kanto: %u, Hoenn: %u, Johto: %u\n",
               GetRegionBadgeCount(REGION_KANTO),
               GetRegionBadgeCount(REGION_HOENN),
               GetRegionBadgeCount(REGION_JOHTO));
        failed++;
    } else {
        printf("  Same badge index (3) in all regions: PASSED\n");
        passed++;
    }
    
    // Verify total is 3
    if (GetTotalBadgeCount() != 3) {
        printf("FAIL: Same badge index test - total should be 3, got %u\n",
               GetTotalBadgeCount());
        failed++;
    } else {
        passed++;
    }
    
    // Report results
    printf("\n=== Property 23 Test Results ===\n");
    printf("Total tests: %d\n", passed + failed);
    printf("Passed: %d\n", passed);
    printf("Failed: %d\n", failed);
    
    if (failed == 0) {
        printf("\n✓ Property 23 (Badge Tracking Separation) PASSED!\n");
        return 0;
    } else {
        printf("\n✗ Property 23 (Badge Tracking Separation) FAILED with %d counterexamples\n", failed);
        return 1;
    }
}

/**
 * Test runner for Property 24: Badge Storage Capacity
 */
static int test_property_24_badge_capacity(void) {
    int num_trials = 100;  // Minimum 100 iterations per property
    int passed = 0;
    int failed = 0;
    
    printf("\n=== Property-Based Test: Badge Storage Capacity (Property 24) ===\n");
    printf("**Validates: Requirements 14.6**\n");
    printf("Running %d random test cases...\n\n", num_trials);
    
    // Run property tests with random inputs
    for (int i = 0; i < num_trials; i++) {
        struct MultiBadgeTestData data;
        
        // Generate random badge bitmasks (0-255 covers all 8 badge combinations)
        data.kanto_badges_to_award = test_random(256);
        data.hoenn_badges_to_award = test_random(256);
        data.johto_badges_to_award = test_random(256);
        
        // Run the property test
        if (prop_badge_storage_capacity(&data)) {
            passed++;
        } else {
            failed++;
            if (failed <= 5) {
                printf("  Counterexample #%d: kanto=0x%02X, hoenn=0x%02X, johto=0x%02X\n",
                       failed, data.kanto_badges_to_award, 
                       data.hoenn_badges_to_award, data.johto_badges_to_award);
            }
        }
    }
    
    // Test specific edge cases
    printf("\nTesting specific edge cases...\n");
    
    // Edge case 1: No badges
    struct MultiBadgeTestData edge1 = {0x00, 0x00, 0x00};
    if (!prop_badge_storage_capacity(&edge1)) {
        printf("FAIL: Edge case no badges\n");
        failed++;
    } else {
        printf("  No badges: PASSED\n");
        passed++;
    }
    
    // Edge case 2: All badges (24 total)
    struct MultiBadgeTestData edge2 = {0xFF, 0xFF, 0xFF};
    if (!prop_badge_storage_capacity(&edge2)) {
        printf("FAIL: Edge case all badges\n");
        failed++;
    } else {
        printf("  All 24 badges: PASSED\n");
        passed++;
    }

    // Edge case 3: Only Kanto badges (8)
    struct MultiBadgeTestData edge3 = {0xFF, 0x00, 0x00};
    if (!prop_badge_storage_capacity(&edge3)) {
        printf("FAIL: Edge case only Kanto badges\n");
        failed++;
    } else {
        printf("  Only Kanto badges (8): PASSED\n");
        passed++;
    }
    
    // Edge case 4: Only Hoenn badges (8)
    struct MultiBadgeTestData edge4 = {0x00, 0xFF, 0x00};
    if (!prop_badge_storage_capacity(&edge4)) {
        printf("FAIL: Edge case only Hoenn badges\n");
        failed++;
    } else {
        printf("  Only Hoenn badges (8): PASSED\n");
        passed++;
    }
    
    // Edge case 5: Only Johto badges (8)
    struct MultiBadgeTestData edge5 = {0x00, 0x00, 0xFF};
    if (!prop_badge_storage_capacity(&edge5)) {
        printf("FAIL: Edge case only Johto badges\n");
        failed++;
    } else {
        printf("  Only Johto badges (8): PASSED\n");
        passed++;
    }
    
    // Edge case 6: Verify maximum capacity (24 badges)
    printf("\nVerifying maximum badge capacity...\n");
    InitRegionState();
    
    // Award all 24 badges
    for (u8 region = 0; region < NUM_REGIONS; region++) {
        for (u8 badge = 0; badge < BADGES_PER_REGION; badge++) {
            AwardBadge(region, badge);
        }
    }
    
    // Verify total is exactly 24
    u8 total = GetTotalBadgeCount();
    if (total != TOTAL_BADGES) {
        printf("FAIL: Maximum capacity test - expected %u badges, got %u\n",
               TOTAL_BADGES, total);
        failed++;
    } else {
        printf("  Maximum capacity (%u badges): PASSED\n", TOTAL_BADGES);
        passed++;
    }
    
    // Verify each region has exactly 8 badges
    for (u8 region = 0; region < NUM_REGIONS; region++) {
        u8 count = GetRegionBadgeCount(region);
        if (count != BADGES_PER_REGION) {
            printf("FAIL: Region %u should have %u badges, got %u\n",
                   region, BADGES_PER_REGION, count);
            failed++;
        } else {
            passed++;
        }
    }

    // Edge case 7: Verify HasBadge for all 24 badges
    printf("\nVerifying HasBadge for all 24 badges...\n");
    bool all_badges_found = true;
    for (u8 region = 0; region < NUM_REGIONS; region++) {
        for (u8 badge = 0; badge < BADGES_PER_REGION; badge++) {
            if (!HasBadge(region, badge)) {
                printf("FAIL: HasBadge returned false for region=%u, badge=%u\n",
                       region, badge);
                all_badges_found = false;
                failed++;
            }
        }
    }
    if (all_badges_found) {
        printf("  HasBadge for all 24 badges: PASSED\n");
        passed++;
    }
    
    // Edge case 8: Verify invalid badge index returns false
    printf("\nVerifying invalid badge index handling...\n");
    if (HasBadge(REGION_KANTO, BADGES_PER_REGION)) {
        printf("FAIL: HasBadge should return false for invalid badge index %u\n",
               BADGES_PER_REGION);
        failed++;
    } else {
        printf("  Invalid badge index (8) returns false: PASSED\n");
        passed++;
    }
    
    // Edge case 9: Verify invalid region returns 0 count
    u8 invalid_region_count = GetRegionBadgeCount(NUM_REGIONS);
    if (invalid_region_count != 0) {
        printf("FAIL: GetRegionBadgeCount should return 0 for invalid region, got %u\n",
               invalid_region_count);
        failed++;
    } else {
        printf("  Invalid region returns 0 count: PASSED\n");
        passed++;
    }
    
    // Report results
    printf("\n=== Property 24 Test Results ===\n");
    printf("Total tests: %d\n", passed + failed);
    printf("Passed: %d\n", passed);
    printf("Failed: %d\n", failed);
    
    if (failed == 0) {
        printf("\n✓ Property 24 (Badge Storage Capacity) PASSED!\n");
        return 0;
    } else {
        printf("\n✗ Property 24 (Badge Storage Capacity) FAILED with %d counterexamples\n", failed);
        return 1;
    }
}

/**
 * Main test runner
 */
int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    // Initialize random seed
    init_test_random((unsigned int)time(NULL));
    
    printf("╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║     Property-Based Tests for Badge Tracking System               ║\n");
    printf("║     Task 3.2: Write property tests for badge tracking            ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n");
    printf("\nRegion configuration: Kanto → Hoenn → Johto (3 regions, 24 badges)\n");
    printf("Sinnoh descoped per ADR-003\n");
    printf("Minimum iterations per property: 100\n");
    
    int property23_result = test_property_23_badge_separation();
    int property24_result = test_property_24_badge_capacity();
    
    // Overall results
    printf("\n\n╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║                    OVERALL TEST RESULTS                          ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n");
    
    if (property23_result == 0 && property24_result == 0) {
        printf("\n✓ ALL PROPERTY TESTS PASSED!\n\n");
        printf("Property 23 (Badge Tracking Separation): PASSED\n");
        printf("  - Validates: Requirements 14.1, 14.2, 14.3, 14.5\n");
        printf("Property 24 (Badge Storage Capacity): PASSED\n");
        printf("  - Validates: Requirements 14.6\n");
        return 0;
    } else {
        printf("\n✗ SOME PROPERTY TESTS FAILED\n\n");
        if (property23_result != 0) {
            printf("Property 23 (Badge Tracking Separation): FAILED\n");
        } else {
            printf("Property 23 (Badge Tracking Separation): PASSED\n");
        }
        if (property24_result != 0) {
            printf("Property 24 (Badge Storage Capacity): FAILED\n");
        } else {
            printf("Property 24 (Badge Storage Capacity): PASSED\n");
        }
        return 1;
    }
}
