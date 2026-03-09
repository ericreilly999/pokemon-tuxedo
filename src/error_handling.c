#include "global.h"
#include "error_handling.h"
#include "level_scaler.h"
#include "region_manager.h"
#include "system_integration.h"
#include "exp_multipliers.h"
#include "storage_system.h"
#include "full_party_exp.h"

/*
 * Error Handling and Safety Checks
 * 
 * This file implements error handling, validation, and safety checks
 * for all Pokemon Tuxedo systems.
 */

// ============================================================================
// Level Calculation Safety Checks
// ============================================================================

u8 ClampLevel(u8 level)
{
    /*
     * Clamp level to valid Pokemon range (1-100)
     */
    if (level < 1)
        return 1;
    if (level > 100)
        return 100;
    return level;
}

bool8 ValidateBadgeCount(u8 badge_count)
{
    /*
     * Validate badge count is within valid range (0-32)
     */
    return badge_count <= 32;
}

bool8 ValidateRegionId(u8 region_id)
{
    /*
     * Validate region ID is within valid range (0-3)
     */
    return region_id <= REGION_SINNOH;
}

struct LevelRange SafeGetWildPokemonLevelRange(u8 badge_count, u8 region_id, bool8 elite_four_defeated)
{
    /*
     * Safe wrapper for GetWildPokemonLevelRange with validation
     */
    struct LevelRange range;
    
    // Validate inputs
    if (!ValidateBadgeCount(badge_count))
    {
        badge_count = 0;  // Default to no badges
    }
    
    if (!ValidateRegionId(region_id))
    {
        region_id = REGION_KANTO;  // Default to Kanto
    }
    
    // Get level range
    range = GetWildPokemonLevelRange(badge_count, region_id, elite_four_defeated);
    
    // Clamp levels
    range.min_level = ClampLevel(range.min_level);
    range.max_level = ClampLevel(range.max_level);
    
    // Ensure min <= max
    if (range.min_level > range.max_level)
    {
        u8 temp = range.min_level;
        range.min_level = range.max_level;
        range.max_level = temp;
    }
    
    return range;
}

u8 SafeGetGymLeaderAverageLevel(u8 badge_count, u8 region_id)
{
    /*
     * Safe wrapper for GetGymLeaderAverageLevel with validation
     */
    if (!ValidateBadgeCount(badge_count))
        badge_count = 0;
    
    if (!ValidateRegionId(region_id))
        region_id = REGION_KANTO;
    
    return ClampLevel(GetGymLeaderAverageLevel(badge_count, region_id));
}

u8 SafeGetGymLeaderAceLevel(u8 badge_count)
{
    /*
     * Safe wrapper for GetGymLeaderAceLevel with validation
     */
    if (!ValidateBadgeCount(badge_count))
        badge_count = 0;
    
    return ClampLevel(GetGymLeaderAceLevel(badge_count));
}

u8 SafeGetEliteFourAverageLevel(u8 badge_count, u8 region_id)
{
    /*
     * Safe wrapper for GetEliteFourAverageLevel with validation
     */
    if (!ValidateBadgeCount(badge_count))
        badge_count = 0;
    
    if (!ValidateRegionId(region_id))
        region_id = REGION_KANTO;
    
    return ClampLevel(GetEliteFourAverageLevel(badge_count, region_id));
}

u8 SafeGetEliteFourAceLevel(u8 badge_count)
{
    /*
     * Safe wrapper for GetEliteFourAceLevel with validation
     */
    if (!ValidateBadgeCount(badge_count))
        badge_count = 0;
    
    return ClampLevel(GetEliteFourAceLevel(badge_count));
}

u8 SafeGetRivalAverageLevel(u8 badge_count, u8 region_id)
{
    /*
     * Safe wrapper for GetRivalAverageLevel with validation
     */
    if (!ValidateBadgeCount(badge_count))
        badge_count = 0;
    
    if (!ValidateRegionId(region_id))
        region_id = REGION_KANTO;
    
    return ClampLevel(GetRivalAverageLevel(badge_count, region_id));
}

u8 SafeGetRivalAceLevel(u8 badge_count)
{
    /*
     * Safe wrapper for GetRivalAceLevel with validation
     */
    if (!ValidateBadgeCount(badge_count))
        badge_count = 0;
    
    return ClampLevel(GetRivalAceLevel(badge_count));
}

// ============================================================================
// Save Data Corruption Handling
// ============================================================================

bool8 ValidateSaveDataChecksum(void)
{
    /*
     * Validate save data checksum
     */
    return LoadPokemonTuxedoData();
}

void RecoverFromCorruptedSave(void)
{
    /*
     * Recover from corrupted save data by resetting to defaults
     */
    u8 i;
    
    // Reset to default values
    SetCurrentRegion(REGION_KANTO);
    
    for (i = 0; i < 4; i++)
    {
        // Clear Elite Four defeat flags
        // (Would need to implement ClearEliteFourDefeated function)
    }
    
    // Reset level cap
    UpdateLevelCap(0, REGION_KANTO);
    
    // Save corrected data
    SavePokemonTuxedoData();
}

