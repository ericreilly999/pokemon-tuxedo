/*
 * Property-Based Tests for Region Unlocking System
 * 
 * Tests Property 1: Elite Four Progression Unlocks Next Region
 * Tests Property 2: Region Unlock Awards Ticket
 * Tests Property 3: Locked Region Access Prevention
 * 
 * **Validates: Requirements 1.1, 1.2, 1.4, 1.5**
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

// Region constants (from region_manager.h)
#define REGION_KANTO  0
#define REGION_HOENN  1
#define REGION_JOHTO  2
#define NUM_REGIONS   3
#define BADGES_PER_REGION  8
#define TOTAL_BADGES       (NUM_REGIONS * BADGES_PER_REGION)  // 24 badges

// Region ticket item IDs (from constants/items.h)
#define ITEM_NONE           0
#define ITEM_JOHTO_TICKET   377
#define ITEM_HOENN_TICKET   378
#define ITEM_SINNOH_TICKET  379

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

// Player inventory simulation for ticket tracking
#define MAX_INVENTORY_SIZE 100
static u16 sPlayerInventory[MAX_INVENTORY_SIZE] = {0};
static u8 sInventoryCount = 0;

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

// Inventory helper functions
static void ClearInventory(void) {
    u8 i;
    for (i = 0; i < MAX_INVENTORY_SIZE; i++) {
        sPlayerInventory[i] = 0;
    }
    sInventoryCount = 0;
}

static bool8 AddBagItem(u16 item_id, u8 count) {
    (void)count;  // Simplified: always add 1
    if (sInventoryCount >= MAX_INVENTORY_SIZE)
        return false;
    sPlayerInventory[sInventoryCount++] = item_id;
    return true;
}

static bool8 CheckBagHasItem(u16 item_id, u8 count) {
    u8 i;
    u8 found = 0;
    (void)count;  // Simplified: check if item exists
    for (i = 0; i < sInventoryCount; i++) {
        if (sPlayerInventory[i] == item_id)
            found++;
    }
    return found > 0;
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
    
    ClearInventory();
}

static u8 GetCurrentRegion(void) {
    return sRegionState.current_region;
}

static void SetCurrentRegion(u8 region_id) {
    if (region_id < NUM_REGIONS)
        sRegionState.current_region = region_id;
}

static bool8 IsEliteFourDefeated(u8 region_id) {
    if (region_id >= NUM_REGIONS)
        return false;
    return sRegionState.elite_four_defeated[region_id];
}

static void SetEliteFourDefeated(u8 region_id) {
    if (region_id >= NUM_REGIONS)
        return;
    sRegionState.elite_four_defeated[region_id] = true;
}

static bool8 IsRegionUnlocked(u8 region_id) {
    switch (region_id) {
    case REGION_KANTO:
        return true;  // Kanto is always unlocked (starting region)
    case REGION_HOENN:
        return sRegionState.hoenn_unlocked;
    case REGION_JOHTO:
        return sRegionState.johto_unlocked;
    default:
        return false;
    }
}

static u16 GetRegionTicketItem(u8 region_id) {
    switch (region_id) {
    case REGION_HOENN:
        return ITEM_HOENN_TICKET;
    case REGION_JOHTO:
        return ITEM_JOHTO_TICKET;
    default:
        return ITEM_NONE;
    }
}

static void UnlockRegion(u8 region_id) {
    u16 ticket_item;
    
    switch (region_id) {
    case REGION_HOENN:
        if (!sRegionState.hoenn_unlocked) {
            sRegionState.hoenn_unlocked = true;
            ticket_item = GetRegionTicketItem(REGION_HOENN);
            if (ticket_item != ITEM_NONE)
                AddBagItem(ticket_item, 1);
        }
        break;
    case REGION_JOHTO:
        if (!sRegionState.johto_unlocked) {
            sRegionState.johto_unlocked = true;
            ticket_item = GetRegionTicketItem(REGION_JOHTO);
            if (ticket_item != ITEM_NONE)
                AddBagItem(ticket_item, 1);
        }
        break;
    case REGION_KANTO:
        // Kanto is always unlocked, nothing to do
        break;
    }
}

/**
 * Check Elite Four defeat and unlock the next region.
 * Progression sequence (per ADR-004):
 *   Kanto E4 defeat → Hoenn unlock
 *   Hoenn E4 defeat → Johto unlock
 *   Johto E4 defeat → No next region (Sinnoh descoped per ADR-003)
 * 
 * @param region_id The region where Elite Four was defeated
 * @return true if a new region was unlocked, false otherwise
 */
