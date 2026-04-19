/*
 * Property-Based Tests for Fly Location System
 * 
 * Tests Property 20: Fly Location Registration
 * Tests Property 21: Fly Menu Region Filtering
 * 
 * **Validates: Requirements 12.1, 12.2, 12.4, 12.5**
 * 
 * Uses Theft library pattern with minimum 100 iterations per property.
 * Region order: Kanto → Hoenn → Johto (per ADR-004)
 * Sinnoh descoped per ADR-003 (3 regions total)
 * 
 * Key functions tested:
 * - RegisterFlyLocation(map_id)
 * - GetCurrentRegionFlyLocations(out_count)
 * - IsFlyLocationRegistered(map_id)
 * - IsFlyLocationRegisteredForRegion(map_id, region_id)
 * - GetCrossRegionFlyLocations(locations, count)
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

// Fly location constants (from fly_location_discovery.h)
#define FLY_LOCATIONS_PER_REGION  16
#define MAX_FLY_LOCATIONS_TOTAL   (FLY_LOCATIONS_PER_REGION * NUM_REGIONS)  // 48

// Map ID ranges for each region (placeholder values matching implementation)
#define KANTO_MAP_START  0
#define KANTO_MAP_END    255
#define HOENN_MAP_START  256
#define HOENN_MAP_END    511
#define JOHTO_MAP_START  512
#define JOHTO_MAP_END    767

// Starting fly locations for each region
static const u16 sRegionStartingLocations[NUM_REGIONS] = {
    0,   // REGION_KANTO: Pallet Town
    16,  // REGION_HOENN: Littleroot Town (offset in Hoenn range)
    32,  // REGION_JOHTO: New Bark Town (offset in Johto range)
};

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

// ============================================================================
// Test implementation of fly location system (mirrors src/fly_location_discovery.c)
// ============================================================================

// Fly location storage (16 per region × 3 regions = 48 total)
static u16 sFlyLocations[MAX_FLY_LOCATIONS_TOTAL] = {0};
static u8 sFlyLocationCount[NUM_REGIONS] = {0};

// Current region state (simulated)
static u8 sCurrentRegion = REGION_KANTO;

// Region unlock state (simulated)
static bool8 sRegionUnlocked[NUM_REGIONS] = {true, false, false};

// Static buffer for returning fly locations
static u16 sCurrentRegionFlyLocationsBuffer[FLY_LOCATIONS_PER_REGION];

/**
 * Get the base index for a region's fly locations in the array.
 */
static u8 GetRegionBaseIndex(u8 region_id) {
    if (region_id >= NUM_REGIONS)
        return 0;
    return region_id * FLY_LOCATIONS_PER_REGION;
}

/**
 * Determine which region a map_id belongs to.
 */
static u8 GetRegionForMapId(u16 map_id) {
    if (map_id < 256)
        return REGION_KANTO;
    else if (map_id < 512)
        return REGION_HOENN;
    else
        return REGION_JOHTO;
}

/**
 * Get current region (simulated).
 */
static u8 GetCurrentRegion(void) {
    return sCurrentRegion;
}

/**
 * Set current region (test helper).
 */
static void SetCurrentRegion(u8 region_id) {
    if (region_id < NUM_REGIONS)
        sCurrentRegion = region_id;
}

/**
 * Check if region is unlocked (simulated).
 */
static bool8 IsRegionUnlocked(u8 region_id) {
    if (region_id >= NUM_REGIONS)
        return false;
    return sRegionUnlocked[region_id];
}

/**
 * Set region unlock state (test helper).
 */
static void SetRegionUnlocked(u8 region_id, bool8 unlocked) {
    if (region_id < NUM_REGIONS)
        sRegionUnlocked[region_id] = unlocked;
}

/**
 * Initialize fly location data for a new game.
 */
static void InitFlyLocationData(void) {
    u8 i;
    
    for (i = 0; i < MAX_FLY_LOCATIONS_TOTAL; i++)
        sFlyLocations[i] = 0;
    
    for (i = 0; i < NUM_REGIONS; i++)
        sFlyLocationCount[i] = 0;
}

/**
 * Reset all test state.
 */
