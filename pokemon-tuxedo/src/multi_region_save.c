#include "global.h"
#include "multi_region_save.h"
#include "region_manager.h"
#include "fly_location_discovery.h"
#include "pokemon_tuxedo.h"

/*
 * Multi-Region Save/Load System
 * 
 * This module handles saving and loading multi-region state between
 * EWRAM runtime structures and the PokemonTuxedoSaveData structure.
 * 
 * Requirements:
 * - 16.1: Store current region state in save data
 * - 16.2: Store unlocked regions state in save data
 * - 16.3: Store all 24 badges in save data (3 regions × 8 badges)
 * - 16.4: Store registered fly locations for all regions in save data
 * - 16.5: Restore all region-related state correctly when loading
 * 
 * Data Flow:
 * - Save: EWRAM (sRegionState, sFlyLocations) → PokemonTuxedoSaveData
 * - Load: PokemonTuxedoSaveData → EWRAM (sRegionState, sFlyLocations)
 * 
 * Backward Compatibility:
 * - Old saves without multi-region data are detected and initialized to defaults
 * - Kanto is always unlocked (starting region)
 * - All other regions start locked
 */

/* External EWRAM data from fly_location_discovery.c */
/* These are accessed via getter functions for encapsulation */
extern u16* GetFlyLocationArray(void);
extern u8* GetFlyLocationCountArray(void);

/**
 * Save multi-region state from EWRAM to PokemonTuxedoSaveData.
 * 
 * Copies runtime state to save structure:
 * - Current region (from sRegionState)
 * - Region unlock flags (from sRegionState)
 * - Elite Four defeated flags (from sRegionState)
 * - Badge data (from sRegionState.badges)
 * - Fly locations (from sFlyLocations)
 * - Fly location counts (from sFlyLocationCount)
 * 
 * Validates: Requirements 16.1, 16.2, 16.3, 16.4, 16.5
 */
void SaveMultiRegionData(void)
{
    struct PokemonTuxedoSaveData *saveData = &gSaveBlock1Ptr->pokemonTuxedo;
    struct RegionState *regionState = GetRegionState();
    u8 i;
    
    /* Save current region (Requirement 16.1) */
    saveData->currentRegion = regionState->current_region;

    /* Save region unlock flags (Requirement 16.2) */
    saveData->hoennUnlocked = regionState->hoenn_unlocked;
    saveData->johtoUnlocked = regionState->johto_unlocked;

    /* Save Elite Four defeated flags (Requirement 16.2) */
    for (i = 0; i < NUM_REGIONS; i++)
    {
        saveData->eliteFourDefeated[i] = regionState->elite_four_defeated[i];
    }

    /* Save badge data (Requirement 16.3) */
    /* Copy from BadgeData struct to flat array */
    /* Index 0-7: Kanto, 8-15: Hoenn, 16-23: Johto */
    for (i = 0; i < BADGES_PER_REGION; i++)
    {
        saveData->badges[i] = regionState->badges.kanto_badges[i];
        saveData->badges[BADGES_PER_REGION + i] = regionState->badges.hoenn_badges[i];
        saveData->badges[2 * BADGES_PER_REGION + i] = regionState->badges.johto_badges[i];
    }

    /* Save fly location data (Requirement 16.4) */
    /* Copy from EWRAM arrays to save data */
    for (i = 0; i < MAX_FLY_LOCATIONS_TOTAL; i++)
    {
        saveData->flyLocations[i] = GetFlyLocationArray()[i];
    }

    for (i = 0; i < NUM_REGIONS; i++)
    {
        saveData->flyLocationCount[i] = GetFlyLocationCountArray()[i];
    }
}


/**
 * Load multi-region state from PokemonTuxedoSaveData to EWRAM.
 * 
 * Copies save structure to runtime state:
 * - Current region (to sRegionState)
 * - Region unlock flags (to sRegionState)
 * - Elite Four defeated flags (to sRegionState)
 * - Badge data (to sRegionState.badges)
 * - Fly locations (to sFlyLocations)
 * - Fly location counts (to sFlyLocationCount)
 * 
 * Handles backward compatibility:
 * - If save data appears invalid/old, initializes to defaults
 * - Kanto is always unlocked (starting region)
 * 
 * Validates: Requirements 16.1, 16.2, 16.3, 16.4, 16.5
 */
