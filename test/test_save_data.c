/*
 * Property-Based Tests for Save Data Round-Trip
 * 
 * Tests Property 27: Save Data Round-Trip
 * 
 * **Validates: Requirements 16.1, 16.2, 16.3, 16.4, 16.5**
 * 
 * Uses Theft library pattern with minimum 100 iterations per property.
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

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

// Region constants (from region_manager.h)
#define REGION_KANTO  0
#define REGION_HOENN  1
#define REGION_JOHTO  2
#define NUM_REGIONS   3
#define BADGES_PER_REGION  8
#define TOTAL_BADGES       (NUM_REGIONS * BADGES_PER_REGION)  // 24 badges

// Fly location constants
#define FLY_LOCATIONS_PER_REGION  16
#define MAX_FLY_LOCATIONS_TOTAL   (FLY_LOCATIONS_PER_REGION * NUM_REGIONS)  // 48

// Validation result codes
#define SAVE_VALID              0
#define SAVE_INVALID_VERSION    1
#define SAVE_INVALID_REGION     2
#define SAVE_INVALID_BADGES     3
#define SAVE_INVALID_FLY_DATA   4
#define SAVE_CORRUPTED          5


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

// ==========================================
// Save Data Structures (simulated)
// ==========================================

// Pokemon Tuxedo save data structure (from global.h)
struct PokemonTuxedoSaveData {
    // Legacy fields
    u8 current_level_cap;
    u8 active_exp_multiplier;
    u8 battle_mode;
    u8 game_speed;
    
    // Multi-region fields (Requirements 16.1-16.5)
    u8 currentRegion;                     // 0=Kanto, 1=Hoenn, 2=Johto
    bool8 hoennUnlocked;
    bool8 johtoUnlocked;
    bool8 eliteFourDefeated[3];
    bool8 badges[24];                     // 8 per region × 3 regions
    u16 flyLocations[48];                 // 16 per region × 3 regions
    u8 flyLocationCount[3];
};

// Badge tracking structure
struct BadgeData {
    bool8 kanto_badges[BADGES_PER_REGION];
    bool8 hoenn_badges[BADGES_PER_REGION];
    bool8 johto_badges[BADGES_PER_REGION];
};

// Region state tracking structure (EWRAM simulation)
struct RegionState {
    u8 current_region;
    bool8 hoenn_unlocked;
    bool8 johto_unlocked;
    bool8 elite_four_defeated[NUM_REGIONS];
    struct BadgeData badges;
};

// Global state for testing
static struct PokemonTuxedoSaveData sSaveData = {0};
static struct RegionState sRegionState = {0};
static u16 sFlyLocations[MAX_FLY_LOCATIONS_TOTAL] = {0};
static u8 sFlyLocationCount[NUM_REGIONS] = {0};


// ==========================================
// State Management Functions
// ==========================================

static void InitRegionState(void) {
    u8 i;
    
    sRegionState.current_region = REGION_KANTO;
    sRegionState.hoenn_unlocked = FALSE;
    sRegionState.johto_unlocked = FALSE;
    
    for (i = 0; i < NUM_REGIONS; i++) {
        sRegionState.elite_four_defeated[i] = FALSE;
    }
    
    for (i = 0; i < BADGES_PER_REGION; i++) {
        sRegionState.badges.kanto_badges[i] = FALSE;
        sRegionState.badges.hoenn_badges[i] = FALSE;
        sRegionState.badges.johto_badges[i] = FALSE;
    }
    
    for (i = 0; i < MAX_FLY_LOCATIONS_TOTAL; i++) {
        sFlyLocations[i] = 0;
    }
    
    for (i = 0; i < NUM_REGIONS; i++) {
        sFlyLocationCount[i] = 0;
    }
}

static void InitSaveData(void) {
    u8 i;
    
    sSaveData.current_level_cap = 100;
    sSaveData.active_exp_multiplier = 1;
    sSaveData.battle_mode = 1;
    sSaveData.game_speed = 1;
    
    sSaveData.currentRegion = REGION_KANTO;
    sSaveData.hoennUnlocked = FALSE;
    sSaveData.johtoUnlocked = FALSE;
    
    for (i = 0; i < NUM_REGIONS; i++) {
        sSaveData.eliteFourDefeated[i] = FALSE;
    }
    
    for (i = 0; i < TOTAL_BADGES; i++) {
        sSaveData.badges[i] = FALSE;
    }
    
    for (i = 0; i < MAX_FLY_LOCATIONS_TOTAL; i++) {
        sSaveData.flyLocations[i] = 0;
    }
    
    for (i = 0; i < NUM_REGIONS; i++) {
        sSaveData.flyLocationCount[i] = 0;
    }
}

// ==========================================
// Save/Load Functions (simulated from multi_region_save.c)
// ==========================================

/**
 * Save multi-region state from EWRAM to save data.
 * Validates: Requirements 16.1, 16.2, 16.3, 16.4, 16.5
 */
