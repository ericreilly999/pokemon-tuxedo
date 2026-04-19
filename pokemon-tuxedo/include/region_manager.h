#ifndef GUARD_REGION_MANAGER_H
#define GUARD_REGION_MANAGER_H

#include "global.h"
#include "constants/items.h"

// Region IDs
#define REGION_KANTO  0
#define REGION_HOENN  1
#define REGION_JOHTO  2

// Number of regions (Sinnoh descoped per ADR-003)
#define NUM_REGIONS   3

// Badges per region
#define BADGES_PER_REGION  8
#define TOTAL_BADGES       (NUM_REGIONS * BADGES_PER_REGION)  // 24 badges

// Region ticket item IDs are defined in constants/items.h
// ITEM_HOENN_TICKET, ITEM_JOHTO_TICKET

// Badge tracking structure for multi-region support
// Stores badge status for all 3 regions (24 badges total)
struct BadgeData {
    bool8 kanto_badges[BADGES_PER_REGION];
    bool8 hoenn_badges[BADGES_PER_REGION];
    bool8 johto_badges[BADGES_PER_REGION];
};

// Region state tracking structure
// Tracks current region, unlock status, and Elite Four completion
struct RegionState {
    u8 current_region;
    bool8 hoenn_unlocked;
    bool8 johto_unlocked;
    bool8 elite_four_defeated[NUM_REGIONS];
    struct BadgeData badges;
};

// Region tracking functions
u8 GetCurrentRegion(void);
void SetCurrentRegion(u8 region_id);

// Badge tracking - per region
u8 GetRegionBadgeCount(u8 region_id);

// Badge tracking - cross region
u8 GetTotalBadgeCount(void);

// Badge management
bool8 HasBadge(u8 region_id, u8 badge_index);
void AwardBadge(u8 region_id, u8 badge_index);

// Legacy badge count function (Kanto only, for backward compatibility)
u8 GetBadgeCount(void);

// Elite Four tracking
bool8 IsEliteFourDefeated(u8 region_id);
void SetEliteFourDefeated(u8 region_id);

// Elite Four defeat processing - checks defeat and unlocks next region
// Returns TRUE if a new region was unlocked, FALSE otherwise
bool8 CheckEliteFourDefeatAndUnlock(u8 region_id);

// Region unlock status
bool8 IsRegionUnlocked(u8 region_id);
void UnlockRegion(u8 region_id);

// Region starting level lookup
u8 GetRegionStartingLevel(u8 region_id);

// Region travel validation
bool8 CanTravelToRegion(u8 target_region);
u16 GetRegionTicketItem(u8 region_id);

// Region ticket rewards
void AwardRegionTicket(void);

// Region transition system (Task 3.11)
// Warp data structure for region starting locations
struct RegionWarpData {
    s8 mapGroup;
    s8 mapNum;
    s8 warpId;
    s8 x;
    s8 y;
};

// Get the starting warp location for a region
// Returns warp data for the region's starting town
// Kanto: Pallet Town, Hoenn: Littleroot Town, Johto: New Bark Town
void GetRegionStartLocation(u8 region_id, struct RegionWarpData *warp_data);

// Check if player can transition to a target region
// Returns TRUE if region is unlocked and player has the ticket
bool8 CanTransitionToRegion(u8 target_region);

// Execute a region transition
// Updates current region state and returns TRUE on success
// Party and inventory are preserved (stored in save data)
bool8 TransitionToRegion(u8 target_region);

// Handle region ticket usage from bag menu
// Returns TRUE if warp was initiated, FALSE if region is locked
bool8 UseRegionTicket(u16 ticket_item);

// Get the region ID for a ticket item
// Returns NUM_REGIONS if ticket is invalid
u8 GetRegionForTicket(u16 ticket_item);

/* Get the global region state (for save/load) */
struct RegionState* GetRegionState(void);

/* Initialize region state (for new game) */
void InitRegionState(void);

/* Sync region state from saved flags (for loading saved games) */
void SyncRegionStateFromFlags(void);

/* ===========================================================
   Starter Distribution System (DEV-027 / Req 14)
   After each E4 defeat, unchosen starters become available.
   =========================================================== */

/* Flag IDs for post-E4 starter distribution (using unused special flags) */
/* These flags gate the distribution NPC dialogue */
#define FLAG_KANTO_STARTERS_AVAILABLE    (SPECIAL_FLAGS_START + 0x10)
#define FLAG_HOENN_STARTERS_AVAILABLE    (SPECIAL_FLAGS_START + 0x11)
#define FLAG_JOHTO_STARTERS_AVAILABLE    (SPECIAL_FLAGS_START + 0x12)

/* Kanto starter species IDs */
#define SPECIES_KANTO_STARTER_1  1    /* Bulbasaur */
#define SPECIES_KANTO_STARTER_2  4    /* Charmander */
#define SPECIES_KANTO_STARTER_3  7    /* Squirtle */

/* Hoenn starter species IDs */
#define SPECIES_HOENN_STARTER_1  252  /* Treecko */
#define SPECIES_HOENN_STARTER_2  255  /* Torchic */
#define SPECIES_HOENN_STARTER_3  258  /* Mudkip */

/* Johto starter species IDs */
#define SPECIES_JOHTO_STARTER_1  152  /* Chikorita */
#define SPECIES_JOHTO_STARTER_2  155  /* Cyndaquil */
#define SPECIES_JOHTO_STARTER_3  158  /* Totodile */

/* Record the chosen Kanto starter (called from oak_speech.c / new_game.c) */
void SetChosenKantoStarter(u16 species);

/* Get the chosen Kanto starter species */
u16 GetChosenKantoStarter(void);

/* Trigger starter distribution for a region's E4 defeat.
   Sets the appropriate availability flag so NPCs can offer the starters. */
void TriggerStarterDistribution(u8 region_id);

/* Check if a given Kanto starter species is available post-E4.
   The chosen starter is excluded from the pool; the other two are available. */
bool8 IsKantoStarterAvailable(u16 species);

#endif /* GUARD_REGION_MANAGER_H */