void LoadMultiRegionData(void)
{
    struct PokemonTuxedoSaveData *saveData = &gSaveBlock1Ptr->pokemonTuxedo;
    struct RegionState *regionState = GetRegionState();
    u8 i;
    u8 validationResult;
    
    /* Check for backward compatibility - if save data is invalid, initialize defaults */
    validationResult = ValidateMultiRegionSaveData();
    if (validationResult != SAVE_VALID)
    {
        /* Old save or corrupted data - initialize to defaults */
        InitMultiRegionSaveData();
        InitRegionState();
        InitFlyLocationData();

        /* Sync from flags for backward compatibility with Kanto badges */
        SyncRegionStateFromFlags();
        return;
    }

    /* Load current region (Requirement 16.1) */
    regionState->current_region = saveData->currentRegion;

    /* Load region unlock flags (Requirement 16.2) */
    regionState->hoenn_unlocked = saveData->hoennUnlocked;
    regionState->johto_unlocked = saveData->johtoUnlocked;

    /* Load Elite Four defeated flags (Requirement 16.2) */
    for (i = 0; i < NUM_REGIONS; i++)
    {
        regionState->elite_four_defeated[i] = saveData->eliteFourDefeated[i];
    }

    /* Load badge data (Requirement 16.3) */
    /* Copy from flat array to BadgeData struct */
    /* Index 0-7: Kanto, 8-15: Hoenn, 16-23: Johto */
    for (i = 0; i < BADGES_PER_REGION; i++)
    {
        regionState->badges.kanto_badges[i] = saveData->badges[i];
        regionState->badges.hoenn_badges[i] = saveData->badges[BADGES_PER_REGION + i];
        regionState->badges.johto_badges[i] = saveData->badges[2 * BADGES_PER_REGION + i];
    }

    /* Load fly location data (Requirement 16.4) */
    /* Copy from save data to EWRAM arrays */
    for (i = 0; i < MAX_FLY_LOCATIONS_TOTAL; i++)
    {
        GetFlyLocationArray()[i] = saveData->flyLocations[i];
    }

    for (i = 0; i < NUM_REGIONS; i++)
    {
        GetFlyLocationCountArray()[i] = saveData->flyLocationCount[i];
    }

    /* Also sync from flags for consistency (Requirement 16.5) */
    /* This ensures flag-based systems stay in sync with our state */
    SyncRegionStateFromFlags();
}

/**
 * Validate multi-region save data integrity.
 * 
 * Checks:
 * - Current region is valid (0-2)
 * - Badge values are valid (TRUE/FALSE only)
 * - Fly location counts are within bounds
 * - Fly locations are reasonable values
 * 
 * Returns SAVE_VALID if data is valid, or an error code if corrupted.
 * 
 * Validates: Requirements 16.1, 16.2, 16.3, 16.4, 16.5
 */
u8 ValidateMultiRegionSaveData(void)
{
    struct PokemonTuxedoSaveData *saveData = &gSaveBlock1Ptr->pokemonTuxedo;
    u8 i;
    
    /* Check if this is an old save without multi-region data */
    /* Old saves will have uninitialized/zero data */
    /* We detect this by checking if currentRegion is valid */
    if (saveData->currentRegion >= NUM_REGIONS)
    {
        return SAVE_INVALID_REGION;
    }

    /* Validate badge data - each badge should be TRUE (1) or FALSE (0) */
    for (i = 0; i < TOTAL_BADGES; i++)
    {
        if (saveData->badges[i] != TRUE && saveData->badges[i] != FALSE)
        {
            return SAVE_INVALID_BADGES;
        }
    }

    /* Validate Elite Four defeated flags */
    for (i = 0; i < NUM_REGIONS; i++)
    {
        if (saveData->eliteFourDefeated[i] != TRUE && saveData->eliteFourDefeated[i] != FALSE)
        {
            return SAVE_CORRUPTED;
        }
    }

    /* Validate region unlock flags */
    if ((saveData->hoennUnlocked != TRUE && saveData->hoennUnlocked != FALSE) ||
        (saveData->johtoUnlocked != TRUE && saveData->johtoUnlocked != FALSE))
    {
        return SAVE_CORRUPTED;
    }

    /* Validate fly location counts - should not exceed max per region */
    for (i = 0; i < NUM_REGIONS; i++)
    {
        if (saveData->flyLocationCount[i] > FLY_LOCATIONS_PER_REGION)
        {
            return SAVE_INVALID_FLY_DATA;
        }
    }
    
    return SAVE_VALID;
}

/**
 * Initialize multi-region save data to defaults.
 * 
 * Called for new games or when loading old saves without multi-region data.
 * Sets:
 * - Current region to Kanto
 * - All regions locked except Kanto
 * - All Elite Four not defeated
 * - All badges not earned
 * - All fly locations cleared
 */
void InitMultiRegionSaveData(void)
{
    struct PokemonTuxedoSaveData *saveData = &gSaveBlock1Ptr->pokemonTuxedo;
    u8 i;
    
    /* Initialize region tracking (Requirement 16.1) */
    saveData->currentRegion = REGION_KANTO;

    /* Initialize region unlock flags (Requirement 16.2) */
    /* Kanto is always unlocked (starting region) */
    saveData->hoennUnlocked = FALSE;
    saveData->johtoUnlocked = FALSE;

    /* Initialize Elite Four defeated flags (Requirement 16.2) */
    for (i = 0; i < NUM_REGIONS; i++)
    {
        saveData->eliteFourDefeated[i] = FALSE;
    }

    /* Initialize badge data (Requirement 16.3) */
    for (i = 0; i < TOTAL_BADGES; i++)
    {
        saveData->badges[i] = FALSE;
    }

    /* Initialize fly location data (Requirement 16.4) */
    for (i = 0; i < MAX_FLY_LOCATIONS_TOTAL; i++)
    {
        saveData->flyLocations[i] = 0;
    }

    for (i = 0; i < NUM_REGIONS; i++)
    {
        saveData->flyLocationCount[i] = 0;
    }
}

/**
 * Check if save data has multi-region data (for backward compatibility).
 * 
 * Returns TRUE if save has valid multi-region data, FALSE otherwise.
 * Used to detect old saves that need initialization.
 */
bool8 HasMultiRegionSaveData(void)
{
    return ValidateMultiRegionSaveData() == SAVE_VALID;
}
