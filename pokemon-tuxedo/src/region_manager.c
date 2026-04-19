#include "global.h"
#include "region_manager.h"
#include "event_data.h"
#include "item.h"
#include "overworld.h"
#include "constants/flags.h"
#include "constants/maps.h"

/* Placeholder for MAP_NEW_BARK_TOWN until Johto maps are integrated.
   This will be replaced by the auto-generated constant when Johto is added. */
#ifndef MAP_NEW_BARK_TOWN
#define MAP_NEW_BARK_TOWN MAP_UNDEFINED
#endif

/* Region starting levels (from requirements).
   Updated for new progression: Kanto -> Hoenn -> Johto (ADR-004). */
static const u8 sRegionStartingLevels[] = {
    2,    /* REGION_KANTO */
    60,   /* REGION_HOENN (unlocked after Kanto E4) */
    110   /* REGION_JOHTO (unlocked after Hoenn E4) */
};

/* Global region state - stored in EWRAM for save data integration */
EWRAM_DATA static struct RegionState sRegionState = {0};

/* DEV-027 / Req 14: Chosen Kanto starter species, set during Oak speech / new game.
   Stored in EWRAM; persisted via save data.  0 = not yet chosen. */
EWRAM_DATA static u16 sChosenKantoStarter = 0;

/**
 * Initialize region state for a new game.
 * Sets Kanto as current region, all other regions locked.
 */
void InitRegionState(void)
{
    u8 i, j;

    sRegionState.current_region = REGION_KANTO;
    sRegionState.hoenn_unlocked = FALSE;
    sRegionState.johto_unlocked = FALSE;

    /* Clear Elite Four defeated flags */
    for (i = 0; i < NUM_REGIONS; i++)
    {
        sRegionState.elite_four_defeated[i] = FALSE;
    }

    /* Clear all badges */
    for (j = 0; j < BADGES_PER_REGION; j++)
    {
        sRegionState.badges.kanto_badges[j] = FALSE;
        sRegionState.badges.hoenn_badges[j] = FALSE;
        sRegionState.badges.johto_badges[j] = FALSE;
    }
}

/**
 * Sync region state from saved flags.
 * Called when loading a saved game to restore state from flags.
 */
void SyncRegionStateFromFlags(void)
{
    /* Sync Kanto badges */
    sRegionState.badges.kanto_badges[0] = FlagGet(FLAG_BADGE01_GET);
    sRegionState.badges.kanto_badges[1] = FlagGet(FLAG_BADGE02_GET);
    sRegionState.badges.kanto_badges[2] = FlagGet(FLAG_BADGE03_GET);
    sRegionState.badges.kanto_badges[3] = FlagGet(FLAG_BADGE04_GET);
    sRegionState.badges.kanto_badges[4] = FlagGet(FLAG_BADGE05_GET);
    sRegionState.badges.kanto_badges[5] = FlagGet(FLAG_BADGE06_GET);
    sRegionState.badges.kanto_badges[6] = FlagGet(FLAG_BADGE07_GET);
    sRegionState.badges.kanto_badges[7] = FlagGet(FLAG_BADGE08_GET);

    /* Sync Hoenn badges */
    sRegionState.badges.hoenn_badges[0] = FlagGet(FLAG_HOENN_BADGE01_GET);
    sRegionState.badges.hoenn_badges[1] = FlagGet(FLAG_HOENN_BADGE02_GET);
    sRegionState.badges.hoenn_badges[2] = FlagGet(FLAG_HOENN_BADGE03_GET);
    sRegionState.badges.hoenn_badges[3] = FlagGet(FLAG_HOENN_BADGE04_GET);
    sRegionState.badges.hoenn_badges[4] = FlagGet(FLAG_HOENN_BADGE05_GET);
    sRegionState.badges.hoenn_badges[5] = FlagGet(FLAG_HOENN_BADGE06_GET);
    sRegionState.badges.hoenn_badges[6] = FlagGet(FLAG_HOENN_BADGE07_GET);
    sRegionState.badges.hoenn_badges[7] = FlagGet(FLAG_HOENN_BADGE08_GET);

    /* Sync Johto badges */
    sRegionState.badges.johto_badges[0] = FlagGet(FLAG_JOHTO_BADGE01_GET);
    sRegionState.badges.johto_badges[1] = FlagGet(FLAG_JOHTO_BADGE02_GET);
    sRegionState.badges.johto_badges[2] = FlagGet(FLAG_JOHTO_BADGE03_GET);
    sRegionState.badges.johto_badges[3] = FlagGet(FLAG_JOHTO_BADGE04_GET);
    sRegionState.badges.johto_badges[4] = FlagGet(FLAG_JOHTO_BADGE05_GET);
    sRegionState.badges.johto_badges[5] = FlagGet(FLAG_JOHTO_BADGE06_GET);
    sRegionState.badges.johto_badges[6] = FlagGet(FLAG_JOHTO_BADGE07_GET);
    sRegionState.badges.johto_badges[7] = FlagGet(FLAG_JOHTO_BADGE08_GET);

    /* Sync Elite Four defeated flags */
    sRegionState.elite_four_defeated[REGION_KANTO] = FlagGet(FLAG_KANTO_ELITE_FOUR_DEFEATED);
    sRegionState.elite_four_defeated[REGION_HOENN] = FlagGet(FLAG_HOENN_ELITE_FOUR_DEFEATED);
    sRegionState.elite_four_defeated[REGION_JOHTO] = FlagGet(FLAG_JOHTO_ELITE_FOUR_DEFEATED);

    /* Sync region unlock flags */
    sRegionState.hoenn_unlocked = FlagGet(FLAG_HOENN_UNLOCKED);
    sRegionState.johto_unlocked = FlagGet(FLAG_JOHTO_UNLOCKED);

    /* B4: Restore chosen Kanto starter from save block on every load. */
    sChosenKantoStarter = gSaveBlock2Ptr->chosenKantoStarter;
}

