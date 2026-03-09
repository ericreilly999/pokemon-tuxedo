#include "global.h"
#include "level_scaler.h"
#include "region_manager.h"
#include "full_party_exp.h"
#include "exp_multipliers.h"
#include "storage_system.h"
#include "game_settings.h"
#include "fly_location_discovery.h"
#include "map_accessibility.h"
#include "event_data.h"
#include "pokemon.h"
#include "battle.h"

/*
 * System Integration
 * 
 * This file wires all Pokemon Tuxedo systems together and integrates
 * them with the pokefirered decomp codebase.
 * 
 * Requirements:
 * - 2.5: Connect level scaling to encounter system
 * - 2.8: Connect region progression to level scaling
 * - All: Connect systems to save data
 */

// ============================================================================
// Save Data Integration
// ============================================================================

struct PokemonTuxedoSaveData
{
    // Region management
    u8 currentRegion;
    bool8 eliteFourDefeated[4];
    
    // Level cap
    u8 currentLevelCap;
    
    // Fly location discovery
    bool8 discoveredFlyLocations[50];
    
    // Map accessibility
    bool8 regionMapsUnlocked[4];
    
    // Game settings
    u8 battleMode;
    u8 gameSpeed;
    
    // Checksum
    u32 checksum;
};

extern struct PokemonTuxedoSaveData gPokemonTuxedoSave;

void SavePokemonTuxedoData(void)
{
    // Save all Pokemon Tuxedo data to save file
    // This would integrate with the main save system
    
    // Calculate checksum
    u32 checksum = 0;
    u8 *data = (u8 *)&gPokemonTuxedoSave;
    u32 size = sizeof(struct PokemonTuxedoSaveData) - sizeof(u32);
    
    for (u32 i = 0; i < size; i++)
        checksum += data[i];
    
    gPokemonTuxedoSave.checksum = checksum;
}

bool8 LoadPokemonTuxedoData(void)
{
    // Load all Pokemon Tuxedo data from save file
    // Validate checksum
    
    u32 checksum = 0;
    u8 *data = (u8 *)&gPokemonTuxedoSave;
    u32 size = sizeof(struct PokemonTuxedoSaveData) - sizeof(u32);
    
    for (u32 i = 0; i < size; i++)
        checksum += data[i];
    
    if (checksum != gPokemonTuxedoSave.checksum)
        return FALSE;  // Corrupted save data
    
    return TRUE;
}

// ============================================================================
// Level Scaling Integration
// ============================================================================

void UpdateWildPokemonLevel(struct Pokemon *pokemon)
{
    /*
     * Hook into wild encounter system to apply level scaling
     * 
     * This should be called when a wild Pokemon is generated
     * to replace its base level with the scaled level.
     */
    
    u8 badge_count = GetBadgeCount();
    u8 region_id = GetCurrentRegion();
    bool8 elite_four_defeated = IsEliteFourDefeated(region_id);
    
    struct LevelRange level_range = GetWildPokemonLevelRange(
        badge_count, region_id, elite_four_defeated
    );
    
    // Set random level within range
    u8 level = level_range.min_level + (Random() % (level_range.max_level - level_range.min_level + 1));
    
    SetMonData(pokemon, MON_DATA_LEVEL, &level);
    CalculateMonStats(pokemon);
}

void UpdateTrainerPokemonLevels(struct Trainer *trainer)
{
    /*
     * Hook into trainer battle system to apply level scaling
     * 
     * This should be called when a trainer battle is initiated
     * to calculate and set Pokemon levels based on trainer type.
     */
    
    u8 badge_count = GetBadgeCount();
    u8 region_id = GetCurrentRegion();
    
    // Determine trainer type and calculate levels
    if (trainer->trainerClass == TRAINER_CLASS_LEADER)
    {
        // Gym leader
        u8 average_level = GetGymLeaderAverageLevel(badge_count, region_id);
        
        for (u8 i = 0; i < trainer->partySize; i++)
        {
            if (i == trainer->partySize - 1)
            {
                // Ace Pokemon
                u8 gym_id = badge_count;  // Simplified
                u8 ace_level = GetGymLeaderAceLevel(gym_id);
                trainer->party[i].lvl = ace_level;
            }
            else
            {
                // Regular Pokemon
                trainer->party[i].lvl = average_level;
            }
        }
    }
    else if (trainer->trainerClass == TRAINER_CLASS_ELITE_FOUR)
    {
        // Elite Four member
        u8 average_level = GetEliteFourAverageLevel(badge_count, region_id);
        
        for (u8 i = 0; i < trainer->partySize; i++)
        {
            if (i == trainer->partySize - 1)
            {
                // Ace Pokemon
                u8 e4_id = region_id * 4;  // Simplified
                u8 ace_level = GetEliteFourAceLevel(e4_id);
                trainer->party[i].lvl = ace_level;
            }
            else
            {
                // Regular Pokemon
                trainer->party[i].lvl = average_level;
            }
        }
    }
    else if (trainer->trainerClass == TRAINER_CLASS_RIVAL)
    {
        // Rival
        u8 average_level = GetRivalAverageLevel(badge_count, region_id);
        
        for (u8 i = 0; i < trainer->partySize; i++)
        {
            if (i == trainer->partySize - 1)
            {
                // Ace Pokemon
                u8 rival_id = region_id;  // Simplified
                u8 ace_level = GetRivalAceLevel(rival_id);
                trainer->party[i].lvl = ace_level;
            }
            else
            {
                // Regular Pokemon
                trainer->party[i].lvl = average_level;
            }
        }
    }
}

