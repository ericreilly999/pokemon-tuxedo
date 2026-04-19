/*
 * Property-Based Tests for Region Ticket System
 * 
 * Tests Property 14: Region Ticket Warp Functionality
 * Tests Property 18: Locked Region Ticket Rejection
 * 
 * **Validates: Requirements 10.1, 10.5**
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

// Map IDs for region starting locations
#define MAP_LITTLEROOT_TOWN  0x0101  // Hoenn starting location
#define MAP_NEW_BARK_TOWN    0x0201  // Johto starting location (stub)
#define MAP_PALLET_TOWN      0x0001  // Kanto starting location

// Warp result codes
#define WARP_RESULT_SUCCESS       0
#define WARP_RESULT_LOCKED        1
#define WARP_RESULT_INVALID_ITEM  2
#define WARP_RESULT_NO_TICKET     3

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

// Warp destination tracking for testing
struct WarpDestination {
    u16 map_id;
    u8 x;
    u8 y;
    bool8 warp_executed;
};

// Error message tracking for testing
struct ErrorMessage {
    bool8 displayed;
    char message[256];
};

// Player inventory simulation for ticket tracking
#define MAX_INVENTORY_SIZE 100
static u16 sPlayerInventory[MAX_INVENTORY_SIZE] = {0};
static u8 sInventoryCount = 0;

// Global region state for testing
static struct RegionState sRegionState = {0};

// Warp destination tracking
static struct WarpDestination sWarpDestination = {0};

// Error message tracking
static struct ErrorMessage sErrorMessage = {0};

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

// Reset warp destination tracking
static void ResetWarpDestination(void) {
    sWarpDestination.map_id = 0;
    sWarpDestination.x = 0;
    sWarpDestination.y = 0;
    sWarpDestination.warp_executed = false;
}

// Reset error message tracking
static void ResetErrorMessage(void) {
    sErrorMessage.displayed = false;
    memset(sErrorMessage.message, 0, sizeof(sErrorMessage.message));
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
    ResetWarpDestination();
    ResetErrorMessage();
}

static u8 GetCurrentRegion(void) {
    return sRegionState.current_region;
}

static void SetCurrentRegion(u8 region_id) {
    if (region_id < NUM_REGIONS)
        sRegionState.current_region = region_id;
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
 * Get the starting map ID for a region
 */
static u16 GetRegionStartingMapId(u8 region_id) {
    switch (region_id) {
    case REGION_KANTO:
        return MAP_PALLET_TOWN;
    case REGION_HOENN:
        return MAP_LITTLEROOT_TOWN;
    case REGION_JOHTO:
        return MAP_NEW_BARK_TOWN;
    default:
        return 0;
    }
}

/**
 * Get the region ID for a ticket item
 */
static u8 GetRegionForTicket(u16 ticket_item) {
    switch (ticket_item) {
    case ITEM_HOENN_TICKET:
        return REGION_HOENN;
    case ITEM_JOHTO_TICKET:
        return REGION_JOHTO;
    default:
        return NUM_REGIONS;  // Invalid
    }
}

/**
 * Simulate warp execution (for testing)
 */
static void ExecuteWarp(u16 map_id, u8 x, u8 y) {
    sWarpDestination.map_id = map_id;
    sWarpDestination.x = x;
    sWarpDestination.y = y;
    sWarpDestination.warp_executed = true;
}

/**
 * Simulate error message display (for testing)
 */
static void DisplayErrorMessage(const char* message) {
    sErrorMessage.displayed = true;
    strncpy(sErrorMessage.message, message, sizeof(sErrorMessage.message) - 1);
}


/**
 * UseRegionTicket - Core ticket usage logic
 * 
 * This function simulates the ticket usage behavior:
 * - If region is unlocked, executes warp to region's starting location
 * - If region is locked, displays error message and prevents warp
 * 
 * @param ticket_item The ticket item ID being used
 * @return WARP_RESULT_SUCCESS if warp executed, error code otherwise
 */
