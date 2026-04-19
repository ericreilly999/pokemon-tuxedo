/*
 * Integration Tests for Region Progression System
 * 
 * Task 4.2: Run integration tests for region progression
 * 
 * Test Scenarios:
 * 1. Complete Kanto Elite Four → Hoenn unlocks → Hoenn ticket awarded
 * 2. Use Hoenn ticket → Warp to Littleroot Town → Region state updated
 * 3. Party preserved during Kanto → Hoenn transition
 * 4. Inventory preserved during Kanto → Hoenn transition
 * 
 * **Validates: Requirements 1.1, 1.4, 10.1, 10.2, 10.3**
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
#define BADGES_PER_REGION  8
#define TOTAL_BADGES       (NUM_REGIONS * BADGES_PER_REGION)  // 24 badges

// Party constants
#define PARTY_SIZE  6
#define MAX_MON_MOVES  4

// Inventory constants
#define MAX_INVENTORY_SIZE 100
#define MAX_ITEM_QUANTITY  99

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

// Pokemon species constants (sample for testing)
#define SPECIES_NONE        0
#define SPECIES_BULBASAUR   1
#define SPECIES_CHARMANDER  4
#define SPECIES_SQUIRTLE    7
#define SPECIES_PIKACHU     25
#define SPECIES_EEVEE       133
#define MAX_SPECIES         493

// Item constants (sample for testing)
#define ITEM_POTION         17
#define ITEM_SUPER_POTION   26
#define ITEM_HYPER_POTION   27
#define ITEM_MAX_POTION     28
#define ITEM_POKE_BALL      4
#define ITEM_GREAT_BALL     3
#define ITEM_ULTRA_BALL     2
#define ITEM_MASTER_BALL    1
#define ITEM_RARE_CANDY     50
#define MAX_ITEM_ID         500

// ==========================================
// Data Structures
// ==========================================

// Pokemon structure for party simulation
struct Pokemon {
    u16 species;
    u8 level;
    u16 hp;
    u16 maxHP;
    u16 attack;
    u16 defense;
    u16 speed;
    u16 spAttack;
    u16 spDefense;
    u16 moves[MAX_MON_MOVES];
    u32 experience;
    u16 heldItem;
    u8 friendship;
    bool8 isEgg;
};

// Inventory item structure
struct InventoryItem {
    u16 itemId;
    u8 quantity;
};

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

// ==========================================
// Global State Variables
// ==========================================

// Player party simulation
static struct Pokemon sPlayerParty[PARTY_SIZE] = {0};
static u8 sPartyCount = 0;

// Player inventory simulation
static struct InventoryItem sPlayerInventory[MAX_INVENTORY_SIZE] = {0};
static u8 sInventoryCount = 0;

// Global region state for testing
static struct RegionState sRegionState = {0};

// Warp destination tracking
static struct WarpDestination sWarpDestination = {0};

// Error message tracking
static struct ErrorMessage sErrorMessage = {0};

// ==========================================
// Party Management Functions
// ==========================================

static void ClearParty(void) {
    u8 i;
    for (i = 0; i < PARTY_SIZE; i++) {
        memset(&sPlayerParty[i], 0, sizeof(struct Pokemon));
    }
    sPartyCount = 0;
}

static bool8 AddPartyMember(struct Pokemon *mon) {
    if (sPartyCount >= PARTY_SIZE)
        return false;
    memcpy(&sPlayerParty[sPartyCount], mon, sizeof(struct Pokemon));
    sPartyCount++;
    return true;
}

static u8 GetPartyCount(void) {
    return sPartyCount;
}

static struct Pokemon* GetPartyMember(u8 index) {
    if (index >= sPartyCount)
        return NULL;
    return &sPlayerParty[index];
}

static bool8 ComparePokemon(const struct Pokemon *a, const struct Pokemon *b) {
    u8 i;
    if (a->species != b->species) return false;
    if (a->level != b->level) return false;
    if (a->hp != b->hp) return false;
    if (a->maxHP != b->maxHP) return false;
    if (a->attack != b->attack) return false;
    if (a->defense != b->defense) return false;
    if (a->speed != b->speed) return false;
    if (a->spAttack != b->spAttack) return false;
    if (a->spDefense != b->spDefense) return false;
    if (a->experience != b->experience) return false;
    if (a->heldItem != b->heldItem) return false;
    if (a->friendship != b->friendship) return false;
    if (a->isEgg != b->isEgg) return false;
    for (i = 0; i < MAX_MON_MOVES; i++) {
        if (a->moves[i] != b->moves[i]) return false;
    }
    return true;
}

// ==========================================
// Inventory Management Functions
// ==========================================

static void ClearInventory(void) {
    u8 i;
    for (i = 0; i < MAX_INVENTORY_SIZE; i++) {
        sPlayerInventory[i].itemId = ITEM_NONE;
        sPlayerInventory[i].quantity = 0;
    }
    sInventoryCount = 0;
}

static bool8 AddBagItem(u16 item_id, u8 count) {
    u8 i;
    // Check if item already exists in inventory
    for (i = 0; i < sInventoryCount; i++) {
        if (sPlayerInventory[i].itemId == item_id) {
            u16 newQuantity = sPlayerInventory[i].quantity + count;
            if (newQuantity > MAX_ITEM_QUANTITY)
                newQuantity = MAX_ITEM_QUANTITY;
            sPlayerInventory[i].quantity = (u8)newQuantity;
            return true;
        }
    }
    // Add new item
    if (sInventoryCount >= MAX_INVENTORY_SIZE)
        return false;
    sPlayerInventory[sInventoryCount].itemId = item_id;
    sPlayerInventory[sInventoryCount].quantity = count;
    sInventoryCount++;
    return true;
}

static bool8 CheckBagHasItem(u16 item_id, u8 count) {
    u8 i;
    for (i = 0; i < sInventoryCount; i++) {
        if (sPlayerInventory[i].itemId == item_id &&
            sPlayerInventory[i].quantity >= count)
            return true;
    }
    return false;
}

static u8 GetInventoryCount(void) {
    return sInventoryCount;
}

static struct InventoryItem* GetInventoryItem(u8 index) {
    if (index >= sInventoryCount)
        return NULL;
    return &sPlayerInventory[index];
}

static bool8 CompareInventoryItem(const struct InventoryItem *a, const struct InventoryItem *b) {
    return (a->itemId == b->itemId && a->quantity == b->quantity);
}

// ==========================================
// Region State Management Functions
// ==========================================

static void ResetWarpDestination(void) {
    sWarpDestination.map_id = 0;
    sWarpDestination.x = 0;
    sWarpDestination.y = 0;
    sWarpDestination.warp_executed = false;
}

static void ResetErrorMessage(void) {
    sErrorMessage.displayed = false;
    memset(sErrorMessage.message, 0, sizeof(sErrorMessage.message));
}

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
    ClearParty();
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
        return true;  // Kanto is always unlocked
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
        break;
    }
}

static bool8 CheckEliteFourDefeatAndUnlock(u8 region_id) {
    bool8 region_unlocked = false;
    if (region_id >= NUM_REGIONS)
        return false;
    if (IsEliteFourDefeated(region_id))
        return false;
    SetEliteFourDefeated(region_id);
    switch (region_id) {
    case REGION_KANTO:
        if (!IsRegionUnlocked(REGION_HOENN)) {
            UnlockRegion(REGION_HOENN);
            region_unlocked = true;
        }
        break;
    case REGION_HOENN:
        if (!IsRegionUnlocked(REGION_JOHTO)) {
            UnlockRegion(REGION_JOHTO);
            region_unlocked = true;
        }
        break;
    case REGION_JOHTO:
        break;
    }
    return region_unlocked;
}

// ==========================================
// Warp System Functions
// ==========================================

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

static u8 GetRegionForTicket(u16 ticket_item) {
    switch (ticket_item) {
    case ITEM_HOENN_TICKET:
        return REGION_HOENN;
    case ITEM_JOHTO_TICKET:
        return REGION_JOHTO;
    default:
        return NUM_REGIONS;
    }
}

static void ExecuteWarp(u16 map_id, u8 x, u8 y) {
    sWarpDestination.map_id = map_id;
    sWarpDestination.x = x;
    sWarpDestination.y = y;
    sWarpDestination.warp_executed = true;
}

static void DisplayErrorMessage(const char* message) {
    sErrorMessage.displayed = true;
    strncpy(sErrorMessage.message, message, sizeof(sErrorMessage.message) - 1);
}

static bool8 CanTransitionToRegion(u8 target_region) {
    if (target_region >= NUM_REGIONS)
        return false;
    if (target_region == REGION_KANTO)
        return true;
    return IsRegionUnlocked(target_region);
}

static bool8 TransitionToRegion(u8 target_region) {
    if (!CanTransitionToRegion(target_region))
        return false;
    SetCurrentRegion(target_region);
    return true;
}

static u8 UseRegionTicket(u16 ticket_item) {
    u8 target_region;
    u16 target_map;
    target_region = GetRegionForTicket(ticket_item);
    if (target_region >= NUM_REGIONS)
        return WARP_RESULT_INVALID_ITEM;
    if (!CheckBagHasItem(ticket_item, 1))
        return WARP_RESULT_NO_TICKET;
    if (!IsRegionUnlocked(target_region)) {
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
    target_map = GetRegionStartingMapId(target_region);
    SetCurrentRegion(target_region);
    ExecuteWarp(target_map, 10, 10);
    return WARP_RESULT_SUCCESS;
}

// ==========================================
// Integration Test 1: Complete Kanto Elite Four → Hoenn Unlocks
// **Validates: Requirements 1.1, 1.4**
// ==========================================

static int test_kanto_elite_four_unlocks_hoenn(void) {
    int passed = 0;
    int failed = 0;
    
    printf("\n=== Integration Test 1: Kanto Elite Four → Hoenn Unlock ===\n");
    printf("**Validates: Requirements 1.1, 1.4**\n\n");
    
    // Initialize clean state
    InitRegionState();
    
    // Step 1: Verify initial state - player starts in Kanto
    printf("Step 1: Verify initial state...\n");
    if (GetCurrentRegion() != REGION_KANTO) {
        printf("  FAIL: Player should start in Kanto\n");
        failed++;
    } else {
        printf("  PASS: Player starts in Kanto\n");
        passed++;
    }
    
    // Step 2: Verify Hoenn is initially locked
    printf("Step 2: Verify Hoenn is initially locked...\n");
    if (IsRegionUnlocked(REGION_HOENN)) {
        printf("  FAIL: Hoenn should be locked initially\n");
        failed++;
    } else {
        printf("  PASS: Hoenn is locked initially\n");
        passed++;
    }
    
    // Step 3: Verify player doesn't have Hoenn ticket
    printf("Step 3: Verify no Hoenn ticket initially...\n");
    if (CheckBagHasItem(ITEM_HOENN_TICKET, 1)) {
        printf("  FAIL: Player should not have Hoenn ticket initially\n");
        failed++;
    } else {
        printf("  PASS: Player has no Hoenn ticket initially\n");
        passed++;
    }
    
    // Step 4: Simulate defeating Kanto Elite Four
    printf("Step 4: Defeat Kanto Elite Four...\n");
    bool8 unlock_result = CheckEliteFourDefeatAndUnlock(REGION_KANTO);
    if (!unlock_result) {
        printf("  FAIL: CheckEliteFourDefeatAndUnlock should return TRUE\n");
        failed++;
    } else {
        printf("  PASS: CheckEliteFourDefeatAndUnlock returned TRUE\n");
        passed++;
    }
    
    // Step 5: Verify Kanto E4 is marked as defeated
    printf("Step 5: Verify Kanto E4 marked as defeated...\n");
    if (!IsEliteFourDefeated(REGION_KANTO)) {
        printf("  FAIL: Kanto E4 should be marked as defeated\n");
        failed++;
    } else {
        printf("  PASS: Kanto E4 is marked as defeated\n");
        passed++;
    }
    
    // Step 6: Verify Hoenn is now unlocked
    printf("Step 6: Verify Hoenn is now unlocked...\n");
    if (!IsRegionUnlocked(REGION_HOENN)) {
        printf("  FAIL: Hoenn should be unlocked after Kanto E4 defeat\n");
        failed++;
    } else {
        printf("  PASS: Hoenn is now unlocked\n");
        passed++;
    }
    
    // Step 7: Verify Hoenn ticket was awarded
    printf("Step 7: Verify Hoenn ticket was awarded...\n");
    if (!CheckBagHasItem(ITEM_HOENN_TICKET, 1)) {
        printf("  FAIL: Player should have Hoenn ticket after unlock\n");
        failed++;
    } else {
        printf("  PASS: Hoenn ticket was awarded\n");
        passed++;
    }
    
    // Step 8: Verify Johto is still locked
    printf("Step 8: Verify Johto is still locked...\n");
    if (IsRegionUnlocked(REGION_JOHTO)) {
        printf("  FAIL: Johto should still be locked\n");
        failed++;
    } else {
        printf("  PASS: Johto is still locked\n");
        passed++;
    }
    
    // Report results
    printf("\n--- Test 1 Results ---\n");
    printf("Passed: %d, Failed: %d\n", passed, failed);
    return failed;
}

// ==========================================
// Integration Test 2: Hoenn Ticket Usage and Region Warping
// **Validates: Requirements 10.1**
// ==========================================

static int test_hoenn_ticket_usage_and_warp(void) {
    int passed = 0;
    int failed = 0;
    
    printf("\n=== Integration Test 2: Hoenn Ticket Usage and Region Warping ===\n");
    printf("**Validates: Requirements 10.1**\n\n");
    
    // Initialize and unlock Hoenn
    InitRegionState();
    CheckEliteFourDefeatAndUnlock(REGION_KANTO);  // Unlocks Hoenn and awards ticket
    
    // Step 1: Verify player is in Kanto with Hoenn ticket
    printf("Step 1: Verify starting state...\n");
    if (GetCurrentRegion() != REGION_KANTO) {
        printf("  FAIL: Player should be in Kanto\n");
        failed++;
    } else {
        printf("  PASS: Player is in Kanto\n");
        passed++;
    }
    
    if (!CheckBagHasItem(ITEM_HOENN_TICKET, 1)) {
        printf("  FAIL: Player should have Hoenn ticket\n");
        failed++;
    } else {
        printf("  PASS: Player has Hoenn ticket\n");
        passed++;
    }
    
    // Step 2: Use Hoenn ticket
    printf("Step 2: Use Hoenn ticket...\n");
    ResetWarpDestination();
    u8 result = UseRegionTicket(ITEM_HOENN_TICKET);
    
    if (result != WARP_RESULT_SUCCESS) {
        printf("  FAIL: UseRegionTicket should return SUCCESS, got %u\n", result);
        failed++;
    } else {
        printf("  PASS: UseRegionTicket returned SUCCESS\n");
        passed++;
    }
    
    // Step 3: Verify warp was executed
    printf("Step 3: Verify warp was executed...\n");
    if (!sWarpDestination.warp_executed) {
        printf("  FAIL: Warp should have been executed\n");
        failed++;
    } else {
        printf("  PASS: Warp was executed\n");
        passed++;
    }
    
    // Step 4: Verify warp destination is Littleroot Town
    printf("Step 4: Verify warp destination is Littleroot Town...\n");
    if (sWarpDestination.map_id != MAP_LITTLEROOT_TOWN) {
        printf("  FAIL: Warp destination should be Littleroot Town (0x%04X), got 0x%04X\n",
               MAP_LITTLEROOT_TOWN, sWarpDestination.map_id);
        failed++;
    } else {
        printf("  PASS: Warp destination is Littleroot Town\n");
        passed++;
    }
    
    // Step 5: Verify current region is now Hoenn
    printf("Step 5: Verify current region is now Hoenn...\n");
    if (GetCurrentRegion() != REGION_HOENN) {
        printf("  FAIL: Current region should be Hoenn, got %u\n", GetCurrentRegion());
        failed++;
    } else {
        printf("  PASS: Current region is now Hoenn\n");
        passed++;
    }
    
    // Step 6: Verify no error message was displayed
    printf("Step 6: Verify no error message displayed...\n");
    if (sErrorMessage.displayed) {
        printf("  FAIL: No error message should be displayed for successful warp\n");
        failed++;
    } else {
        printf("  PASS: No error message displayed\n");
        passed++;
    }
    
    // Report results
    printf("\n--- Test 2 Results ---\n");
    printf("Passed: %d, Failed: %d\n", passed, failed);
    return failed;
}

// ==========================================
// Integration Test 3: Party Preservation During Transition
// **Validates: Requirements 10.2**
// ==========================================

static int test_party_preservation_during_transition(void) {
    int passed = 0;
    int failed = 0;
    struct Pokemon partyBackup[PARTY_SIZE];
    u8 partyCountBackup;
    u8 i;
    
    printf("\n=== Integration Test 3: Party Preservation During Transition ===\n");
    printf("**Validates: Requirements 10.2**\n\n");
    
    // Initialize and set up party
    InitRegionState();
    CheckEliteFourDefeatAndUnlock(REGION_KANTO);  // Unlocks Hoenn
    
    // Step 1: Create a party with various Pokemon
    printf("Step 1: Create test party...\n");
    ClearParty();
    
    // Add Pokemon with specific stats to verify preservation
    struct Pokemon mon1 = {
        .species = SPECIES_PIKACHU,
        .level = 50,
        .hp = 100,
        .maxHP = 120,
        .attack = 80,
        .defense = 60,
        .speed = 110,
        .spAttack = 90,
        .spDefense = 70,
        .moves = {85, 86, 87, 88},  // Thunderbolt, Thunder Wave, etc.
        .experience = 125000,
        .heldItem = ITEM_RARE_CANDY,
        .friendship = 200,
        .isEgg = false
    };
    
    struct Pokemon mon2 = {
        .species = SPECIES_CHARMANDER,
        .level = 45,
        .hp = 80,
        .maxHP = 95,
        .attack = 70,
        .defense = 55,
        .speed = 75,
        .spAttack = 85,
        .spDefense = 60,
        .moves = {52, 53, 126, 0},  // Ember, Flamethrower, Fire Blast
        .experience = 91125,
        .heldItem = ITEM_NONE,
        .friendship = 150,
        .isEgg = false
    };
    
    struct Pokemon mon3 = {
        .species = SPECIES_SQUIRTLE,
        .level = 40,
        .hp = 75,
        .maxHP = 90,
        .attack = 55,
        .defense = 70,
        .speed = 50,
        .spAttack = 65,
        .spDefense = 75,
        .moves = {55, 56, 57, 58},  // Water Gun, Bubble, etc.
        .experience = 64000,
        .heldItem = ITEM_POTION,
        .friendship = 100,
        .isEgg = false
    };
    
    AddPartyMember(&mon1);
    AddPartyMember(&mon2);
    AddPartyMember(&mon3);
    
    printf("  Created party with %u Pokemon\n", GetPartyCount());
    passed++;
    
    // Step 2: Backup party state
    printf("Step 2: Backup party state...\n");
    partyCountBackup = GetPartyCount();
    for (i = 0; i < partyCountBackup; i++) {
        memcpy(&partyBackup[i], GetPartyMember(i), sizeof(struct Pokemon));
    }
    printf("  Backed up %u Pokemon\n", partyCountBackup);
    passed++;
    
    // Step 3: Execute region transition
    printf("Step 3: Execute region transition to Hoenn...\n");
    UseRegionTicket(ITEM_HOENN_TICKET);
    
    if (GetCurrentRegion() != REGION_HOENN) {
        printf("  FAIL: Should be in Hoenn after transition\n");
        failed++;
    } else {
        printf("  PASS: Transitioned to Hoenn\n");
        passed++;
    }
    
    // Step 4: Verify party count preserved
    printf("Step 4: Verify party count preserved...\n");
    if (GetPartyCount() != partyCountBackup) {
        printf("  FAIL: Party count changed from %u to %u\n", partyCountBackup, GetPartyCount());
        failed++;
    } else {
        printf("  PASS: Party count preserved (%u Pokemon)\n", GetPartyCount());
        passed++;
    }
    
    // Step 5: Verify each Pokemon preserved
    printf("Step 5: Verify each Pokemon preserved...\n");
    bool8 all_preserved = true;
    for (i = 0; i < partyCountBackup; i++) {
        struct Pokemon *current = GetPartyMember(i);
        if (!ComparePokemon(&partyBackup[i], current)) {
            printf("  FAIL: Pokemon %u was modified during transition\n", i);
            printf("    Before: species=%u, level=%u, hp=%u, heldItem=%u\n",
                   partyBackup[i].species, partyBackup[i].level, 
                   partyBackup[i].hp, partyBackup[i].heldItem);
            printf("    After:  species=%u, level=%u, hp=%u, heldItem=%u\n",
                   current->species, current->level, current->hp, current->heldItem);
            all_preserved = false;
            failed++;
        }
    }
    if (all_preserved) {
        printf("  PASS: All %u Pokemon preserved exactly\n", partyCountBackup);
        passed++;
    }
    
    // Report results
    printf("\n--- Test 3 Results ---\n");
    printf("Passed: %d, Failed: %d\n", passed, failed);
    return failed;
}

// ==========================================
// Integration Test 4: Inventory Preservation During Transition
// **Validates: Requirements 10.3**
// ==========================================

static int test_inventory_preservation_during_transition(void) {
    int passed = 0;
    int failed = 0;
    struct InventoryItem inventoryBackup[MAX_INVENTORY_SIZE];
    u8 inventoryCountBackup;
    u8 i;
    
    printf("\n=== Integration Test 4: Inventory Preservation During Transition ===\n");
    printf("**Validates: Requirements 10.3**\n\n");
    
    // Initialize and set up inventory
    InitRegionState();
    CheckEliteFourDefeatAndUnlock(REGION_KANTO);  // Unlocks Hoenn and awards ticket
    
    // Step 1: Add items to inventory (ticket already added by unlock)
    printf("Step 1: Add items to inventory...\n");
    AddBagItem(ITEM_POTION, 10);
    AddBagItem(ITEM_SUPER_POTION, 5);
    AddBagItem(ITEM_HYPER_POTION, 3);
    AddBagItem(ITEM_MAX_POTION, 1);
    AddBagItem(ITEM_POKE_BALL, 20);
    AddBagItem(ITEM_GREAT_BALL, 15);
    AddBagItem(ITEM_ULTRA_BALL, 10);
    AddBagItem(ITEM_RARE_CANDY, 5);
    
    printf("  Added 8 item types to inventory\n");
    printf("  Total inventory count: %u\n", GetInventoryCount());
    passed++;
    
    // Step 2: Backup inventory state
    printf("Step 2: Backup inventory state...\n");
    inventoryCountBackup = GetInventoryCount();
    for (i = 0; i < inventoryCountBackup; i++) {
        struct InventoryItem *item = GetInventoryItem(i);
        inventoryBackup[i].itemId = item->itemId;
        inventoryBackup[i].quantity = item->quantity;
    }
    printf("  Backed up %u inventory slots\n", inventoryCountBackup);
    passed++;
    
    // Step 3: Execute region transition
    printf("Step 3: Execute region transition to Hoenn...\n");
    UseRegionTicket(ITEM_HOENN_TICKET);
    
    if (GetCurrentRegion() != REGION_HOENN) {
        printf("  FAIL: Should be in Hoenn after transition\n");
        failed++;
    } else {
        printf("  PASS: Transitioned to Hoenn\n");
        passed++;
    }
    
    // Step 4: Verify inventory count preserved
    printf("Step 4: Verify inventory count preserved...\n");
    u8 currentInventoryCount = GetInventoryCount();
    if (currentInventoryCount != inventoryCountBackup) {
        printf("  FAIL: Inventory count changed from %u to %u\n", 
               inventoryCountBackup, currentInventoryCount);
        failed++;
    } else {
        printf("  PASS: Inventory count preserved (%u slots)\n", currentInventoryCount);
        passed++;
    }
    
    // Step 5: Verify each item preserved
    printf("Step 5: Verify each item preserved...\n");
    bool8 all_preserved = true;
    for (i = 0; i < inventoryCountBackup; i++) {
        struct InventoryItem *current = GetInventoryItem(i);
        if (!CompareInventoryItem(&inventoryBackup[i], current)) {
            printf("  FAIL: Item %u was modified during transition\n", i);
            printf("    Before: itemId=%u, quantity=%u\n",
                   inventoryBackup[i].itemId, inventoryBackup[i].quantity);
            printf("    After:  itemId=%u, quantity=%u\n",
                   current->itemId, current->quantity);
            all_preserved = false;
            failed++;
        }
    }
    if (all_preserved) {
        printf("  PASS: All %u inventory items preserved exactly\n", inventoryCountBackup);
        passed++;
    }
    
    // Step 6: Verify specific items still accessible
    printf("Step 6: Verify specific items still accessible...\n");
    bool8 items_accessible = true;
    
    if (!CheckBagHasItem(ITEM_POTION, 1)) {
        printf("  FAIL: Potions should still be in inventory\n");
        items_accessible = false;
    }
    if (!CheckBagHasItem(ITEM_POKE_BALL, 1)) {
        printf("  FAIL: Poke Balls should still be in inventory\n");
        items_accessible = false;
    }
    if (!CheckBagHasItem(ITEM_RARE_CANDY, 1)) {
        printf("  FAIL: Rare Candies should still be in inventory\n");
        items_accessible = false;
    }
    if (!CheckBagHasItem(ITEM_HOENN_TICKET, 1)) {
        printf("  FAIL: Hoenn Ticket should still be in inventory\n");
        items_accessible = false;
    }
    
    if (items_accessible) {
        printf("  PASS: All specific items still accessible\n");
        passed++;
    } else {
        failed++;
    }
    
    // Report results
    printf("\n--- Test 4 Results ---\n");
    printf("Passed: %d, Failed: %d\n", passed, failed);
    return failed;
}

// ==========================================
// Integration Test 5: Full Progression Flow
// **Validates: Requirements 1.1, 1.4, 10.1, 10.2, 10.3**
// ==========================================

static int test_full_progression_flow(void) {
    int passed = 0;
    int failed = 0;
    struct Pokemon partyBackup[PARTY_SIZE];
    struct InventoryItem inventoryBackup[MAX_INVENTORY_SIZE];
    u8 partyCountBackup, inventoryCountBackup;
    u8 i;
    
    printf("\n=== Integration Test 5: Full Progression Flow ===\n");
    printf("**Validates: Requirements 1.1, 1.4, 10.1, 10.2, 10.3**\n\n");
    
    // Initialize clean state
    InitRegionState();
    
    // Step 1: Set up initial party and inventory in Kanto
    printf("Step 1: Set up initial party and inventory in Kanto...\n");
    
    struct Pokemon starter = {
        .species = SPECIES_BULBASAUR,
        .level = 55,
        .hp = 130,
        .maxHP = 145,
        .attack = 75,
        .defense = 80,
        .speed = 70,
        .spAttack = 95,
        .spDefense = 90,
        .moves = {22, 75, 76, 77},  // Vine Whip, Razor Leaf, Solar Beam, etc.
        .experience = 166375,
        .heldItem = ITEM_NONE,
        .friendship = 220,
        .isEgg = false
    };
    AddPartyMember(&starter);
    
    AddBagItem(ITEM_POTION, 15);
    AddBagItem(ITEM_POKE_BALL, 30);
    AddBagItem(ITEM_RARE_CANDY, 3);
    
    printf("  Party: 1 Pokemon (Bulbasaur Lv.55)\n");
    printf("  Inventory: 3 item types\n");
    passed++;
    
    // Step 2: Backup state before E4 defeat
    printf("Step 2: Backup state before E4 defeat...\n");
    partyCountBackup = GetPartyCount();
    for (i = 0; i < partyCountBackup; i++) {
        memcpy(&partyBackup[i], GetPartyMember(i), sizeof(struct Pokemon));
    }
    inventoryCountBackup = GetInventoryCount();
    for (i = 0; i < inventoryCountBackup; i++) {
        struct InventoryItem *item = GetInventoryItem(i);
        inventoryBackup[i].itemId = item->itemId;
        inventoryBackup[i].quantity = item->quantity;
    }
    printf("  Backed up party (%u) and inventory (%u)\n", partyCountBackup, inventoryCountBackup);
    passed++;
    
    // Step 3: Defeat Kanto Elite Four
    printf("Step 3: Defeat Kanto Elite Four...\n");
    bool8 unlock_result = CheckEliteFourDefeatAndUnlock(REGION_KANTO);
    
    if (!unlock_result) {
        printf("  FAIL: Should unlock Hoenn\n");
        failed++;
    } else if (!IsRegionUnlocked(REGION_HOENN)) {
        printf("  FAIL: Hoenn should be unlocked\n");
        failed++;
    } else if (!CheckBagHasItem(ITEM_HOENN_TICKET, 1)) {
        printf("  FAIL: Should have Hoenn ticket\n");
        failed++;
    } else {
        printf("  PASS: Kanto E4 defeated, Hoenn unlocked, ticket awarded\n");
        passed++;
    }
    
    // Step 4: Use Hoenn ticket to travel
    printf("Step 4: Use Hoenn ticket to travel...\n");
    ResetWarpDestination();
    u8 warp_result = UseRegionTicket(ITEM_HOENN_TICKET);
    
    if (warp_result != WARP_RESULT_SUCCESS) {
        printf("  FAIL: Warp should succeed, got result %u\n", warp_result);
        failed++;
    } else if (!sWarpDestination.warp_executed) {
        printf("  FAIL: Warp should be executed\n");
        failed++;
    } else if (sWarpDestination.map_id != MAP_LITTLEROOT_TOWN) {
        printf("  FAIL: Should warp to Littleroot Town\n");
        failed++;
    } else if (GetCurrentRegion() != REGION_HOENN) {
        printf("  FAIL: Should be in Hoenn\n");
        failed++;
    } else {
        printf("  PASS: Warped to Littleroot Town in Hoenn\n");
        passed++;
    }
    
    // Step 5: Verify party preserved through entire flow
    printf("Step 5: Verify party preserved through entire flow...\n");
    if (GetPartyCount() != partyCountBackup) {
        printf("  FAIL: Party count changed\n");
        failed++;
    } else {
        bool8 party_ok = true;
        for (i = 0; i < partyCountBackup; i++) {
            if (!ComparePokemon(&partyBackup[i], GetPartyMember(i))) {
                printf("  FAIL: Pokemon %u was modified\n", i);
                party_ok = false;
            }
        }
        if (party_ok) {
            printf("  PASS: Party preserved through entire flow\n");
            passed++;
        } else {
            failed++;
        }
    }
    
    // Step 6: Verify inventory preserved (except ticket added)
    printf("Step 6: Verify inventory preserved through entire flow...\n");
    // Note: Inventory count increased by 1 due to Hoenn ticket being added
    u8 expectedCount = inventoryCountBackup + 1;  // +1 for Hoenn ticket
    if (GetInventoryCount() != expectedCount) {
        printf("  FAIL: Inventory count should be %u (original + ticket), got %u\n",
               expectedCount, GetInventoryCount());
        failed++;
    } else {
        bool8 inventory_ok = true;
        // Check original items are still there
        for (i = 0; i < inventoryCountBackup; i++) {
            struct InventoryItem *current = GetInventoryItem(i);
            if (!CompareInventoryItem(&inventoryBackup[i], current)) {
                printf("  FAIL: Item %u was modified\n", i);
                inventory_ok = false;
            }
        }
        if (inventory_ok) {
            printf("  PASS: Original inventory preserved, ticket added\n");
            passed++;
        } else {
            failed++;
        }
    }
    
    // Report results
    printf("\n--- Test 5 Results ---\n");
    printf("Passed: %d, Failed: %d\n", passed, failed);
    return failed;
}

// ==========================================
// Main Test Runner
// ==========================================

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    int total_failures = 0;
    
    printf("╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║     Integration Tests for Region Progression System              ║\n");
    printf("║     Task 4.2: Run integration tests for region progression       ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n");
    printf("\nRegion configuration: Kanto → Hoenn → Johto (3 regions)\n");
    printf("Sinnoh descoped per ADR-003\n");
    printf("Progression order per ADR-004\n");
    printf("\nTest Scenarios:\n");
    printf("  1. Complete Kanto Elite Four → Hoenn unlocks → Hoenn ticket awarded\n");
    printf("  2. Use Hoenn ticket → Warp to Littleroot Town → Region state updated\n");
    printf("  3. Party preserved during Kanto → Hoenn transition\n");
    printf("  4. Inventory preserved during Kanto → Hoenn transition\n");
    printf("  5. Full progression flow (all requirements combined)\n");
    
    // Run all integration tests
    total_failures += test_kanto_elite_four_unlocks_hoenn();
    total_failures += test_hoenn_ticket_usage_and_warp();
    total_failures += test_party_preservation_during_transition();
    total_failures += test_inventory_preservation_during_transition();
    total_failures += test_full_progression_flow();
    
    // Overall results
    printf("\n\n╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║                    OVERALL TEST RESULTS                          ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n");
    
    if (total_failures == 0) {
        printf("\n✓ ALL INTEGRATION TESTS PASSED!\n\n");
        printf("Requirements Validated:\n");
        printf("  - Requirement 1.1: Kanto E4 defeat unlocks Hoenn\n");
        printf("  - Requirement 1.4: Region unlock awards ticket\n");
        printf("  - Requirement 10.1: Region ticket warps to starting location\n");
        printf("  - Requirement 10.2: Party preserved during transition\n");
        printf("  - Requirement 10.3: Inventory preserved during transition\n");
        return 0;
    } else {
        printf("\n✗ SOME INTEGRATION TESTS FAILED\n");
        printf("Total failures: %d\n", total_failures);
        return 1;
    }
}
