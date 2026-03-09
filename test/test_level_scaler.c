#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

// Type definitions to match GBA types
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef bool bool8;

// Include the level scaler interface
#include "../include/level_scaler.h"

// Simple random number generator for testing
static unsigned int test_seed = 0;

static void init_test_random(unsigned int seed) {
    test_seed = seed;
}

static unsigned int test_random(unsigned int max) {
    test_seed = (test_seed * 1103515245 + 12345) & 0x7fffffff;
    return test_seed % max;
}

// Property test data structure
struct wild_level_test_data {
    u8 badge_count;
    u8 region_id;
    bool elite_four_defeated;
};

/**
 * Property 1: Wild Pokemon Level Range Calculation
 * 
 * For any badge count and region, the wild Pokemon level range should be calculated as:
 * - minimum = (badge_count × 5) + region_starting_level
 * - maximum = minimum + 10
 * - Special case: when badge_count is 0, levels should be 2-10
 * 
 * **Validates: Requirements 2.1, 2.2, 2.3**
 */
static bool prop_wild_pokemon_level_range(struct wild_level_test_data *data) {
    // Get the level range from the function under test
    struct LevelRange range = GetWildPokemonLevelRange(
        data->badge_count,
        data->region_id,
        data->elite_four_defeated
    );
    
    // Get region starting level
    u8 region_starting_level = GetRegionStartingLevel(data->region_id);
    
    // Calculate expected values
    u8 expected_min, expected_max;
    
    // Special case: badge_count == 0 should give levels 2-10
    if (data->badge_count == 0) {
        expected_min = 2;
        expected_max = 10;
    } else {
        // General formula: min = (badge_count * 5) + region_starting_level
        expected_min = (data->badge_count * 5) + region_starting_level;
        expected_max = expected_min + 10;
    }
    
    // Apply Elite Four bonus if defeated (+10 levels)
    if (data->elite_four_defeated) {
        expected_min += 10;
        expected_max += 10;
    }
    
    // Clamp expected values to valid range (1-100) for comparison
    if (expected_min > 100) expected_min = 100;
    if (expected_max > 100) expected_max = 100;
    
    // Verify the calculated range matches expected values
    if (range.min_level != expected_min) {
        printf("FAIL: badge=%u, region=%u, e4=%d - Expected min_level=%u, got %u\n",
               data->badge_count, data->region_id, data->elite_four_defeated,
               expected_min, range.min_level);
        return false;
    }
    
    if (range.max_level != expected_max) {
        printf("FAIL: badge=%u, region=%u, e4=%d - Expected max_level=%u, got %u\n",
               data->badge_count, data->region_id, data->elite_four_defeated,
               expected_max, range.max_level);
        return false;
    }
    
    // Verify that max is always min + 10 (unless clamped or badge_count == 0)
    // Special case: badge_count == 0 has a fixed range of 2-10 (8 level spread)
    if (data->badge_count == 0) {
        // For badge_count == 0, the range is always 2-10 before E4 bonus
        // After E4 bonus, it becomes 12-20
        u8 expected_spread = 8;
        if (range.max_level != range.min_level + expected_spread) {
            printf("FAIL: badge=%u, region=%u, e4=%d - For badge_count=0, max_level should be min_level + 8, got min=%u, max=%u\n",
                   data->badge_count, data->region_id, data->elite_four_defeated,
                   range.min_level, range.max_level);
            return false;
        }
    } else if (range.max_level != range.min_level + 10 && range.max_level != 100) {
        printf("FAIL: badge=%u, region=%u, e4=%d - max_level should be min_level + 10, got min=%u, max=%u\n",
               data->badge_count, data->region_id, data->elite_four_defeated,
               range.min_level, range.max_level);
        return false;
    }
    
    // Verify levels are within valid Pokemon level range (1-100)
    if (range.min_level < 1 || range.min_level > 100) {
        printf("FAIL: badge=%u, region=%u, e4=%d - min_level out of valid range (1-100): %u\n",
               data->badge_count, data->region_id, data->elite_four_defeated,
               range.min_level);
        return false;
    }
    
    if (range.max_level < 1 || range.max_level > 100) {
        printf("FAIL: badge=%u, region=%u, e4=%d - max_level out of valid range (1-100): %u\n",
               data->badge_count, data->region_id, data->elite_four_defeated,
               range.max_level);
        return false;
    }
    
    return true;
}

/**
 * Property 3: Region Starting Level Applied to Calculations
 * 
 * For any region, wild Pokemon levels should include the region's starting level
 * as a base offset in all level calculations.
 * 
 * This property verifies that:
 * 1. For badge_count > 0, the minimum level includes the region starting level
 * 2. The region starting level is correctly applied for all four regions
 * 3. The starting level offset is maintained even with Elite Four bonuses
 * 
 * **Validates: Requirements 2.6, 6.5**
 */