static void SaveMultiRegionData(void) {
    u8 i;
    
    // Save current region (Requirement 16.1)
    sSaveData.currentRegion = sRegionState.current_region;
    
    // Save region unlock flags (Requirement 16.2)
    sSaveData.hoennUnlocked = sRegionState.hoenn_unlocked;
    sSaveData.johtoUnlocked = sRegionState.johto_unlocked;
    
    // Save Elite Four defeated flags (Requirement 16.2)
    for (i = 0; i < NUM_REGIONS; i++) {
        sSaveData.eliteFourDefeated[i] = sRegionState.elite_four_defeated[i];
    }
    
    // Save badge data (Requirement 16.3)
    for (i = 0; i < BADGES_PER_REGION; i++) {
        sSaveData.badges[i] = sRegionState.badges.kanto_badges[i];
        sSaveData.badges[BADGES_PER_REGION + i] = sRegionState.badges.hoenn_badges[i];
        sSaveData.badges[2 * BADGES_PER_REGION + i] = sRegionState.badges.johto_badges[i];
    }
    
    // Save fly location data (Requirement 16.4)
    for (i = 0; i < MAX_FLY_LOCATIONS_TOTAL; i++) {
        sSaveData.flyLocations[i] = sFlyLocations[i];
    }
    
    for (i = 0; i < NUM_REGIONS; i++) {
        sSaveData.flyLocationCount[i] = sFlyLocationCount[i];
    }
}


/**
 * Load multi-region state from save data to EWRAM.
 * Validates: Requirements 16.1, 16.2, 16.3, 16.4, 16.5
 */
static void LoadMultiRegionData(void) {
    u8 i;
    
    // Load current region (Requirement 16.1)
    sRegionState.current_region = sSaveData.currentRegion;
    
    // Load region unlock flags (Requirement 16.2)
    sRegionState.hoenn_unlocked = sSaveData.hoennUnlocked;
    sRegionState.johto_unlocked = sSaveData.johtoUnlocked;
    
    // Load Elite Four defeated flags (Requirement 16.2)
    for (i = 0; i < NUM_REGIONS; i++) {
        sRegionState.elite_four_defeated[i] = sSaveData.eliteFourDefeated[i];
    }
    
    // Load badge data (Requirement 16.3)
    for (i = 0; i < BADGES_PER_REGION; i++) {
        sRegionState.badges.kanto_badges[i] = sSaveData.badges[i];
        sRegionState.badges.hoenn_badges[i] = sSaveData.badges[BADGES_PER_REGION + i];
        sRegionState.badges.johto_badges[i] = sSaveData.badges[2 * BADGES_PER_REGION + i];
    }
    
    // Load fly location data (Requirement 16.4)
    for (i = 0; i < MAX_FLY_LOCATIONS_TOTAL; i++) {
        sFlyLocations[i] = sSaveData.flyLocations[i];
    }
    
    for (i = 0; i < NUM_REGIONS; i++) {
        sFlyLocationCount[i] = sSaveData.flyLocationCount[i];
    }
}

/**
 * Validate multi-region save data integrity.
 */