static void ResetTestState(void) {
    InitFlyLocationData();
    sCurrentRegion = REGION_KANTO;
    sRegionUnlocked[REGION_KANTO] = true;
    sRegionUnlocked[REGION_HOENN] = false;
    sRegionUnlocked[REGION_JOHTO] = false;
}

/**
 * Check if a fly location is registered for a specific region.
 */
static bool8 IsFlyLocationRegisteredForRegion(u16 map_id, u8 region_id) {
    u8 base_index;
    u8 count;
    u8 i;
    
    if (region_id >= NUM_REGIONS)
        return false;
    
    base_index = GetRegionBaseIndex(region_id);
    count = sFlyLocationCount[region_id];
    
    for (i = 0; i < count; i++) {
        if (sFlyLocations[base_index + i] == map_id)
            return true;
    }
    
    return false;
}

/**
 * Register a fly location for a specific region.
 */
static void RegisterFlyLocationForRegion(u16 map_id, u8 region_id) {
    u8 base_index;
    u8 count;
    
    // Validate region
    if (region_id >= NUM_REGIONS)
        return;
    
    // Check if already at max capacity for this region
    count = sFlyLocationCount[region_id];
    if (count >= FLY_LOCATIONS_PER_REGION)
        return;
    
    // Check if already registered
    if (IsFlyLocationRegisteredForRegion(map_id, region_id))
        return;
    
    // Register the location
    base_index = GetRegionBaseIndex(region_id);
    sFlyLocations[base_index + count] = map_id;
    sFlyLocationCount[region_id] = count + 1;
}

/**
 * Register a fly location for the current region.
 * Validates: Requirements 12.1, 12.2
 */
static void RegisterFlyLocation(u16 map_id) {
    u8 current_region = GetCurrentRegion();
    RegisterFlyLocationForRegion(map_id, current_region);
}

/**
 * Check if a fly location is registered (in any region).
 */
static bool8 IsFlyLocationRegistered(u16 map_id) {
    u8 region;
    
    for (region = 0; region < NUM_REGIONS; region++) {
        if (IsFlyLocationRegisteredForRegion(map_id, region))
            return true;
    }
    
    return false;
}

/**
 * Get fly locations for the current region.
 * Validates: Requirements 12.4
 */
static u16* GetCurrentRegionFlyLocations(u8 *out_count) {
    u8 current_region = GetCurrentRegion();
    u8 base_index = GetRegionBaseIndex(current_region);
    u8 count = sFlyLocationCount[current_region];
    u8 i;
    
    // Copy to static buffer
    for (i = 0; i < count && i < FLY_LOCATIONS_PER_REGION; i++)
        sCurrentRegionFlyLocationsBuffer[i] = sFlyLocations[base_index + i];
    
    if (out_count != NULL)
        *out_count = count;
    
    return sCurrentRegionFlyLocationsBuffer;
}

/**
 * Get the count of registered fly locations for a region.
 */
static u8 GetFlyLocationCountForRegion(u8 region_id) {
    if (region_id >= NUM_REGIONS)
        return 0;
    
    return sFlyLocationCount[region_id];
}

/**
 * Get all fly locations for a specific region.
 */
static void GetFlyLocationsForRegion(u8 region_id, u16 *locations, u8 *count) {
    u8 base_index;
    u8 loc_count;
    u8 i;
    
    if (region_id >= NUM_REGIONS || locations == NULL || count == NULL) {
        if (count != NULL)
            *count = 0;
        return;
    }
    
    base_index = GetRegionBaseIndex(region_id);
    loc_count = sFlyLocationCount[region_id];
    
    for (i = 0; i < loc_count; i++)
        locations[i] = sFlyLocations[base_index + i];
    
    *count = loc_count;
}

/**
 * Get the starting fly location for a region.
 */
static u16 GetRegionStartingFlyLocation(u8 region_id) {
    if (region_id >= NUM_REGIONS)
        return 0;
    
    return sRegionStartingLocations[region_id];
}

/**
 * Get cross-region fly locations (starting locations of unlocked regions).
 * Validates: Requirements 12.5
 */