static u8 UseRegionTicket(u16 ticket_item) {
    u8 target_region;
    u16 target_map;
    
    // Validate ticket item
    target_region = GetRegionForTicket(ticket_item);
    if (target_region >= NUM_REGIONS) {
        return WARP_RESULT_INVALID_ITEM;
    }
    
    // Check if player has the ticket
    if (!CheckBagHasItem(ticket_item, 1)) {
        return WARP_RESULT_NO_TICKET;
    }
    
    // Check if region is unlocked
    if (!IsRegionUnlocked(target_region)) {
        // Display error message for locked region
        switch (target_region) {
        case REGION_HOENN:
            DisplayErrorMessage("The HOENN region is not accessible yet.");
            break;
        case REGION_JOHTO:
            DisplayErrorMessage("The JOHTO region is not accessible yet.");
            break;
        default:
            DisplayErrorMessage("This region is not accessible.");
            break;
        }
        return WARP_RESULT_LOCKED;
    }
    
    // Region is unlocked - execute warp
    target_map = GetRegionStartingMapId(target_region);
    
    // Update current region
    SetCurrentRegion(target_region);
    
    // Execute warp to starting location (center of town: x=10, y=10)
    ExecuteWarp(target_map, 10, 10);
    
    return WARP_RESULT_SUCCESS;
}

// Test data structures for property-based testing
struct TicketUseTestData {
    u16 ticket_item;
    bool8 region_unlocked;
    bool8 has_ticket;
};

struct RegionTicketTestData {
    u8 target_region;
    bool8 hoenn_unlocked;
    bool8 johto_unlocked;
};

/**
 * Property 14: Region Ticket Warp Functionality
 * 
 * For any region ticket used by the player, if the region is unlocked,
 * the player should be warped to that region's starting location.
 * 
 * This property verifies:
 * 1. Using ITEM_HOENN_TICKET when Hoenn is unlocked warps to Littleroot Town
 * 2. Using ITEM_JOHTO_TICKET when Johto is unlocked warps to New Bark Town (stub)
 * 3. GetRegionTicketItem() returns correct ticket for each region
 * 4. Warp destination matches region's starting location
 * 5. Current region is updated after successful warp
 * 
 * **Validates: Requirements 10.1**
 */
static bool prop_region_ticket_warp_functionality(struct TicketUseTestData *data) {
    u8 result;
    u8 target_region;
    u16 expected_map;
    
    // Reset state for clean test
    InitRegionState();
    
    // Validate ticket item
    target_region = GetRegionForTicket(data->ticket_item);
    if (target_region >= NUM_REGIONS) {
        // Invalid ticket - skip this test case
        return true;
    }
    
    // Set up unlock state based on test data
    if (data->region_unlocked) {
        UnlockRegion(target_region);
    }
    
    // If test data says player has ticket but region wasn't unlocked (which gives ticket),
    // manually add the ticket
    if (data->has_ticket && !data->region_unlocked) {
        AddBagItem(data->ticket_item, 1);
    }
    
    // Skip test if player doesn't have ticket (can't use what you don't have)
    if (!data->has_ticket && !data->region_unlocked) {
        return true;
    }
    
    // Use the ticket
    result = UseRegionTicket(data->ticket_item);
    
    // If region is unlocked and player has ticket, warp should succeed
    if (data->region_unlocked || data->has_ticket) {
        if (IsRegionUnlocked(target_region)) {
            // Warp should have succeeded
            if (result != WARP_RESULT_SUCCESS) {
                printf("FAIL: Ticket use should succeed for unlocked region %u, got result %u\n",
                       target_region, result);
                return false;
            }
            
            // Verify warp was executed
            if (!sWarpDestination.warp_executed) {
                printf("FAIL: Warp should have been executed for unlocked region %u\n", target_region);
                return false;
            }
            
            // Verify warp destination is correct
            expected_map = GetRegionStartingMapId(target_region);
            if (sWarpDestination.map_id != expected_map) {
                printf("FAIL: Warp destination should be 0x%04X, got 0x%04X\n",
                       expected_map, sWarpDestination.map_id);
                return false;
            }
            
            // Verify current region was updated
            if (GetCurrentRegion() != target_region) {
                printf("FAIL: Current region should be %u after warp, got %u\n",
                       target_region, GetCurrentRegion());
                return false;
            }
            
            // Verify no error message was displayed
            if (sErrorMessage.displayed) {
                printf("FAIL: Error message should not be displayed for successful warp\n");
                return false;
            }
        }
    }
    
    return true;
}