static u8 ValidateMultiRegionSaveData(void) {
    u8 i;
    
    // Check current region is valid
    if (sSaveData.currentRegion >= NUM_REGIONS) {
        return SAVE_INVALID_REGION;
    }
    
    // Validate badge data
    for (i = 0; i < TOTAL_BADGES; i++) {
        if (sSaveData.badges[i] != TRUE && sSaveData.badges[i] != FALSE) {
            return SAVE_INVALID_BADGES;
        }
    }
    
    // Validate Elite Four defeated flags
    for (i = 0; i < NUM_REGIONS; i++) {
        if (sSaveData.eliteFourDefeated[i] != TRUE && sSaveData.eliteFourDefeated[i] != FALSE) {
            return SAVE_CORRUPTED;
        }
    }
    
    // Validate region unlock flags
    if ((sSaveData.hoennUnlocked != TRUE && sSaveData.hoennUnlocked != FALSE) ||
        (sSaveData.johtoUnlocked != TRUE && sSaveData.johtoUnlocked != FALSE)) {
        return SAVE_CORRUPTED;
    }
    
    // Validate fly location counts
    for (i = 0; i < NUM_REGIONS; i++) {
        if (sSaveData.flyLocationCount[i] > FLY_LOCATIONS_PER_REGION) {
            return SAVE_INVALID_FLY_DATA;
        }
    }
    
    return SAVE_VALID;
}


// ==========================================
// Test Data Generation
// ==========================================

struct SaveDataTestInput {
    u8 currentRegion;
    bool8 hoennUnlocked;
    bool8 johtoUnlocked;
    bool8 eliteFourDefeated[3];
    bool8 badges[24];
    u16 flyLocations[48];
    u8 flyLocationCount[3];
};

/**
 * Generate random valid save data for testing.
 */
static void GenerateRandomSaveData(struct SaveDataTestInput *input) {
    u8 i;
    
    // Random current region (0-2)
    input->currentRegion = test_random(NUM_REGIONS);
    
    // Random unlock flags
    input->hoennUnlocked = test_random(2);
    input->johtoUnlocked = test_random(2);
    
    // Random Elite Four defeated flags
    for (i = 0; i < NUM_REGIONS; i++) {
        input->eliteFourDefeated[i] = test_random(2);
    }
    
    // Random badges
    for (i = 0; i < TOTAL_BADGES; i++) {
        input->badges[i] = test_random(2);
    }
    
    // Random fly location counts (0 to max per region)
    for (i = 0; i < NUM_REGIONS; i++) {
        input->flyLocationCount[i] = test_random(FLY_LOCATIONS_PER_REGION + 1);
    }
    
    // Random fly locations (map IDs)
    for (i = 0; i < MAX_FLY_LOCATIONS_TOTAL; i++) {
        input->flyLocations[i] = test_random(500);  // Random map IDs
    }
}

/**
 * Apply test input to EWRAM state.
 */
static void ApplyTestInputToState(struct SaveDataTestInput *input) {
    u8 i;
    
    sRegionState.current_region = input->currentRegion;
    sRegionState.hoenn_unlocked = input->hoennUnlocked;
    sRegionState.johto_unlocked = input->johtoUnlocked;
    
    for (i = 0; i < NUM_REGIONS; i++) {
        sRegionState.elite_four_defeated[i] = input->eliteFourDefeated[i];
    }
    
    for (i = 0; i < BADGES_PER_REGION; i++) {
        sRegionState.badges.kanto_badges[i] = input->badges[i];
        sRegionState.badges.hoenn_badges[i] = input->badges[BADGES_PER_REGION + i];
        sRegionState.badges.johto_badges[i] = input->badges[2 * BADGES_PER_REGION + i];
    }
    
    for (i = 0; i < MAX_FLY_LOCATIONS_TOTAL; i++) {
        sFlyLocations[i] = input->flyLocations[i];
    }
    
    for (i = 0; i < NUM_REGIONS; i++) {
        sFlyLocationCount[i] = input->flyLocationCount[i];
    }
}

/**
 * Compare EWRAM state against expected input.
 */