static void GetCrossRegionFlyLocations(u16 *locations, u8 *count) {
    u8 region;
    u8 loc_count = 0;
    u8 current_region = GetCurrentRegion();
    
    if (locations == NULL || count == NULL) {
        if (count != NULL)
            *count = 0;
        return;
    }
    
    for (region = 0; region < NUM_REGIONS; region++) {
        // Skip current region (those are in GetCurrentRegionFlyLocations)
        if (region == current_region)
            continue;
        
        // Only include unlocked regions
        if (IsRegionUnlocked(region)) {
            locations[loc_count] = GetRegionStartingFlyLocation(region);
            loc_count++;
        }
    }
    
    *count = loc_count;
}

// ============================================================================
// Test data structures
// ============================================================================

struct FlyLocationTestData {
    u16 map_id;
    u8 region_id;
    u8 current_region;
};

struct FlyMenuTestData {
    u8 current_region;
    bool8 kanto_unlocked;
    bool8 hoenn_unlocked;
    bool8 johto_unlocked;
    u8 kanto_locations_count;
    u8 hoenn_locations_count;
    u8 johto_locations_count;
};

// ============================================================================
// Property 20: Fly Location Registration
// ============================================================================

/**
 * Property 20: Fly Location Registration
 * 
 * For any city with a designated fly location in any region, visiting that city
 * should register the fly location for that region.
 * 
 * This property verifies:
 * 1. RegisterFlyLocation() correctly registers locations for the current region
 * 2. IsFlyLocationRegistered() correctly identifies registered locations
 * 3. IsFlyLocationRegisteredForRegion() correctly identifies region-specific registrations
 * 4. Fly locations are stored per-region (16 per region × 3 regions = 48 total)
 * 5. Duplicate registrations are prevented
 * 6. Maximum capacity per region (16) is enforced
 * 
 * **Validates: Requirements 12.1, 12.2**
 */
static bool prop_fly_location_registration(struct FlyLocationTestData *data) {
    u8 count_before;
    u8 count_after;
    bool8 was_registered;
    bool8 is_registered;
    
    // Validate inputs
    if (data->region_id >= NUM_REGIONS) {
        return true;  // Skip invalid region
    }
    
    // Reset state for clean test
    ResetTestState();
    
    // Set current region
    SetCurrentRegion(data->region_id);
    
    // Check initial state
    count_before = GetFlyLocationCountForRegion(data->region_id);
    was_registered = IsFlyLocationRegisteredForRegion(data->map_id, data->region_id);
    
    // Register the fly location
    RegisterFlyLocation(data->map_id);
    
    // Check final state
    count_after = GetFlyLocationCountForRegion(data->region_id);
    is_registered = IsFlyLocationRegisteredForRegion(data->map_id, data->region_id);
    
    // Property 1: After registration, location should be registered
    if (!is_registered) {
        printf("FAIL: Location %u not registered after RegisterFlyLocation() in region %u\n",
               data->map_id, data->region_id);
        return false;
    }
    
    // Property 2: IsFlyLocationRegistered() should also return true
    if (!IsFlyLocationRegistered(data->map_id)) {
        printf("FAIL: IsFlyLocationRegistered(%u) returned false after registration\n",
               data->map_id);
        return false;
    }
    
    // Property 3: Count should increase by 1 (if not already registered)
    if (!was_registered && count_after != count_before + 1) {
        printf("FAIL: Count did not increase after registration: before=%u, after=%u\n",
               count_before, count_after);
        return false;
    }
    
    // Property 4: If already registered, count should not change
    if (was_registered && count_after != count_before) {
        printf("FAIL: Count changed for duplicate registration: before=%u, after=%u\n",
               count_before, count_after);
        return false;
    }
    
    return true;
}

/**
 * Property 20 sub-property: Duplicate registration prevention
 * 
 * Registering the same location twice should not create duplicates.
 */
static bool prop_fly_location_no_duplicates(struct FlyLocationTestData *data) {
    u8 count_after_first;
    u8 count_after_second;
    
    // Validate inputs
    if (data->region_id >= NUM_REGIONS) {
        return true;
    }
    
    // Reset state
    ResetTestState();
    SetCurrentRegion(data->region_id);
    
    // Register once
    RegisterFlyLocation(data->map_id);
    count_after_first = GetFlyLocationCountForRegion(data->region_id);
    
    // Register again (duplicate)
    RegisterFlyLocation(data->map_id);
    count_after_second = GetFlyLocationCountForRegion(data->region_id);
    
    // Count should not change on duplicate registration
    if (count_after_second != count_after_first) {
        printf("FAIL: Duplicate registration changed count: first=%u, second=%u\n",
               count_after_first, count_after_second);
        return false;
    }
    
    return true;
}

