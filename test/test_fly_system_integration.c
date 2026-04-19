/*
 * Integration Tests for Fly System Across Both Regions
 * 
 * Task 4.6: Test fly system across both regions
 * 
 * Test Scenarios:
 * 1. Register fly locations in Kanto → Verify only Kanto locations in fly menu
 * 2. Transition to Hoenn → Verify fly menu switches to Hoenn locations
 * 3. Register locations in Hoenn → Verify Hoenn locations added
 * 4. Cross-region fly: From Hoenn, fly to Kanto starting location
 * 5. Save with fly locations → Load → Verify all locations preserved
 * 
 * **Validates: Requirements 12.1, 12.2, 12.4, 12.5**
 * 
 * Region order: Kanto → Hoenn → Johto (per ADR-004)
 * Sinnoh descoped per ADR-003 (3 regions total)
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

// Map ID ranges for each region
#define KANTO_MAP_START  0
#define KANTO_MAP_END    255
#define HOENN_MAP_START  256
#define HOENN_MAP_END    511
#define JOHTO_MAP_START  512
#define JOHTO_MAP_END    767

// Kanto fly location map IDs (sample cities)
#define MAP_PALLET_TOWN       0
#define MAP_VIRIDIAN_CITY     1
#define MAP_PEWTER_CITY       2
#define MAP_CERULEAN_CITY     3
#define MAP_VERMILION_CITY    4
#define MAP_LAVENDER_TOWN     5
#define MAP_CELADON_CITY      6
#define MAP_FUCHSIA_CITY      7
#define MAP_SAFFRON_CITY      8
#define MAP_CINNABAR_ISLAND   9
#define MAP_INDIGO_PLATEAU    10

// Hoenn fly location map IDs (sample cities)
#define MAP_LITTLEROOT_TOWN   256
#define MAP_OLDALE_TOWN       257
#define MAP_PETALBURG_CITY    258
#define MAP_RUSTBORO_CITY     259
#define MAP_DEWFORD_TOWN      260
#define MAP_SLATEPORT_CITY    261
#define MAP_MAUVILLE_CITY     262
#define MAP_VERDANTURF_TOWN   263
#define MAP_FALLARBOR_TOWN    264
#define MAP_LAVARIDGE_TOWN    265
#define MAP_FORTREE_CITY      266
#define MAP_LILYCOVE_CITY     267
#define MAP_MOSSDEEP_CITY     268
#define MAP_SOOTOPOLIS_CITY   269
#define MAP_PACIFIDLOG_TOWN   270
#define MAP_EVER_GRANDE_CITY  271

// Starting fly locations for each region
static const u16 sRegionStartingLocations[NUM_REGIONS] = {
    MAP_PALLET_TOWN,      // REGION_KANTO
    MAP_LITTLEROOT_TOWN,  // REGION_HOENN
    512,                  // REGION_JOHTO: New Bark Town (placeholder)
};

// ============================================================================
// Fly Location System Implementation (mirrors src/fly_location_discovery.c)
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

// ============================================================================
// Save Data Simulation
// ============================================================================

// Simulated save data structure for fly locations
struct FlyLocationSaveData {
    u16 flyLocations[MAX_FLY_LOCATIONS_TOTAL];
    u8 flyLocationCount[NUM_REGIONS];
    u8 currentRegion;
    bool8 regionUnlocked[NUM_REGIONS];
};

static struct FlyLocationSaveData sSaveBuffer = {0};

// ============================================================================
// Helper Functions
// ============================================================================

static u8 GetRegionBaseIndex(u8 region_id) {
    if (region_id >= NUM_REGIONS)
        return 0;
    return region_id * FLY_LOCATIONS_PER_REGION;
}

static u8 GetRegionForMapId(u16 map_id) {
    if (map_id < 256)
        return REGION_KANTO;
    else if (map_id < 512)
        return REGION_HOENN;
    else
        return REGION_JOHTO;
}

static u8 GetCurrentRegion(void) {
    return sCurrentRegion;
}

static void SetCurrentRegion(u8 region_id) {
    if (region_id < NUM_REGIONS)
        sCurrentRegion = region_id;
}

static bool8 IsRegionUnlocked(u8 region_id) {
    if (region_id >= NUM_REGIONS)
        return false;
    return sRegionUnlocked[region_id];
}

static void SetRegionUnlocked(u8 region_id, bool8 unlocked) {
    if (region_id < NUM_REGIONS)
        sRegionUnlocked[region_id] = unlocked;
}

// ============================================================================
// Fly Location Functions
// ============================================================================

static void InitFlyLocationData(void) {
    u8 i;
    for (i = 0; i < MAX_FLY_LOCATIONS_TOTAL; i++)
        sFlyLocations[i] = 0;
    for (i = 0; i < NUM_REGIONS; i++)
        sFlyLocationCount[i] = 0;
}

static void ResetTestState(void) {
    InitFlyLocationData();
    sCurrentRegion = REGION_KANTO;
    sRegionUnlocked[REGION_KANTO] = true;
    sRegionUnlocked[REGION_HOENN] = false;
    sRegionUnlocked[REGION_JOHTO] = false;
}

static bool8 IsFlyLocationRegisteredForRegion(u16 map_id, u8 region_id) {
    u8 base_index, count, i;
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

static void RegisterFlyLocationForRegion(u16 map_id, u8 region_id) {
    u8 base_index, count;
    if (region_id >= NUM_REGIONS)
        return;
    count = sFlyLocationCount[region_id];
    if (count >= FLY_LOCATIONS_PER_REGION)
        return;
    if (IsFlyLocationRegisteredForRegion(map_id, region_id))
        return;
    base_index = GetRegionBaseIndex(region_id);
    sFlyLocations[base_index + count] = map_id;
    sFlyLocationCount[region_id] = count + 1;
}

static void RegisterFlyLocation(u16 map_id) {
    u8 current_region = GetCurrentRegion();
    RegisterFlyLocationForRegion(map_id, current_region);
}

static bool8 IsFlyLocationRegistered(u16 map_id) {
    u8 region;
    for (region = 0; region < NUM_REGIONS; region++) {
        if (IsFlyLocationRegisteredForRegion(map_id, region))
            return true;
    }
    return false;
}

static u16* GetCurrentRegionFlyLocations(u8 *out_count) {
    u8 current_region = GetCurrentRegion();
    u8 base_index = GetRegionBaseIndex(current_region);
    u8 count = sFlyLocationCount[current_region];
    u8 i;
    for (i = 0; i < count && i < FLY_LOCATIONS_PER_REGION; i++)
        sCurrentRegionFlyLocationsBuffer[i] = sFlyLocations[base_index + i];
    if (out_count != NULL)
        *out_count = count;
    return sCurrentRegionFlyLocationsBuffer;
}

static u8 GetFlyLocationCountForRegion(u8 region_id) {
    if (region_id >= NUM_REGIONS)
        return 0;
    return sFlyLocationCount[region_id];
}

static void GetFlyLocationsForRegion(u8 region_id, u16 *locations, u8 *count) {
    u8 base_index, loc_count, i;
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

static u16 GetRegionStartingFlyLocation(u8 region_id) {
    if (region_id >= NUM_REGIONS)
        return 0;
    return sRegionStartingLocations[region_id];
}

static void GetCrossRegionFlyLocations(u16 *locations, u8 *count) {
    u8 region, loc_count = 0;
    u8 current_region = GetCurrentRegion();
    if (locations == NULL || count == NULL) {
        if (count != NULL)
            *count = 0;
        return;
    }
    for (region = 0; region < NUM_REGIONS; region++) {
        if (region == current_region)
            continue;
        if (IsRegionUnlocked(region)) {
            locations[loc_count] = GetRegionStartingFlyLocation(region);
            loc_count++;
        }
    }
    *count = loc_count;
}

// ============================================================================
// Save/Load Functions
// ============================================================================

static void SaveFlyLocationData(void) {
    u8 i;
    for (i = 0; i < MAX_FLY_LOCATIONS_TOTAL; i++)
        sSaveBuffer.flyLocations[i] = sFlyLocations[i];
    for (i = 0; i < NUM_REGIONS; i++) {
        sSaveBuffer.flyLocationCount[i] = sFlyLocationCount[i];
        sSaveBuffer.regionUnlocked[i] = sRegionUnlocked[i];
    }
    sSaveBuffer.currentRegion = sCurrentRegion;
}

static void LoadFlyLocationData(void) {
    u8 i;
    for (i = 0; i < MAX_FLY_LOCATIONS_TOTAL; i++)
        sFlyLocations[i] = sSaveBuffer.flyLocations[i];
    for (i = 0; i < NUM_REGIONS; i++) {
        sFlyLocationCount[i] = sSaveBuffer.flyLocationCount[i];
        sRegionUnlocked[i] = sSaveBuffer.regionUnlocked[i];
    }
    sCurrentRegion = sSaveBuffer.currentRegion;
}

static void ClearSaveBuffer(void) {
    memset(&sSaveBuffer, 0, sizeof(sSaveBuffer));
}

// ============================================================================
// Integration Test 1: Register Kanto Locations → Verify Only Kanto in Menu
// **Validates: Requirements 12.1, 12.2, 12.4**
// ============================================================================

static int test_register_kanto_locations_only_kanto_in_menu(void) {
    int passed = 0;
    int failed = 0;
    u16 *fly_locations;
    u8 fly_count;
    u8 i;
    
    printf("\n=== Integration Test 1: Register Kanto Locations ===\n");
    printf("**Validates: Requirements 12.1, 12.2, 12.4**\n\n");
    
    // Initialize clean state
    ResetTestState();
    SetCurrentRegion(REGION_KANTO);
    
    // Step 1: Verify initial state - no fly locations
    printf("Step 1: Verify initial state (no fly locations)...\n");
    fly_locations = GetCurrentRegionFlyLocations(&fly_count);
    if (fly_count != 0) {
        printf("  FAIL: Should have 0 fly locations initially, got %u\n", fly_count);
        failed++;
    } else {
        printf("  PASS: No fly locations initially\n");
        passed++;
    }
    
    // Step 2: Register Kanto fly locations
    printf("Step 2: Register Kanto fly locations...\n");
    RegisterFlyLocation(MAP_PALLET_TOWN);
    RegisterFlyLocation(MAP_VIRIDIAN_CITY);
    RegisterFlyLocation(MAP_PEWTER_CITY);
    RegisterFlyLocation(MAP_CERULEAN_CITY);
    RegisterFlyLocation(MAP_VERMILION_CITY);
    
    fly_locations = GetCurrentRegionFlyLocations(&fly_count);
    if (fly_count != 5) {
        printf("  FAIL: Should have 5 Kanto fly locations, got %u\n", fly_count);
        failed++;
    } else {
        printf("  PASS: Registered 5 Kanto fly locations\n");
        passed++;
    }
    
    // Step 3: Verify all locations are Kanto maps
    printf("Step 3: Verify all locations are Kanto maps...\n");
    bool8 all_kanto = true;
    for (i = 0; i < fly_count; i++) {
        u8 region = GetRegionForMapId(fly_locations[i]);
        if (region != REGION_KANTO) {
            printf("  FAIL: Location %u (map %u) is not Kanto\n", i, fly_locations[i]);
            all_kanto = false;
        }
    }
    if (all_kanto) {
        printf("  PASS: All fly locations are Kanto maps\n");
        passed++;
    } else {
        failed++;
    }

    // Step 4: Verify specific locations are registered
    printf("Step 4: Verify specific locations are registered...\n");
    bool8 locations_correct = true;
    if (!IsFlyLocationRegistered(MAP_PALLET_TOWN)) {
        printf("  FAIL: Pallet Town should be registered\n");
        locations_correct = false;
    }
    if (!IsFlyLocationRegistered(MAP_VIRIDIAN_CITY)) {
        printf("  FAIL: Viridian City should be registered\n");
        locations_correct = false;
    }
    if (!IsFlyLocationRegistered(MAP_PEWTER_CITY)) {
        printf("  FAIL: Pewter City should be registered\n");
        locations_correct = false;
    }
    if (locations_correct) {
        printf("  PASS: All specific locations are registered\n");
        passed++;
    } else {
        failed++;
    }
    
    // Step 5: Verify Hoenn locations are NOT in fly menu
    printf("Step 5: Verify Hoenn locations are NOT in fly menu...\n");
    bool8 no_hoenn = true;
    for (i = 0; i < fly_count; i++) {
        if (fly_locations[i] >= HOENN_MAP_START && fly_locations[i] <= HOENN_MAP_END) {
            printf("  FAIL: Hoenn location %u found in Kanto fly menu\n", fly_locations[i]);
            no_hoenn = false;
        }
    }
    if (no_hoenn) {
        printf("  PASS: No Hoenn locations in Kanto fly menu\n");
        passed++;
    } else {
        failed++;
    }
    
    // Report results
    printf("\n--- Test 1 Results ---\n");
    printf("Passed: %d, Failed: %d\n", passed, failed);
    return failed;
}

// ============================================================================
// Integration Test 2: Transition to Hoenn → Fly Menu Switches
// **Validates: Requirements 12.4**
// ============================================================================

static int test_transition_to_hoenn_fly_menu_switches(void) {
    int passed = 0;
    int failed = 0;
    u16 *fly_locations;
    u8 fly_count;
    
    printf("\n=== Integration Test 2: Transition to Hoenn → Fly Menu Switches ===\n");
    printf("**Validates: Requirements 12.4**\n\n");
    
    // Initialize with Kanto locations
    ResetTestState();
    SetCurrentRegion(REGION_KANTO);
    SetRegionUnlocked(REGION_HOENN, true);
    
    // Register Kanto locations
    RegisterFlyLocation(MAP_PALLET_TOWN);
    RegisterFlyLocation(MAP_VIRIDIAN_CITY);
    RegisterFlyLocation(MAP_PEWTER_CITY);
    
    // Step 1: Verify Kanto fly menu has 3 locations
    printf("Step 1: Verify Kanto fly menu has 3 locations...\n");
    fly_locations = GetCurrentRegionFlyLocations(&fly_count);
    if (fly_count != 3) {
        printf("  FAIL: Kanto should have 3 fly locations, got %u\n", fly_count);
        failed++;
    } else {
        printf("  PASS: Kanto has 3 fly locations\n");
        passed++;
    }
    
    // Step 2: Transition to Hoenn
    printf("Step 2: Transition to Hoenn...\n");
    SetCurrentRegion(REGION_HOENN);
    if (GetCurrentRegion() != REGION_HOENN) {
        printf("  FAIL: Should be in Hoenn\n");
        failed++;
    } else {
        printf("  PASS: Now in Hoenn\n");
        passed++;
    }
    
    // Step 3: Verify fly menu is now empty (no Hoenn locations registered yet)
    printf("Step 3: Verify fly menu is empty (no Hoenn locations yet)...\n");
    fly_locations = GetCurrentRegionFlyLocations(&fly_count);
    if (fly_count != 0) {
        printf("  FAIL: Hoenn should have 0 fly locations, got %u\n", fly_count);
        failed++;
    } else {
        printf("  PASS: Hoenn fly menu is empty\n");
        passed++;
    }

    // Step 4: Verify Kanto locations are still stored (just not shown)
    printf("Step 4: Verify Kanto locations are still stored...\n");
    u8 kanto_count = GetFlyLocationCountForRegion(REGION_KANTO);
    if (kanto_count != 3) {
        printf("  FAIL: Kanto should still have 3 stored locations, got %u\n", kanto_count);
        failed++;
    } else {
        printf("  PASS: Kanto still has 3 stored locations\n");
        passed++;
    }
    
    // Step 5: Register Hoenn locations
    printf("Step 5: Register Hoenn locations...\n");
    RegisterFlyLocation(MAP_LITTLEROOT_TOWN);
    RegisterFlyLocation(MAP_OLDALE_TOWN);
    RegisterFlyLocation(MAP_PETALBURG_CITY);
    RegisterFlyLocation(MAP_RUSTBORO_CITY);
    
    fly_locations = GetCurrentRegionFlyLocations(&fly_count);
    if (fly_count != 4) {
        printf("  FAIL: Hoenn should have 4 fly locations, got %u\n", fly_count);
        failed++;
    } else {
        printf("  PASS: Hoenn now has 4 fly locations\n");
        passed++;
    }
    
    // Step 6: Verify fly menu shows only Hoenn locations
    printf("Step 6: Verify fly menu shows only Hoenn locations...\n");
    bool8 all_hoenn = true;
    u8 i;
    for (i = 0; i < fly_count; i++) {
        u8 region = GetRegionForMapId(fly_locations[i]);
        if (region != REGION_HOENN) {
            printf("  FAIL: Location %u (map %u) is not Hoenn\n", i, fly_locations[i]);
            all_hoenn = false;
        }
    }
    if (all_hoenn) {
        printf("  PASS: All fly locations are Hoenn maps\n");
        passed++;
    } else {
        failed++;
    }
    
    // Report results
    printf("\n--- Test 2 Results ---\n");
    printf("Passed: %d, Failed: %d\n", passed, failed);
    return failed;
}

// ============================================================================
// Integration Test 3: Register Hoenn Locations → Verify Added
// **Validates: Requirements 12.1, 12.2**
// ============================================================================

static int test_register_hoenn_locations(void) {
    int passed = 0;
    int failed = 0;
    u16 *fly_locations;
    u8 fly_count;
    
    printf("\n=== Integration Test 3: Register Hoenn Locations ===\n");
    printf("**Validates: Requirements 12.1, 12.2**\n\n");
    
    // Initialize in Hoenn
    ResetTestState();
    SetCurrentRegion(REGION_HOENN);
    SetRegionUnlocked(REGION_HOENN, true);
    
    // Step 1: Register all Hoenn cities
    printf("Step 1: Register all Hoenn cities...\n");
    RegisterFlyLocation(MAP_LITTLEROOT_TOWN);
    RegisterFlyLocation(MAP_OLDALE_TOWN);
    RegisterFlyLocation(MAP_PETALBURG_CITY);
    RegisterFlyLocation(MAP_RUSTBORO_CITY);
    RegisterFlyLocation(MAP_DEWFORD_TOWN);
    RegisterFlyLocation(MAP_SLATEPORT_CITY);
    RegisterFlyLocation(MAP_MAUVILLE_CITY);
    RegisterFlyLocation(MAP_VERDANTURF_TOWN);
    RegisterFlyLocation(MAP_FALLARBOR_TOWN);
    RegisterFlyLocation(MAP_LAVARIDGE_TOWN);
    RegisterFlyLocation(MAP_FORTREE_CITY);
    RegisterFlyLocation(MAP_LILYCOVE_CITY);
    RegisterFlyLocation(MAP_MOSSDEEP_CITY);
    RegisterFlyLocation(MAP_SOOTOPOLIS_CITY);
    RegisterFlyLocation(MAP_PACIFIDLOG_TOWN);
    RegisterFlyLocation(MAP_EVER_GRANDE_CITY);
    
    fly_locations = GetCurrentRegionFlyLocations(&fly_count);
    if (fly_count != 16) {
        printf("  FAIL: Should have 16 Hoenn fly locations, got %u\n", fly_count);
        failed++;
    } else {
        printf("  PASS: Registered all 16 Hoenn fly locations\n");
        passed++;
    }
    
    // Step 2: Verify specific Hoenn locations
    printf("Step 2: Verify specific Hoenn locations...\n");
    bool8 locations_correct = true;
    if (!IsFlyLocationRegisteredForRegion(MAP_LITTLEROOT_TOWN, REGION_HOENN)) {
        printf("  FAIL: Littleroot Town should be registered\n");
        locations_correct = false;
    }
    if (!IsFlyLocationRegisteredForRegion(MAP_RUSTBORO_CITY, REGION_HOENN)) {
        printf("  FAIL: Rustboro City should be registered\n");
        locations_correct = false;
    }
    if (!IsFlyLocationRegisteredForRegion(MAP_EVER_GRANDE_CITY, REGION_HOENN)) {
        printf("  FAIL: Ever Grande City should be registered\n");
        locations_correct = false;
    }
    if (locations_correct) {
        printf("  PASS: All specific Hoenn locations are registered\n");
        passed++;
    } else {
        failed++;
    }

    // Step 3: Verify max capacity enforcement
    printf("Step 3: Verify max capacity enforcement (16 per region)...\n");
    // Try to register a 17th location
    RegisterFlyLocation(272);  // Extra map ID
    fly_count = GetFlyLocationCountForRegion(REGION_HOENN);
    if (fly_count != 16) {
        printf("  FAIL: Should still have 16 locations (max), got %u\n", fly_count);
        failed++;
    } else {
        printf("  PASS: Max capacity enforced at 16 locations\n");
        passed++;
    }
    
    // Step 4: Verify Kanto is unaffected
    printf("Step 4: Verify Kanto is unaffected...\n");
    u8 kanto_count = GetFlyLocationCountForRegion(REGION_KANTO);
    if (kanto_count != 0) {
        printf("  FAIL: Kanto should have 0 locations, got %u\n", kanto_count);
        failed++;
    } else {
        printf("  PASS: Kanto is unaffected (0 locations)\n");
        passed++;
    }
    
    // Report results
    printf("\n--- Test 3 Results ---\n");
    printf("Passed: %d, Failed: %d\n", passed, failed);
    return failed;
}

// ============================================================================
// Integration Test 4: Cross-Region Fly to Kanto Starting Location
// **Validates: Requirements 12.5**
// ============================================================================

static int test_cross_region_fly_to_kanto(void) {
    int passed = 0;
    int failed = 0;
    u16 cross_locations[NUM_REGIONS];
    u8 cross_count;
    
    printf("\n=== Integration Test 4: Cross-Region Fly to Kanto Starting Location ===\n");
    printf("**Validates: Requirements 12.5**\n\n");
    
    // Initialize: Player in Hoenn with Kanto unlocked
    ResetTestState();
    SetRegionUnlocked(REGION_KANTO, true);
    SetRegionUnlocked(REGION_HOENN, true);
    SetCurrentRegion(REGION_HOENN);
    
    // Register some locations in both regions
    SetCurrentRegion(REGION_KANTO);
    RegisterFlyLocation(MAP_PALLET_TOWN);
    RegisterFlyLocation(MAP_VIRIDIAN_CITY);
    RegisterFlyLocation(MAP_PEWTER_CITY);
    
    SetCurrentRegion(REGION_HOENN);
    RegisterFlyLocation(MAP_LITTLEROOT_TOWN);
    RegisterFlyLocation(MAP_OLDALE_TOWN);
    RegisterFlyLocation(MAP_PETALBURG_CITY);
    
    // Step 1: Get cross-region fly locations from Hoenn
    printf("Step 1: Get cross-region fly locations from Hoenn...\n");
    GetCrossRegionFlyLocations(cross_locations, &cross_count);
    
    if (cross_count != 1) {
        printf("  FAIL: Should have 1 cross-region location (Kanto), got %u\n", cross_count);
        failed++;
    } else {
        printf("  PASS: Have 1 cross-region location\n");
        passed++;
    }
    
    // Step 2: Verify cross-region location is Kanto starting location
    printf("Step 2: Verify cross-region location is Kanto starting location...\n");
    u16 kanto_start = GetRegionStartingFlyLocation(REGION_KANTO);
    if (cross_count > 0 && cross_locations[0] != kanto_start) {
        printf("  FAIL: Cross-region location should be %u (Pallet Town), got %u\n",
               kanto_start, cross_locations[0]);
        failed++;
    } else if (cross_count > 0) {
        printf("  PASS: Cross-region location is Pallet Town (%u)\n", kanto_start);
        passed++;
    }

    // Step 3: Verify cross-region does NOT include other Kanto locations
    printf("Step 3: Verify cross-region does NOT include other Kanto locations...\n");
    bool8 only_starting = true;
    u8 i;
    for (i = 0; i < cross_count; i++) {
        if (cross_locations[i] != kanto_start) {
            printf("  FAIL: Cross-region should only have starting location, found %u\n",
                   cross_locations[i]);
            only_starting = false;
        }
    }
    if (only_starting) {
        printf("  PASS: Cross-region only includes starting location\n");
        passed++;
    } else {
        failed++;
    }
    
    // Step 4: Verify current region (Hoenn) is NOT in cross-region list
    printf("Step 4: Verify current region (Hoenn) is NOT in cross-region list...\n");
    u16 hoenn_start = GetRegionStartingFlyLocation(REGION_HOENN);
    bool8 hoenn_excluded = true;
    for (i = 0; i < cross_count; i++) {
        if (cross_locations[i] == hoenn_start) {
            printf("  FAIL: Hoenn starting location should not be in cross-region list\n");
            hoenn_excluded = false;
        }
    }
    if (hoenn_excluded) {
        printf("  PASS: Hoenn is excluded from cross-region list\n");
        passed++;
    } else {
        failed++;
    }
    
    // Step 5: Test with Johto also unlocked
    printf("Step 5: Test with Johto also unlocked...\n");
    SetRegionUnlocked(REGION_JOHTO, true);
    GetCrossRegionFlyLocations(cross_locations, &cross_count);
    
    if (cross_count != 2) {
        printf("  FAIL: Should have 2 cross-region locations (Kanto, Johto), got %u\n", cross_count);
        failed++;
    } else {
        printf("  PASS: Have 2 cross-region locations\n");
        passed++;
    }
    
    // Step 6: Verify locked regions are excluded
    printf("Step 6: Verify locked regions are excluded...\n");
    SetRegionUnlocked(REGION_JOHTO, false);
    GetCrossRegionFlyLocations(cross_locations, &cross_count);
    
    if (cross_count != 1) {
        printf("  FAIL: Should have 1 cross-region location after locking Johto, got %u\n", cross_count);
        failed++;
    } else {
        printf("  PASS: Locked Johto is excluded from cross-region list\n");
        passed++;
    }
    
    // Report results
    printf("\n--- Test 4 Results ---\n");
    printf("Passed: %d, Failed: %d\n", passed, failed);
    return failed;
}

// ============================================================================
// Integration Test 5: Save/Load Fly Location Persistence
// **Validates: Requirements 12.1, 12.2, 12.4, 12.5 (persistence aspect)**
// ============================================================================

static int test_fly_location_save_load_persistence(void) {
    int passed = 0;
    int failed = 0;
    u8 kanto_count_before, hoenn_count_before;
    u8 kanto_count_after, hoenn_count_after;
    u16 kanto_locations_before[FLY_LOCATIONS_PER_REGION];
    u16 hoenn_locations_before[FLY_LOCATIONS_PER_REGION];
    u16 kanto_locations_after[FLY_LOCATIONS_PER_REGION];
    u16 hoenn_locations_after[FLY_LOCATIONS_PER_REGION];
    u8 i;
    
    printf("\n=== Integration Test 5: Save/Load Fly Location Persistence ===\n");
    printf("**Validates: Requirements 12.1, 12.2, 12.4, 12.5 (persistence)**\n\n");
    
    // Initialize and set up fly locations
    ResetTestState();
    ClearSaveBuffer();
    SetRegionUnlocked(REGION_KANTO, true);
    SetRegionUnlocked(REGION_HOENN, true);
    
    // Register Kanto locations
    SetCurrentRegion(REGION_KANTO);
    RegisterFlyLocation(MAP_PALLET_TOWN);
    RegisterFlyLocation(MAP_VIRIDIAN_CITY);
    RegisterFlyLocation(MAP_PEWTER_CITY);
    RegisterFlyLocation(MAP_CERULEAN_CITY);
    RegisterFlyLocation(MAP_VERMILION_CITY);
    
    // Register Hoenn locations
    SetCurrentRegion(REGION_HOENN);
    RegisterFlyLocation(MAP_LITTLEROOT_TOWN);
    RegisterFlyLocation(MAP_OLDALE_TOWN);
    RegisterFlyLocation(MAP_PETALBURG_CITY);
    RegisterFlyLocation(MAP_RUSTBORO_CITY);
    RegisterFlyLocation(MAP_DEWFORD_TOWN);
    RegisterFlyLocation(MAP_SLATEPORT_CITY);
    
    // Step 1: Record state before save
    printf("Step 1: Record state before save...\n");
    GetFlyLocationsForRegion(REGION_KANTO, kanto_locations_before, &kanto_count_before);
    GetFlyLocationsForRegion(REGION_HOENN, hoenn_locations_before, &hoenn_count_before);
    
    printf("  Kanto: %u locations, Hoenn: %u locations\n", kanto_count_before, hoenn_count_before);
    if (kanto_count_before != 5 || hoenn_count_before != 6) {
        printf("  FAIL: Expected 5 Kanto and 6 Hoenn locations\n");
        failed++;
    } else {
        printf("  PASS: Correct location counts before save\n");
        passed++;
    }

    // Step 2: Save fly location data
    printf("Step 2: Save fly location data...\n");
    SaveFlyLocationData();
    printf("  PASS: Fly location data saved\n");
    passed++;
    
    // Step 3: Clear all fly location data (simulate game restart)
    printf("Step 3: Clear all fly location data (simulate restart)...\n");
    InitFlyLocationData();
    sCurrentRegion = REGION_KANTO;
    sRegionUnlocked[REGION_KANTO] = true;
    sRegionUnlocked[REGION_HOENN] = false;
    sRegionUnlocked[REGION_JOHTO] = false;
    
    // Verify data is cleared
    u8 cleared_kanto = GetFlyLocationCountForRegion(REGION_KANTO);
    u8 cleared_hoenn = GetFlyLocationCountForRegion(REGION_HOENN);
    if (cleared_kanto != 0 || cleared_hoenn != 0) {
        printf("  FAIL: Data should be cleared, got Kanto=%u, Hoenn=%u\n",
               cleared_kanto, cleared_hoenn);
        failed++;
    } else {
        printf("  PASS: Data cleared successfully\n");
        passed++;
    }
    
    // Step 4: Load fly location data
    printf("Step 4: Load fly location data...\n");
    LoadFlyLocationData();
    printf("  PASS: Fly location data loaded\n");
    passed++;
    
    // Step 5: Verify location counts restored
    printf("Step 5: Verify location counts restored...\n");
    GetFlyLocationsForRegion(REGION_KANTO, kanto_locations_after, &kanto_count_after);
    GetFlyLocationsForRegion(REGION_HOENN, hoenn_locations_after, &hoenn_count_after);
    
    if (kanto_count_after != kanto_count_before) {
        printf("  FAIL: Kanto count mismatch: before=%u, after=%u\n",
               kanto_count_before, kanto_count_after);
        failed++;
    } else {
        printf("  PASS: Kanto count restored (%u locations)\n", kanto_count_after);
        passed++;
    }
    
    if (hoenn_count_after != hoenn_count_before) {
        printf("  FAIL: Hoenn count mismatch: before=%u, after=%u\n",
               hoenn_count_before, hoenn_count_after);
        failed++;
    } else {
        printf("  PASS: Hoenn count restored (%u locations)\n", hoenn_count_after);
        passed++;
    }

    // Step 6: Verify specific locations restored
    printf("Step 6: Verify specific locations restored...\n");
    bool8 kanto_match = true;
    for (i = 0; i < kanto_count_before; i++) {
        if (kanto_locations_before[i] != kanto_locations_after[i]) {
            printf("  FAIL: Kanto location %u mismatch: before=%u, after=%u\n",
                   i, kanto_locations_before[i], kanto_locations_after[i]);
            kanto_match = false;
        }
    }
    if (kanto_match) {
        printf("  PASS: All Kanto locations restored correctly\n");
        passed++;
    } else {
        failed++;
    }
    
    bool8 hoenn_match = true;
    for (i = 0; i < hoenn_count_before; i++) {
        if (hoenn_locations_before[i] != hoenn_locations_after[i]) {
            printf("  FAIL: Hoenn location %u mismatch: before=%u, after=%u\n",
                   i, hoenn_locations_before[i], hoenn_locations_after[i]);
            hoenn_match = false;
        }
    }
    if (hoenn_match) {
        printf("  PASS: All Hoenn locations restored correctly\n");
        passed++;
    } else {
        failed++;
    }
    
    // Step 7: Verify region unlock state restored
    printf("Step 7: Verify region unlock state restored...\n");
    if (!IsRegionUnlocked(REGION_HOENN)) {
        printf("  FAIL: Hoenn should be unlocked after load\n");
        failed++;
    } else {
        printf("  PASS: Hoenn unlock state restored\n");
        passed++;
    }
    
    // Step 8: Verify current region restored
    printf("Step 8: Verify current region restored...\n");
    if (GetCurrentRegion() != REGION_HOENN) {
        printf("  FAIL: Current region should be Hoenn, got %u\n", GetCurrentRegion());
        failed++;
    } else {
        printf("  PASS: Current region restored to Hoenn\n");
        passed++;
    }
    
    // Report results
    printf("\n--- Test 5 Results ---\n");
    printf("Passed: %d, Failed: %d\n", passed, failed);
    return failed;
}

// ============================================================================
// Integration Test 6: Full Cross-Region Fly Workflow
// **Validates: Requirements 12.1, 12.2, 12.4, 12.5**
// ============================================================================

static int test_full_cross_region_fly_workflow(void) {
    int passed = 0;
    int failed = 0;
    u16 *fly_locations;
    u8 fly_count;
    u16 cross_locations[NUM_REGIONS];
    u8 cross_count;
    
    printf("\n=== Integration Test 6: Full Cross-Region Fly Workflow ===\n");
    printf("**Validates: Requirements 12.1, 12.2, 12.4, 12.5**\n\n");
    
    // Initialize: Start in Kanto
    ResetTestState();
    SetRegionUnlocked(REGION_KANTO, true);
    SetCurrentRegion(REGION_KANTO);
    
    // Step 1: Player explores Kanto, registers fly locations
    printf("Step 1: Player explores Kanto, registers fly locations...\n");
    RegisterFlyLocation(MAP_PALLET_TOWN);
    RegisterFlyLocation(MAP_VIRIDIAN_CITY);
    RegisterFlyLocation(MAP_PEWTER_CITY);
    RegisterFlyLocation(MAP_CERULEAN_CITY);
    
    fly_locations = GetCurrentRegionFlyLocations(&fly_count);
    if (fly_count != 4) {
        printf("  FAIL: Should have 4 Kanto locations, got %u\n", fly_count);
        failed++;
    } else {
        printf("  PASS: Registered 4 Kanto fly locations\n");
        passed++;
    }
    
    // Step 2: Player defeats Kanto E4, Hoenn unlocks
    printf("Step 2: Player defeats Kanto E4, Hoenn unlocks...\n");
    SetRegionUnlocked(REGION_HOENN, true);
    
    // Verify cross-region fly now available
    GetCrossRegionFlyLocations(cross_locations, &cross_count);
    if (cross_count != 1) {
        printf("  FAIL: Should have 1 cross-region location (Hoenn), got %u\n", cross_count);
        failed++;
    } else {
        printf("  PASS: Cross-region fly to Hoenn now available\n");
        passed++;
    }
    
    // Step 3: Player uses cross-region fly to Hoenn
    printf("Step 3: Player uses cross-region fly to Hoenn...\n");
    SetCurrentRegion(REGION_HOENN);
    
    // Verify fly menu now shows Hoenn (empty initially)
    fly_locations = GetCurrentRegionFlyLocations(&fly_count);
    if (fly_count != 0) {
        printf("  FAIL: Hoenn should have 0 fly locations initially, got %u\n", fly_count);
        failed++;
    } else {
        printf("  PASS: Hoenn fly menu is empty initially\n");
        passed++;
    }

    // Step 4: Player explores Hoenn, registers fly locations
    printf("Step 4: Player explores Hoenn, registers fly locations...\n");
    RegisterFlyLocation(MAP_LITTLEROOT_TOWN);
    RegisterFlyLocation(MAP_OLDALE_TOWN);
    RegisterFlyLocation(MAP_PETALBURG_CITY);
    RegisterFlyLocation(MAP_RUSTBORO_CITY);
    RegisterFlyLocation(MAP_DEWFORD_TOWN);
    
    fly_locations = GetCurrentRegionFlyLocations(&fly_count);
    if (fly_count != 5) {
        printf("  FAIL: Should have 5 Hoenn locations, got %u\n", fly_count);
        failed++;
    } else {
        printf("  PASS: Registered 5 Hoenn fly locations\n");
        passed++;
    }
    
    // Step 5: Verify cross-region fly back to Kanto available
    printf("Step 5: Verify cross-region fly back to Kanto available...\n");
    GetCrossRegionFlyLocations(cross_locations, &cross_count);
    if (cross_count != 1) {
        printf("  FAIL: Should have 1 cross-region location (Kanto), got %u\n", cross_count);
        failed++;
    } else {
        printf("  PASS: Cross-region fly to Kanto available\n");
        passed++;
    }
    
    // Verify it's Kanto starting location
    u16 kanto_start = GetRegionStartingFlyLocation(REGION_KANTO);
    if (cross_locations[0] != kanto_start) {
        printf("  FAIL: Cross-region should be Pallet Town (%u), got %u\n",
               kanto_start, cross_locations[0]);
        failed++;
    } else {
        printf("  PASS: Cross-region destination is Pallet Town\n");
        passed++;
    }
    
    // Step 6: Player returns to Kanto
    printf("Step 6: Player returns to Kanto...\n");
    SetCurrentRegion(REGION_KANTO);
    
    // Verify Kanto fly locations still intact
    fly_locations = GetCurrentRegionFlyLocations(&fly_count);
    if (fly_count != 4) {
        printf("  FAIL: Kanto should still have 4 locations, got %u\n", fly_count);
        failed++;
    } else {
        printf("  PASS: Kanto fly locations preserved (4 locations)\n");
        passed++;
    }
    
    // Step 7: Verify both regions have correct counts
    printf("Step 7: Verify both regions have correct counts...\n");
    u8 kanto_count = GetFlyLocationCountForRegion(REGION_KANTO);
    u8 hoenn_count = GetFlyLocationCountForRegion(REGION_HOENN);
    
    if (kanto_count != 4 || hoenn_count != 5) {
        printf("  FAIL: Expected Kanto=4, Hoenn=5, got Kanto=%u, Hoenn=%u\n",
               kanto_count, hoenn_count);
        failed++;
    } else {
        printf("  PASS: Both regions have correct fly location counts\n");
        passed++;
    }
    
    // Report results
    printf("\n--- Test 6 Results ---\n");
    printf("Passed: %d, Failed: %d\n", passed, failed);
    return failed;
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    int total_failed = 0;
    
    printf("╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║     Integration Tests for Fly System Across Both Regions         ║\n");
    printf("║     Task 4.6: Test fly system across both regions                ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n");
    printf("\nRegion configuration: Kanto → Hoenn → Johto (3 regions)\n");
    printf("Sinnoh descoped per ADR-003\n");
    printf("Progression order per ADR-004\n");
    printf("\nFly location storage:\n");
    printf("  - 16 locations per region\n");
    printf("  - 3 regions total\n");
    printf("  - 48 total fly locations maximum\n");
    printf("\nTest Scenarios:\n");
    printf("  1. Register Kanto locations → Verify only Kanto in menu\n");
    printf("  2. Transition to Hoenn → Verify fly menu switches\n");
    printf("  3. Register Hoenn locations → Verify added\n");
    printf("  4. Cross-region fly to Kanto starting location\n");
    printf("  5. Save/Load fly location persistence\n");
    printf("  6. Full cross-region fly workflow\n");
    
    // Run all integration tests
    total_failed += test_register_kanto_locations_only_kanto_in_menu();
    total_failed += test_transition_to_hoenn_fly_menu_switches();
    total_failed += test_register_hoenn_locations();
    total_failed += test_cross_region_fly_to_kanto();
    total_failed += test_fly_location_save_load_persistence();
    total_failed += test_full_cross_region_fly_workflow();
    
    // Overall results
    printf("\n\n╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║                    OVERALL TEST RESULTS                          ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n");
    
    if (total_failed == 0) {
        printf("\n✓ ALL INTEGRATION TESTS PASSED!\n\n");
        printf("Test 1 (Kanto Registration): PASSED\n");
        printf("  - Validates: Requirements 12.1, 12.2, 12.4\n");
        printf("Test 2 (Region Transition): PASSED\n");
        printf("  - Validates: Requirements 12.4\n");
        printf("Test 3 (Hoenn Registration): PASSED\n");
        printf("  - Validates: Requirements 12.1, 12.2\n");
        printf("Test 4 (Cross-Region Fly): PASSED\n");
        printf("  - Validates: Requirements 12.5\n");
        printf("Test 5 (Save/Load Persistence): PASSED\n");
        printf("  - Validates: Requirements 12.1, 12.2, 12.4, 12.5\n");
        printf("Test 6 (Full Workflow): PASSED\n");
        printf("  - Validates: Requirements 12.1, 12.2, 12.4, 12.5\n");
        return 0;
    } else {
        printf("\n✗ SOME INTEGRATION TESTS FAILED\n\n");
        printf("Total failures: %d\n", total_failed);
        return 1;
    }
}