static bool CompareStateToInput(struct SaveDataTestInput *input) {
    u8 i;
    
    if (sRegionState.current_region != input->currentRegion) {
        printf("  FAIL: currentRegion mismatch: expected %u, got %u\n",
               input->currentRegion, sRegionState.current_region);
        return false;
    }
    
    if (sRegionState.hoenn_unlocked != input->hoennUnlocked) {
        printf("  FAIL: hoennUnlocked mismatch: expected %d, got %d\n",
               input->hoennUnlocked, sRegionState.hoenn_unlocked);
        return false;
    }
    
    if (sRegionState.johto_unlocked != input->johtoUnlocked) {
        printf("  FAIL: johtoUnlocked mismatch: expected %d, got %d\n",
               input->johtoUnlocked, sRegionState.johto_unlocked);
        return false;
    }
    
    for (i = 0; i < NUM_REGIONS; i++) {
        if (sRegionState.elite_four_defeated[i] != input->eliteFourDefeated[i]) {
            printf("  FAIL: eliteFourDefeated[%u] mismatch: expected %d, got %d\n",
                   i, input->eliteFourDefeated[i], sRegionState.elite_four_defeated[i]);
            return false;
        }
    }
    
    for (i = 0; i < BADGES_PER_REGION; i++) {
        if (sRegionState.badges.kanto_badges[i] != input->badges[i]) {
            printf("  FAIL: kanto_badges[%u] mismatch\n", i);
            return false;
        }
        if (sRegionState.badges.hoenn_badges[i] != input->badges[BADGES_PER_REGION + i]) {
            printf("  FAIL: hoenn_badges[%u] mismatch\n", i);
            return false;
        }
        if (sRegionState.badges.johto_badges[i] != input->badges[2 * BADGES_PER_REGION + i]) {
            printf("  FAIL: johto_badges[%u] mismatch\n", i);
            return false;
        }
    }
    
    for (i = 0; i < MAX_FLY_LOCATIONS_TOTAL; i++) {
        if (sFlyLocations[i] != input->flyLocations[i]) {
            printf("  FAIL: flyLocations[%u] mismatch: expected %u, got %u\n",
                   i, input->flyLocations[i], sFlyLocations[i]);
            return false;
        }
    }
    
    for (i = 0; i < NUM_REGIONS; i++) {
        if (sFlyLocationCount[i] != input->flyLocationCount[i]) {
            printf("  FAIL: flyLocationCount[%u] mismatch: expected %u, got %u\n",
                   i, input->flyLocationCount[i], sFlyLocationCount[i]);
            return false;
        }
    }
    
    return true;
}


// ==========================================
// Property Test Functions
// ==========================================

/**
 * Property 27: Save Data Round-Trip
 * 
 * For any valid multi-region state:
 * 1. Save state to save data
 * 2. Clear EWRAM state
 * 3. Load state from save data
 * 4. State should match original
 * 
 * **Validates: Requirements 16.1, 16.2, 16.3, 16.4, 16.5**
 */
static bool prop_save_data_round_trip(struct SaveDataTestInput *input) {
    // Initialize clean state
    InitRegionState();
    InitSaveData();
    
    // Apply test input to EWRAM state
    ApplyTestInputToState(input);
    
    // Save state to save data
    SaveMultiRegionData();
    
    // Validate save data
    u8 validationResult = ValidateMultiRegionSaveData();
    if (validationResult != SAVE_VALID) {
        printf("  FAIL: Save data validation failed with code %u\n", validationResult);
        return false;
    }
    
    // Clear EWRAM state (simulate game restart)
    InitRegionState();
    
    // Load state from save data
    LoadMultiRegionData();
    
    // Compare loaded state to original input
    return CompareStateToInput(input);
}

// ==========================================
// Test Runners
// ==========================================

/**
 * Test runner for Property 27: Save Data Round-Trip
 */