/**
 * Property 20 sub-property: Region isolation
 * 
 * Registering a location in one region should not affect other regions.
 */
static bool prop_fly_location_region_isolation(struct FlyLocationTestData *data) {
    u8 other_region;
    u8 other_count_before;
    u8 other_count_after;
    
    // Validate inputs
    if (data->region_id >= NUM_REGIONS) {
        return true;
    }
    
    // Reset state
    ResetTestState();
    SetCurrentRegion(data->region_id);
    
    // Get counts for other regions before registration
    for (other_region = 0; other_region < NUM_REGIONS; other_region++) {
        if (other_region == data->region_id)
            continue;
        
        other_count_before = GetFlyLocationCountForRegion(other_region);
        
        // Register in current region
        RegisterFlyLocation(data->map_id);
        
        // Check other region count
        other_count_after = GetFlyLocationCountForRegion(other_region);
        
        if (other_count_after != other_count_before) {
            printf("FAIL: Registration in region %u affected region %u count: before=%u, after=%u\n",
                   data->region_id, other_region, other_count_before, other_count_after);
            return false;
        }
        
        // Reset for next iteration
        ResetTestState();
        SetCurrentRegion(data->region_id);
    }
    
    return true;
}

/**
 * Property 20 sub-property: Maximum capacity enforcement
 * 
 * Each region should support exactly 16 fly locations maximum.
 */
static bool prop_fly_location_max_capacity(u8 region_id) {
    u8 i;
    u8 count;
    u16 map_id;
    
    if (region_id >= NUM_REGIONS) {
        return true;
    }
    
    // Reset state
    ResetTestState();
    SetCurrentRegion(region_id);
    
    // Register 16 locations (should all succeed)
    for (i = 0; i < FLY_LOCATIONS_PER_REGION; i++) {
        map_id = (region_id * 256) + i;  // Unique map IDs per region
        RegisterFlyLocation(map_id);
    }
    
    count = GetFlyLocationCountForRegion(region_id);
    if (count != FLY_LOCATIONS_PER_REGION) {
        printf("FAIL: Expected %u locations after registering max, got %u\n",
               FLY_LOCATIONS_PER_REGION, count);
        return false;
    }
    
    // Try to register one more (should be rejected)
    map_id = (region_id * 256) + FLY_LOCATIONS_PER_REGION;
    RegisterFlyLocation(map_id);
    
    count = GetFlyLocationCountForRegion(region_id);
    if (count != FLY_LOCATIONS_PER_REGION) {
        printf("FAIL: Count exceeded max capacity: got %u, expected %u\n",
               count, FLY_LOCATIONS_PER_REGION);
        return false;
    }
    
    // The 17th location should NOT be registered
    if (IsFlyLocationRegisteredForRegion(map_id, region_id)) {
        printf("FAIL: 17th location was registered despite max capacity\n");
        return false;
    }
    
    return true;
}

// ============================================================================
// Property 21: Fly Menu Region Filtering
// ============================================================================

/**
 * Property 21: Fly Menu Region Filtering
 * 
 * For any use of HM Fly, the displayed fly locations should only include
 * locations from the current region (plus starting locations of unlocked regions).
 * 
 * This property verifies:
 * 1. GetCurrentRegionFlyLocations() returns only locations from current region
 * 2. GetCrossRegionFlyLocations() returns starting locations of unlocked regions
 * 3. Locked regions are not included in cross-region fly locations
 * 4. Current region is excluded from cross-region fly locations
 * 
 * **Validates: Requirements 12.4, 12.5**
 */