static bool prop_region_starting_level_applied(struct wild_level_test_data *data) {
    // Skip badge_count == 0 as it has special handling (fixed 2-10 range)
    if (data->badge_count == 0) {
        return true;
    }
    
    // Get the level range from the function under test
    struct LevelRange range = GetWildPokemonLevelRange(
        data->badge_count,
        data->region_id,
        data->elite_four_defeated
    );
    
    // Get region starting level
    u8 region_starting_level = GetRegionStartingLevel(data->region_id);
    
    // Calculate the base level without region offset (badge_count * 5)
    u8 base_level = data->badge_count * 5;
    
    // Calculate expected minimum level with region starting level applied
    u8 expected_min = base_level + region_starting_level;
    
    // Apply Elite Four bonus if defeated
    if (data->elite_four_defeated) {
        expected_min += 10;
    }
    
    // Clamp to valid range for comparison
    if (expected_min > 100) expected_min = 100;
    
    // Verify that the region starting level is included in the calculation
    if (range.min_level != expected_min) {
        printf("FAIL: Region starting level not applied correctly\n");
        printf("  badge=%u, region=%u, e4=%d\n",
               data->badge_count, data->region_id, data->elite_four_defeated);
        printf("  Region starting level: %u\n", region_starting_level);
        printf("  Base level (badge*5): %u\n", base_level);
        printf("  Expected min (base + region_start + e4_bonus): %u\n", expected_min);
        printf("  Actual min: %u\n", range.min_level);
        return false;
    }
    
    // Verify that different regions with the same badge count have different levels
    // (unless clamped to 100)
    if (data->region_id > 0 && range.min_level < 100) {
        // Calculate what the level would be in Kanto (region 0) with same badges
        u8 kanto_starting_level = GetRegionStartingLevel(0);
        u8 kanto_expected_min = base_level + kanto_starting_level;
        if (data->elite_four_defeated) {
            kanto_expected_min += 10;
        }
        if (kanto_expected_min > 100) kanto_expected_min = 100;
        
        // If we're in a later region, levels should be higher than Kanto
        // (unless both are clamped to 100)
        if (kanto_expected_min < 100 && range.min_level <= kanto_expected_min) {
            printf("FAIL: Region starting level offset not creating higher levels in later regions\n");
            printf("  badge=%u, region=%u, e4=%d\n",
                   data->badge_count, data->region_id, data->elite_four_defeated);
            printf("  Current region min: %u\n", range.min_level);
            printf("  Kanto equivalent min: %u\n", kanto_expected_min);
            printf("  Region starting levels: Kanto=%u, Current=%u\n",
                   kanto_starting_level, region_starting_level);
            return false;
        }
    }
    
    return true;
}


int main(int argc, char **argv) {
    int num_trials = 1000;
    int passed = 0;
    int failed = 0;
    
    // Initialize random seed
    init_test_random((unsigned int)time(NULL));
    
    printf("=== Property-Based Test: Wild Pokemon Level Range Calculation ===\n");
    printf("Validates: Requirements 2.1, 2.2, 2.3\n");
    printf("Running %d random test cases...\n\n", num_trials);
    
    // Run property tests with random inputs
    for (int i = 0; i < num_trials; i++) {
        struct wild_level_test_data data;
        
        // Generate random test data
        data.badge_count = test_random(33);  // 0-32
        data.region_id = test_random(4);     // 0-3
        data.elite_four_defeated = test_random(2); // 0-1
        
        // Run the property test
        if (prop_wild_pokemon_level_range(&data)) {
            passed++;
        } else {
            failed++;
            // Print first few failures for debugging
            if (failed <= 5) {
                printf("  Counterexample #%d: badge_count=%u, region_id=%u, elite_four_defeated=%d\n",
                       failed, data.badge_count, data.region_id, data.elite_four_defeated);
            }
        }
    }
    
    // Also test specific edge cases
    printf("\nTesting specific edge cases...\n");
    
    // Edge case 1: badge_count = 0 (should always be 2-10)
    struct wild_level_test_data edge1 = {0, 0, false};
    if (!prop_wild_pokemon_level_range(&edge1)) {
        printf("FAIL: Edge case badge_count=0 failed\n");
        failed++;
    } else {
        passed++;
    }
    
    // Edge case 2: badge_count = 0 with Elite Four defeated
    struct wild_level_test_data edge2 = {0, 0, true};
    if (!prop_wild_pokemon_level_range(&edge2)) {
        printf("FAIL: Edge case badge_count=0 with E4 defeated failed\n");
        failed++;
    } else {
        passed++;
    }
    
    // Edge case 3: Maximum badges (32)
    struct wild_level_test_data edge3 = {32, 3, true};
    if (!prop_wild_pokemon_level_range(&edge3)) {
        printf("FAIL: Edge case badge_count=32 failed\n");
        failed++;
    } else {
        passed++;
    }
    
    // Edge case 4: First badge in each region
    for (u8 region = 0; region < 4; region++) {
        struct wild_level_test_data edge = {1, region, false};
        if (!prop_wild_pokemon_level_range(&edge)) {
            printf("FAIL: Edge case badge_count=1, region=%u failed\n", region);
            failed++;
        } else {
            passed++;
        }
    }
    
    // Report results for Property 1
    printf("\n=== Property 1 Test Results ===\n");
    printf("Total tests: %d\n", passed + failed);
    printf("Passed: %d\n", passed);
    printf("Failed: %d\n", failed);
    
    int property1_failed = failed;
    
    if (failed == 0) {
        printf("\n✓ Property 1 tests PASSED!\n");
    } else {
        printf("\n✗ Property 1 tests FAILED with %d counterexamples\n", failed);
    }
    
    // Run Property 3 tests
    int property3_result = test_property_3_region_starting_level();
    
    // Overall results
    printf("\n\n=== OVERALL TEST RESULTS ===\n");
    if (property1_failed == 0 && property3_result == 0) {
        printf("✓ ALL PROPERTY TESTS PASSED!\n");
        return 0;
    } else {
        printf("✗ SOME PROPERTY TESTS FAILED\n");
        if (property1_failed > 0) {
            printf("  - Property 1 (Wild Pokemon Level Range): FAILED\n");
        } else {
            printf("  - Property 1 (Wild Pokemon Level Range): PASSED\n");
        }
        if (property3_result != 0) {
            printf("  - Property 3 (Region Starting Level): FAILED\n");
        } else {
            printf("  - Property 3 (Region Starting Level): PASSED\n");
        }
        return 1;
    }
}