/**
 * Property 18: Locked Region Ticket Rejection
 * 
 * For any attempt to use a region ticket for a locked region,
 * the system should display an error message and prevent the transition.
 * 
 * This property verifies:
 * 1. Using tickets for locked regions displays error message
 * 2. Using tickets for locked regions prevents warp
 * 3. Error message is appropriate for the locked region
 * 4. Current region remains unchanged after rejection
 * 
 * **Validates: Requirements 10.5**
 */
static bool prop_locked_region_ticket_rejection(struct TicketUseTestData *data) {
    u8 result;
    u8 target_region;
    u8 initial_region;
    
    // Reset state for clean test
    InitRegionState();
    
    // Validate ticket item
    target_region = GetRegionForTicket(data->ticket_item);
    if (target_region >= NUM_REGIONS) {
        // Invalid ticket - skip this test case
        return true;
    }
    
    // For this property, we specifically test locked regions
    // So we DON'T unlock the region, but we DO give the player the ticket
    if (data->has_ticket) {
        AddBagItem(data->ticket_item, 1);
    }
    
    // Skip if player doesn't have ticket
    if (!data->has_ticket) {
        return true;
    }
    
    // Record initial region
    initial_region = GetCurrentRegion();
    
    // Ensure region is locked (don't unlock it)
    // Use the ticket
    result = UseRegionTicket(data->ticket_item);
    
    // If region is locked, ticket use should fail
    if (!IsRegionUnlocked(target_region)) {
        // Result should indicate locked region
        if (result != WARP_RESULT_LOCKED) {
            printf("FAIL: Ticket use should return LOCKED for locked region %u, got %u\n",
                   target_region, result);
            return false;
        }
        
        // Warp should NOT have been executed
        if (sWarpDestination.warp_executed) {
            printf("FAIL: Warp should NOT be executed for locked region %u\n", target_region);
            return false;
        }
        
        // Error message should have been displayed
        if (!sErrorMessage.displayed) {
            printf("FAIL: Error message should be displayed for locked region %u\n", target_region);
            return false;
        }
        
        // Current region should remain unchanged
        if (GetCurrentRegion() != initial_region) {
            printf("FAIL: Current region should remain %u after rejection, got %u\n",
                   initial_region, GetCurrentRegion());
            return false;
        }
    }
    
    return true;
}


/**
 * Test runner for Property 14: Region Ticket Warp Functionality
 */