static bool prop_fly_menu_region_filtering(struct FlyMenuTestData *data) {
    u16 *current_locations;
    u8 current_count;
    u16 cross_locations[NUM_REGIONS];
    u8 cross_count;
    u8 i;
    u8 region;
    
    // Validate inputs
    if (data->current_region >= NUM_REGIONS) {
        return true;
    }
    
    // Reset state
    ResetTestState();
    
    // Set up region unlock state
    SetRegionUnlocked(REGION_KANTO, data->kanto_unlocked);
    SetRegionUnlocked(REGION_HOENN, data->hoenn_unlocked);
    SetRegionUnlocked(REGION_JOHTO, data->johto_unlocked);
    SetCurrentRegion(data->current_region);
    
    // Register some fly locations in each region
    for (region = 0; region < NUM_REGIONS; region++) {
        u8 loc_count = 0;
        switch (region) {
            case REGION_KANTO: loc_count = data->kanto_locations_count; break;
            case REGION_HOENN: loc_count = data->hoenn_locations_count; break;
            case REGION_JOHTO: loc_count = data->johto_locations_count; break;
        }
        
        // Cap at max
        if (loc_count > FLY_LOCATIONS_PER_REGION)
            loc_count = FLY_LOCATIONS_PER_REGION;
        
        for (i = 0; i < loc_count; i++) {
            u16 map_id = (region * 256) + i;
            RegisterFlyLocationForRegion(map_id, region);
        }
    }
    
    // Get current region fly locations
    current_locations = GetCurrentRegionFlyLocations(&current_count);
    
    // Property 1: Current region locations count should match what we registered
    u8 expected_current_count = 0;
    switch (data->current_region) {
        case REGION_KANTO: expected_current_count = data->kanto_locations_count; break;
        case REGION_HOENN: expected_current_count = data->hoenn_locations_count; break;
        case REGION_JOHTO: expected_current_count = data->johto_locations_count; break;
    }
    if (expected_current_count > FLY_LOCATIONS_PER_REGION)
        expected_current_count = FLY_LOCATIONS_PER_REGION;
    
    if (current_count != expected_current_count) {
        printf("FAIL: Current region locations count mismatch: expected %u, got %u\n",
               expected_current_count, current_count);
        return false;
    }
    
    // Property 2: All returned locations should be from current region
    for (i = 0; i < current_count; i++) {
        u8 loc_region = GetRegionForMapId(current_locations[i]);
        if (loc_region != data->current_region) {
            printf("FAIL: Location %u from region %u returned for current region %u\n",
                   current_locations[i], loc_region, data->current_region);
            return false;
        }
    }
    
    // Get cross-region fly locations
    GetCrossRegionFlyLocations(cross_locations, &cross_count);
    
    // Property 3: Cross-region should not include current region
    for (i = 0; i < cross_count; i++) {
        u16 starting_loc = GetRegionStartingFlyLocation(data->current_region);
        if (cross_locations[i] == starting_loc) {
            printf("FAIL: Current region starting location included in cross-region list\n");
            return false;
        }
    }
    
    // Property 4: Cross-region should only include unlocked regions
    u8 expected_cross_count = 0;
    for (region = 0; region < NUM_REGIONS; region++) {
        if (region == data->current_region)
            continue;
        if (IsRegionUnlocked(region))
            expected_cross_count++;
    }
    
    if (cross_count != expected_cross_count) {
        printf("FAIL: Cross-region count mismatch: expected %u, got %u\n",
               expected_cross_count, cross_count);
        return false;
    }
    
    return true;
}

/**
 * Property 21 sub-property: Cross-region fly only to starting locations
 * 
 * Cross-region fly should only allow travel to the starting location of each
 * unlocked region, not to any registered fly location in that region.
 */