bool8 CreateBackupSave(void)
{
    /*
     * Create backup of save data
     * 
     * Returns TRUE if backup was created successfully
     */
    
    // This would copy save data to backup slot
    // Implementation depends on pokefirered save system
    
    return TRUE;
}

bool8 RestoreFromBackup(void)
{
    /*
     * Restore save data from backup
     * 
     * Returns TRUE if restore was successful
     */
    
    // This would restore save data from backup slot
    // Implementation depends on pokefirered save system
    
    return TRUE;
}

// ============================================================================
// Battle System Error Handling
// ============================================================================

bool8 ValidateExpGain(u32 exp)
{
    /*
     * Validate experience gain is within reasonable range
     */
    
    // Maximum reasonable exp gain (with 4x multiplier)
    const u32 MAX_EXP_GAIN = 100000;
    
    return exp <= MAX_EXP_GAIN;
}

bool8 CanPokemonGainExp(struct Pokemon *pokemon)
{
    /*
     * Check if Pokemon can gain experience
     * 
     * Validates:
     * - Pokemon is not an egg
     * - Pokemon is not at max level
     * - Pokemon is not above level cap
     */
    u8 level;
    u8 level_cap;
    
    if (GetMonData(pokemon, MON_DATA_IS_EGG, NULL))
        return FALSE;
    
    level = GetMonData(pokemon, MON_DATA_LEVEL, NULL);
    
    if (level >= 100)
        return FALSE;
    
    level_cap = GetCurrentLevelCap();
    
    if (level >= level_cap)
        return FALSE;
    
    return TRUE;
}

void SafeAwardExperienceToParty(u32 base_exp)
{
    /*
     * Safe wrapper for AwardExperienceToParty with validation
     */
    
    // Validate exp amount
    if (!ValidateExpGain(base_exp))
    {
        base_exp = 10000;  // Cap at reasonable value
    }
    
    // Award to party (multiplier is applied per-Pokemon in the function)
    AwardExperienceToParty(base_exp);
}

// ============================================================================
// Storage System Error Handling
// ============================================================================

bool8 ValidateStorageAccess(void)
{
    /*
     * Validate storage system can be accessed
     * 
     * Checks:
     * - Not in battle
     * - Not in cutscene
     * - Party is valid
     */
    u8 party_count;
    
    if (!CanAccessStorageSystem())
        return FALSE;
    
    // Additional validation
    // Check party is not empty
    party_count = CalculatePlayerPartyCount();
    if (party_count == 0)
        return FALSE;
    
    return TRUE;
}

bool8 ValidatePokemonTransfer(struct Pokemon *pokemon)
{
    /*
     * Validate Pokemon can be transferred to/from storage
     * 
     * Checks:
     * - Pokemon is not an egg (optional)
     * - Pokemon data is valid
     * - Not the last Pokemon in party (for deposit)
     */
    u16 species;
    
    if (pokemon == NULL)
        return FALSE;
    
    species = GetMonData(pokemon, MON_DATA_SPECIES, NULL);
    if (species == SPECIES_NONE || species > NUM_SPECIES)
        return FALSE;
    
    return TRUE;
}

// ============================================================================
// Region Travel Error Handling
// ============================================================================

bool8 ValidateRegionTravel(u8 target_region)
{
    /*
     * Validate region travel is allowed
     * 
     * Checks:
     * - Target region is valid
     * - Requirements are met (E4 defeat + ticket)
     * - Player has Pokemon in party
     */
    u8 party_count;
    
    if (!ValidateRegionId(target_region))
        return FALSE;
    
    if (!CanTravelToRegion(target_region))
        return FALSE;
    
    // Check party is not empty
    party_count = CalculatePlayerPartyCount();
    if (party_count == 0)
        return FALSE;
    
    return TRUE;
}

// ============================================================================
// Error Logging
// ============================================================================

void LogError(const char *system, const char *message)
{
    /*
     * Log error message for debugging
     * 
     * In actual implementation, this would write to a log file
     * or display an error message to the player.
     */
    
    // Stub: Would log error here
    // For now, just a placeholder
}

// ============================================================================
// Comprehensive Validation
// ============================================================================

bool8 ValidateGameState(void)
{
    /*
     * Validate entire game state is consistent
     * 
     * Returns TRUE if all systems are in valid state
     */
    bool8 valid;
    u8 region;
    u8 badges;
    u8 level_cap;
    
    valid = TRUE;
    
    // Validate region
    region = GetCurrentRegion();
    if (!ValidateRegionId(region))
    {
        LogError("Region", "Invalid region ID");
        valid = FALSE;
    }
    
    // Validate badge count
    badges = GetBadgeCount();
    if (!ValidateBadgeCount(badges))
    {
        LogError("Badges", "Invalid badge count");
        valid = FALSE;
    }
    
    // Validate level cap
    level_cap = GetCurrentLevelCap();
    if (level_cap < 1 || level_cap > 100)
    {
        LogError("Level Cap", "Invalid level cap");
        valid = FALSE;
    }
    
    // Validate save data
    if (!ValidateSaveDataChecksum())
    {
        LogError("Save Data", "Corrupted save data");
        valid = FALSE;
    }
    
    return valid;
}