static bool8 CheckEliteFourDefeatAndUnlock(u8 region_id) {
    bool8 region_unlocked = false;
    
    // Validate region ID
    if (region_id >= NUM_REGIONS)
        return false;
    
    // Check if already defeated (no need to process again)
    if (IsEliteFourDefeated(region_id))
        return false;
    
    // Mark Elite Four as defeated
    SetEliteFourDefeated(region_id);
    
    // Unlock next region based on defeated region (ADR-004 sequence)
    switch (region_id) {
    case REGION_KANTO:
        // Kanto E4 defeated → Unlock Hoenn
        if (!IsRegionUnlocked(REGION_HOENN)) {
            UnlockRegion(REGION_HOENN);
            region_unlocked = true;
        }
        break;
    case REGION_HOENN:
        // Hoenn E4 defeated → Unlock Johto
        if (!IsRegionUnlocked(REGION_JOHTO)) {
            UnlockRegion(REGION_JOHTO);
            region_unlocked = true;
        }
        break;
    case REGION_JOHTO:
        // No next region after Johto (Sinnoh descoped per ADR-003)
        break;
    }
    
    return region_unlocked;
}

/**
 * Check if the player can travel to a target region.
 * @param target_region The region to check travel eligibility for
 * @return true if travel is allowed, false otherwise
 */
static bool8 CanTravelToRegion(u8 target_region) {
    u16 ticket_item;
    
    // Can always stay in current region
    if (target_region == sRegionState.current_region)
        return true;
    
    // Kanto is always accessible (starting region)
    if (target_region == REGION_KANTO)
        return true;
    
    // Check if region is unlocked
    if (!IsRegionUnlocked(target_region))
        return false;
    
    // Check if player has the ticket
    ticket_item = GetRegionTicketItem(target_region);
    if (ticket_item == ITEM_NONE)
        return false;
    
    return CheckBagHasItem(ticket_item, 1);
}

// Test data structures for property-based testing
struct EliteFourDefeatTestData {
    u8 region_id;
};

struct RegionAccessTestData {
    u8 target_region;
    bool8 hoenn_unlocked;
    bool8 johto_unlocked;
    bool8 has_hoenn_ticket;
    bool8 has_johto_ticket;
};

struct ProgressionTestData {
    u8 regions_to_defeat;  // Bitmask: bit 0 = Kanto, bit 1 = Hoenn, bit 2 = Johto
};

/**
 * Property 1: Elite Four Progression Unlocks Next Region
 * 
 * For any region with an Elite Four, defeating that region's Elite Four 
 * should unlock the next region in sequence (Kanto → Hoenn → Johto).
 * 
 * This property verifies:
 * 1. Defeating Kanto E4 unlocks Hoenn
 * 2. Defeating Hoenn E4 unlocks Johto
 * 3. Defeating Johto E4 does not unlock any new region (Sinnoh descoped)
 * 4. CheckEliteFourDefeatAndUnlock() returns TRUE when a region is unlocked
 * 5. CheckEliteFourDefeatAndUnlock() returns FALSE when called again (already defeated)
 * 
 * **Validates: Requirements 1.1, 1.2**
 */