static bool prop_cross_region_starting_locations_only(void) {
    u16 cross_locations[NUM_REGIONS];
    u8 cross_count;
    u8 i;
    bool found_kanto_start = false;
    bool found_hoenn_start = false;
    
    // Reset state
    ResetTestState();
    
    // Unlock Hoenn, stay in Kanto
    SetRegionUnlocked(REGION_HOENN, true);
    SetCurrentRegion(REGION_KANTO);
    
    // Register multiple fly locations in Hoenn
    for (i = 0; i < 5; i++) {
        u16 map_id = HOENN_MAP_START + i;
        RegisterFlyLocationForRegion(map_id, REGION_HOENN);
    }
    
    // Get cross-region locations
    GetCrossRegionFlyLocations(cross_locations, &cross_count);
    
    // Should only have 1 location (Hoenn starting location)
    if (cross_count != 1) {
        printf("FAIL: Expected 1 cross-region location, got %u\n", cross_count);
        return false;
    }
    
    // That location should be Hoenn's starting location
    u16 hoenn_start = GetRegionStartingFlyLocation(REGION_HOENN);
    if (cross_locations[0] != hoenn_start) {
        printf("FAIL: Cross-region location %u is not Hoenn start %u\n",
               cross_locations[0], hoenn_start);
        return false;
    }
    
    // Now test from Hoenn with Kanto unlocked
    ResetTestState();
    SetRegionUnlocked(REGION_KANTO, true);
    SetRegionUnlocked(REGION_HOENN, true);
    SetCurrentRegion(REGION_HOENN);
    
    // Register multiple fly locations in Kanto
    for (i = 0; i < 5; i++) {
        u16 map_id = KANTO_MAP_START + i;
        RegisterFlyLocationForRegion(map_id, REGION_KANTO);
    }
    
    // Get cross-region locations
    GetCrossRegionFlyLocations(cross_locations, &cross_count);
    
    // Should only have 1 location (Kanto starting location)
    if (cross_count != 1) {
        printf("FAIL: Expected 1 cross-region location from Hoenn, got %u\n", cross_count);
        return false;
    }
    
    // That location should be Kanto's starting location
    u16 kanto_start = GetRegionStartingFlyLocation(REGION_KANTO);
    if (cross_locations[0] != kanto_start) {
        printf("FAIL: Cross-region location %u is not Kanto start %u\n",
               cross_locations[0], kanto_start);
        return false;
    }
    
    return true;
}

// ============================================================================
// Test Runners
// ============================================================================

/**
 * Test runner for Property 20: Fly Location Registration
 */
static int test_property_20_fly_location_registration(void) {
    int num_trials = 100;  // Minimum 100 iterations per property
    int passed = 0;
    int failed = 0;
    
    printf("\n=== Property-Based Test: Fly Location Registration (Property 20) ===\n");
    printf("**Validates: Requirements 12.1, 12.2**\n");
    printf("Running %d random test cases...\n\n", num_trials);
    
    // Run property tests with random inputs
    for (int i = 0; i < num_trials; i++) {
        struct FlyLocationTestData data;
        
        // Generate random test data
        data.region_id = test_random(NUM_REGIONS);
        data.map_id = (data.region_id * 256) + test_random(100);  // Valid map ID for region
        data.current_region = data.region_id;
        
        if (prop_fly_location_registration(&data)) {
            passed++;
        } else {
            failed++;
            if (failed <= 5) {
                printf("  Counterexample #%d: region=%u, map_id=%u\n",
                       failed, data.region_id, data.map_id);
            }
        }
    }
    
    // Test duplicate prevention
    printf("\nTesting duplicate prevention...\n");
    for (int i = 0; i < 50; i++) {
        struct FlyLocationTestData data;
        data.region_id = test_random(NUM_REGIONS);
        data.map_id = (data.region_id * 256) + test_random(50);
        
        if (prop_fly_location_no_duplicates(&data)) {
            passed++;
        } else {
            failed++;
        }
    }
    printf("  Duplicate prevention: %s\n", failed == 0 ? "PASSED" : "FAILED");
    
    // Test region isolation
    printf("\nTesting region isolation...\n");
    for (int i = 0; i < 30; i++) {
        struct FlyLocationTestData data;
        data.region_id = test_random(NUM_REGIONS);
        data.map_id = (data.region_id * 256) + test_random(50);
        
        if (prop_fly_location_region_isolation(&data)) {
            passed++;
        } else {
            failed++;
        }
    }
    printf("  Region isolation: %s\n", failed == 0 ? "PASSED" : "FAILED");
    
    // Test max capacity for each region
    printf("\nTesting maximum capacity (16 per region)...\n");
    for (u8 region = 0; region < NUM_REGIONS; region++) {
        if (prop_fly_location_max_capacity(region)) {
            printf("  Region %u max capacity: PASSED\n", region);
            passed++;
        } else {
            printf("  Region %u max capacity: FAILED\n", region);
            failed++;
        }
    }
    
    // Report results
    printf("\n=== Fly Location Registration Test Results ===\n");
    printf("Total tests: %d\n", passed + failed);
    printf("Passed: %d\n", passed);
    printf("Failed: %d\n", failed);
    
    if (failed == 0) {
        printf("\n✓ Fly Location Registration (Property 20) PASSED!\n");
        return 0;
    } else {
        printf("\n✗ Fly Location Registration (Property 20) FAILED with %d counterexamples\n", failed);
        return 1;
    }
}