/**
 * Test runner for Property 3: Region Starting Level Applied to Calculations
 */
int test_property_3_region_starting_level(void) {
    int num_trials = 1000;
    int passed = 0;
    int failed = 0;
    
    // Initialize random seed
    init_test_random((unsigned int)time(NULL) + 12345);
    
    printf("\n\n=== Property-Based Test: Region Starting Level Applied to Calculations ===\n");
    printf("Validates: Requirements 2.6, 6.5\n");
    printf("Running %d random test cases...\n\n", num_trials);
    
    // Run property tests with random inputs
    for (int i = 0; i < num_trials; i++) {
        struct wild_level_test_data data;
        
        // Generate random test data
        // Focus on badge_count > 0 since badge_count == 0 has special handling
        data.badge_count = 1 + test_random(32);  // 1-32
        data.region_id = test_random(4);         // 0-3
        data.elite_four_defeated = test_random(2); // 0-1
        
        // Run the property test
        if (prop_region_starting_level_applied(&data)) {
            passed++;
        } else {
            failed++;
            // Print first few failures for debugging
            if (failed <= 5) {
                printf("  Counterexample #%d: badge_count=%u, region_id=%u, elite_four_defeated=%d\n",
                       failed, data.badge_count, data.region_id, data.elite_four_defeated);
            }
        }
    }
    
    // Test specific edge cases for each region
    printf("\nTesting specific edge cases for each region...\n");
    
    // Edge case 1: First badge in each region (should show region offset clearly)
    for (u8 region = 0; region < 4; region++) {
        struct wild_level_test_data edge = {1, region, false};
        if (!prop_region_starting_level_applied(&edge)) {
            printf("FAIL: Edge case badge_count=1, region=%u failed\n", region);
            failed++;
        } else {
            passed++;
        }
    }
    
    // Edge case 2: Same badge count across all regions (should show different levels)
    u8 test_badge_count = 8; // Mid-game badge count
    for (u8 region = 0; region < 4; region++) {
        struct wild_level_test_data edge = {test_badge_count, region, false};
        if (!prop_region_starting_level_applied(&edge)) {
            printf("FAIL: Edge case badge_count=%u, region=%u failed\n", test_badge_count, region);
            failed++;
        } else {
            passed++;
        }
    }
    
    // Edge case 3: Verify region starting levels are correctly defined
    printf("\nVerifying region starting levels:\n");
    u8 expected_starting_levels[4] = {2, 60, 110, 160};
    for (u8 region = 0; region < 4; region++) {
        u8 actual = GetRegionStartingLevel(region);
        u8 expected = expected_starting_levels[region];
        if (actual != expected) {
            printf("FAIL: Region %u starting level is %u, expected %u\n", region, actual, expected);
            failed++;
        } else {
            printf("  Region %u starting level: %u ✓\n", region, actual);
            passed++;
        }
    }
    
    // Edge case 4: Test with Elite Four defeated in each region
    for (u8 region = 0; region < 4; region++) {
        struct wild_level_test_data edge = {8, region, true};
        if (!prop_region_starting_level_applied(&edge)) {
            printf("FAIL: Edge case badge_count=8, region=%u, E4 defeated failed\n", region);
            failed++;
        } else {
            passed++;
        }
    }
    
    // Report results
    printf("\n=== Property 3 Test Results ===\n");
    printf("Total tests: %d\n", passed + failed);
    printf("Passed: %d\n", passed);
    printf("Failed: %d\n", failed);
    
    if (failed == 0) {
        printf("\n✓ Property 3 tests PASSED!\n");
        return 0;
    } else {
        printf("\n✗ Property 3 tests FAILED with %d counterexamples\n", failed);
        return 1;
    }
}