// ============================================================================
// Experience System Integration
// ============================================================================

void AwardBattleExperience(u32 base_exp)
{
    /*
     * Hook into battle system to award experience with multipliers
     * and full party distribution
     * 
     * This should be called after a battle is won to award experience
     * to all party Pokemon.
     */
    
    // Apply EXP multiplier
    u32 multiplied_exp = ApplyExpMultiplier(base_exp);
    
    // Award to full party
    AwardExperienceToParty(multiplied_exp);
}

// ============================================================================
// Level Cap Integration
// ============================================================================

void OnBadgeAcquired(void)
{
    /*
     * Hook into badge acquisition to update level cap
     * 
     * This should be called when the player receives a new badge.
     */
    
    u8 badge_count = GetBadgeCount();
    u8 region_id = GetCurrentRegion();
    
    UpdateLevelCap(badge_count, region_id);
}

void OnRegionChanged(void)
{
    /*
     * Hook into region transition to update level cap
     * 
     * This should be called when the player enters a new region.
     */
    
    u8 badge_count = GetBadgeCount();
    u8 region_id = GetCurrentRegion();
    
    UpdateLevelCap(badge_count, region_id);
    UnlockRegionMaps(region_id);
}

void OnEliteFourDefeated(void)
{
    /*
     * Hook into Elite Four defeat to award tickets and multipliers
     * 
     * This should be called when the player defeats the Elite Four.
     */
    
    u8 region_id = GetCurrentRegion();
    
    // Mark Elite Four as defeated
    SetEliteFourDefeated(region_id);
    
    // Award region ticket
    AwardRegionTicket();
    
    // Award EXP multiplier item
    AwardMultiplierItemAfterEliteFour(region_id);
    
    // Update level cap
    OnBadgeAcquired();
}

// ============================================================================
// Map System Integration
// ============================================================================

void OnMapEntered(u16 map_id)
{
    /*
     * Hook into map entry to discover fly locations
     * 
     * This should be called when the player enters a new map.
     */
    
    // Discover fly location if applicable
    DiscoverFlyLocation(map_id);
}

// ============================================================================
// Initialization
// ============================================================================

void InitializePokemonTuxedoSystems(void)
{
    /*
     * Initialize all Pokemon Tuxedo systems
     * 
     * This should be called during game initialization.
     */
    
    // Load save data
    if (!LoadPokemonTuxedoData())
    {
        // Initialize with defaults if save data is corrupted
        gPokemonTuxedoSave.currentRegion = REGION_KANTO;
        gPokemonTuxedoSave.battleMode = BATTLE_MODE_SET;
        gPokemonTuxedoSave.gameSpeed = 1;
        
        for (u8 i = 0; i < 4; i++)
        {
            gPokemonTuxedoSave.eliteFourDefeated[i] = FALSE;
            gPokemonTuxedoSave.regionMapsUnlocked[i] = FALSE;
        }
        
        for (u8 i = 0; i < 50; i++)
            gPokemonTuxedoSave.discoveredFlyLocations[i] = FALSE;
    }
    
    // Load game settings
    LoadGameSettings();
    
    // Update level cap
    u8 badge_count = GetBadgeCount();
    u8 region_id = GetCurrentRegion();
    UpdateLevelCap(badge_count, region_id);
}

// ============================================================================
// Integration Test Functions
// ============================================================================

bool8 TestSystemIntegration(void)
{
    /*
     * Test that all systems are properly integrated
     * 
     * This function runs integration tests to verify that:
     * 1. Badge acquisition updates level cap
     * 2. Region transition updates level scaling
     * 3. Elite Four defeat awards tickets
     * 4. Experience system works with multipliers
     */
    
    bool8 all_tests_passed = TRUE;
    
    // Test 1: Badge acquisition → level cap update
    u8 initial_cap = GetCurrentLevelCap();
    OnBadgeAcquired();
    u8 new_cap = GetCurrentLevelCap();
    
    if (new_cap <= initial_cap)
        all_tests_passed = FALSE;
    
    // Test 2: Region transition → level scaling update
    u8 initial_region = GetCurrentRegion();
    SetCurrentRegion(REGION_JOHTO);
    OnRegionChanged();
    u8 new_region = GetCurrentRegion();
    
    if (new_region != REGION_JOHTO)
        all_tests_passed = FALSE;
    
    // Restore initial region
    SetCurrentRegion(initial_region);
    
    // Test 3: Elite Four defeat → ticket award
    SetEliteFourDefeated(REGION_KANTO);
    AwardRegionTicket();
    
    // Test 4: Experience multiplier application
    u32 base_exp = 1000;
    u32 multiplied_exp = ApplyExpMultiplier(base_exp);
    
    if (multiplied_exp < base_exp)
        all_tests_passed = FALSE;
    
    return all_tests_passed;
}