static int test_property_27_save_data_round_trip(void) {
    int num_trials = 100;  // Minimum 100 iterations per property
    int passed = 0;
    int failed = 0;
    
    printf("\n=== Property-Based Test: Save Data Round-Trip (Property 27) ===\n");
    printf("**Validates: Requirements 16.1, 16.2, 16.3, 16.4, 16.5**\n");
    printf("Running %d random test cases...\n\n", num_trials);
    
    // Run property tests with random inputs
    for (int i = 0; i < num_trials; i++) {
        struct SaveDataTestInput input;
        
        // Generate random test data
        GenerateRandomSaveData(&input);
        
        // Run the property test
        if (prop_save_data_round_trip(&input)) {
            passed++;
        } else {
            failed++;
            if (failed <= 5) {
                printf("  Counterexample #%d: region=%u, hoenn=%d, johto=%d\n",
                       failed, input.currentRegion, input.hoennUnlocked, input.johtoUnlocked);
            }
        }
    }
    
    // Test specific edge cases
    printf("\nTesting specific edge cases...\n");
    
    // Edge case 1: All zeros (new game state)
    printf("\nTesting new game state (all zeros)...\n");
    {
        struct SaveDataTestInput input = {0};
        
        if (prop_save_data_round_trip(&input)) {
            printf("  New game state: PASSED\n");
            passed++;
        } else {
            printf("  New game state: FAILED\n");
            failed++;
        }
    }
    
    // Edge case 2: All badges earned
    printf("\nTesting all badges earned...\n");
    {
        struct SaveDataTestInput input = {0};
        input.currentRegion = REGION_JOHTO;
        input.hoennUnlocked = TRUE;
        input.johtoUnlocked = TRUE;
        for (int i = 0; i < NUM_REGIONS; i++) {
            input.eliteFourDefeated[i] = TRUE;
        }
        for (int i = 0; i < TOTAL_BADGES; i++) {
            input.badges[i] = TRUE;
        }
        
        if (prop_save_data_round_trip(&input)) {
            printf("  All badges earned: PASSED\n");
            passed++;
        } else {
            printf("  All badges earned: FAILED\n");
            failed++;
        }
    }
    
    // Edge case 3: Max fly locations
    printf("\nTesting max fly locations...\n");
    {
        struct SaveDataTestInput input = {0};
        input.currentRegion = REGION_HOENN;
        input.hoennUnlocked = TRUE;
        
        for (int i = 0; i < NUM_REGIONS; i++) {
            input.flyLocationCount[i] = FLY_LOCATIONS_PER_REGION;
        }
        for (int i = 0; i < MAX_FLY_LOCATIONS_TOTAL; i++) {
            input.flyLocations[i] = 100 + i;  // Unique map IDs
        }
        
        if (prop_save_data_round_trip(&input)) {
            printf("  Max fly locations: PASSED\n");
            passed++;
        } else {
            printf("  Max fly locations: FAILED\n");
            failed++;
        }
    }
    
    // Edge case 4: Partial progress (mid-game state)
    printf("\nTesting mid-game state...\n");
    {
        struct SaveDataTestInput input = {0};
        input.currentRegion = REGION_HOENN;
        input.hoennUnlocked = TRUE;
        input.johtoUnlocked = FALSE;
        input.eliteFourDefeated[REGION_KANTO] = TRUE;
        input.eliteFourDefeated[REGION_HOENN] = FALSE;
        input.eliteFourDefeated[REGION_JOHTO] = FALSE;
        
        // 8 Kanto badges, 4 Hoenn badges
        for (int i = 0; i < 8; i++) {
            input.badges[i] = TRUE;
        }
        for (int i = 8; i < 12; i++) {
            input.badges[i] = TRUE;
        }
        
        // Some fly locations
        input.flyLocationCount[REGION_KANTO] = 10;
        input.flyLocationCount[REGION_HOENN] = 5;
        for (int i = 0; i < 15; i++) {
            input.flyLocations[i] = 50 + i;
        }
        
        if (prop_save_data_round_trip(&input)) {
            printf("  Mid-game state: PASSED\n");
            passed++;
        } else {
            printf("  Mid-game state: FAILED\n");
            failed++;
        }
    }
    
    // Edge case 5: Each region as current
    printf("\nTesting each region as current...\n");
    for (int r = 0; r < NUM_REGIONS; r++) {
        struct SaveDataTestInput input = {0};
        input.currentRegion = r;
        if (r >= REGION_HOENN) input.hoennUnlocked = TRUE;
        if (r >= REGION_JOHTO) input.johtoUnlocked = TRUE;
        
        if (prop_save_data_round_trip(&input)) {
            printf("  Region %d as current: PASSED\n", r);
            passed++;
        } else {
            printf("  Region %d as current: FAILED\n", r);
            failed++;
        }
    }
    
    // Report results
    printf("\n=== Property 27 Test Results ===\n");
    printf("Total tests: %d\n", passed + failed);
    printf("Passed: %d\n", passed);
    printf("Failed: %d\n", failed);
    
    if (failed == 0) {
        printf("\n✓ Property 27 (Save Data Round-Trip) PASSED!\n");
        return 0;
    } else {
        printf("\n✗ Property 27 (Save Data Round-Trip) FAILED with %d counterexamples\n", failed);
        return 1;
    }
}


/**
 * Test runner for save data validation
 */