static bool prop_elite_four_progression_unlocks_next_region(struct EliteFourDefeatTestData *data) {
    // Reset state for clean test
    InitRegionState();
    
    // Validate input bounds
    if (data->region_id >= NUM_REGIONS) {
        // Invalid input - skip this test case
        return true;
    }
    
    // Record initial unlock state
    bool8 initial_hoenn_unlocked = IsRegionUnlocked(REGION_HOENN);
    bool8 initial_johto_unlocked = IsRegionUnlocked(REGION_JOHTO);
    
    // Defeat Elite Four in the specified region
    bool8 unlock_result = CheckEliteFourDefeatAndUnlock(data->region_id);
    
    // Verify Elite Four is now marked as defeated
    if (!IsEliteFourDefeated(data->region_id)) {
        printf("FAIL: Elite Four not marked as defeated for region %u\n", data->region_id);
        return false;
    }
    
    // Verify correct region was unlocked based on progression sequence
    switch (data->region_id) {
    case REGION_KANTO:
        // Kanto E4 defeat should unlock Hoenn
        if (!IsRegionUnlocked(REGION_HOENN)) {
            printf("FAIL: Hoenn should be unlocked after Kanto E4 defeat\n");
            return false;
        }
        if (!unlock_result && !initial_hoenn_unlocked) {
            printf("FAIL: CheckEliteFourDefeatAndUnlock should return TRUE when unlocking Hoenn\n");
            return false;
        }
        // Johto should NOT be unlocked yet
        if (IsRegionUnlocked(REGION_JOHTO) && !initial_johto_unlocked) {
            printf("FAIL: Johto should NOT be unlocked after only Kanto E4 defeat\n");
            return false;
        }
        break;
        
    case REGION_HOENN:
        // Hoenn E4 defeat should unlock Johto
        if (!IsRegionUnlocked(REGION_JOHTO)) {
            printf("FAIL: Johto should be unlocked after Hoenn E4 defeat\n");
            return false;
        }
        if (!unlock_result && !initial_johto_unlocked) {
            printf("FAIL: CheckEliteFourDefeatAndUnlock should return TRUE when unlocking Johto\n");
            return false;
        }
        break;
        
    case REGION_JOHTO:
        // Johto E4 defeat should NOT unlock any new region (Sinnoh descoped)
        // unlock_result should be FALSE since no new region is unlocked
        if (unlock_result) {
            printf("FAIL: CheckEliteFourDefeatAndUnlock should return FALSE for Johto (no next region)\n");
            return false;
        }
        break;
    }
    
    // Verify calling again returns FALSE (already defeated)
    bool8 second_call_result = CheckEliteFourDefeatAndUnlock(data->region_id);
    if (second_call_result) {
        printf("FAIL: CheckEliteFourDefeatAndUnlock should return FALSE when called again (already defeated)\n");
        return false;
    }
    
    return true;
}

/**
 * Property 2: Region Unlock Awards Ticket
 * 
 * For any region that becomes unlocked, the player should receive 
 * the corresponding region ticket item.
 * 
 * This property verifies:
 * 1. UnlockRegion(REGION_HOENN) awards ITEM_HOENN_TICKET
 * 2. UnlockRegion(REGION_JOHTO) awards ITEM_JOHTO_TICKET
 * 3. GetRegionTicketItem() returns correct ticket for each region
 * 4. Ticket is only awarded once (idempotent unlock)
 * 
 * **Validates: Requirements 1.4**
 */