/**
 * Get pointer to the global region state.
 * Used for save/load operations.
 */
struct RegionState* GetRegionState(void)
{
    return &sRegionState;
}

/**
 * Get the current region the player is in.
 */
u8 GetCurrentRegion(void)
{
    return sRegionState.current_region;
}

/**
 * Set the current region.
 * @param region_id The region to set as current (REGION_KANTO, REGION_HOENN, or REGION_JOHTO)
 */
void SetCurrentRegion(u8 region_id)
{
    if (region_id < NUM_REGIONS)
        sRegionState.current_region = region_id;
}

/**
 * Get badge count for a specific region.
 * @param region_id The region to count badges for
 * @return Number of badges earned in that region (0-8)
 */
u8 GetRegionBadgeCount(u8 region_id)
{
    u8 count;
    u8 i;
    bool8 *badges;

    count = 0;

    switch (region_id)
    {
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

    for (i = 0; i < BADGES_PER_REGION; i++)
    {
        if (badges[i])
            count++;
    }

    return count;
}

/**
 * Get total badge count across all regions.
 * @return Total number of badges earned (0-24)
 */
u8 GetTotalBadgeCount(void)
{
    return GetRegionBadgeCount(REGION_KANTO) +
           GetRegionBadgeCount(REGION_HOENN) +
           GetRegionBadgeCount(REGION_JOHTO);
}

/**
 * Legacy function for backward compatibility.
 * Returns Kanto badge count using the original flag-based system.
 */
u8 GetBadgeCount(void)
{
    u8 count;

    count = 0;

    /* Count Kanto badges using original flags for backward compatibility */
    if (FlagGet(FLAG_BADGE01_GET)) count++;
    if (FlagGet(FLAG_BADGE02_GET)) count++;
    if (FlagGet(FLAG_BADGE03_GET)) count++;
    if (FlagGet(FLAG_BADGE04_GET)) count++;
    if (FlagGet(FLAG_BADGE05_GET)) count++;
    if (FlagGet(FLAG_BADGE06_GET)) count++;
    if (FlagGet(FLAG_BADGE07_GET)) count++;
    if (FlagGet(FLAG_BADGE08_GET)) count++;

    return count;
}

/**
 * Check if a specific badge has been earned.
 * @param region_id The region the badge belongs to
 * @param badge_index The badge index (0-7)
 * @return TRUE if the badge has been earned, FALSE otherwise
 */
bool8 HasBadge(u8 region_id, u8 badge_index)
{
    if (badge_index >= BADGES_PER_REGION)
        return FALSE;

    switch (region_id)
    {
    case REGION_KANTO:
        return sRegionState.badges.kanto_badges[badge_index];
    case REGION_HOENN:
        return sRegionState.badges.hoenn_badges[badge_index];
    case REGION_JOHTO:
        return sRegionState.badges.johto_badges[badge_index];
    default:
        return FALSE;
    }
}

/**
 * Award a badge to the player.
 * @param region_id The region the badge belongs to
 * @param badge_index The badge index (0-7)
 */
void AwardBadge(u8 region_id, u8 badge_index)
{
    if (badge_index >= BADGES_PER_REGION)
        return;

    switch (region_id)
    {
    case REGION_KANTO:
        sRegionState.badges.kanto_badges[badge_index] = TRUE;
        /* Also set the original flag for backward compatibility */
        switch (badge_index)
        {
        case 0: FlagSet(FLAG_BADGE01_GET); break;
        case 1: FlagSet(FLAG_BADGE02_GET); break;
        case 2: FlagSet(FLAG_BADGE03_GET); break;
        case 3: FlagSet(FLAG_BADGE04_GET); break;
        case 4: FlagSet(FLAG_BADGE05_GET); break;
        case 5: FlagSet(FLAG_BADGE06_GET); break;
        case 6: FlagSet(FLAG_BADGE07_GET); break;
        case 7: FlagSet(FLAG_BADGE08_GET); break;
        }
        break;
    case REGION_HOENN:
        sRegionState.badges.hoenn_badges[badge_index] = TRUE;
        /* Set Hoenn badge flags for persistence */
        switch (badge_index)
        {
        case 0: FlagSet(FLAG_HOENN_BADGE01_GET); break;
        case 1: FlagSet(FLAG_HOENN_BADGE02_GET); break;
        case 2: FlagSet(FLAG_HOENN_BADGE03_GET); break;
        case 3: FlagSet(FLAG_HOENN_BADGE04_GET); break;
        case 4: FlagSet(FLAG_HOENN_BADGE05_GET); break;
        case 5: FlagSet(FLAG_HOENN_BADGE06_GET); break;
        case 6: FlagSet(FLAG_HOENN_BADGE07_GET); break;
        case 7: FlagSet(FLAG_HOENN_BADGE08_GET); break;
        }
        break;
    case REGION_JOHTO:
        sRegionState.badges.johto_badges[badge_index] = TRUE;
        /* Set Johto badge flags for persistence */
        switch (badge_index)
        {
        case 0: FlagSet(FLAG_JOHTO_BADGE01_GET); break;
        case 1: FlagSet(FLAG_JOHTO_BADGE02_GET); break;
        case 2: FlagSet(FLAG_JOHTO_BADGE03_GET); break;
        case 3: FlagSet(FLAG_JOHTO_BADGE04_GET); break;
        case 4: FlagSet(FLAG_JOHTO_BADGE05_GET); break;
        case 5: FlagSet(FLAG_JOHTO_BADGE06_GET); break;
        case 6: FlagSet(FLAG_JOHTO_BADGE07_GET); break;
        case 7: FlagSet(FLAG_JOHTO_BADGE08_GET); break;
        }
        break;
    }
}

/**
 * Check if the Elite Four has been defeated in a region.
 * @param region_id The region to check
 * @return TRUE if Elite Four defeated, FALSE otherwise
 */
bool8 IsEliteFourDefeated(u8 region_id)
{
    if (region_id >= NUM_REGIONS)
        return FALSE;

    return sRegionState.elite_four_defeated[region_id];
}

/**
 * Mark the Elite Four as defeated in a region.
 * @param region_id The region where Elite Four was defeated
 */
void SetEliteFourDefeated(u8 region_id)
{
    if (region_id >= NUM_REGIONS)
        return;

    sRegionState.elite_four_defeated[region_id] = TRUE;

    /* Set flag for persistence */
    switch (region_id)
    {
    case REGION_KANTO:
        FlagSet(FLAG_KANTO_ELITE_FOUR_DEFEATED);
        break;
    case REGION_HOENN:
        FlagSet(FLAG_HOENN_ELITE_FOUR_DEFEATED);
        break;
    case REGION_JOHTO:
        FlagSet(FLAG_JOHTO_ELITE_FOUR_DEFEATED);
        break;
    }
}

/**
 * Check Elite Four defeat and unlock the next region.
 * This is the main entry point for processing Elite Four defeat.
 * Should be called when the player defeats the Champion.
 *
 * Progression sequence (per ADR-004):
 *   Kanto E4 defeat -> Hoenn unlock
 *   Hoenn E4 defeat -> Johto unlock
 *   Johto E4 defeat -> No next region (Sinnoh descoped per ADR-003)
 *
 * @param region_id The region where Elite Four was defeated
 * @return TRUE if a new region was unlocked, FALSE otherwise
 */
bool8 CheckEliteFourDefeatAndUnlock(u8 region_id)
{
    bool8 region_unlocked;

    region_unlocked = FALSE;

    /* Validate region ID */
    if (region_id >= NUM_REGIONS)
        return FALSE;

    /* Check if already defeated (no need to process again) */
    if (IsEliteFourDefeated(region_id))
        return FALSE;

    /* Mark Elite Four as defeated */
    SetEliteFourDefeated(region_id);

    /* DEV-027 / Req 14: Trigger post-E4 starter distribution for this region. */
    TriggerStarterDistribution(region_id);

    /* Unlock next region based on defeated region (ADR-004 sequence) */
    switch (region_id)
    {
    case REGION_KANTO:
        /* Kanto E4 defeated -> Unlock Hoenn */
        if (!IsRegionUnlocked(REGION_HOENN))
        {
            UnlockRegion(REGION_HOENN);
            region_unlocked = TRUE;
        }
        break;
    case REGION_HOENN:
        /* Hoenn E4 defeated -> Unlock Johto */
        if (!IsRegionUnlocked(REGION_JOHTO))
        {
            UnlockRegion(REGION_JOHTO);
            region_unlocked = TRUE;
        }
        break;
    case REGION_JOHTO:
        /* No next region after Johto (Sinnoh descoped per ADR-003) */
        break;
    }

    return region_unlocked;
}

/**
 * Check if a region is unlocked for travel.
 * @param region_id The region to check
 * @return TRUE if the region is unlocked, FALSE otherwise
 */
bool8 IsRegionUnlocked(u8 region_id)
{
    switch (region_id)
    {
    case REGION_KANTO:
        /* Kanto is always unlocked (starting region) */
        return TRUE;
    case REGION_HOENN:
        return sRegionState.hoenn_unlocked;
    case REGION_JOHTO:
        return sRegionState.johto_unlocked;
    default:
        return FALSE;
    }
}

/**
 * Unlock a region for travel.
 * Also awards the corresponding region ticket.
 * @param region_id The region to unlock
 */
void UnlockRegion(u8 region_id)
{
    u16 ticket_item;

    switch (region_id)
    {
    case REGION_HOENN:
        if (!sRegionState.hoenn_unlocked)
        {
            sRegionState.hoenn_unlocked = TRUE;
            FlagSet(FLAG_HOENN_UNLOCKED);
            ticket_item = GetRegionTicketItem(REGION_HOENN);
            if (ticket_item != ITEM_NONE)
                AddBagItem(ticket_item, 1);
        }
        break;
    case REGION_JOHTO:
        if (!sRegionState.johto_unlocked)
        {
            sRegionState.johto_unlocked = TRUE;
            FlagSet(FLAG_JOHTO_UNLOCKED);
            ticket_item = GetRegionTicketItem(REGION_JOHTO);
            if (ticket_item != ITEM_NONE)
                AddBagItem(ticket_item, 1);
        }
        break;
    case REGION_KANTO:
        /* Kanto is always unlocked, nothing to do */
        break;
    }
}

/**
 * Get the starting level for a region.
 * @param region_id The region to get the starting level for
 * @return The starting level for wild Pokemon in that region
 */
u8 GetRegionStartingLevel(u8 region_id)
{
    if (region_id >= NUM_REGIONS)
        return 2;

    return sRegionStartingLevels[region_id];
}

/**
 * Get the ticket item for a region.
 * @param region_id The region to get the ticket for
 * @return The item ID of the region ticket, or ITEM_NONE if invalid
 */
u16 GetRegionTicketItem(u8 region_id)
{
    switch (region_id)
    {
    case REGION_HOENN:
        return ITEM_HOENN_TICKET;
    case REGION_JOHTO:
        return ITEM_JOHTO_TICKET;
    default:
        return ITEM_NONE;
    }
}

/**
 * Check if the player can travel to a target region.
 * @param target_region The region to check travel eligibility for
 * @return TRUE if travel is allowed, FALSE otherwise
 */
bool8 CanTravelToRegion(u8 target_region)
{
    u16 ticket_item;

    /* Can always stay in current region */
    if (target_region == sRegionState.current_region)
        return TRUE;

    /* Kanto is always accessible (starting region) */
    if (target_region == REGION_KANTO)
        return TRUE;

    /* Check if region is unlocked */
    if (!IsRegionUnlocked(target_region))
        return FALSE;

    /* Check if player has the ticket */
    ticket_item = GetRegionTicketItem(target_region);
    if (ticket_item == ITEM_NONE)
        return FALSE;

    return CheckBagHasItem(ticket_item, 1);
}

/**
 * Award the next region ticket based on current region and Elite Four status.
 * Called after defeating Elite Four.
 * Progression: Kanto E4 -> Hoenn Ticket, Hoenn E4 -> Johto Ticket (ADR-004)
 */
void AwardRegionTicket(void)
{
    /* Only award ticket if Elite Four defeated in current region */
    if (!IsEliteFourDefeated(sRegionState.current_region))
        return;

    /* Unlock next region based on current region */
    switch (sRegionState.current_region)
    {
    case REGION_KANTO:
        /* Kanto E4 defeated -> Unlock Hoenn */
        UnlockRegion(REGION_HOENN);
        break;
    case REGION_HOENN:
        /* Hoenn E4 defeated -> Unlock Johto */
        UnlockRegion(REGION_JOHTO);
        break;
    case REGION_JOHTO:
        /* No next region after Johto (Sinnoh descoped per ADR-003) */
        break;
    }
}


/* ==========================================
   Region Transition System (Task 3.11)
   Implements: Requirements 10.1, 10.2, 10.3, 10.4, 10.5
   ========================================== */

/**
 * Get the region ID for a ticket item.
 * @param ticket_item The ticket item ID
 * @return The region ID, or NUM_REGIONS if invalid
 *
 * Validates: Requirements 10.1
 */
u8 GetRegionForTicket(u16 ticket_item)
{
    switch (ticket_item)
    {
    case ITEM_HOENN_TICKET:
        return REGION_HOENN;
    case ITEM_JOHTO_TICKET:
        return REGION_JOHTO;
    default:
        return NUM_REGIONS;  /* Invalid ticket */
    }
}

/**
 * Get the starting warp location for a region.
 * Returns the warp data for the region's starting town.
 *
 * Starting locations:
 *   Kanto: Pallet Town (center of town)
 *   Hoenn: Littleroot Town (center of town)
 *   Johto: New Bark Town (center of town)
 *
 * @param region_id The region to get the starting location for
 * @param warp_data Output parameter for the warp data
 *
 * Validates: Requirements 10.1
 */
void GetRegionStartLocation(u8 region_id, struct RegionWarpData *warp_data)
{
    if (warp_data == NULL)
        return;

    /* Default to invalid warp */
    warp_data->mapGroup = -1;
    warp_data->mapNum = -1;
    warp_data->warpId = -1;
    warp_data->x = 0;
    warp_data->y = 0;

    switch (region_id)
    {
    case REGION_KANTO:
        /* Pallet Town - center of town */
        warp_data->mapGroup = MAP_GROUP(MAP_PALLET_TOWN);
        warp_data->mapNum = MAP_NUM(MAP_PALLET_TOWN);
        warp_data->warpId = WARP_ID_NONE;
        warp_data->x = 9;
        warp_data->y = 8;
        break;
    case REGION_HOENN:
        /* Littleroot Town - center of town */
        warp_data->mapGroup = MAP_GROUP(MAP_LITTLEROOT_TOWN);
        warp_data->mapNum = MAP_NUM(MAP_LITTLEROOT_TOWN);
        warp_data->warpId = WARP_ID_NONE;
        warp_data->x = 10;
        warp_data->y = 10;
        break;
    case REGION_JOHTO:
        /* New Bark Town - center of town (stub - Johto not yet integrated).
           Using placeholder coordinates until Johto maps are available. */
        warp_data->mapGroup = MAP_GROUP(MAP_NEW_BARK_TOWN);
        warp_data->mapNum = MAP_NUM(MAP_NEW_BARK_TOWN);
        warp_data->warpId = WARP_ID_NONE;
        warp_data->x = 10;
        warp_data->y = 10;
        break;
    }
}

/**
 * Check if player can transition to a target region.
 * Validates that the region is unlocked before allowing transition.
 *
 * @param target_region The region to check
 * @return TRUE if transition is allowed, FALSE otherwise
 *
 * Validates: Requirements 10.5
 */
bool8 CanTransitionToRegion(u8 target_region)
{
    /* Validate region ID */
    if (target_region >= NUM_REGIONS)
        return FALSE;

    /* Kanto is always accessible (starting region) */
    if (target_region == REGION_KANTO)
        return TRUE;

    /* Check if region is unlocked */
    return IsRegionUnlocked(target_region);
}

/**
 * Execute a region transition.
 * Updates the Region_Manager current region state.
 * Party and inventory are preserved automatically (stored in save data).
 *
 * @param target_region The region to transition to
 * @return TRUE if transition was successful, FALSE otherwise
 *
 * Validates: Requirements 10.2, 10.3, 10.4
 */
bool8 TransitionToRegion(u8 target_region)
{
    /* Validate transition is allowed */
    if (!CanTransitionToRegion(target_region))
        return FALSE;

    /* Update current region state (Requirement 10.4) */
    SetCurrentRegion(target_region);

    /* Party is preserved automatically - stored in gPlayerParty (save data).
       Inventory is preserved automatically - stored in gSaveBlock1Ptr->bagPocket (save data).
       No explicit action needed for Requirements 10.2 and 10.3. */

    return TRUE;
}

/**
 * Handle region ticket usage.
 * This is the main entry point for using a region ticket.
 *
 * If the region is unlocked:
 *   - Updates current region state
 *   - Returns TRUE to indicate warp should be initiated
 *
 * If the region is locked:
 *   - Returns FALSE to indicate warp should not occur
 *   - Caller should display appropriate error message
 *
 * @param ticket_item The ticket item ID being used
 * @return TRUE if warp should be initiated, FALSE if region is locked
 *
 * Validates: Requirements 10.1, 10.5
 */
bool8 UseRegionTicket(u16 ticket_item)
{
    u8 target_region;

    /* Get the target region for this ticket */
    target_region = GetRegionForTicket(ticket_item);

    /* Validate ticket */
    if (target_region >= NUM_REGIONS)
        return FALSE;

    /* Check if region is unlocked (Requirement 10.5) */
    if (!CanTransitionToRegion(target_region))
        return FALSE;

    /* Execute the transition (updates region state) */
    return TransitionToRegion(target_region);
}


/* ===========================================================
   Starter Distribution System (DEV-027 / Req 14)
   =========================================================== */

/*
 * Record the species of the Kanto starter chosen by the player.
 * Called from oak_speech.c / new_game.c immediately after the player
 * picks a starter from Professor Oak.
 */
void SetChosenKantoStarter(u16 species)
{
    sChosenKantoStarter = species;
    /* B4: Persist to save block so value survives power cycles. */
    gSaveBlock2Ptr->chosenKantoStarter = species;
}

/*
 * Return the species of the chosen Kanto starter.
 * Returns 0 if no starter has been chosen yet.
 */
u16 GetChosenKantoStarter(void)
{
    return sChosenKantoStarter;
}

/*
 * Set the availability flag for a region's post-E4 starter distribution.
 * This flag gates the distribution NPC dialogue so that NPCs only offer
 * starters after the player has cleared the corresponding Elite Four.
 *
 * For Kanto: sets FLAG_KANTO_STARTERS_AVAILABLE
 * For Hoenn: sets FLAG_HOENN_STARTERS_AVAILABLE
 * For Johto: sets FLAG_JOHTO_STARTERS_AVAILABLE
 *
 * Called automatically by CheckEliteFourDefeatAndUnlock().
 */
void TriggerStarterDistribution(u8 region_id)
{
    switch (region_id)
    {
    case REGION_KANTO:
        FlagSet(FLAG_KANTO_STARTERS_AVAILABLE);
        break;
    case REGION_HOENN:
        FlagSet(FLAG_HOENN_STARTERS_AVAILABLE);
        break;
    case REGION_JOHTO:
        FlagSet(FLAG_JOHTO_STARTERS_AVAILABLE);
        break;
    default:
        break;
    }
}

/*
 * Check whether a given Kanto starter species is available for distribution.
 *
 * Availability rules:
 *   1. The Kanto starters availability flag must be set (E4 must be defeated).
 *   2. The species must be one of the three Kanto starters.
 *   3. The species must NOT be the one the player originally chose.
 *
 * The player's chosen starter is excluded so they cannot obtain a second copy
 * through this system.
 *
 * Returns TRUE if the species is available, FALSE otherwise.
 */
bool8 IsKantoStarterAvailable(u16 species)
{
    /* E4 must be defeated first */
    if (!FlagGet(FLAG_KANTO_STARTERS_AVAILABLE))
        return FALSE;

    /* Species must be a Kanto starter but not the one originally chosen */
    if (species != SPECIES_KANTO_STARTER_1
     && species != SPECIES_KANTO_STARTER_2
     && species != SPECIES_KANTO_STARTER_3)
        return FALSE;

    /* Exclude the originally chosen starter */
    if (species == sChosenKantoStarter)
        return FALSE;

    return TRUE;
}