static int test_save_data_validation(void) {
    int passed = 0;
    int failed = 0;
    
    printf("\n=== Save Data Validation Tests ===\n");
    
    // Test 1: Valid save data
    printf("\nTesting valid save data...\n");
    {
        InitSaveData();
        sSaveData.currentRegion = REGION_HOENN;
        sSaveData.hoennUnlocked = TRUE;
        
        if (ValidateMultiRegionSaveData() == SAVE_VALID) {
            printf("  Valid save data: PASSED\n");
            passed++;
        } else {
            printf("  Valid save data: FAILED\n");
            failed++;
        }
    }
    
    // Test 2: Invalid region
    printf("\nTesting invalid region...\n");
    {
        InitSaveData();
        sSaveData.currentRegion = NUM_REGIONS;  // Invalid
        
        if (ValidateMultiRegionSaveData() == SAVE_INVALID_REGION) {
            printf("  Invalid region detected: PASSED\n");
            passed++;
        } else {
            printf("  Invalid region detected: FAILED\n");
            failed++;
        }
    }
    
    // Test 3: Invalid badge value
    printf("\nTesting invalid badge value...\n");
    {
        InitSaveData();
        sSaveData.badges[5] = 5;  // Invalid (should be 0 or 1)
        
        if (ValidateMultiRegionSaveData() == SAVE_INVALID_BADGES) {
            printf("  Invalid badge detected: PASSED\n");
            passed++;
        } else {
            printf("  Invalid badge detected: FAILED\n");
            failed++;
        }
    }
    
    // Test 4: Invalid fly location count
    printf("\nTesting invalid fly location count...\n");
    {
        InitSaveData();
        sSaveData.flyLocationCount[0] = FLY_LOCATIONS_PER_REGION + 1;  // Too many
        
        if (ValidateMultiRegionSaveData() == SAVE_INVALID_FLY_DATA) {
            printf("  Invalid fly count detected: PASSED\n");
            passed++;
        } else {
            printf("  Invalid fly count detected: FAILED\n");
            failed++;
        }
    }
    
    // Test 5: Invalid unlock flag
    printf("\nTesting invalid unlock flag...\n");
    {
        InitSaveData();
        sSaveData.hoennUnlocked = 5;  // Invalid (should be 0 or 1)
        
        if (ValidateMultiRegionSaveData() == SAVE_CORRUPTED) {
            printf("  Invalid unlock flag detected: PASSED\n");
            passed++;
        } else {
            printf("  Invalid unlock flag detected: FAILED\n");
            failed++;
        }
    }
    
    // Report results
    printf("\n=== Validation Test Results ===\n");
    printf("Total tests: %d\n", passed + failed);
    printf("Passed: %d\n", passed);
    printf("Failed: %d\n", failed);
    
    return (failed == 0) ? 0 : 1;
}

// ==========================================
// Main Test Runner
// ==========================================

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    // Initialize random seed
    init_test_random((unsigned int)time(NULL));
    
    printf("╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║     Property-Based Tests for Save Data Round-Trip                ║\n");
    printf("║     Task 3.15: Write property test for save data round-trip      ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n");
    printf("\nRegion configuration: Kanto → Hoenn → Johto (3 regions)\n");
    printf("Sinnoh descoped per ADR-003\n");
    printf("Progression order per ADR-004\n");
    printf("Minimum iterations per property: 100\n");
    
    int property27_result = test_property_27_save_data_round_trip();
    int validation_result = test_save_data_validation();
    
    // Overall results
    printf("\n\n╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║                    OVERALL TEST RESULTS                          ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n");
    
    if (property27_result == 0 && validation_result == 0) {
        printf("\n✓ ALL PROPERTY TESTS PASSED!\n\n");
        printf("Property 27 (Save Data Round-Trip): PASSED\n");
        printf("  - Validates: Requirements 16.1, 16.2, 16.3, 16.4, 16.5\n");
        printf("Save Data Validation: PASSED\n");
        return 0;
    } else {
        printf("\n✗ SOME PROPERTY TESTS FAILED\n\n");
        if (property27_result != 0) {
            printf("Property 27 (Save Data Round-Trip): FAILED\n");
        } else {
            printf("Property 27 (Save Data Round-Trip): PASSED\n");
        }
        if (validation_result != 0) {
            printf("Save Data Validation: FAILED\n");
        } else {
            printf("Save Data Validation: PASSED\n");
        }
        return 1;
    }
}