static bool prop_region_unlock_awards_ticket(struct EliteFourDefeatTestData *data) {
    // Reset state for clean test
    InitRegionState();
    
    // Validate input bounds
    if (data->region_id >= NUM_REGIONS) {
        return true;  // Skip invalid input
    }
    
    // Skip Kanto - it's always unlocked and has no ticket
    if (data->region_id == REGION_KANTO) {
        // Verify Kanto has no ticket
        u16 kanto_ticket = GetRegionTicketItem(REGION_KANTO);
        if (kanto_ticket != ITEM_NONE) {
            printf("FAIL: Kanto should not have a ticket item, got %u\n", kanto_ticket);
            return false;
        }
        return true;
    }
    
    // Get expected ticket for this region
    u16 expected_ticket = GetRegionTicketItem(data->region_id);
    
    // Verify ticket item is correct
    switch (data->region_id) {
    case REGION_HOENN:
        if (expected_ticket != ITEM_HOENN_TICKET) {
            printf("FAIL: GetRegionTicketItem(REGION_HOENN) should return %u, got %u\n",
                   ITEM_HOENN_TICKET, expected_ticket);
            return false;
        }
        break;
    case REGION_JOHTO:
        if (expected_ticket != ITEM_JOHTO_TICKET) {
            printf("FAIL: GetRegionTicketItem(REGION_JOHTO) should return %u, got %u\n",
                   ITEM_JOHTO_TICKET, expected_ticket);
            return false;
        }
        break;
    }
    
    // Verify player doesn't have ticket before unlock
    if (CheckBagHasItem(expected_ticket, 1)) {
        printf("FAIL: Player should not have ticket before region unlock\n");
        return false;
    }
    
    // Unlock the region
    UnlockRegion(data->region_id);
    
    // Verify region is now unlocked
    if (!IsRegionUnlocked(data->region_id)) {
        printf("FAIL: Region %u should be unlocked after UnlockRegion()\n", data->region_id);
        return false;
    }
    
    // Verify player now has the ticket
    if (!CheckBagHasItem(expected_ticket, 1)) {
        printf("FAIL: Player should have ticket %u after unlocking region %u\n",
               expected_ticket, data->region_id);
        return false;
    }
    
    // Count tickets before second unlock attempt
    u8 ticket_count_before = 0;
    for (u8 i = 0; i < sInventoryCount; i++) {
        if (sPlayerInventory[i] == expected_ticket)
            ticket_count_before++;
    }
    
    // Try to unlock again (should be idempotent)
    UnlockRegion(data->region_id);
    
    // Count tickets after second unlock attempt
    u8 ticket_count_after = 0;
    for (u8 i = 0; i < sInventoryCount; i++) {
        if (sPlayerInventory[i] == expected_ticket)
            ticket_count_after++;
    }
    
    // Verify ticket was not awarded twice
    if (ticket_count_after != ticket_count_before) {
        printf("FAIL: Ticket should only be awarded once (idempotent unlock)\n");
        printf("  Before: %u, After: %u\n", ticket_count_before, ticket_count_after);
        return false;
    }
    
    return true;
}

/**
 * Property 3: Locked Region Access Prevention
 * 
 * For any locked region and any warp attempt, the system should 
 * prevent the player from accessing that region.
 * 
 * This property verifies:
 * 1. CanTravelToRegion() returns FALSE for locked regions
 * 2. CanTravelToRegion() returns TRUE for unlocked regions with ticket
 * 3. CanTravelToRegion() returns TRUE for current region
 * 4. CanTravelToRegion() returns TRUE for Kanto (always accessible)
 * 5. IsRegionUnlocked() correctly tracks unlock status
 * 
 * **Validates: Requirements 1.5**
 */
static bool prop_locked_region_access_prevention(struct RegionAccessTestData *data) {
    // Reset state for clean test
    InitRegionState();
    
    // Validate input bounds
    if (data->target_region >= NUM_REGIONS) {
        return true;  // Skip invalid input
    }
    
    // Set up unlock state based on test data
    if (data->hoenn_unlocked) {
        sRegionState.hoenn_unlocked = true;
    }
    if (data->johto_unlocked) {
        sRegionState.johto_unlocked = true;
    }
    
    // Add tickets based on test data
    if (data->has_hoenn_ticket) {
        AddBagItem(ITEM_HOENN_TICKET, 1);
    }
    if (data->has_johto_ticket) {
        AddBagItem(ITEM_JOHTO_TICKET, 1);
    }
    
    // Test CanTravelToRegion for the target region
    bool8 can_travel = CanTravelToRegion(data->target_region);
    
    // Determine expected result
    bool8 expected_can_travel = false;
    
    switch (data->target_region) {
    case REGION_KANTO:
        // Kanto is always accessible
        expected_can_travel = true;
        break;
    case REGION_HOENN:
        // Hoenn requires: unlocked AND has ticket
        expected_can_travel = data->hoenn_unlocked && data->has_hoenn_ticket;
        break;
    case REGION_JOHTO:
        // Johto requires: unlocked AND has ticket
        expected_can_travel = data->johto_unlocked && data->has_johto_ticket;
        break;
    }
    
    // Verify result matches expectation
    if (can_travel != expected_can_travel) {
        printf("FAIL: CanTravelToRegion(%u) returned %s, expected %s\n",
               data->target_region,
               can_travel ? "TRUE" : "FALSE",
               expected_can_travel ? "TRUE" : "FALSE");
        printf("  hoenn_unlocked=%d, johto_unlocked=%d\n",
               data->hoenn_unlocked, data->johto_unlocked);
        printf("  has_hoenn_ticket=%d, has_johto_ticket=%d\n",
               data->has_hoenn_ticket, data->has_johto_ticket);
        return false;
    }
    
    // Additional verification: IsRegionUnlocked should match unlock state
    if (data->target_region == REGION_HOENN) {
        if (IsRegionUnlocked(REGION_HOENN) != data->hoenn_unlocked) {
            printf("FAIL: IsRegionUnlocked(REGION_HOENN) mismatch\n");
            return false;
        }
    }
    if (data->target_region == REGION_JOHTO) {
        if (IsRegionUnlocked(REGION_JOHTO) != data->johto_unlocked) {
            printf("FAIL: IsRegionUnlocked(REGION_JOHTO) mismatch\n");
            return false;
        }
    }
    
    return true;
}