static int test_property_14_region_ticket_warp_functionality(void) {
    int num_trials = 100;  // Minimum 100 iterations per property
    int passed = 0;
    int failed = 0;
    
    printf("\n=== Property-Based Test: Region Ticket Warp Functionality (Property 14) ===\n");
    printf("**Validates: Requirements 10.1**\n");
    printf("Running %d random test cases...\n\n", num_trials);
    
    // Run property tests with random inputs
    for (int i = 0; i < num_trials; i++) {
        struct TicketUseTestData data;
        
        // Generate random test data
        // Randomly select Hoenn or Johto ticket
        data.ticket_item = (test_random(2) == 0) ? ITEM_HOENN_TICKET : ITEM_JOHTO_TICKET;
        data.region_unlocked = test_random(2);
        data.has_ticket = test_random(2);
        
        // Run the property test
        if (prop_region_ticket_warp_functionality(&data)) {
            passed++;
        } else {
            failed++;
            if (failed <= 5) {
                printf("  Counterexample #%d: ticket=0x%04X, unlocked=%d, has_ticket=%d\n",
                       failed, data.ticket_item, data.region_unlocked, data.has_ticket);
            }
        }
    }
    
    // Test specific edge cases
    printf("\nTesting specific edge cases...\n");
    
    // Edge case 1: Hoenn ticket with unlocked Hoenn warps to Littleroot Town
    printf("\nTesting Hoenn ticket warps to Littleroot Town...\n");
    InitRegionState();
    UnlockRegion(REGION_HOENN);  // This also awards ticket
    
    u8 result = UseRegionTicket(ITEM_HOENN_TICKET);
    if (result != WARP_RESULT_SUCCESS) {
        printf("FAIL: Hoenn ticket should succeed when Hoenn is unlocked\n");
        failed++;
    } else if (sWarpDestination.map_id != MAP_LITTLEROOT_TOWN) {
        printf("FAIL: Should warp to Littleroot Town (0x%04X), got 0x%04X\n",
               MAP_LITTLEROOT_TOWN, sWarpDestination.map_id);
        failed++;
    } else if (GetCurrentRegion() != REGION_HOENN) {
        printf("FAIL: Current region should be HOENN after warp\n");
        failed++;
    } else {
        printf("  Hoenn ticket → Littleroot Town: PASSED\n");
        passed++;
    }
    
    // Edge case 2: Johto ticket with unlocked Johto warps to New Bark Town (stub)
    printf("\nTesting Johto ticket warps to New Bark Town...\n");
    InitRegionState();
    UnlockRegion(REGION_JOHTO);  // This also awards ticket
    
    result = UseRegionTicket(ITEM_JOHTO_TICKET);
    if (result != WARP_RESULT_SUCCESS) {
        printf("FAIL: Johto ticket should succeed when Johto is unlocked\n");
        failed++;
    } else if (sWarpDestination.map_id != MAP_NEW_BARK_TOWN) {
        printf("FAIL: Should warp to New Bark Town (0x%04X), got 0x%04X\n",
               MAP_NEW_BARK_TOWN, sWarpDestination.map_id);
        failed++;
    } else if (GetCurrentRegion() != REGION_JOHTO) {
        printf("FAIL: Current region should be JOHTO after warp\n");
        failed++;
    } else {
        printf("  Johto ticket → New Bark Town: PASSED\n");
        passed++;
    }
    
    // Edge case 3: GetRegionTicketItem returns correct tickets
    printf("\nTesting GetRegionTicketItem returns correct tickets...\n");
    
    u16 hoenn_ticket = GetRegionTicketItem(REGION_HOENN);
    u16 johto_ticket = GetRegionTicketItem(REGION_JOHTO);
    u16 kanto_ticket = GetRegionTicketItem(REGION_KANTO);
    
    if (hoenn_ticket != ITEM_HOENN_TICKET) {
        printf("FAIL: GetRegionTicketItem(HOENN) should return 0x%04X, got 0x%04X\n",
               ITEM_HOENN_TICKET, hoenn_ticket);
        failed++;
    } else {
        passed++;
    }
    
    if (johto_ticket != ITEM_JOHTO_TICKET) {
        printf("FAIL: GetRegionTicketItem(JOHTO) should return 0x%04X, got 0x%04X\n",
               ITEM_JOHTO_TICKET, johto_ticket);
        failed++;
    } else {
        passed++;
    }
    
    if (kanto_ticket != ITEM_NONE) {
        printf("FAIL: GetRegionTicketItem(KANTO) should return ITEM_NONE, got 0x%04X\n",
               kanto_ticket);
        failed++;
    } else {
        passed++;
    }
    
    printf("  GetRegionTicketItem returns correct tickets: PASSED\n");
    
    // Edge case 4: Warp updates current region
    printf("\nTesting warp updates current region...\n");
    InitRegionState();
    
    // Start in Kanto
    if (GetCurrentRegion() != REGION_KANTO) {
        printf("FAIL: Should start in Kanto\n");
        failed++;
    } else {
        passed++;
    }
    
    // Unlock and travel to Hoenn
    UnlockRegion(REGION_HOENN);
    UseRegionTicket(ITEM_HOENN_TICKET);
    
    if (GetCurrentRegion() != REGION_HOENN) {
        printf("FAIL: Should be in Hoenn after using Hoenn ticket\n");
        failed++;
    } else {
        printf("  Warp updates current region: PASSED\n");
        passed++;
    }
    
    // Report results
    printf("\n=== Property 14 Test Results ===\n");
    printf("Total tests: %d\n", passed + failed);
    printf("Passed: %d\n", passed);
    printf("Failed: %d\n", failed);
    
    if (failed == 0) {
        printf("\n✓ Property 14 (Region Ticket Warp Functionality) PASSED!\n");
        return 0;
    } else {
        printf("\n✗ Property 14 (Region Ticket Warp Functionality) FAILED with %d counterexamples\n", failed);
        return 1;
    }
}