/**
 * Test runner for Property 21: Fly Menu Region Filtering
 */
static int test_property_21_fly_menu_region_filtering(void) {
    int num_trials = 100;  // Minimum 100 iterations per property
    int passed = 0;
    int failed = 0;
    
    printf("\n=== Property-Based Test: Fly Menu Region Filtering (Property 21) ===\n");
    printf("**Validates: Requirements 12.4, 12.5**\n");
    printf("Running %d random test cases...\n\n", num_trials);
    
    // Run property tests with random inputs
    for (int i = 0; i < num_trials; i++) {
        struct FlyMenuTestData data;
        
        // Generate random test data
        data.current_region = test_random(NUM_REGIONS);
        data.kanto_unlocked = test_random(2);
        data.hoenn_unlocked = test_random(2);
        data.johto_unlocked = test_random(2);
        data.kanto_locations_count = test_random(FLY_LOCATIONS_PER_REGION + 1);
        data.hoenn_locations_count = test_random(FLY_LOCATIONS_PER_REGION + 1);
        data.johto_locations_count = test_random(FLY_LOCATIONS_PER_REGION + 1);
        
        // Current region must be unlocked
        switch (data.current_region) {
            case REGION_KANTO: data.kanto_unlocked = true; break;
            case REGION_HOENN: data.hoenn_unlocked = true; break;
            case REGION_JOHTO: data.johto_unlocked = true; break;
        }
        
        if (prop_fly_menu_region_filtering(&data)) {
            passed++;
        } else {
            failed++;
            if (failed <= 5) {
                printf("  Counterexample #%d: current=%u, unlocked=[K:%d,H:%d,J:%d], counts=[%u,%u,%u]\n",
                       failed, data.current_region,
                       data.kanto_unlocked, data.hoenn_unlocked, data.johto_unlocked,
                       data.kanto_locations_count, data.hoenn_locations_count, data.johto_locations_count);
            }
        }
    }
    
    // Test cross-region starting locations only
    printf("\nTesting cross-region starting locations only...\n");
    if (prop_cross_region_starting_locations_only()) {
        printf("  Cross-region starting locations: PASSED\n");
        passed++;
    } else {
        printf("  Cross-region starting locations: FAILED\n");
        failed++;
    }
    
    // Test specific edge cases
    printf("\nTesting specific edge cases...\n");
    
    // Edge case 1: All regions unlocked
    printf("\nTesting all regions unlocked...\n");
    {
        struct FlyMenuTestData data = {
            .current_region = REGION_KANTO,
            .kanto_unlocked = true,
            .hoenn_unlocked = true,
            .johto_unlocked = true,
            .kanto_locations_count = 5,
            .hoenn_locations_count = 3,
            .johto_locations_count = 4
        };
        
        if (prop_fly_menu_region_filtering(&data)) {
            printf("  All regions unlocked (from Kanto): PASSED\n");
            passed++;
        } else {
            printf("  All regions unlocked (from Kanto): FAILED\n");
            failed++;
        }
    }
    
    // Edge case 2: Only current region unlocked
    printf("\nTesting only current region unlocked...\n");
    {
        struct FlyMenuTestData data = {
            .current_region = REGION_HOENN,
            .kanto_unlocked = false,
            .hoenn_unlocked = true,
            .johto_unlocked = false,
            .kanto_locations_count = 0,
            .hoenn_locations_count = 8,
            .johto_locations_count = 0
        };
        
        ResetTestState();
        SetRegionUnlocked(REGION_KANTO, false);
        SetRegionUnlocked(REGION_HOENN, true);
        SetRegionUnlocked(REGION_JOHTO, false);
        SetCurrentRegion(REGION_HOENN);
        
        // Register locations in Hoenn
        for (int i = 0; i < 8; i++) {
            RegisterFlyLocation(HOENN_MAP_START + i);
        }
        
        u16 cross_locations[NUM_REGIONS];
        u8 cross_count;
        GetCrossRegionFlyLocations(cross_locations, &cross_count);
        
        if (cross_count == 0) {
            printf("  Only current region unlocked: PASSED\n");
            passed++;
        } else {
            printf("  Only current region unlocked: FAILED (cross_count=%u)\n", cross_count);
            failed++;
        }
    }
    
    // Edge case 3: Empty fly locations
    printf("\nTesting empty fly locations...\n");
    {
        ResetTestState();
        SetCurrentRegion(REGION_KANTO);
        
        u8 count;
        u16 *locations = GetCurrentRegionFlyLocations(&count);
        
        if (count == 0) {
            printf("  Empty fly locations: PASSED\n");
            passed++;
        } else {
            printf("  Empty fly locations: FAILED (count=%u)\n", count);
            failed++;
        }
    }
    
    // Edge case 4: Maximum fly locations in current region
    printf("\nTesting maximum fly locations in current region...\n");
    {
        ResetTestState();
        SetCurrentRegion(REGION_KANTO);
        
        // Register max locations
        for (int i = 0; i < FLY_LOCATIONS_PER_REGION; i++) {
            RegisterFlyLocation(KANTO_MAP_START + i);
        }
        
        u8 count;
        u16 *locations = GetCurrentRegionFlyLocations(&count);
        
        if (count == FLY_LOCATIONS_PER_REGION) {
            printf("  Maximum fly locations: PASSED\n");
            passed++;
        } else {
            printf("  Maximum fly locations: FAILED (count=%u)\n", count);
            failed++;
        }
    }
    
    // Report results
    printf("\n=== Fly Menu Region Filtering Test Results ===\n");
    printf("Total tests: %d\n", passed + failed);
    printf("Passed: %d\n", passed);
    printf("Failed: %d\n", failed);
    
    if (failed == 0) {
        printf("\n✓ Fly Menu Region Filtering (Property 21) PASSED!\n");
        return 0;
    } else {
        printf("\n✗ Fly Menu Region Filtering (Property 21) FAILED with %d counterexamples\n", failed);
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
    printf("║     Property-Based Tests for Fly Location System                 ║\n");
    printf("║     Task 3.10: Write property tests for fly system               ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n");
    printf("\nRegion configuration: Kanto → Hoenn → Johto (3 regions)\n");
    printf("Sinnoh descoped per ADR-003\n");
    printf("Progression order per ADR-004\n");
    printf("Minimum iterations per property: 100\n");
    printf("\nFly location storage:\n");
    printf("  - 16 locations per region\n");
    printf("  - 3 regions total\n");
    printf("  - 48 total fly locations maximum\n");
    printf("\nKey functions tested:\n");
    printf("  - RegisterFlyLocation(map_id)\n");
    printf("  - GetCurrentRegionFlyLocations(out_count)\n");
    printf("  - IsFlyLocationRegistered(map_id)\n");
    printf("  - IsFlyLocationRegisteredForRegion(map_id, region_id)\n");
    printf("  - GetCrossRegionFlyLocations(locations, count)\n");
    
    int prop20_result = test_property_20_fly_location_registration();
    int prop21_result = test_property_21_fly_menu_region_filtering();
    
    // Overall results
    printf("\n\n╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║                    OVERALL TEST RESULTS                          ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n");
    
    if (prop20_result == 0 && prop21_result == 0) {
        printf("\n✓ ALL PROPERTY TESTS PASSED!\n\n");
        printf("Property 20 (Fly Location Registration): PASSED\n");
        printf("  - Validates: Requirements 12.1, 12.2\n");
        printf("Property 21 (Fly Menu Region Filtering): PASSED\n");
        printf("  - Validates: Requirements 12.4, 12.5\n");
        return 0;
    } else {
        printf("\n✗ SOME PROPERTY TESTS FAILED\n\n");
        if (prop20_result != 0) {
            printf("Property 20 (Fly Location Registration): FAILED\n");
        } else {
            printf("Property 20 (Fly Location Registration): PASSED\n");
        }
        if (prop21_result != 0) {
            printf("Property 21 (Fly Menu Region Filtering): FAILED\n");
        } else {
            printf("Property 21 (Fly Menu Region Filtering): PASSED\n");
        }
        return 1;
    }
}