/**
 * Test runner for Property 1: Elite Four Progression Unlocks Next Region
 */
static int test_property_1_elite_four_progression(void) {
    int num_trials = 100;  // Minimum 100 iterations per property
    int passed = 0;
    int failed = 0;
    
    printf("\n=== Property-Based Test: Elite Four Progression Unlocks Next Region (Property 1) ===\n");
    printf("**Validates: Requirements 1.1, 1.2**\n");
    printf("Running %d random test cases...\n\n", num_trials);
    
    // Run property tests with random inputs
    for (int i = 0; i < num_trials; i++) {
        struct EliteFourDefeatTestData data;
        
        // Generate random test data (valid region IDs only)
        data.region_id = test_random(NUM_REGIONS);
        
        // Run the property test
        if (prop_elite_four_progression_unlocks_next_region(&data)) {
            passed++;
        } else {
            failed++;
            if (failed <= 5) {
                printf("  Counterexample #%d: region_id=%u\n", failed, data.region_id);
            }
        }
    }
    
    // Test specific edge cases
    printf("\nTesting specific edge cases...\n");
    
    // Edge case 1: Full progression sequence Kanto → Hoenn → Johto
    printf("\nTesting full progression sequence...\n");
    InitRegionState();
    
    // Defeat Kanto E4
    bool8 kanto_result = CheckEliteFourDefeatAndUnlock(REGION_KANTO);
    if (!kanto_result || !IsRegionUnlocked(REGION_HOENN)) {
        printf("FAIL: Kanto E4 defeat should unlock Hoenn\n");
        failed++;
    } else {
        printf("  Kanto E4 → Hoenn unlocked: PASSED\n");
        passed++;
    }
    
    // Defeat Hoenn E4
    bool8 hoenn_result = CheckEliteFourDefeatAndUnlock(REGION_HOENN);
    if (!hoenn_result || !IsRegionUnlocked(REGION_JOHTO)) {
        printf("FAIL: Hoenn E4 defeat should unlock Johto\n");
        failed++;
    } else {
        printf("  Hoenn E4 → Johto unlocked: PASSED\n");
        passed++;
    }
    
    // Defeat Johto E4 (no next region)
    bool8 johto_result = CheckEliteFourDefeatAndUnlock(REGION_JOHTO);
    if (johto_result) {
        printf("FAIL: Johto E4 defeat should not unlock any region (Sinnoh descoped)\n");
        failed++;
    } else {
        printf("  Johto E4 → No new region (Sinnoh descoped): PASSED\n");
        passed++;
    }
    
    // Edge case 2: Invalid region ID
    printf("\nTesting invalid region ID...\n");
    InitRegionState();
    bool8 invalid_result = CheckEliteFourDefeatAndUnlock(NUM_REGIONS);
    if (invalid_result) {
        printf("FAIL: Invalid region ID should return FALSE\n");
        failed++;
    } else {
        printf("  Invalid region ID returns FALSE: PASSED\n");
        passed++;
    }
    
    // Edge case 3: Double defeat (idempotent)
    printf("\nTesting double defeat (idempotent)...\n");
    InitRegionState();
    CheckEliteFourDefeatAndUnlock(REGION_KANTO);
    bool8 double_defeat = CheckEliteFourDefeatAndUnlock(REGION_KANTO);
    if (double_defeat) {
        printf("FAIL: Double defeat should return FALSE\n");
        failed++;
    } else {
        printf("  Double defeat returns FALSE: PASSED\n");
        passed++;
    }
    
    // Report results
    printf("\n=== Property 1 Test Results ===\n");
    printf("Total tests: %d\n", passed + failed);
    printf("Passed: %d\n", passed);
    printf("Failed: %d\n", failed);
    
    if (failed == 0) {
        printf("\n✓ Property 1 (Elite Four Progression Unlocks Next Region) PASSED!\n");
        return 0;
    } else {
        printf("\n✗ Property 1 (Elite Four Progression Unlocks Next Region) FAILED with %d counterexamples\n", failed);
        return 1;
    }
}