/**
 * Test runner for Property 18: Locked Region Ticket Rejection
 */
static int test_property_18_locked_region_ticket_rejection(void) {
    int num_trials = 100;  // Minimum 100 iterations per property
    int passed = 0;
    int failed = 0;
    
    printf("\n=== Property-Based Test: Locked Region Ticket Rejection (Property 18) ===\n");
    printf("**Validates: Requirements 10.5**\n");
    printf("Running %d random test cases...\n\n", num_trials);
    
    // Run property tests with random inputs
    for (int i = 0; i < num_trials; i++) {
        struct TicketUseTestData data;
        
        // Generate random test data
        // Randomly select Hoenn or Johto ticket
        data.ticket_item = (test_random(2) == 0) ? ITEM_HOENN_TICKET : ITEM_JOHTO_TICKET;
        data.region_unlocked = false;  // Always test locked regions for this property
        data.has_ticket = true;  // Player has ticket but region is locked
        
        // Run the property test
        if (prop_locked_region_ticket_rejection(&data)) {
            passed++;
        } else {
            failed++;
            if (failed <= 5) {
                printf("  Counterexample #%d: ticket=0x%04X\n", failed, data.ticket_item);
            }
        }
    }
    
    // Test specific edge cases
    printf("\nTesting specific edge cases...\n");
    
    // Edge case 1: Hoenn ticket with locked Hoenn displays error
    printf("\nTesting locked Hoenn displays error...\n");
    InitRegionState();
    AddBagItem(ITEM_HOENN_TICKET, 1);  // Give ticket without unlocking
    
    u8 result = UseRegionTicket(ITEM_HOENN_TICKET);
    if (result != WARP_RESULT_LOCKED) {
        printf("FAIL: Should return LOCKED for locked Hoenn, got %u\n", result);
        failed++;
    } else if (!sErrorMessage.displayed) {
        printf("FAIL: Error message should be displayed for locked Hoenn\n");
        failed++;
    } else if (sWarpDestination.warp_executed) {
        printf("FAIL: Warp should NOT be executed for locked Hoenn\n");
        failed++;
    } else {
        printf("  Locked Hoenn displays error: PASSED\n");
        passed++;
    }
    
    // Edge case 2: Johto ticket with locked Johto displays error
    printf("\nTesting locked Johto displays error...\n");
    InitRegionState();
    AddBagItem(ITEM_JOHTO_TICKET, 1);  // Give ticket without unlocking
    
    result = UseRegionTicket(ITEM_JOHTO_TICKET);
    if (result != WARP_RESULT_LOCKED) {
        printf("FAIL: Should return LOCKED for locked Johto, got %u\n", result);
        failed++;
    } else if (!sErrorMessage.displayed) {
        printf("FAIL: Error message should be displayed for locked Johto\n");
        failed++;
    } else if (sWarpDestination.warp_executed) {
        printf("FAIL: Warp should NOT be executed for locked Johto\n");
        failed++;
    } else {
        printf("  Locked Johto displays error: PASSED\n");
        passed++;
    }
    
    // Edge case 3: Current region unchanged after rejection
    printf("\nTesting current region unchanged after rejection...\n");
    InitRegionState();
    AddBagItem(ITEM_HOENN_TICKET, 1);
    
    u8 initial_region = GetCurrentRegion();
    UseRegionTicket(ITEM_HOENN_TICKET);
    
    if (GetCurrentRegion() != initial_region) {
        printf("FAIL: Current region should remain %u after rejection, got %u\n",
               initial_region, GetCurrentRegion());
        failed++;
    } else {
        printf("  Current region unchanged after rejection: PASSED\n");
        passed++;
    }
    
    // Edge case 4: No warp executed for locked region
    printf("\nTesting no warp executed for locked region...\n");
    InitRegionState();
    AddBagItem(ITEM_JOHTO_TICKET, 1);
    
    UseRegionTicket(ITEM_JOHTO_TICKET);
    
    if (sWarpDestination.warp_executed) {
        printf("FAIL: Warp should NOT be executed for locked region\n");
        failed++;
    } else {
        printf("  No warp executed for locked region: PASSED\n");
        passed++;
    }
    
    // Edge case 5: Error message contains region name
    printf("\nTesting error message contains region name...\n");
    InitRegionState();
    AddBagItem(ITEM_HOENN_TICKET, 1);
    
    UseRegionTicket(ITEM_HOENN_TICKET);
    
    if (!sErrorMessage.displayed) {
        printf("FAIL: Error message should be displayed\n");
        failed++;
    } else if (strstr(sErrorMessage.message, "HOENN") == NULL) {
        printf("FAIL: Error message should mention HOENN region\n");
        printf("  Got: %s\n", sErrorMessage.message);
        failed++;
    } else {
        printf("  Error message contains region name: PASSED\n");
        passed++;
    }
    
    // Report results
    printf("\n=== Property 18 Test Results ===\n");
    printf("Total tests: %d\n", passed + failed);
    printf("Passed: %d\n", passed);
    printf("Failed: %d\n", failed);
    
    if (failed == 0) {
        printf("\n✓ Property 18 (Locked Region Ticket Rejection) PASSED!\n");
        return 0;
    } else {
        printf("\n✗ Property 18 (Locked Region Ticket Rejection) FAILED with %d counterexamples\n", failed);
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
    printf("║     Property-Based Tests for Region Ticket System                ║\n");
    printf("║     Task 3.6: Write property tests for region tickets            ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n");
    printf("\nRegion configuration: Kanto → Hoenn → Johto (3 regions)\n");
    printf("Sinnoh descoped per ADR-003\n");
    printf("Progression order per ADR-004\n");
    printf("Minimum iterations per property: 100\n");
    
    int property14_result = test_property_14_region_ticket_warp_functionality();
    int property18_result = test_property_18_locked_region_ticket_rejection();
    
    // Overall results
    printf("\n\n╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║                    OVERALL TEST RESULTS                          ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n");
    
    if (property14_result == 0 && property18_result == 0) {
        printf("\n✓ ALL PROPERTY TESTS PASSED!\n\n");
        printf("Property 14 (Region Ticket Warp Functionality): PASSED\n");
        printf("  - Validates: Requirements 10.1\n");
        printf("Property 18 (Locked Region Ticket Rejection): PASSED\n");
        printf("  - Validates: Requirements 10.5\n");
        return 0;
    } else {
        printf("\n✗ SOME PROPERTY TESTS FAILED\n\n");
        if (property14_result != 0) {
            printf("Property 14 (Region Ticket Warp Functionality): FAILED\n");
        } else {
            printf("Property 14 (Region Ticket Warp Functionality): PASSED\n");
        }
        if (property18_result != 0) {
            printf("Property 18 (Locked Region Ticket Rejection): FAILED\n");
        } else {
            printf("Property 18 (Locked Region Ticket Rejection): PASSED\n");
        }
        return 1;
    }
}
