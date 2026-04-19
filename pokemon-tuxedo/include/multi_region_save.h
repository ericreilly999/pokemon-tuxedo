#ifndef GUARD_MULTI_REGION_SAVE_H
#define GUARD_MULTI_REGION_SAVE_H

#include "global.h"
#include "region_manager.h"
#include "fly_location_discovery.h"

// Multi-region save data version for backward compatibility
// Increment this when save data format changes
#define MULTI_REGION_SAVE_VERSION 1

// Validation result codes
#define SAVE_VALID              0
#define SAVE_INVALID_VERSION    1
#define SAVE_INVALID_REGION     2
#define SAVE_INVALID_BADGES     3
#define SAVE_INVALID_FLY_DATA   4
#define SAVE_CORRUPTED          5

// Save multi-region state from EWRAM to PokemonTuxedoSaveData
// Copies runtime state (sRegionState, sFlyLocations) to save structure
// Validates: Requirements 16.1, 16.2, 16.3, 16.4, 16.5
void SaveMultiRegionData(void);

// Load multi-region state from PokemonTuxedoSaveData to EWRAM
// Copies save structure to runtime state (sRegionState, sFlyLocations)
// Handles backward compatibility with saves that don't have multi-region data
// Validates: Requirements 16.1, 16.2, 16.3, 16.4, 16.5
void LoadMultiRegionData(void);

// Validate multi-region save data integrity
// Returns SAVE_VALID if data is valid, or an error code if corrupted
// Validates: Requirements 16.1, 16.2, 16.3, 16.4, 16.5
u8 ValidateMultiRegionSaveData(void);

// Initialize multi-region save data to defaults
// Called for new games or when loading old saves without multi-region data
void InitMultiRegionSaveData(void);

// Check if save data has multi-region data (for backward compatibility)
// Returns TRUE if save has valid multi-region data, FALSE otherwise
bool8 HasMultiRegionSaveData(void);

// Get fly location data pointers for save/load operations
u16* GetFlyLocationArray(void);
u8* GetFlyLocationCountArray(void);

#endif // GUARD_MULTI_REGION_SAVE_H