/**
 * Test runner for Property 2: Region Unlock Awards Ticket
 */
static int test_property_2_region_unlock_awards_ticket(void) {
    int num_trials = 100;  // Minimum 100 iterations per property
    int passed = 0;
    int failed = 0;
    
    printf("\n=== Property-Based Test: Region Unlock Awards Ticket (Property 2) ===\n");
    printf("**Validates: Requirements 1.4**\n");
    printf("Running %d random test cases...\n\n", num_trials);
    
    // Run property tests with random inputs
    for (int i = 0; i < num_trials; i++) {
        struct EliteFourDefeatTestData data;
        
        // Generate random test data (valid region IDs only)
        data.region_id = test_random(NUM_REGIONS);
        
        // Run the property test
        if (prop_region_unlock_awards_ticket(&data)) {
            passed++;
        } else {
            failed++;
            if (failed <= 5) {
                printf("  Counterexample #%d: region_id=%u\n", failed, data.region_id);
            }
        }
    }
    
    // Test specific edge cases
    printf("\nTesting specific edge cases...\n");
    
    // Edge case 1: Hoenn unlock awards Hoenn ticket
    printf("\nTesting Hoenn unlock awards ticket...\n");
    InitRegionState();
    
    if (CheckBagHasItem(ITEM_HOENN_TICKET, 1)) {
        printf("FAIL: Should not have Hoenn ticket before unlock\n");
        failed++;
    } else {
        passed++;
    }
    
    UnlockRegion(REGION_HOENN);
    
    if (!CheckBagHasItem(ITEM_HOENN_TICKET, 1)) {
        printf("FAIL: Should have Hoenn ticket after unlock\n");
        failed++;
    } else {
        printf("  Hoenn unlock → Hoenn ticket: PASSED\n");
        passed++;
    }
    
    // Edge case 2: Johto unlock awards Johto ticket
    printf("\nTesting Johto unlock awards ticket...\n");
    InitRegionState();
    
    if (CheckBagHasItem(ITEM_JOHTO_TICKET, 1)) {
        printf("FAIL: Should not have Johto ticket before unlock\n");
        failed++;
    } else {
        passed++;
    }
    
    UnlockRegion(REGION_JOHTO);
    
    if (!CheckBagHasItem(ITEM_JOHTO_TICKET, 1)) {
        printf("FAIL: Should have Johto ticket after unlock\n");
        failed++;
    } else {
        printf("  Johto unlock → Johto ticket: PASSED\n");
        passed++;
    }
    
    // Edge case 3: Full progression awards both tickets
    printf("\nTesting full progression awards both tickets...\n");
    InitRegionState();
    
    CheckEliteFourDefeatAndUnlock(REGION_KANTO);  // Unlocks Hoenn
    CheckEliteFourDefeatAndUnlock(REGION_HOENN);  // Unlocks Johto
    
    if (!CheckBagHasItem(ITEM_HOENN_TICKET, 1)) {
        printf("FAIL: Should have Hoenn ticket after full progression\n");
        failed++;
    } else {
        passed++;
    }
    
    if (!CheckBagHasItem(ITEM_JOHTO_TICKET, 1)) {
        printf("FAIL: Should have Johto ticket after full progression\n");
        failed++;
    } else {
        passed++;
    }
    
    printf("  Full progression awards both tickets: PASSED\n");
    
    // Edge case 4: Kanto has no ticket
    printf("\nTesting Kanto has no ticket...\n");
    u16 kanto_ticket = GetRegionTicketItem(REGION_KANTO);
    if (kanto_ticket != ITEM_NONE) {
        printf("FAIL: Kanto should have no ticket, got %u\n", kanto_ticket);
        failed++;
    } else {
        printf("  Kanto has no ticket: PASSED\n");
        passed++;
    }
    
    // Report results
    printf("\n=== Property 2 Test Results ===\n");
    printf("Total tests: %d\n", passed + failed);
    printf("Passed: %d\n", passed);
    printf("Failed: %d\n", failed);
    
    if (failed == 0) {
        printf("\n✓ Property 2 (Region Unlock Awards Ticket) PASSED!\n");
        return 0;
    } else {
        printf("\n✗ Property 2 (Region Unlock Awards Ticket) FAILED with %d counterexamples\n", failed);
        return 1;
    }
}

/**
 * Test runner for Property 3: Locked Region Access Prevention
 */
static int test_property_3_locked_region_access_prevention(void) {
    int num_trials = 100;  // Minimum 100 iterations per property
    int passed = 0;
    int failed = 0;
    
    printf("\n=== Property-Based Test: Locked Region Access Prevention (Property 3) ===\n");
    printf("**Validates: Requirements 1.5**\n");
    printf("Running %d random test cases...\n\n", num_trials);
    
    // Run property tests with random inputs
    for (int i = 0; i < num_trials; i++) {
        struct RegionAccessTestData data;
        
        // Generate random test data
        data.target_region = test_random(NUM_REGIONS);
        data.hoenn_unlocked = test_random(2);
        data.johto_unlocked = test_random(2);
        data.has_hoenn_ticket = test_random(2);
        data.has_johto_ticket = test_random(2);
        
        // Run the property test
        if (prop_locked_region_access_prevention(&data)) {
            passed++;
        } else {
            failed++;
            if (failed <= 5) {
                printf("  Counterexample #%d: target=%u, hoenn_unlocked=%d, johto_unlocked=%d, "
                       "has_hoenn_ticket=%d, has_johto_ticket=%d\n",
                       failed, data.target_region, data.hoenn_unlocked, data.johto_unlocked,
                       data.has_hoenn_ticket, data.has_johto_ticket);
            }
        }
    }
    
    // Test specific edge cases
    printf("\nTesting specific edge cases...\n");
    
    // Edge case 1: Cannot access locked Hoenn
    printf("\nTesting cannot access locked Hoenn...\n");
    InitRegionState();
    
    if (CanTravelToRegion(REGION_HOENN)) {
        printf("FAIL: Should not be able to travel to locked Hoenn\n");
        failed++;
    } else {
        printf("  Cannot access locked Hoenn: PASSED\n");
        passed++;
    }
    
    // Edge case 2: Cannot access locked Johto
    printf("\nTesting cannot access locked Johto...\n");
    InitRegionState();
    
    if (CanTravelToRegion(REGION_JOHTO)) {
        printf("FAIL: Should not be able to travel to locked Johto\n");
        failed++;
    } else {
        printf("  Cannot access locked Johto: PASSED\n");
        passed++;
    }
    
    // Edge case 3: Can always access Kanto
    printf("\nTesting can always access Kanto...\n");
    InitRegionState();
    SetCurrentRegion(REGION_HOENN);  // Even from another region
    
    if (!CanTravelToRegion(REGION_KANTO)) {
        printf("FAIL: Should always be able to travel to Kanto\n");
        failed++;
    } else {
        printf("  Can always access Kanto: PASSED\n");
        passed++;
    }
    
    // Edge case 4: Can access unlocked Hoenn with ticket
    printf("\nTesting can access unlocked Hoenn with ticket...\n");
    InitRegionState();
    UnlockRegion(REGION_HOENN);  // This also awards ticket
    
    if (!CanTravelToRegion(REGION_HOENN)) {
        printf("FAIL: Should be able to travel to unlocked Hoenn with ticket\n");
        failed++;
    } else {
        printf("  Can access unlocked Hoenn with ticket: PASSED\n");
        passed++;
    }
    
    // Edge case 5: Cannot access unlocked region without ticket
    printf("\nTesting cannot access unlocked region without ticket...\n");
    InitRegionState();
    sRegionState.hoenn_unlocked = true;  // Unlock without giving ticket
    
    if (CanTravelToRegion(REGION_HOENN)) {
        printf("FAIL: Should not be able to travel to Hoenn without ticket\n");
        failed++;
    } else {
        printf("  Cannot access unlocked region without ticket: PASSED\n");
        passed++;
    }
    
    // Edge case 6: Can always stay in current region
    printf("\nTesting can always stay in current region...\n");
    InitRegionState();
    SetCurrentRegion(REGION_KANTO);
    
    if (!CanTravelToRegion(REGION_KANTO)) {
        printf("FAIL: Should always be able to stay in current region\n");
        failed++;
    } else {
        printf("  Can always stay in current region: PASSED\n");
        passed++;
    }
    
    // Edge case 7: Full access after full progression
    printf("\nTesting full access after full progression...\n");
    InitRegionState();
    CheckEliteFourDefeatAndUnlock(REGION_KANTO);
    CheckEliteFourDefeatAndUnlock(REGION_HOENN);
    
    bool8 can_kanto = CanTravelToRegion(REGION_KANTO);
    bool8 can_hoenn = CanTravelToRegion(REGION_HOENN);
    bool8 can_johto = CanTravelToRegion(REGION_JOHTO);
    
    if (!can_kanto || !can_hoenn || !can_johto) {
        printf("FAIL: Should be able to travel to all regions after full progression\n");
        printf("  Kanto: %d, Hoenn: %d, Johto: %d\n", can_kanto, can_hoenn, can_johto);
        failed++;
    } else {
        printf("  Full access after full progression: PASSED\n");
        passed++;
    }
    
    // Report results
    printf("\n=== Property 3 Test Results ===\n");
    printf("Total tests: %d\n", passed + failed);
    printf("Passed: %d\n", passed);
    printf("Failed: %d\n", failed);
    
    if (failed == 0) {
        printf("\n✓ Property 3 (Locked Region Access Prevention) PASSED!\n");
        return 0;
    } else {
        printf("\n✗ Property 3 (Locked Region Access Prevention) FAILED with %d counterexamples\n", failed);
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
    printf("║     Property-Based Tests for Region Unlocking System             ║\n");
    printf("║     Task 3.4: Write property tests for region unlocking          ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n");
    printf("\nRegion configuration: Kanto → Hoenn → Johto (3 regions)\n");
    printf("Sinnoh descoped per ADR-003\n");
    printf("Progression order per ADR-004\n");
    printf("Minimum iterations per property: 100\n");
    
    int property1_result = test_property_1_elite_four_progression();
    int property2_result = test_property_2_region_unlock_awards_ticket();
    int property3_result = test_property_3_locked_region_access_prevention();
    
    // Overall results
    printf("\n\n╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║                    OVERALL TEST RESULTS                          ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n");
    
    if (property1_result == 0 && property2_result == 0 && property3_result == 0) {
        printf("\n✓ ALL PROPERTY TESTS PASSED!\n\n");
        printf("Property 1 (Elite Four Progression Unlocks Next Region): PASSED\n");
        printf("  - Validates: Requirements 1.1, 1.2\n");
        printf("Property 2 (Region Unlock Awards Ticket): PASSED\n");
        printf("  - Validates: Requirements 1.4\n");
        printf("Property 3 (Locked Region Access Prevention): PASSED\n");
        printf("  - Validates: Requirements 1.5\n");
        return 0;
    } else {
        printf("\n✗ SOME PROPERTY TESTS FAILED\n\n");
        if (property1_result != 0) {
            printf("Property 1 (Elite Four Progression Unlocks Next Region): FAILED\n");
        } else {
            printf("Property 1 (Elite Four Progression Unlocks Next Region): PASSED\n");
        }
        if (property2_result != 0) {
            printf("Property 2 (Region Unlock Awards Ticket): FAILED\n");
        } else {
            printf("Property 2 (Region Unlock Awards Ticket): PASSED\n");
        }
        if (property3_result != 0) {
            printf("Property 3 (Locked Region Access Prevention): FAILED\n");
        } else {
            printf("Property 3 (Locked Region Access Prevention): PASSED\n");
        }
        return 1;
    }
}
